#include "ast.h"
#include "atl.h"
#include "mers.h"
#include "sae_par.h"

AstFrameSet *atlFrameSetSplit( AstFrameSet *fset, const char *domain, int *status ){
/*
*+
*  Name:
*     atlFrameSetSplit

*  Purpose:
*     Extract axes from a supplied FrameSet to create a new FrameSet.

*  Invocation:
*     AstFrameSet *atlFrameSetSplit( AstFrameSet *fset, const char *domain,
*                                    int *status )

*  Description:
*     This function searches the current Frame of the supplied FrameSet
*     for axes that have a specified Domain. If any are found, and if they
*     correspond to a distinct subset of axes in the base Frame of the
*     supplied FrameSet (i.e. they are independent of the other axes), a new
*     FrameSet is created and returned in which the current Frame contains the
*     requested axes from the current Frame of the supplied FrameSet, and the
*     base Frame contains the corresponding axes from the base Frame of the
*     supplied FrameSet. If possible, any other Frames in the supplied
*     FrameSet are also split and added to the returned FrameSet.
*
*     If the search is unsuccessful, or if the required current Frame axes
*     are not independent of the other axes, then a NULL pointer is returned
*     but no error is reported.

*  Arguments:
*     fset
*        The FrameSet to be split.
*     domain
*        The Domain value for the required current Frame axes.
*     status
*        The global status.

*  Returned Value:
*     A pointer to a new FrameSet, or NULL if no axes with the required
*     Domain could be found, or if the required axes do not correspond
*     to a distinct set of base Frame axes, or if an error occurs.

*  Copyright:
*     Copyright (C) 2014 Science & Technology Facilities Council.
*     All Rights Reserved.

*  Licence:
*     This program is free software; you can redistribute it and/or
*     modify it under the terms of the GNU General Public License as
*     published by the Free Software Foundation; either version 2 of
*     the License, or (at your option) any later version.
*
*     This program is distributed in the hope that it will be
*     useful,but WITHOUT ANY WARRANTY; without even the implied
*     warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
*     PURPOSE. See the GNU General Public License for more details.
*
*     You should have received a copy of the GNU General Public License
*     along with this program; if not, write to the Free Software
*     Foundation, Inc., 51 Franklin Street,Fifth Floor, Boston, MA
*     02110-1301, USA

*  Authors:
*     DSB: David S. Berry (JAC, Hawaii)
*     {enter_new_authors_here}

*  History:
*     28-MAR-2014 (DSB):
*        Original version.
*     {enter_further_changes_here}
*-
*/


/* Local Variables: */
   AstFrame *bfrm_in;
   AstFrame *bfrm_out;
   AstFrame *cfrm_in;
   AstFrame *cfrm_out;
   AstFrame *ffrm;
   AstFrame *frm;
   AstFrameSet *result;
   AstMapping *bcmap;
   AstMapping *fmap;
   AstMapping *oldmap;
   char attrib[20];
   const char *axis_domain;
   int *baxes;
   int *caxes;
   int *faxes;
   int i;
   int ibase;
   int icurr;
   int iframe;
   int nax;
   int nb_in;
   int nb_out;
   int nc_in;
   int nc_out;
   int nframe;

/* Initialise returned values. */
   result = NULL;

/* Check the inherited status. */
   if( *status != SAI__OK ) return result;

/* Begin an AST context so that all AST objects created in this function
   are freed automatically when the context is ended. */
   astBegin;

/* Get a pointer to the current Frame in the supplied FrameSet, and get
   the number of axes it contains. */
   cfrm_in = astGetFrame( fset, AST__CURRENT );
   nc_in = astGetI( cfrm_in, "Naxes" );

/* Get a pointer to the base Frame in the supplied FrameSet, and get
   the number of axes it contains. */
   bfrm_in = astGetFrame( fset, AST__BASE );
   nb_in = astGetI( bfrm_in, "Naxes" );

/* Allocate arrays to hold the indicies of the axes that are to be used
   to create the returned FrameSet. */
   caxes = astMalloc( nc_in*sizeof( *caxes ) );
   baxes = astMalloc( nb_in*sizeof( *baxes ) );
   if( *status == SAI__OK ) {

/* Check each axis of the current Frame of the supplied FrameSet. If its
   Domain matches the required Domain, add its index to the above array. */
      nc_out = 0;
      for( i = 0; i < nc_in; i++ ) {
         sprintf( attrib, "Domain(%d)", i + 1 );
         axis_domain = astGetC( cfrm_in, attrib );
         if( axis_domain ) {
            if( !strcmp( axis_domain, domain ) ) caxes[ nc_out++ ] = i + 1;
         } else {
            break;
         }
      }

/* Check some axes were found. */
      if( nc_out > 0 ) {

/* Attempt to find a set of corresponding base Frame axes. We need to
   invert the FrameSet first since astMapSplit selects inputs, not outputs. */
         astInvert( fset );
         astMapSplit( fset, nc_out, caxes, baxes, &bcmap );

/* Invert the FrameSet again so that it is returned in its original state. */
         astInvert( fset );

/* Check the required axes are independent of the other axes. */
         if( bcmap ) {

/* The Mapping returned by astMapSplit above, goes from current Frame to
   base Frame, so invert it. */
            astInvert( bcmap );

/* Select the required axes from the current Frame of the supplied
   FrameSet. */
            cfrm_out = astPickAxes( cfrm_in, nc_out, caxes, NULL );

/* Select the required axes from the base Frame of the supplied
   FrameSet. */
            nb_out = astGetI( bcmap, "Nin" );
            bfrm_out = astPickAxes( bfrm_in, nb_out, baxes, NULL );

/* Initialise the returned FrameSet to hold the required base Frame axes. */
            result = astFrameSet( bfrm_out, " " );

/* Now attempt to add other Frames to the returned FrameSet. We leave the
   current Frame until last. */
            ibase = astGetI( fset, "Base" );
            icurr = astGetI( fset, "Current" );
            nframe = astGetI( fset, "NFrame" );
            for( iframe = 1; iframe <= nframe; iframe++ ) {

/* Skip the current and base Frames as they are dealt with separately. */
               if( iframe != ibase && iframe != icurr ) {

/* Get the Mapping from the base Frame to this Frame. */
                  oldmap = astGetMapping( fset, ibase, iframe );

/* An array to hold the indicies of the axes to pick from this Frame. */
                  faxes = astMalloc( astGetI( oldmap, "Nout" )*sizeof( *faxes ) );

/* See if the selected base Frame axes feed a distinct subset of axes in
   this Frame. If so, we can add this Frame into the returned FrameSet. */
                  astMapSplit( oldmap, nb_out, baxes, faxes, &fmap );
                  if( fmap ) {

/* Select the required axes from this Frame. */
                     frm = astGetFrame( fset, iframe );
                     nax = astGetI( fmap, "Nout" );
                     ffrm = astPickAxes( frm, nax, faxes, NULL );

/* Add the axes into the returned FrameSet. */
                     astAddFrame( result, ibase, fmap, ffrm );
                  }

                  faxes = astFree( faxes );
               }
            }

/* Finally add in the current Frame (which then remains the curent Frame
   because this is the last call to astAddFrame). */
            astAddFrame( result, ibase, bcmap, cfrm_out );
         }
      }
   }

/* Free resources. */
   caxes = astFree( caxes );
   baxes = astFree( baxes );

/* Export the result from the current AST context so the pointer is not
   annulled by the following call to astEnd. */
   if( result ) astExport( result );

/* End the AST context. This will annull all AST objects created in this
   function. */
   astEnd;

/* Return the FrameSet. */
   return result;
}

