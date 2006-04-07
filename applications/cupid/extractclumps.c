#include "sae_par.h" 
#include "mers.h" 
#include "ndf.h" 
#include "star/ndg.h" 
#include "ast.h" 
#include "star/kaplibs.h" 
#include "star/grp.h" 
#include "star/hds.h" 
#include "par.h" 
#include "prm_par.h"
#include "cupid.h" 
#include <math.h> 
#include <string.h> 
#include <stdio.h>


void extractclumps() {
/*
*+
*  Name:
*     EXTRACTCLUMPS

*  Purpose:
*     Extract previously identified clumps of emission from an NDF.

*  Language:
*     C

*  Type of Module:
*     ADAM A-task

*  Synopsis:
*     void extractclumps();

*  Description:
*     This application extract previously identified clumps of emission 
*     from a 1, 2 or 3 dimensional NDF. Usually, FINDCLUMPS will first be
*     used to identify the clumps within a given array, and then 
*     EXTRACTCLUMPS can be used to find the parameters of the same clumps 
*     in a second array.
*
*     Two input NDFs are supplied; the NDF associated with parameter DATA 
*     contains the physical data values from which the clumps are to be 
*     extracted, whilst the NDF associated with parameter MASK contains 
*     integer values that identify the clump to which each pixel belongs. The 
*     two NDFs are assumed to be aligned in PIXEL coordinates. An output NDF 
*     is created that is a copy of the MASK NDF. Parameters describing the 
*     clumps extracted from the DATA NDF are stored in the CUPID extension 
*     of the output NDF, and may also be stored in an output catalogue. 
*     These are in the same form as the clump parameters created by the 
*     FINDCLUMPS command.

*  Usage:
*     extractclumps mask data out outcat 

*  ADAM Parameters:
*     FWHMBEAM = _REAL (Read)
*        The FWHM of the instrument beam, in pixels. The clump widths written 
*        to the output catalogue are reduced (in quadrature) by this amount. 
*        The default value is the value stored in the CONFIG component 
*        of the CUPID extension in the mask NDF, or 2.0 if the CUPID
*        extension does not contain a CONFIG component. []
*     DATA = NDF (Read)
*        The input NDF containing the physical data values.
*     MASK = NDF (Read)
*        The input NDF containing the pixel assignments. This will
*        usually have been created by the FINDCLUMPS command.
*     OUT = NDF (Write)
*        The output NDF.
*     OUTCAT = FILENAME (Write)
*        An optional output catalogue in which to store the clump parameters.
*        See the description of the OUTCAT parameter for the FINDCLUMPS 
*        command for further information.
*     VELORES = _REAL (Read)
*        The velocity resolution of the instrument, in channels. The velocity 
*        width of each clump written to the output catalogue is reduced (in 
*        quadrature) by this amount. The default value is the value stored in 
*        the CONFIG component of the CUPID extension in the mask NDF, or 2.0 
*        if the CUPID extension does not contain a CONFIG component. []

*  Authors:
*     DSB: David S. Berry
*     {enter_new_authors_here}

*  History:
*     6-APR-2006 (DSB):
*        Original version.
*     {enter_further_changes_here}

*  Bugs:
*     {note_any_bugs_here}

*-
*/      

/* Local Variables: */
   AstFrameSet *iwcs;           /* Pointer to the WCS FrameSet */
   AstKeyMap *config;           /* Pointer to KeyMap holding used config settings */
   AstObject *mconfig;          /* Pointer to KeyMap holding algorithm settings */
   Grp *grp;                    /* GRP group holding NDF names */
   HDSLoc *ndfs;                /* Array of NDFs, one for each clump */
   HDSLoc *xloc;                /* HDS locator for CUPID extension */
   IRQLocs *qlocs;              /* HDS locators for quality name information */
   const char *method;          /* Algorithm string supplied by user */
   char dtype[ 20 ];            /* NDF data type */
   char itype[ 20 ];            /* NDF data type */
   double beamcorr[ 3 ];        /* Beam width corrections */
   double fb;                   /* FWHMBEAM value */
   double vr;                   /* VELORES value */
   float *rmask;                /* Pointer to cump mask array */
   int *clbnd;                  /* Lower GRID bounds of each clump */
   int *cubnd;                  /* Upper GRID bounds of each clump */
   int *ipa;                    /* Pointer to pixel assignment array */
   int *pa;                     /* Pointer to next pixel assignment value */
   int *pid;                    /* Pointer to next clump ID */
   int dim[ NDF__MXDIM ];       /* Pixel axis dimensions */
   int dims[ 3 ];               /* Significant pixel axis dimensions */
   int el;                      /* Number of array elements mapped */
   int gotwcs;                  /* Does input NDF contain a WCS FrameSet? */
   int i;                       /* Loop count */
   int id;                      /* Clump identifier */
   int idmax;                   /* Maximum clump ID in supplied mask */
   int idmin;                   /* Minimum clump ID in supplied mask */
   int indf1;                   /* Identifier for input data NDF */
   int indf2;                   /* Identifier for input mask NDF */
   int indf3;                   /* Identifier for output mask NDF */
   int ix;                      /* GRID value on 1st axis */
   int iy;                      /* GRID value on 2nd axis */
   int iz;                      /* GRID value on 3rd axis */
   int n;                       /* Number of values summed in "sum" */
   int nclump;                  /* Number of clump IDs */
   int ndim;                    /* Total number of pixel axes */
   int nsig;                    /* Number of significant pixel axes */
   int sdim[ NDF__MXDIM ];      /* The indices of the significant pixel axes */
   int size;                    /* Number of elements in "grp" */
   int skip[ 3 ];               /* Pixel axis skips */
   int slbnd[ NDF__MXDIM ];     /* The lower bounds of the significant pixel axes */
   int subnd[ NDF__MXDIM ];     /* The upper bounds of the significant pixel axes */
   int there;                   /* Does object exist? */
   int type;                    /* Integer identifier for data type */
   void *ipd;                   /* Pointer to Data array */

/* Abort if an error has already occurred. */
   if( *status != SAI__OK ) return;

/* Initialise things to safe values. */
   xloc = NULL;
   ndfs = NULL;

/* Begin an AST context */
   astBegin;

/* Start an NDF context */
   ndfBegin();

/* Get an identifier for the two input NDFs. We use NDG (via kpg1_Rgndf)
   instead of calling ndfAssoc directly since NDF/HDS has problems with
   file names containing spaces, which NDG does not have. */
   kpg1Rgndf( "MASK", 1, 1, "", &grp, &size, status );
   ndgNdfas( grp, 1, "READ", &indf2, status );
   grpDelet( &grp, status );

   kpg1Rgndf( "DATA", 1, 1, "", &grp, &size, status );
   ndgNdfas( grp, 1, "READ", &indf1, status );
   grpDelet( &grp, status );

/* Match the bounds of the two NDFs. */
   ndfMbnd( "TRIM", &indf1, &indf2, status );

/* Get the dimensions of the NDF, and count the significant ones. */
   ndfDim( indf1, NDF__MXDIM, dim, &ndim, status );
   nsig = 0;
   for( i = 0; i < ndim; i++ ) {
      if( dim[ i ] > 1 ) nsig++;
   }

/* Abort if the NDF is not 1-, 2- or 3- dimensional. */
   if( nsig > 3 ) {
      if( *status == SAI__OK ) {
         *status = SAI__ERROR;
         msgSeti( "N", nsig );
         errRep( "", "Supplied NDFs have ^N significant pixel axes", status );
         errRep( "", "This application requires 1, 2 or 3 significant pixel axes", status );
      }
   }          

/* Get the WCS FrameSet and the significant axis bounds. */
   kpg1Asget( indf1, nsig, 1, 0, 0, sdim, slbnd, subnd, &iwcs, status );

/* Find the size of each dimension of the data array, and the skip in 1D 
   vector index needed to move by pixel along an axis. */
   for( i = 0; i < nsig; i++ ) {
      dims[ i ] = subnd[ i ] - slbnd[ i ] + 1;
      skip[ i ] = ( i == 0 ) ? 1 : skip[ i - 1 ]*dims[ i - 1 ];
   }
   for( ; i < 3; i++ ) {
      dims[ i ] = 1;
      skip[ i ] = 0;
   }

/* Choose the data type to use when mapping the DATA Data array. */
   ndfMtype( "_REAL,_DOUBLE", indf1, indf1, "DATA", itype, 20, dtype, 20,
             status );
   if( !strcmp( itype, "_DOUBLE" ) ) {
      type = CUPID__DOUBLE;
   } else {
      type = CUPID__FLOAT;
   }

/* Map the DATA Data array. */
   ndfMap( indf1, "DATA", itype, "READ", &ipd, &el, status );

/* Create the output NDF from the MASK NDF. */
   ndfProp( indf2, "AXIS,WCS", "OUT", &indf3, status );

/* Map the input mask array. */
   ndfMap( indf2, "DATA", "_INTEGER", "READ", (void *) &ipa, &el, status );

/* Find the largest and smallest clump identifier values in the output Data
   array. */
   if( *status == SAI__OK ) {
      idmax = VAL__MINI;
      idmin = VAL__MAXI;
      pid = ipa;
      for( i = 0; i < el; i++, pid++ ) {
         if( *pid != VAL__BADI ) {
            if( *pid < idmin ) idmin = *pid;
            if( *pid > idmax ) idmax = *pid;
         }
      }

      if( idmax < idmin ) {
         *status = SAI__ERROR;
         ndfMsg( "M", indf2 );
         errRep( "", "No clumps identified by mask NDF ^M", status );
      }
   }

/* Allocate memory to hold the pixel bounds of the clumps. */
   nclump = idmax - idmin + 1;
   clbnd = astMalloc( sizeof( int )*nclump*3 );
   cubnd = astMalloc( sizeof( int )*nclump*3 );

/* Find the upper and lower pixel bounds of each clump. */
   if( cubnd ) {

/* Initialise the bounding boxes. */
      for( i = 0; i < nclump*3; i++ ) {
         clbnd[ i ] = VAL__MAXI;
         cubnd[ i ] = VAL__MINI;
      }

/* Loop round every pixel in the pixel assignment array. */
      pa = ipa;
      for( iz = 1; iz <= dims[ 2 ]; iz++ ){
         for( iy = 1; iy <= dims[ 1 ]; iy++ ){
            for( ix = 1; ix <= dims[ 0 ]; ix++, pa++ ){

/* Skip pixels which are not in any clump. */
               if( *pa >= 0 ) {

/* Get the index within the clbnd and cubnd arrays of the current bounds
   on the x axis for this clump. */
                  i = 3*( *pa - idmin );

/* Update the bounds for the x axis, then increment to get the index of
   the y axis bounds. */
                  if( ix < clbnd[ i ] ) clbnd[ i ] = ix;
                  if( ix > cubnd[ i ] ) cubnd[ i ] = ix;
                  i++;

/* Update the bounds for the y axis, then increment to get the index of
   the z axis bounds. */
                  if( iy < clbnd[ i ] ) clbnd[ i ] = iy;
                  if( iy > cubnd[ i ] ) cubnd[ i ] = iy;
                  i++;

/* Update the bounds for the z axis. */
                  if( iz < clbnd[ i ] ) clbnd[ i ] = iz;
                  if( iz > cubnd[ i ] ) cubnd[ i ] = iz;
               }
            }
         }
      }

/* Loop round each clump, creating an NDF containing a description of the
   clump. */
      for( id = idmin; id <= idmax; id++ ) {
         i = 3*( id - idmin );
         ndfs = cupidNdfClump( type, ipd, ipa, el, nsig, dims, skip, slbnd, id,
                               clbnd + i, cubnd + i, NULL, ndfs, VAL__MAXI );
      }
   }

/* Unmap the input pixel assignment array. */
   ndfUnmap( indf2, "*", status );

/* Skip the rest if no clumps were found. */
   if( ndfs ) {

/* Get a locator for the CUPID extension in the output NDF, creating a
   new one if none exists. Erase any CLUMPS component from the extension. */
      ndfXstat( indf3, "CUPID", &there, status );
      if( there ) {
         ndfXloc( indf3, "CUPID", "UPDATE", &xloc, status );
         datErase( xloc, "CLUMPS", status );
      } else {
         ndfXnew( indf3, "CUPID", "CUPID_EXT", 0, NULL, &xloc, status );
      }

/* Retrieve any configuration parameters from the CUPID extension. */
      config = cupidRetrieveConfig( xloc );

/* Get the beam sizes from the CONFIG array in the CUPID extension. */
      mconfig = NULL;
      method = "FELLWALKER";
      astMapGet0A( config, method, &mconfig );

      if( !mconfig ) {
         method = "CLUMPFIND";
         astMapGet0A( config, method, &mconfig );
      }

      if( !mconfig ) {
         method = "REINHOLD";
         astMapGet0A( config, method, &mconfig );
      }

      if( !mconfig ) {
         method = "GAUSSCLUMPS";
         astMapGet0A( config, method, &mconfig );
      }

      if( mconfig ) {
         if( !astMapGet0D( (AstKeyMap *) mconfig, "FWHMBEAM", &fb ) ) fb = 2.0;
         if( !astMapGet0D( (AstKeyMap *) mconfig, "VELORES", &vr ) ) vr = 2.0;
      } else {
         method = "";
         fb = 2.0;
         vr = 2.0;
      }      

/* Allow the user to specify alternate values. */
      parDef0d( "FWHMBEAM", fb, status );
      parGet0d( "FWHMBEAM", &fb, status );
      beamcorr[ 0 ] = fb;
      beamcorr[ 1 ] = fb;

      if( ndim > 2 ) {
         parDef0d( "VELORES", vr, status );
         parGet0d( "VELORES", &vr, status );
         beamcorr[ 2 ] = vr;
      }

/* Store the clump properties in the CUPID extension and output catalogue
   (if needed). */
      ndfState( indf1, "WCS", &gotwcs, status );
      msgBlank( status );
      cupidStoreClumps( "OUTCAT", xloc, ndfs, nsig, beamcorr, 
                        "Output from CUPID:EXTRACTCLUMPS", 
                        gotwcs ? iwcs : NULL, 1 );

/* Map the output pixel assignment array. */
      ndfMap( indf3, "DATA", "_INTEGER", "WRITE", (void *) &ipa, &el, status );
      ndfSbad( 1, indf3, "DATA", status );

/* Allocate room for a mask holding bad values for points which are not 
   inside any clump. */
      rmask = astMalloc( sizeof( float )*(size_t) el );

/* Create the output data array by summing the contents of the NDFs describing 
   the  found and usable clumps. This also fills the above mask array. */
      cupidSumClumps( type, ipd, 1, nsig, slbnd, subnd, el, ndfs, 
                      rmask, ipa, method );

/* Delete any existing quality name information from the output NDF, and 
   create a structure to hold new quality name info. */
      irqDelet( indf3, status ); 
      irqNew( indf3, "CUPID", &qlocs, status );

/* Add in two quality names; "CLUMP"and "BACKGROUND". */
      irqAddqn( qlocs, "CLUMP", 0, "set iff a pixel is within a clump", 
                status );
      irqAddqn( qlocs, "BACKGROUND", 0, "set iff a pixel is not within a clump", 
                status );

/* Transfer the pixel mask to the NDF quality array. */
      irqSetqm( qlocs, 1, "BACKGROUND", el, rmask, &n, status );
      irqSetqm( qlocs, 0, "CLUMP", el, rmask, &n, status );

/* Release the quality name information. */
      rmask = astFree( rmask );
      irqRlse( &qlocs, status );

/* Relase the extension locator.*/
      datAnnul( &xloc, status );
   }

/* Release the HDS object containing the list of NDFs describing the clumps. */
   if( ndfs ) datAnnul( &ndfs, status );

/* Free other resources. */
   clbnd = astFree( clbnd );
   cubnd = astFree( cubnd );

/* End the NDF context */
   ndfEnd( status );

/* End the AST context */
   astEnd;

/* If an error has occurred, issue another error report identifying the 
   program which has failed (i.e. this one). */
   if( *status != SAI__OK ) {
      errRep( "EXTRACTCLUMPS_ERR", "EXTRACTCLUMPS: Failed to extract clumps "
              "of emission from a 1, 2 or 3-D NDF.", status );
   }

}

