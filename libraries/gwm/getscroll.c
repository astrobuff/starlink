/*
**
**  INCLUDE FILES
**
*/

#include <string.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include "gwm_err.h"
#include "gwm.h"

int GWM_GetScroll( Display *display, Window win_id, int *xoffset, int
	*yoffset)
/*
*+
*  Name :
*     GWM_GetScroll
*
*  Purpose :
*     Get scroll offset
*
*  Language :
*     C
*
*  Invocation :
*     status = GWM_GetScroll(display, win_id, &xoffset, &yoffset);
*
*  Description :
*     The values of the GWM_x_offset and GWM_y_offset properties are
*     fetched from the window.
*
*  Arguments :
*     display = *Display (given)
*        Display id
*     win_id = Window (given)
*        Window id
*     xoffset = int (returned)
*        Scroll offset in x
*     yoffset = int (returned)
*        Scroll offset in y
*
*  Authors :
*     DLT: David Terrett (Starlink RAL)
*     {enter_new_authors_here}
*
*  History :
*      9-JUL-1991 (DLT):
*        Orignal version
*     {enter_changes_here}
*
*  Bugs:
*     {note_any_bugs_here}
*-
*/
{
    int status;
    Atom atom, actual_type;
    int actual_format; 
    long *local_off;
    unsigned long nitems, bytes_after;
        
/*	  
**  x Offset
*/	  
    atom = XInternAtom(display, "GWM_x_offset", False );
    if (!atom) return GWM_NO_OFFSET;

    status = XGetWindowProperty( display, win_id , atom, 0, 1, False,
	XA_INTEGER, &actual_type, &actual_format, &nitems, &bytes_after,
	(unsigned char**)(&local_off));
    if ( status != Success || nitems == 0) return GWM_NO_OFFSET;
    
/*
**  Copy the offset value and release the storage 
*/
    *xoffset = (int)*local_off;
    XFree( (char*)local_off);

/*	  
**  y Offset
*/	  
    atom = XInternAtom(display, "GWM_y_offset", False );
    if (!atom) return GWM_NO_OFFSET;
    
    status = XGetWindowProperty( display, win_id , atom, 0, 1, False,
	XA_INTEGER, &actual_type, &actual_format, &nitems, &bytes_after,
	(unsigned char**)(&local_off));
    if ( status != Success || nitems == 0) return GWM_NO_OFFSET;
    
/*
**  Copy the offset value and release the storage 
*/
    *yoffset = (int)*local_off;
    XFree( (char*)local_off);

    return GWM_SUCCESS;
}
