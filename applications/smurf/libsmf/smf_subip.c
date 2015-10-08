/*
*+
*  Name:
*     smf_subip

*  Purpose:
*     Make correction for instrumental polarisation in POL2 data.

*  Language:
*     Starlink ANSI C

*  Type of Module:
*     C function

*  Invocation:
*     void smf_subip( ThrWorkForce *wf, smfArray *res, smfArray *lut,
*                     int *lbnd_out, int *ubnd_out, AstKeyMap *keymap,
*                     int *status )

*  Arguments:
*     wf = ThrWorkForce * (Given)
*        Pointer to a pool of worker threads (can be NULL)
*     res = smfArray * (Given)
*        Pointer to a smfArray holding the bolometer time-stream values for
*        all scuba-2 subarrays.
*     lut = smfArray * (Given)
*        Pointer to a smfArray holding the map pixel index for each
*        bolometer value in all scuba-2 subarrays.
*     lbnd_out = int * (Given)
*        A 2-element array - the lower pixel index bounds of the output map.
*     ubnd_out = int * (Given)
*        A 2-element array - the upper pixel index bounds of the output map.
*     keymap = AstKeyMap * (Given)
*        A KeyMap holding all configuration parameters.
*     status = int* (Given and Returned)
*        Pointer to global status.

*  Description:
*     This function modifies the supplied bolometer Q/U time-stream data
*     by subtracting off estimates of the Q or U caused by instrumental
*     polarisation, based on the total intensity values specified by the
*     map associated with environment parameter IPREF. The values in this
*     map are multiplied by factors determined from the current elevation
*     and the parameters of the Johnstone/Kennedy IP model for POL2 to
*     get the instrumental Q and U values which are subtracted off the
*     supplied bolometer data. The parameters for the IP model are read
*     frpm a data file specified via configuration parameter IPDATA.

*  Authors:
*     David S Berry (EAO)
*     {enter_new_authors_here}

*  History:
*     7-OCT-2015 (DSB):
*        Original version.
*     {enter_further_changes_here}

*  Copyright:
*     Copyright (C) 2015 East Asian Observatory.
*     All Rights Reserved.

*  Licence:
*     This program is free software; you can redistribute it and/or
*     modify it under the terms of the GNU General Public License as
*     published by the Free Software Foundation; either version 3 of
*     the License, or (at your option) any later version.
*
*     This program is distributed in the hope that it will be
*     useful, but WITHOUT ANY WARRANTY; without even the implied
*     warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
*     PURPOSE. See the GNU General Public License for more details.
*
*     You should have received a copy of the GNU General Public
*     License along with this program; if not, write to the Free
*     Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
*     MA 02110-1301, USA

*  Bugs:
*     {note_any_bugs_here}
*-
*/

/* Starlink includes */
#include "ast.h"
#include "mers.h"
#include "sae_par.h"
#include "prm_par.h"
#include "par.h"

/* SMURF includes */
#include "libsmf/smf.h"
#include "libsmf/smf_typ.h"
#include "libsmf/smf_err.h"

/* Prototypes for local static functions. */
static void smf1_subip( void *job_data_ptr, int *status );

/* Local data types */
typedef struct smfSubIPData {
   JCMTState *allstate;
   const char *qu;
   dim_t b1;
   dim_t b2;
   dim_t ntslice;
   double *angcdata;
   double *c0data;
   double *imapdata;
   double *p0data;
   double *p1data;
   double *res_data;
   int *lut_data;
   size_t bstride;
   size_t tstride;
   smf_qual_t *qua_data;
} SmfSubIPData;


void smf_subip(  ThrWorkForce *wf, smfArray *res, smfArray *lut, int *lbnd_out,
                 int *ubnd_out, AstKeyMap *keymap, int *status ) {

/* Local Variables: */
   HDSLoc *loc = NULL;
   HDSLoc *sloc = NULL;
   SmfSubIPData *job_data = NULL;
   SmfSubIPData *pdata;
   char ipref[200];
   char subname[10];
   const char *ipdata;
   const char *qu;
   dim_t bolostep;
   dim_t nbolo;
   dim_t ntslice;
   double *angcdata;
   double *c0data;
   double *imapdata;
   double *p0data;
   double *p1data;
   int angcndf;
   int c0ndf;
   int imapndf;
   int iw;
   int nmap;
   int nw;
   int p0ndf;
   int p1ndf;
   int tndf;
   size_t bstride;
   size_t idx;
   size_t tstride;
   smfData *data;
   smf_qual_t *qua_data;

/* Check inherited status */
   if( *status != SAI__OK ) return;

/* Check if we have pol2 data, and see if it is Q or U. */
   qu = NULL;
   for( idx = 0; idx < res->ndat; idx++ ) {
      data = res->sdata[idx];

      if( !strcmp( data->hdr->dlabel, "Q" ) ){
         if( !qu ) {
            qu = "Q";
         } else if( strcmp( qu, "Q" ) ) {
            *status = SAI__ERROR;
            break;
         }

      } else if( !strcmp( data->hdr->dlabel, "U" ) ) {
         if( !qu ) {
            qu = "U";
         } else if( strcmp( qu, "U" ) ) {
            *status = SAI__ERROR;
            break;
         }

      } else if( qu ) {
         *status = SAI__ERROR;
         qu = NULL;
         break;
      }
   }

/* Report an error if there is a mix of pol2 and non-pol2, or a mix of Q
   and U. */
   if( *status != SAI__OK ) {
      if( qu ) {
         errRep( "", "smf_subip: Input data contains mix of Q and U "
                 "data", status );
      } else {
         errRep( "", "smf_subip: Input data contains mix of POL2 and "
                 "non-POL2 data", status );
      }

/* If we have pol2 data, get the path to the total intensity image that
   is to be used to define the level of IP correction required. If no
   value is supplied, annul the error and set "qu" NULL to indicate we should
   leave immediately. */
   } else if( qu && *status == SAI__OK ) {
      parGet0c( "IPREF", ipref, sizeof(ipref), status );
      if( *status == PAR__NULL ) {
         errAnnul( status );
         qu = NULL;
      }
   }

/* If we are applying IP correction... */
   if( qu && *status == SAI__OK ) {
      msgOutf( "", "smf_subip: applying instrumental polarisation %s "
               "correction based on total intensity map `%s'", status,
               qu, ipref );

/* Open the IPREF NDF, get a section from it matching the bounds of
   the output map, then close the original NDF - retaining the section. */
      ndfFind( NULL, ipref, &tndf, status );
      ndfSect( tndf, 2, lbnd_out, ubnd_out, &imapndf, status );
      ndfAnnul( &tndf, status );

/* Map the data as double precision */
      ndfMap( imapndf, "DATA", "_DOUBLE", "READ", (void **) &imapdata,
              &nmap, status );

/* Create structures used to pass information to the worker threads. */
      nw = wf ? wf->nworker : 1;
      job_data = astMalloc( nw*sizeof( *job_data ) );

/* Get the path to the container file holding the IP model parameters. */
      ipdata = "$STARLINK_DIR/share/smurf/ipdata.sdf";
      astMapGet0C( keymap, "IPDATA", &ipdata );

/* Open the container file. */
      hdsOpen( ipdata, "READ", &loc, status );

/* Do the IP correction for each subarray (s8a, s8b, etc) in turn. */
      for( idx = 0; idx < res->ndat && *status == SAI__OK; idx++ ) {
         data = res->sdata[idx];

/* Get the number of bolometers and time slices for the current subarray,
   together with the strides between adjacent bolometers and adjacent
   time slices. */
         smf_get_dims( data,  NULL, NULL, &nbolo, &ntslice, NULL, &bstride,
                       &tstride, status );

/* Get a locator for the structure holding the IP parameters for the
   current subarray */
         smf_find_subarray( data->hdr, subname, sizeof( subname ), NULL,
                            status );
         datFind( loc, subname, &sloc, status );

/* Begin an NDF context. */
         ndfBegin();

/* Get identifiers for the NDFs holding the individual parameters. Each
   NDF holds a parameter value for each bolometer. */
         ndfFind( sloc, "P0", &p0ndf, status );
         ndfFind( sloc, "P1", &p1ndf, status );
         ndfFind( sloc, "C0", &c0ndf, status );
         ndfFind( sloc, "ANGC", &angcndf, status );

/* Map them. Check each one has the expected number of elements. */
         ndfMap( p0ndf, "DATA", "_DOUBLE", "READ", (void **) &p0data,
                 &nmap, status );
         if( nmap != (int) nbolo && *status == SAI__OK ) {
            *status = SAI__ERROR;
            ndfMsg( "N", p0ndf );
            errRep( "", "smf_subip: Bad dimensions for ^N - should be 32x40.", status );
         }

         ndfMap( p1ndf, "DATA", "_DOUBLE", "READ", (void **) &p1data,
                 &nmap, status );
         if( nmap != (int) nbolo && *status == SAI__OK ) {
            *status = SAI__ERROR;
            ndfMsg( "N", p1ndf );
            errRep( "", "smf_subip: Bad dimensions for ^N - should be 32x40.", status );
         }

         ndfMap( c0ndf, "DATA", "_DOUBLE", "READ", (void **) &c0data,
                 &nmap, status );
         if( nmap != (int) nbolo && *status == SAI__OK ) {
            *status = SAI__ERROR;
            ndfMsg( "N", c0ndf );
            errRep( "", "smf_subip: Bad dimensions for ^N - should be 32x40.", status );
         }

         ndfMap( angcndf, "DATA", "_DOUBLE", "READ", (void **) &angcdata,
                 &nmap, status );
         if( nmap != (int) nbolo && *status == SAI__OK ) {
            *status = SAI__ERROR;
            ndfMsg( "N", angcndf );
            errRep( "", "smf_subip: Bad dimensions for ^N - should be 32x40.", status );
         }

/* Get a pointer to the quality array for the residuals. */
         qua_data = smf_select_qualpntr( data, NULL, status );

/* See how many bolometers to process in each thread. */
         bolostep = nbolo/nw;
         if( bolostep == 0 ) bolostep = 1;

/* Create jobs to apply the IP correction to a range of bolometers. */
         for( iw = 0; iw < nw; iw++ ) {
            pdata = job_data + iw;

/* Set the range of bolometers (b1 to b2) to be processed by the current
   job. */
            pdata->b1 = iw*bolostep;
            if( iw < nw - 1 ) {
               pdata->b2 = pdata->b1 + bolostep - 1;
            } else {
               pdata->b2 = nbolo - 1 ;
            }

/* Store the other info needed by the worker thread. */
            pdata->ntslice = ntslice;
            pdata->res_data = res->sdata[idx]->pntr[0];
            pdata->lut_data = lut->sdata[idx]->pntr[0];
            pdata->qua_data = qua_data;
            pdata->bstride = bstride;
            pdata->tstride = tstride;
            pdata->imapdata = imapdata;
            pdata->qu = qu;
            pdata->p0data = p0data;
            pdata->p1data = p1data;
            pdata->c0data = c0data;
            pdata->angcdata = angcdata;
            pdata->allstate = data->hdr->allState;

/* Submit the job for execution by the next available thread. */
            thrAddJob( wf, 0, pdata, smf1_subip, 0, NULL, status );
         }

/* Wait for all jobs to complete. */
         thrWait( wf, status );

/* End the NDF context, thus unmapping and freeing all NDF identifiers
   created since the context was started. */
         ndfEnd( status );

/* Free locator for subarray IP parameters. */
         datAnnul( &sloc, status );
      }

/* Free resources. */
      datAnnul( &loc, status );
      job_data = astFree( job_data );
      ndfAnnul( &imapndf, status );
   }
}


static void smf1_subip( void *job_data_ptr, int *status ) {
/*
*  Name:
*     smf1_subip

*  Purpose:
*     Executed in a worker thread to apply IP correction to a range of
*     bolometers.

*  Invocation:
*     smf1_subip( void *job_data_ptr, int *status )

*  Arguments:
*     job_data_ptr = SmfSubIPData * (Given)
*        Data structure describing the job to be performed by the worker
*        thread.
*     status = int * (Given and Returned)
*        Inherited status.

*/

/* Local Variables: */
   JCMTState *state;
   SmfSubIPData *pdata;
   const char *qu;
   dim_t ibolo;
   dim_t itime;
   dim_t ntslice;
   double *imapdata;
   double *pr;
   double angc;
   double c0;
   double ipfactor;
   double p0;
   double p1;
   double tmp1;
   double tmp2;
   double tmp3;
   int *pl;
   size_t bstride;
   size_t tstride;
   smf_qual_t *pq;

/* Check inherited status */
   if( *status != SAI__OK ) return;

/* Get a pointer that can be used for accessing the required items in the
   supplied structure. */
   pdata = (SmfSubIPData *) job_data_ptr;

/* Save some local values for speed. */
   ntslice = pdata->ntslice;
   tstride = pdata->tstride;
   bstride = pdata->bstride;
   imapdata = pdata->imapdata;
   qu = pdata->qu;

/* Loop round each bolometer to be processed by this thread. */
   for( ibolo = pdata->b1; ibolo <= pdata->b2; ibolo++ ) {

/* Get a pointer to the first quality value for the current bolometer. */
      pq = pdata->qua_data + ibolo*bstride;

/* Check that the whole bolometer has not been flagged as bad. */
      if( !( *pq & SMF__Q_BADB ) ) {

/* Get the IP model parameters for this bolometer. */
         p0 = pdata->p0data[ ibolo ];
         p1 = pdata->p1data[ ibolo ];
         c0 = pdata->c0data[ ibolo ];
         angc = pdata->angcdata[ ibolo ];

/* If any of them are bad, flag the whole bolometer as unusable. */
         if( p0 == VAL__BADD || p1 == VAL__BADD ||
             c0 == VAL__BADD || angc == VAL__BADD ) {

            for( itime = 0; itime < ntslice; itime++ ) {
               *pq |= ( SMF__Q_IP | SMF__Q_BADB );
               pq += tstride;
            }

/* If all the IP parameters are good, we can do the IP correction. */
         } else {

/* Cache values needed in the itime loop. */
            tmp1 = p0*cos( 2*angc*AST__DD2R );
            tmp2 = p0*sin( 2*angc*AST__DD2R );
            tmp3 = 2*( c0 + angc )*AST__DD2R;

/* Get pointers to the first residual (i.e. the uncorrected Q or U value)
   and lut value (i.e. the index of the map pixel that receives the Q/U
   value) for the current bolometer. */
            pr = pdata->res_data + ibolo*bstride;
            pl = pdata->lut_data + ibolo*bstride;

/* Loop round each time slice, maintaining a pointer to the JCMTState
   info for the slice (we need this to get the elevation for each slice). */
            state = pdata->allstate;
            for( itime = 0; itime < ntslice; itime++,state++ ) {

/* Skip this sample if the residual is bad or flagged, of the
   corresponding map pixel is undefined, or the telescope elevation is
   unknown. */
               if( *pr != VAL__BADD && !( *pq & SMF__Q_MOD ) &&
                   *pl != VAL__BADI && state->tcs_az_ac2 != VAL__BADD) {

/* Find the factor by which to multiply the total intensity value
   to get the required IP correction to Q or U. */
                  if( *qu == 'Q' ) {
                     ipfactor = tmp1 + p1*cos( tmp3 + 2*state->tcs_az_ac2 );
                  } else {
                     ipfactor = tmp2 + p1*sin( tmp3 + 2*state->tcs_az_ac2 );
                  }

/* Correct the residual Q or U value. */
                  *pr -= imapdata[ *pl ]*ipfactor;
               }

/* Move onto the next time slice. */
               pq += tstride;
               pr += tstride;
               pl += tstride;
            }
         }
      }
   }
}

