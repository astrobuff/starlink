/*
*class++
*  Name:
*     Frame

*  Purpose:
*     Coordinate system description.

*  Constructor Function:
c     astFrame
f     AST_FRAME

*  Description:
*     This class is used to represent coordinate systems. It does this
*     in rather the same way that a frame around a graph describes the
*     coordinate space in which data are plotted. Consequently, a
*     Frame has a Title (string) attribute, which describes the
*     coordinate space, and contains axes which in turn hold
*     information such as Label and Units strings which are used for
*     labelling (e.g.) graphical output. In general, however, the
*     number of axes is not restricted to two.
*
*     Functions are available for converting Frame coordinate values
*     into a form suitable for display, and also for calculating
*     distances and offsets between positions within the Frame.
*
*     Frames may also contain knowledge of how to transform to and
*     from related coordinate systems.

*  Inheritance:
*     The Frame class inherits from the Mapping class.

*  Attributes:
*     In addition to those attributes common to all Mappings, every
*     Frame also has the following attributes:
*
*     - Digits/Digits(axis): Number of digits of precision
*     - Direction(axis): Display axis in conventional direction?
*     - Domain: Coordinate system domain
*     - Format(axis): Format specification for axis values
*     - Label(axis): Axis label
*     - MatchEnd: Match trailing axes?
*     - MaxAxes: Maximum number of Frame axes to match
*     - MinAxes: Minimum number of Frame axes to match
*     - Naxes: Number of Frame axes
*     - Permute: Permute axis order?
*     - PreserveAxes: Preserve axes?
*     - Symbol(axis): Axis symbol
*     - Title: Frame title
*     - Unit(axis): Axis physical units

*  Functions:
c     In addition to those functions applicable to all Mappings, the
c     following functions may also be applied to all Frames:
f     In addition to those routines applicable to all Mappings, the
f     following routines may also be applied to all Frames:
*
c     - astConvert: Determine how to convert between two coordinate systems
c     - astDistance: Calculate the distance between two points in a Frame
c     - astFindFrame: Find a coordinate system with specified characteristics
c     - astFormat: Format a coordinate value for a Frame axis
c     - astNorm: Normalise a set of Frame coordinates
c     - astOffset: Calculate an offset along a geodesic curve
c     - astPermAxes: Permute the order of a Frame's axes
c     - astPickAxes: Create a new Frame by picking axes from an existing one
c     - astUnformat: Read a formatted coordinate value for a Frame axis
f     - AST_CONVERT: Determine how to convert between two coordinate systems
f     - AST_DISTANCE: Calculate the distance between two points in a Frame
f     - AST_FINDFRAME: Find a coordinate system with specified characteristics
f     - AST_FORMAT: Format a coordinate value for a Frame axis
f     - AST_NORM: Normalise a set of Frame coordinates
f     - AST_OFFSET: Calculate an offset along a geodesic curve
f     - AST_PERMAXES: Permute the order of a Frame's axes
f     - AST_PICKAXES: Create a new Frame by picking axes from an existing one
f     - AST_UNFORMAT: Read a formatted coordinate value for a Frame axis

*  Notes:
*     - When used as a Mapping, a Frame implements a unit (null)
*     transformation in both the forward and inverse directions
*     (equivalent to a UnitMap). The Nin and Nout attribute values are
*     both equal to the number of Frame axes.

*  Copyright:
*     <COPYRIGHT_STATEMENT>

*  Authors:
*     RFWS: R.F. Warren-Smith (Starlink)
*     DSB: B.S. Berry (Starlink)

*  History:
*     1-MAR-1996 (RFWS):
*        Original version.
*     4-JUN-1996 (RFWS):
*        Added the CleanDomain function to fold all Domain strings to
*        upper case and remove white space.
*     12-JUL-1996 (RFWS):
*        Over-ride the astReportPoints method to provide
*        Frame-specific formatting.
*     11-SEP-1996 (RFWS):
*        Added astGap (written by DSB).
*     10-JUN-1997 (RFWS):
*        Re-implemented astConvert and astFindFrame.
*     1-SEP-1997 (RFWS):
*        Added missing return statement in astAbbrev_.
*     14-NOV-1997 (RFWS):
*        Fixed wrong amount of memory allocated in ValidateAxisSelection.
*     20-NOV-1997 (RFWS):
*        Updated astConvert prologue.
*     22-DEC-1997 (RFWS):
*        Updated astConvert prologue again.
*     15-FEB-1998 (RFWS):
*        Added astUnformat.
*     2-MAR-1999 (RFWS);
*        Fixed missing STATUS arguments in examples for AST_FINDFRAME
*        prologue.
*class--
*/

/* Module Macros. */
/* ============== */
/* Set the name of the class we are implementing. This indicates to
   the header files that define class interfaces that they should make
   "protected" symbols available. */
#define astCLASS Frame

/* Define numerical constants for use in this module. */
#define LABEL_BUFF_LEN 100       /* Max length of default axis Label string */
#define SYMBOL_BUFF_LEN 50       /* Max length of default axis Symbol string */
#define TITLE_BUFF_LEN 100       /* Max length of default title string */

/* Define macros to implement methods for accessing axis attributes. */
/*
*  Name:
*     MAKE_CLEAR

*  Purpose:
*     Implement a method to clear an attribute value for a Frame axis.

*  Type:
*     Private macro.

*  Synopsis:
*     #include "frame.h"
*     MAKE_CLEAR(attribute)

*  Class Membership:
*     Defined by the Frame class.

*  Description:
*     This macro expands to an implementation of a private member function of
*     the form:
*
*        static void Clear<Attribute>( AstFrame *this, int axis )
*
*     and an external interface function of the form:
*
*        void astClear<Attribute>_( AstFrame *this, int axis )
*
*     which implement a method for clearing an attribute value for a specified
*     axis of a Frame.

*  Parameters:
*     attribute
*        The name of the attribute to be cleared, as it appears in the
*        function name (e.g. Label in "astClearLabel").

*  Notes:
*     -  This macro assumes the existence of a method of the form:
*
*        void astClearAxis<Attribute>( AstAxis *this )
*
*     which clears the required attribute for an Axis object.
*     -  To avoid problems with some compilers, you should not leave any white
*     space around the macro arguments.
*/

/* Define the macro. */
#define MAKE_CLEAR(attribute) \
\
/* Private member function. */ \
/* ------------------------ */ \
static void Clear##attribute( AstFrame *this, int axis ) { \
   AstAxis *ax;                  /* Pointer to Axis object */ \
\
/* Check the global error status. */ \
   if ( !astOK ) return; \
\
/* Validate the axis index and obtain a pointer to the required Axis. */ \
   (void) astValidateAxis( this, axis, "astClear" #attribute ); \
   ax = astGetAxis( this, axis ); \
\
/* Clear the Axis attribute. */ \
   astClearAxis##attribute( ax ); \
\
/* Annul the Axis pointer. */ \
   ax = astAnnul( ax ); \
} \
\
/* External interface. */ \
/* ------------------- */ \
void astClear##attribute##_( AstFrame *this, int axis ) { \
\
/* Check the global error status. */ \
   if ( !astOK ) return; \
\
/* Invoke the required method via the virtual function table. */ \
   (**astMEMBER(this,Frame,Clear##attribute))( this, axis ); \
}

/*
*  Name:
*     MAKE_GET

*  Purpose:
*     Implement a method to get an attribute value for a Frame axis.

*  Type:
*     Private macro.

*  Synopsis:
#     #include "frame.h"
*     MAKE_GET(attribute,type,bad_value,default,assign_default)

*  Class Membership:
*     Defined by the Frame class.

*  Description:
*     This macro expands to an implementation of a private member function of
*     the form:
*
*        static <Type> Get<Attribute>( AstFrame *this, int axis )
*
*     and an external interface function of the form:
*
*        Type astGet<Attribute>_( AstFrame *this, int axis )
*
*     which implement a method for getting an attribute value for a specified
*     axis of a Frame.

*  Parameters:
*     attribute
*        The name of the attribute whose value is to be obtained, as
*        it appears in the function name (e.g. Label in
*        "astGetLabel").
*     type
*        The C type of the attribute.
*     bad_value
*        A constant value to return if the global error status is set, or if
*        the function fails.
*     default
*        A boolean (int) constant that indicates whether a new default value
*        should be returned by the method if the requested attribute has not
*        been set for the axis. If this value is zero, the axis default will
*        be used instead.
*     assign_default
*        An expression that evaluates to the new default value to be assigned.
*        This value is ignored if "default" is zero, but a valid (e.g.
*        constant) value should nevertheless be supplied.

*  Notes:
*     -  This macro assumes the existence of a method of the form:
*
*           <Type> astGetAxis<Attribute>( AstAxis *this )
*
*     which gets the required attribute for an Axis object.
*     -  To avoid problems with some compilers, you should not leave any white
*     space around the macro arguments.
*/

/* Define the macro. */
#define MAKE_GET(attribute,type,bad_value,default,assign_default) \
\
/* Private member function. */ \
/* ------------------------ */ \
static type Get##attribute( AstFrame *this, int axis ) { \
   AstAxis *ax;                  /* Pointer to Axis object */ \
   int digits_set;               /* Axis Digits attribute set? */ \
   int old_axis;                 /* Original (un-permuted) axis index */ \
   type result;                  /* Result to be returned */ \
\
/* Initialise. */ \
   result = (bad_value); \
\
/* Check the global error status. */ \
   if ( !astOK ) return result; \
\
/* Validate and permute the axis index and obtain a pointer to the required \
   Axis. */ \
   old_axis = axis; \
   axis = astValidateAxis( this, axis, "astGet" #attribute ); \
   ax = astGetAxis( this, old_axis ); \
\
/* Since the Axis is "managed" by the enclosing Frame, we next test if any \
   Axis attributes which may affect the result are undefined (i.e. have not \
   been explicitly set). If so, we over-ride them, giving them temporary \
   values dictated by the Frame. Only the Digits attribute is relevant \
   here. */ \
   digits_set = astTestAxisDigits( ax ); \
   if ( !digits_set ) astSetAxisDigits( ax, astGetDigits( this ) ); \
\
/* If the default value is to be over-ridden, test if the Axis attribute has \
   been set. Then, if required, obtain the attribute value from the Axis. */ \
   if ( !(default) || astTestAxis##attribute( ax ) ) { \
      result = astGetAxis##attribute( ax ); \
\
/* If required, assign the new default value. */ \
   } else { \
      result = (assign_default); \
   } \
\
/* Clear Axis attributes which were temporarily over-ridden above and annul \
   the Axis pointer. */ \
   if ( !digits_set ) astClearAxisDigits( ax ); \
   ax = astAnnul( ax ); \
\
/* If an error occurred, clear the result value. */ \
   if ( !astOK ) result = (bad_value); \
\
/* Return the result. */ \
   return result; \
} \
\
/* External interface. */ \
/* ------------------- */ \
type astGet##attribute##_( AstFrame *this, int axis ) { \
\
/* Check the global error status. */ \
   if ( !astOK ) return (bad_value); \
\
/* Invoke the required method via the virtual function table. */ \
   return (**astMEMBER(this,Frame,Get##attribute))( this, axis ); \
}

/*
*  Name:
*     MAKE_SET

*  Purpose:
*     Implement a method to set an attribute value for a Frame axis.

*  Type:
*     Private macro.

*  Synopsis:
*     #include "frame.h"
*     MAKE_SET(attribute,type)

*  Class Membership:
*     Defined by the Frame class.

*  Description:
*     This macro expands to an implementation of a private member function of
*     the form:
*
*        static void Set<Attribute>( AstFrame *this, int axis, <Type> value )
*
*     and an external interface function of the form:
*
*        void astSet<Attribute>_( AstFrame *this, int axis, <Type> value )
*
*     which implement a method for setting an attribute value for a specified
*     axis of a Frame.

*  Parameters:
*      attribute
*         The name of the attribute to be set, as it appears in the
*         function name (e.g. Label in "astSetLabel").
*      type
*         The C type of the attribute.

*  Notes:
*     -  This macro assumes the existence of a method of the form:
*
*           void astSetAxis<Attribute>( AstAxis *this, <Type> value )
*
*     which sets the required attribute for an Axis object.
*     -  To avoid problems with some compilers, you should not leave any white
*     space around the macro arguments.
*/

/* Define the macro. */
#define MAKE_SET(attribute,type) \
\
/* Private member function. */ \
/* ------------------------ */ \
static void Set##attribute( AstFrame *this, int axis, type value ) { \
   AstAxis *ax;                  /* Pointer to Axis object */ \
\
/* Check the global error status. */ \
   if ( !astOK ) return; \
\
/* Validate the axis index and obtain a pointer to the required Axis. */ \
   (void) astValidateAxis( this, axis, "astSet" #attribute ); \
   ax = astGetAxis( this, axis ); \
\
/* Set the Axis attribute value. */ \
   astSetAxis##attribute( ax, value ); \
\
/* Annul the Axis pointer. */ \
   ax = astAnnul( ax ); \
} \
\
/* External interface. */ \
/* ------------------- */ \
void astSet##attribute##_( AstFrame *this, int axis, type value ) { \
\
/* Check the global error status. */ \
   if ( !astOK ) return; \
\
/* Invoke the required method via the virtual function table. */ \
   (**astMEMBER(this,Frame,Set##attribute))( this, axis, value ); \
}

/*
*  Name:
*     MAKE_TEST

*  Purpose:
*     Implement a method to test if an attribute has been set for a Frame axis.

*  Type:
*     Private macro.

*  Synopsis:
*     #include "frame.h"
*     MAKE_TEST(attribute)

*  Class Membership:
*     Defined by the Frame class.

*  Description:
*     This macro expands to an implementation of a private member function of
*     the form:
*
*        static int Test<Attribute>( AstFrame *this, int axis )
*
*     and an external interface function of the form:
*
*        int astTest<Attribute>_( AstFrame *this, int axis )
*
*     which implement a method for testing if an attribute has been set for a
*     specified axis of a Frame.

*  Parameters:
*      attribute
*         The name of the attribute to be tested, as it appears in the
*         function name (e.g. Label in "astTestLabel").

*  Notes:
*     -  This macro assumes the existence of a method of the form:
*
*           void astTestAxis<Attribute>( AstAxis *this )
*
*     which tests the required attribute for an Axis object.
*     -  To avoid problems with some compilers, you should not leave any white
*     space around the macro arguments.
*/

/* Define the macro. */
#define MAKE_TEST(attribute) \
\
/* Private member function. */ \
/* ------------------------ */ \
static int Test##attribute( AstFrame *this, int axis ) { \
   AstAxis *ax;                  /* Pointer to Axis object */ \
   int result;                   /* Value to be returned */ \
\
/* Check the global error status. */ \
   if ( !astOK ) return 0; \
\
/* Validate the axis index and obtain a pointer to the required Axis. */ \
   (void) astValidateAxis( this, axis, "astTest" #attribute ); \
   ax = astGetAxis( this, axis ); \
\
/* Test if the attribute has been set. */ \
   result = astTestAxis##attribute( ax ); \
\
/* Annul the Axis pointer. */ \
   ax = astAnnul( ax ); \
\
/* If an error occurred, clear the result value. */ \
   if ( !astOK ) result = 0; \
\
/* Return the result. */ \
   return result; \
} \
\
/* External interface. */ \
/* ------------------- */ \
int astTest##attribute##_( AstFrame *this, int axis ) { \
\
/* Check the global error status. */ \
   if ( !astOK ) return 0; \
\
/* Invoke the required method via the virtual function table. */ \
   return (**astMEMBER(this,Frame,Test##attribute))( this, axis ); \
}

/* Header files. */
/* ============= */
/* Interface definitions. */
/* ---------------------- */
#include "error.h"               /* Error reporting facilities */
#include "memory.h"              /* Memory allocation facilities */
#include "object.h"              /* Base Object class */
#include "mapping.h"             /* Coordinate mappings (parent class) */
#include "pointset.h"            /* Sets of points */
#include "unitmap.h"             /* Unit Mapping */
#include "permmap.h"             /* Coordinate permutation Mapping */
#include "cmpmap.h"              /* Compound Mappings */
#include "axis.h"                /* Coordinate Axis */
#include "channel.h"             /* I/O channels */
#include "frame.h"               /* Interface definition for this class */

/* Error code definitions. */
/* ----------------------- */
#include "ast_err.h"             /* AST error codes */

/* C header files. */
/* --------------- */
#include <ctype.h>
#include <limits.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

/* Module Variables. */
/* ================= */
/* Define the class virtual function table and its initialisation flag as
   static variables. */
static AstFrameVtab class_vtab;  /* Virtual function table */
static int class_init = 0;       /* Virtual function table initialised? */

/* Pointers to parent class methods which are extended by this class. */
static const char *(* parent_getattrib)( AstObject *, const char * );
static int (* parent_testattrib)( AstObject *, const char * );
static void (* parent_clearattrib)( AstObject *, const char * );
static void (* parent_setattrib)( AstObject *, const char * );

/* Define other static variables. */
static char label_buff[ LABEL_BUFF_LEN + 1 ]; /* Default Label string buffer */
static char symbol_buff[ SYMBOL_BUFF_LEN + 1 ]; /* Default Symbol buffer */
static char title_buff[ TITLE_BUFF_LEN + 1 ]; /* Default Title string buffer */

/* Prototypes for Private Member Functions. */
/* ======================================== */
static AstAxis *GetAxis( AstFrame *, int );
static AstFrame *PickAxes( AstFrame *, int, const int[], AstMapping ** );
static AstFrameSet *Convert( AstFrame *, AstFrame *, const char * );
static AstFrameSet *ConvertX( AstFrame *, AstFrame *, const char * );
static AstFrameSet *FindFrame( AstFrame *, AstFrame *, const char * );
static AstPointSet *Transform( AstMapping *, AstPointSet *, int, AstPointSet * );
static char *CleanDomain( char * );
static const char *Abbrev( AstFrame *, int, const char *, const char * );
static const char *Format( AstFrame *, int, double );
static const char *GetAttrib( AstObject *, const char * );
static const char *GetDomain( AstFrame * );
static const char *GetFormat( AstFrame *, int );
static const char *GetLabel( AstFrame *, int );
static const char *GetSymbol( AstFrame *, int );
static const char *GetTitle( AstFrame * );
static const char *GetUnit( AstFrame *, int );
static const int *GetPerm( AstFrame * );
static double Distance( AstFrame *, const double[], const double[] );
static double Gap( AstFrame *, int, double, int * );
static int ConsistentMaxAxes( AstFrame *, int );
static int ConsistentMinAxes( AstFrame *, int );
static int DefaultMaxAxes( AstFrame * );
static int DefaultMinAxes( AstFrame * );
static int GetDigits( AstFrame * );
static int GetDirection( AstFrame *, int );
static int GetMatchEnd( AstFrame * );
static int GetMaxAxes( AstFrame * );
static int GetMinAxes( AstFrame * );
static int GetNaxes( AstFrame * );
static int GetNin( AstMapping * );
static int GetNout( AstMapping * );
static int GetPermute( AstFrame * );
static int GetPreserveAxes( AstFrame * );
static int Match( AstFrame *, AstFrame *, int **, int **, AstMapping **, AstFrame ** );
static int SubFrame( AstFrame *, AstFrame *, int, const int *, const int *, AstMapping **, AstFrame ** );
static int TestAttrib( AstObject *, const char * );
static int TestDigits( AstFrame * );
static int TestDirection( AstFrame *, int );
static int TestDomain( AstFrame * );
static int TestFormat( AstFrame *, int );
static int TestLabel( AstFrame *, int );
static int TestMatchEnd( AstFrame * );
static int TestMaxAxes( AstFrame * );
static int TestMinAxes( AstFrame * );
static int TestPermute( AstFrame * );
static int TestPreserveAxes( AstFrame * );
static int TestSymbol( AstFrame *, int );
static int TestTitle( AstFrame * );
static int TestUnit( AstFrame *, int );
static int Unformat( AstFrame *, int, const char *, double * );
static int ValidateAxis( AstFrame *, int, const char * );
static void AddUnderscores( char * );
static void CheckPerm( AstFrame *, const int *, const char * );
static void ClearAttrib( AstObject *, const char * );
static void ClearDigits( AstFrame * );
static void ClearDirection( AstFrame *, int );
static void ClearDomain( AstFrame * );
static void ClearFormat( AstFrame *, int );
static void ClearLabel( AstFrame *, int );
static void ClearMatchEnd( AstFrame * );
static void ClearMaxAxes( AstFrame * );
static void ClearMinAxes( AstFrame * );
static void ClearPermute( AstFrame * );
static void ClearPreserveAxes( AstFrame * );
static void ClearSymbol( AstFrame *, int );
static void ClearTitle( AstFrame * );
static void ClearUnit( AstFrame *, int );
static void Copy( const AstObject *, AstObject * );
static void Delete( AstObject * );
static void Dump( AstObject *, AstChannel * );
static void InitVtab( AstFrameVtab * );
static void Norm( AstFrame *, double[] );
static void Offset( AstFrame *this, const double[], const double[], double, double[] );
static void Overlay( AstFrame *, const int *, AstFrame * );
static void PermAxes( AstFrame *, const int[] );
static void PrimaryFrame( AstFrame *, int, AstFrame **, int * );
static void ReportPoints( AstMapping *, int, AstPointSet *, AstPointSet * );
static void SetAttrib( AstObject *, const char * );
static void SetAxis( AstFrame *, int, AstAxis * );
static void SetDigits( AstFrame *, int );
static void SetDirection( AstFrame *, int, int );
static void SetDomain( AstFrame *, const char * );
static void SetFormat( AstFrame *, int, const char * );
static void SetLabel( AstFrame *, int, const char * );
static void SetMatchEnd( AstFrame *, int );
static void SetMaxAxes( AstFrame *, int );
static void SetMinAxes( AstFrame *, int );
static void SetPermute( AstFrame *, int );
static void SetPreserveAxes( AstFrame *, int );
static void SetSymbol( AstFrame *, int, const char * );
static void SetTitle( AstFrame *, const char * );
static void SetUnit( AstFrame *, int, const char * );
static void ValidateAxisSelection( AstFrame *, int, const int *, const char * );

/* Member functions. */
/* ================= */
static const char *Abbrev( AstFrame *this, int axis,
                           const char *str1, const char *str2 ) {
/*
*+
*  Name:
*     astAbbrev

*  Purpose:
*     Abbreviate a formatted Frame axis value by skipping leading fields.

*  Type:
*     Protected virtual function.

*  Synopsis:
*     #include "frame.h"
*     const char *astAbbrev( AstFrame *this, int axis,
*                            const char *str1, const char *str2 )

*  Class Membership:
*     Frame method.

*  Description:
*     This function compares two Frame axis values that have been
*     formatted (using astFormat) and determines if they have any
*     redundant leading fields (i.e. leading fields in common which
*     can be suppressed when tabulating the values or plotting them on
*     the axis of a graph).

*  Parameters:
*     this
*        Pointer to the Frame.
*     axis
*        The number of the Frame axis for which the values have been
*        formatted (axis numbering starts at zero for the first axis).
*     str1
*        Pointer to a constant null-terminated string containing the
*        first formatted value.
*     str1
*        Pointer to a constant null-terminated string containing the
*        second formatted value.

*  Returned Value:
*     A pointer into the "str2" string which locates the first
*     character in the first field that differs between the two
*     formatted values.
*
*     If the two values have no leading fields in common, the returned
*     value will point at the start of string "str2". If the two
*     values are equal, it will point at the terminating null at the
*     end of this string.

*  Notes:
*     - This function assumes that the format specification used was
*     the same when both values were formatted and that they both
*     apply to the same Frame axis.
*     - A pointer to the start of "str2" will be returned if this
*     function is invoked with the global error status set, or if it
*     should fail for any reason.
*-
*/

/* Local Variables: */
   AstAxis *ax;                  /* Pointer to Axis object */
   const char *result;           /* Result pointer to return */

/* Initialise. */
   result = str2;

/* Check the global error status. */
   if ( !astOK ) return result;

/* Validate the axis index and obtain a pointer to the required
   Axis. */
   (void) astValidateAxis( this, axis, "astAbbrev" );
   ax = astGetAxis( this, axis );

/* Invoke the Axis astAxisAbbrev method to perform the processing. */
   result = astAxisAbbrev( ax, str1, str2 );

/* Annul the Axis pointer. */
   ax = astAnnul( ax );

/* If an error occurred, clear the returned value. */
   if ( !astOK ) result = str2;

/* Return the result. */
   return result;
}

static void AddUnderscores( char *string ) {
/*
*  Name:
*     AddUnderscores

*  Purpose:
*     Add underscores to a string in place of white space.

*  Type:
*     Private function.

*  Synopsis:
*     #include "frame.h"
*     void AddUnderscores( char *string )

*  Class Membership:
*     Frame member function.

*  Description:
*     This function changes all white space characters in a string into
*     the underscore character '_'.

*  Parameters:
*     this
*        Pointer to the Frame.
*     string
*        Pointer to the null terminated string to be processed.
*/

/* Local Variables. */
   int i;                        /* Loop counter for characters */

/* Inspect each character in the string. */
   for ( i = 0; string[ i ]; i++ ) {

/* If it is a white space character, replace it with an underscore. */
      if ( isspace( string[ i ] ) ) string[ i ] = '_';
   }
}

static void CheckPerm( AstFrame *this, const int *perm, const char *method ) {
/*
*+
*  Name:
*     astCheckPerm

*  Purpose:
*     Check that an array contains a valid permutation.

*  Type:
*     Protected virtual function.

*  Synopsis:
*     #include "frame.h"
*     void astCheckPerm( AstFrame *this, const int *perm, const char *method )

*  Class Membership:
*     Frame method.

*  Description:
*     This function checks the validity of a permutation array that
*     will be used to permute the order of a Frame's axes. If the
*     permutation specified by the array is not valid, an error is
*     reported and the global error status is set. Otherwise, the
*     function returns without further action.

*  Parameters:
*     this
*        Pointer to the Frame.
*     perm
*        Pointer to an array of integers with the same number of
*        elements as there are axes in the Frame. For each axis, the
*        corresponding integer gives the (zero based) axis index to be
*        used to identify the information for that axis (using the
*        un-permuted axis numbering). To be valid, the integers in
*        this array should therefore all lie in the range zero to
*        (naxes-1) inclusive, where "naxes" is the number of Frame
*        axes, and each value should occur exactly once.
*     method
*        Pointer to a constant null-terminated character string
*        containing the name of the method that invoked this function
*        to validate a permutation array. This method name is used
*        solely for constructing error messages.

*  Notes:
*     - Error messages issued by this function refer to the external
*     (public) numbering system used for axes (which is one-based),
*     whereas zero-based axis indices are used internally.
*-
*/

/* Local Variables: */
   int *there;                   /* Pointer to temporary array */
   int axis;                     /* Loop counter for axes */
   int naxes;                    /* Number of Frame axes */
   int valid;                    /* Permutation array is valid? */

/* Check the global error status. */
   if ( !astOK ) return;

/* Initialise. */
   valid = 1;

/* Obtain the number of Frame axes and allocate a temporary array of integers
   with the same number of elements. */
   naxes = astGetNaxes( this );
   there = astMalloc( sizeof( int ) * (size_t) naxes );
   if ( astOK ) {

/* Initialise the temporary array to zero. */
      for ( axis = 0; axis < naxes; axis++ ) there[ axis ] = 0;

/* Scan the permutation array, checking that each permuted axis index it
   contains is within the correct range. Note an error and quit checking
   if an invalid value is found. */
      for ( axis = 0; axis < naxes; axis++ ) {
         if ( ( perm[ axis ] < 0 ) || ( perm[ axis ] >= naxes ) ) {
            valid = 0;
            break;

/* Use the temporary array to count how many times each valid axis index
   occurs. */
	 } else {
            there[ perm[ axis ] ]++;
	 }
      }

/* If all the axis indices were within range, check to ensure that each value
   occurred only once. */
      if ( valid ) {
         for ( axis = 0; axis < naxes; axis++ ) {

/* Note an error and quit checking if any value did not occur exactly once. */
            if ( there[ axis ] != 1 ) {
               valid = 0;
               break;
	    }
	 }
      }
   }

/* Free the temporary array. */
   there = astFree( there );

/* If an invalid permutation was detected and no other error has
   occurred, then report an error (note we convert to one-based axis
   numbering in the error message). */
   if ( !valid && astOK ) {
      astError( AST__PRMIN, "%s(%s): Invalid axis permutation array.",
                method, astGetClass( this ) );
      astError( AST__PRMIN, "Each axis index should lie in the range 1 to %d "
                "and should occur only once.", naxes );
   }
}

static char *CleanDomain( char *domain ) {
/*
*  Name:
*     CleanDomain

*  Purpose:
*     Clean a Domain string and convert to upper case.

*  Type:
*     Private function.

*  Synopsis:
*     #include "frame.h"
*     char *CleanDomain( char *domain )

*  Class Membership:
*     Frame member function.

*  Description:
*     This function removes all white space from a string and converts
*     other characters to upper case. It is intended for cleaning up
*     values supplied for the Domain attribute of a Frame.

*  Parameters:
*     domain
*        Pointer to the null terminated Domain string to be modified.

*  Returned Value:
*     The pointer value "domain" is always returned (even under error
*     conditions).
*/

/* Local Variables: */
   int i;                        /* Loop counter for characters */
   int j;                        /* Good character count */

/* Check the global error status. */
   if ( !astOK ) return domain;

/* Eliminate white space characters and convert the rest to upper
   case. */
   for ( i = j = 0; domain[ i ]; i++ ) {
      if ( !isspace( domain[ i ] ) ) domain[ j++ ] = toupper( domain[ i ] );
   }
   domain[ j ] = '\0';

/* Return the string pointer. */
   return domain;
}

static void ClearAttrib( AstObject *this_object, const char *attrib ) {
/*
*  Name:
*     ClearAttrib

*  Purpose:
*     Clear an attribute value for a Frame.

*  Type:
*     Private function.

*  Synopsis:
*     #include "frame.h"
*     void ClearAttrib( AstObject *this, const char *attrib )

*  Class Membership:
*     Frame member function (over-rides the astClearAttrib protected
*     method inherited from the Mapping class).

*  Description:
*     This function clears the value of a specified attribute for a
*     Frame, so that the default value will subsequently be used.

*  Parameters:
*     this
*        Pointer to the Frame.
*     attrib
*        Pointer to a null terminated string specifying the attribute
*        name.  This should be in lower case with no surrounding white
*        space.

*  Notes:
*     - This function uses one-based axis numbering so that it is
*     suitable for external (public) use.
*/

/* Local Variables: */
   AstAxis *ax;                  /* Pointer to Axis */
   AstFrame *this;               /* Pointer to the Frame structure */
   char *axis_attrib;            /* Pointer to axis attribute name */
   int axis;                     /* Frame axis number */
   int axis_nc;                  /* No. characters in axis attribute name */
   int len;                      /* Length of attrib string */
   int nc;                       /* No. characters read by sscanf */

/* Check the global error status. */
   if ( !astOK ) return;

/* Obtain a pointer to the Frame structure. */
   this = (AstFrame *) this_object;

/* Obtain the length of the "attrib" string. */
   len = strlen( attrib );

/* Check the attribute name and clear the appropriate attribute. */

/* Digits. */
/* ------- */
   if ( !strcmp( attrib, "digits" ) ) {
      astClearDigits( this );

/* Digits(axis). */
/* ------------- */
   } else if ( nc = 0,
               ( 1 == sscanf( attrib, "digits(%d)%n", &axis, &nc ) )
               && ( nc >= len ) ) {

/* There is no function to clear the Digits attribute for an axis
   directly, so obtain a pointer to the Axis and use this to clear the
   attribute. */
      (void) astValidateAxis( this, axis - 1, "astClearDigits(axis)" );
      ax = astGetAxis( this, axis - 1 );
      astClearAxisDigits( ax );
      ax = astAnnul( ax );

/* Direction(axis). */
/* ---------------- */
   } else if ( nc = 0,
               ( 1 == sscanf( attrib, "direction(%d)%n", &axis, &nc ) )
               && ( nc >= len ) ) {
      astClearDirection( this, axis - 1 );

/* Domain. */
/* ------- */
   } else if ( !strcmp( attrib, "domain" ) ) {
      astClearDomain( this );

/* Format(axis). */
/* ------------- */
   } else if ( nc = 0,
               ( 1 == sscanf( attrib, "format(%d)%n", &axis, &nc ) )
               && ( nc >= len ) ) {
      astClearFormat( this, axis - 1 );

/* Label(axis). */
/* ------------ */
   } else if ( nc = 0,
               ( 1 == sscanf( attrib, "label(%d)%n", &axis, &nc ) )
               && ( nc >= len ) ) {
      astClearLabel( this, axis - 1 );

/* MatchEnd. */
/* --------- */
   } else if ( !strcmp( attrib, "matchend" ) ) {
      astClearMatchEnd( this );

/* MaxAxes. */
/* -------- */
   } else if ( !strcmp( attrib, "maxaxes" ) ) {
      astClearMaxAxes( this );

/* MinAxes. */
/* -------- */
   } else if ( !strcmp( attrib, "minaxes" ) ) {
      astClearMinAxes( this );

/* Permute. */
/* -------- */
   } else if ( !strcmp( attrib, "permute" ) ) {
      astClearPermute( this );

/* PreserveAxes. */
/* ------------- */
   } else if ( !strcmp( attrib, "preserveaxes" ) ) {
      astClearPreserveAxes( this );

/* Symbol(axis). */
/* ------------- */
   } else if ( nc = 0,
               ( 1 == sscanf( attrib, "symbol(%d)%n", &axis, &nc ) )
               && ( nc >= len ) ) {
      astClearSymbol( this, axis - 1 );

/* Title. */
/* ------ */
   } else if ( !strcmp( attrib, "title" ) ) {
      astClearTitle( this );

/* Unit(axis). */
/* ----------- */
   } else if ( nc = 0,
               ( 1 == sscanf( attrib, "unit(%d)%n", &axis, &nc ) )
               && ( nc >= len ) ) {
      astClearUnit( this, axis - 1 );

/* Read-only attributes. */
/* --------------------- */
/* Test if the attribute name matches any of the read-only attributes
   of this class. If it does, then report an error. */
   } else if ( !strcmp( attrib, "naxes" ) ) {
      astError( AST__NOWRT, "astClear: Invalid attempt to clear the \"%s\" "
                "value for a %s.", attrib, astGetClass( this ) );
      astError( AST__NOWRT, "This is a read-only attribute." );

/* Other axis attributes. */
/* ---------------------- */
/* If the attribute was not identified above, but appears to refer to
   a Frame axis, then it may refer to an Axis object of a derived type
   (which has additional attributes not recognised here). */
   } else if ( nc = 0,
               ( 1 == sscanf( attrib, "%*[^()]%n(%d)%n",
                                      &axis_nc, &axis, &nc ) )
               && ( nc >= len ) ) {

/* Validate the axis index and extract the attribute name. */
      (void) astValidateAxis( this, axis - 1, "astClear" );
      axis_attrib = astString( attrib, axis_nc );

/* Obtain a pointer to the Axis object and use its astClearAttrib method
   to clear the attribute value. */
      ax = astGetAxis( this, axis - 1 );
      astClearAttrib( ax, axis_attrib );

/* Annul the Axis pointer and free the memory holding the attribute
   name. */
      ax = astAnnul( ax );
      axis_attrib = astFree( axis_attrib );

/* Not recognised. */
/* --------------- */
/* If the attribute is still not recognised, pass it on to the parent
   method for further interpretation. */
   } else {
      (*parent_clearattrib)( this_object, attrib );
   }
}

static int ConsistentMaxAxes( AstFrame *this, int value ) {
/*
*  Name:
*     ConsistentMaxAxes

*  Purpose:
*     Ensure a consistent value when setting the MaxAxes attribute.

*  Type:
*     Private function.

*  Synopsis:
*     #include "frame.h"
*     int ConsistentMaxAxes( AstFrame *this, int value )

*  Class Membership:
*     Frame member function.

*  Description:
*     This function accepts a value which is to be set for a Frame's MaxAxes
*     attribute and returns an appropriately adjusted value to be assigned
*     to the Frame structure's max_axes component. If necessary, the Frame's
*     MinAxes attribute is adjusted to remain consistent with the new MaxAxes
*     value (but note that the MaxAxes value itself is not assigned by this
*     function).

*  Parameters:
*     this
*        Pointer to the Frame.
*     value
*        The new value being set for the MaxAxes attribute.

*  Returned Value:
*     The value to be assigned to the max_axes component.

*  Notes:
*     - A value of -INT_MAX will be returned if this function is
*     invoked with the global error status set, or if it should fail
*     for any reason.
*/

/* Local Variables: */
   int result;                   /* Result to be returned */

/* Check the global error status. */
   if ( !astOK ) return -INT_MAX;

/* Ensure that the result value isn't negative. */
   result = ( value >= 0 ) ? value : 0;

/* Check if the MinAxes attribute is set. If not, its default value will be
   consistent with the MaxAxes value (the DefaultMinAxes function ensures
   this). Otherwise, obtain its value to check for consistency. */
   if ( astTestMinAxes( this ) ) {

/* If necessary, set a new MinAxes value to prevent it exceeding the MaxAxes
   value about to be returned. */
      if ( astGetMinAxes( this ) > result ) astSetMinAxes( this, result );
   }

/* If an error occurred, clear the result value. */
   if ( !astOK ) result = -INT_MAX;

/* Return the result. */
   return result;
}

static int ConsistentMinAxes( AstFrame *this, int value ) {
/*
*  Name:
*     ConsistentMinAxes

*  Purpose:
*     Ensure a consistent value when setting the MinAxes attribute.

*  Type:
*     Private function.

*  Synopsis:
*     #include "frame.h"
*     int ConsistentMinAxes( AstFrame *this, int value )

*  Class Membership:
*     Frame member function.

*  Description:
*     This function accepts a value which is to be set for a Frame's MinAxes
*     attribute and returns an appropriately adjusted value to be assigned
*     to the Frame structure's min_axes component. If necessary, the Frame's
*     MaxAxes attribute is adjusted to remain consistent with the new MinAxes
*     value (but note that the MinAxes value itself is not assigned by this
*     function).

*  Parameters:
*     this
*        Pointer to the Frame.
*     value
*        The new value being set for the MinAxes attribute.

*  Returned Value:
*     The value to be assigned to the min_axes component.

*  Notes:
*     - A value of -INT_MAX will be returned if this function is
*     invoked with the global error status set, or if it should fail
*     for any reason.
*/

/* Local Variables: */
   int result;                   /* Result to be returned */

/* Check the global error status. */
   if ( !astOK ) return -INT_MAX;

/* Ensure that the result value isn't negative. */
   result = ( value >= 0 ) ? value : 0;

/* Check if the MaxAxes attribute is set. If not, its default value will be
   consistent with the MinAxes value (the DefaultMaxAxes function ensures
   this). Otherwise, obtain its value to check for consistency. */
   if ( astTestMaxAxes( this ) ) {

/* If necessary, set a new MaxAxes value to prevent it being less than the
   MinAxes value about to be returned. */
      if ( astGetMaxAxes( this ) < result ) astSetMaxAxes( this, result );
   }

/* If an error occurred, clear the result value. */
   if ( !astOK ) result = -INT_MAX;

/* Return the result. */
   return result;
}

static AstFrameSet *Convert( AstFrame *from, AstFrame *to,
                             const char *domainlist ) {
/*
*++
*  Name:
c     astConvert
f     AST_CONVERT

*  Purpose:
*     Determine how to convert between two coordinate systems.

*  Type:
*     Public virtual function.

*  Synopsis:
c     #include "frame.h"
c     AstFrameSet *astConvert( AstFrame *from, AstFrame *to,
c                              const char *domainlist )
f     RESULT = AST_CONVERT( FROM, TO, DOMAINLIST, STATUS )

*  Class Membership:
*     Frame method.

*  Description:
*     This function compares two Frames and determines whether it is
*     possible to convert between the coordinate systems which they
*     represent. If conversion is possible, it returns a FrameSet
*     which describes the conversion and which may be used (as a
*     Mapping) to transform coordinate values in either direction.
*
*     The same function may also be used to determine how to convert
*     between two FrameSets (or between a Frame and a FrameSet, or
*     vice versa). This mode is intended for use when (for example)
*     two images have been calibrated by attaching a FrameSet to each.
c     astConvert might then be used to search for a
f     AST_CONVERT might then be used to search for a
*     celestial coordinate system that both images have in common, and
*     the result could then be used to convert between the pixel
*     coordinates of both images -- having effectively used their
*     celestial coordinate systems to align them.
*
*     When using FrameSets, there may be more than one possible
*     intermediate coordinate system in which to perform the
*     conversion (for instance, two FrameSets might both have
*     celestial coordinates, detector coordinates, pixel coordinates,
*     etc.). A comma-separated list of coordinate system domains may
*     therefore be given which defines a priority order to use when
*     selecting the intermediate coordinate system.  The path used for
*     conversion must go via an intermediate coordinate system whose
*     Domain attribute matches one of the domains given. If conversion
*     cannot be achieved using the first domain, the next one is
*     considered, and so on, until success is achieved.

*  Parameters:
c     from
f     FROM = INTEGER (Given)
*        Pointer to a Frame which represents the "source" coordinate
*        system.  This is the coordinate system in which you already
*        have coordinates available.
*
*        If a FrameSet is given, its current Frame (as determined by
*        its Current attribute) is taken to describe the source
*        coordinate system. Note that the Base attribute of this
*        FrameSet may be modified by this function to indicate which
*        intermediate coordinate system was used (see under
*        "FrameSets" in the "Class Applicability" section for
*        details).
c     to
f     TO = INTEGER (Given)
*        Pointer to a Frame which represents the "destination"
*        coordinate system. This is the coordinate system into which
*        you wish to convert your coordinates.
*
*        If a FrameSet is given, its current Frame (as determined by
*        its Current attribute) is taken to describe the destination
*        coordinate system. Note that the Base attribute of this
*        FrameSet may be modified by this function to indicate which
*        intermediate coordinate system was used (see under
*        "FrameSets" in the "Class Applicability" section for
*        details).
c     domainlist
f     DOMAINLIST = CHARACTER * ( * ) (Given)
c        Pointer to a null-terminated character string containing a
f        A character string containing a
*        comma-separated list of Frame domains. This may be used to
*        define a priority order for the different intermediate
*        coordinate systems that might be used to perform the
*        conversion.
*
*        The function will first try to obtain a conversion by making
*        use only of an intermediate coordinate system whose Domain
*        attribute matches the first domain in this list. If this
*        fails, the second domain in the list will be used, and so on,
*        until conversion is achieved. A blank domain (e.g. two
*        consecutive commas) indicates that all coordinate systems
*        should be considered, regardless of their domains.
*
*        This list is case-insensitive and all white space is ignored.
*        If you do not wish to restrict the domain in this way,
c        you should supply an empty string. This is normally
f        you should supply a blank string. This is normally
*        appropriate if either of the source or destination coordinate
*        systems are described by Frames (rather than FrameSets),
*        since there is then usually only one possible choice of
*        intermediate coordinate system.
f     STATUS = INTEGER (Given and Returned)
f        The global status.

*  Returned Value:
c     astConvert()
f     AST_CONVERT = INTEGER
*        If the requested coordinate conversion is possible, the
*        function returns a pointer to a FrameSet which describes the
*        conversion. Otherwise, a null Object pointer (AST__NULL) is
*        returned without error.
*
*        If a FrameSet is returned, it will contain two Frames. Frame
*        number 1 (its base Frame) will describe the source coordinate
c        system, corresponding to the "from" parameter. Frame number 2
f        system, corresponding to the FROM argument. Frame number 2
*        (its current Frame) will describe the destination coordinate
c        system, corresponding to the "to" parameter. The Mapping
f        system, corresponding to the TO argument. The Mapping
*        which inter-relates these two Frames will perform the
*        required conversion between their respective coordinate
*        systems.
*
*        Note that a FrameSet may be used both as a Mapping and as a
*        Frame.  If the result is used as a Mapping (e.g. with
c        astTran2), then it provides a means of converting coordinates
f        AST_TRAN2), then it provides a means of converting coordinates
*        from the source to the destination coordinate system (or
*        vice versa if its inverse transformation is selected). If it
*        is used as a Frame, its attributes will describe the
*        destination coordinate system.

*  Applicability:
*     Frame
*        This function applies to all Frames.
*     FrameSet
c        If either of the "from" or "to" parameters is a pointer to a
f        If either of the FROM or TO arguments is a pointer to a
c        FrameSet, then astConvert will attempt to convert from the
f        FrameSet, then AST_CONVERT will attempt to convert from the
c        coordinate system described by the current Frame of the "from"
f        coordinate system described by the current Frame of the FROM
c        FrameSet to that described by the current Frame of the "to"
f        FrameSet to that described by the current Frame of the TO
*        FrameSet.
*
*        To achieve this, it will consider all of the Frames within
*        each FrameSet as a possible way of reaching an intermediate
*        coordinate system that can be used for the conversion. There
*        is then the possibility that more than one conversion path
*        may exist and, unless the choice is sufficiently restricted
c        by the "domainlist" string, the sequence in which the Frames
f        by the DOMAINLIST string, the sequence in which the Frames
*        are considered can be important. In this case, the search
*        for a conversion path proceeds as follows:
c        - Each field in the "domainlist" string is considered in turn.
f        - Each field in the DOMAINLIST string is considered in turn.
*        - The Frames within each FrameSet are considered in a
*        specific order: (1) the base Frame is always considered
*        first, (2) after this come all the other Frames in
*        Frame-index order (but omitting the base and current Frames),
*        (3) the current Frame is always considered last.  However, if
*        either FrameSet's Invert attribute is set to a non-zero value
*        (so that the FrameSet is inverted), then its Frames are
*        considered in reverse order. (Note that this still means that
*        the base Frame is considered first and the current Frame
*        last, because the Invert value will also cause these Frames
*        to swap places.)
*        - All source Frames are first considered (in the appropriate
*        order) for conversion to the first destination Frame. If no
*        suitable intermediate coordinate system emerges, they are
*        then considered again for conversion to the second
*        destination Frame (in the appropriate order), and so on.
*        - Generally, the first suitable intermediate coordinate
*        system found is used. However, the overall Mapping between
*        the source and destination coordinate systems is also
*        examined.  Preference is given to cases where both the
*        forward and inverse transformations are defined (as indicated
*        by the TranForward and TranInverse attributes). If only one
*        transformation is defined, the forward one is preferred.
*        - If the domain of the intermediate coordinate system matches
c        the current "domainlist" field, the conversion path is
f        the current DOMAINLIST field, the conversion path is
c        accepted. Otherwise, the next "domainlist" field is considered
f        accepted. Otherwise, the next DOMAINLIST field is considered
*        and the process repeated.
*
*        If conversion is possible, the Base attributes of the two
*        FrameSets will be modified on exit to identify the Frames
*        used to access the intermediate coordinate system which was
*        finally accepted.
*
*        Note that it is possible to force a particular Frame within a
*        FrameSet to be used as the basis for the intermediate
*        coordinate system, if it is suitable, by (a) focussing
*        attention on
c        it by specifying its domain in the "domainlist" string, or (b)
f        it by specifying its domain in the DOMAINLIST string, or (b)
*        making it the base Frame, since this is always considered
*        first.

*  Examples:
c     cvt = astConvert( a, b, "" );
f     CVT = AST_CONVERT( A, B, ' ', STATUS )
*        Attempts to convert between the coordinate systems represented
c        by "a" and "b" (assumed to be Frames). If successful, a FrameSet
f        by A and B (assumed to be Frames). If successful, a FrameSet
c        is returned via the "cvt" pointer which may be used to apply the
f        is returned via the CVT pointer which may be used to apply the
c        conversion to sets of coordinates (e.g. using astTran2).
f        conversion to sets of coordinates (e.g. using AST_TRAN2).
c     cvt = astConvert( astSkyFrame(""), astSkyFrame("Equinox=2005"), "" );
f     CVT = AST_CONVERT( AST_SKYFRAME( ' ', STATUS ), AST_SKYFRAME( 'Equinox=2005', STATUS ), ' ', STATUS )
*        Creates a FrameSet which describes precession in the default
*        FK5 celestial coordinate system between equinoxes J2000 (also
c        the default) and J2005. The returned "cvt" pointer may then
f        the default) and J2005. The returned CVT pointer may then
c        be passed to astTran2 to apply this precession correction to
f        be passed to AST_TRAN2 to apply this precession correction to
*        any number of coordinate values given in radians.
*
*        Note that the returned FrameSet also contains information
*        about how to format coordinate values. This means that
*        setting its Report attribute to 1 is a simple way to obtain
*        printed output (formatted in sexagesimal notation) to show
*        the coordinate values before and after conversion.
c     cvt = astConvert( a, b, "sky,detector," );
f     CVT = AST_CONVERT( A, B, 'SKY,DETECTOR,', STATUS )
*        Attempts to convert between the coordinate systems
c        represented by the current Frames of "a" and "b"
f        represented by the current Frames of A and B
*        (now assumed to be FrameSets), via the intermediate "SKY"
*        coordinate system.  This, by default, is the Domain
*        associated with a celestial coordinate system represented by
*        a SkyFrame.
*
*        If this fails (for example, because either FrameSet lacks
*        celestial coordinate information), then the user-defined
*        "DETECTOR" coordinate system is used instead. If this also
*        fails, then all other possible ways of achieving conversion
*        are considered before giving up.
*
c        The returned pointer "cvt" indicates whether conversion was
f        The returned pointer CVT indicates whether conversion was
*        possible and will have the value AST__NULL if it was not. If
c        conversion was possible, "cvt" will point at a new FrameSet
f        conversion was possible, CVT will point at a new FrameSet
*        describing the conversion.
*
*        The Base attributes of the two FrameSets 
c        will be set by astConvert to indicate which of their Frames was
f        will be set by AST_CONVERT to indicate which of their Frames was
*        used for the intermediate coordinate system. This means that
*        you can subsequently determine which coordinate system was
*        used by enquiring the Domain attribute of either base Frame.

*  Notes:
*     - When aligning (say) two images, which have been calibrated by
*     attaching FrameSets to them, it is usually necessary to convert
*     between the base Frames (representing "native" pixel
*     coordinates) of both FrameSets. This may be achieved by
*     inverting the FrameSets (e.g. using astInvert) so as to
*     interchange their base and current Frames before using
*     astConvert.
*     - A null Object pointer (AST__NULL) will be returned if this
c     function is invoked with the AST error status set, or if it
f     function is invoked with STATUS set to an error value, or if it
*     should fail for any reason.
*--

* Implementation Notes:
*    This function is simply a wrap-up for the protected astConvertX
*    method which performs the required processing but swaps the order
*    of the first two arguments. This is a trick to allow the
*    astConvert method to be over-ridden by derived classes on the
*    basis of the class of either of the first two arguments.
*/

/* Check the global error status. */
   if ( !astOK ) return NULL;

/* Invoke the "astConvertX" method with the first two arguments
   swapped. */
   return astConvertX( to, from, domainlist );
}

static AstFrameSet *ConvertX( AstFrame *to, AstFrame *from,
                              const char *domainlist ) {
/*
*+
*  Name:
*     astConvertX

*  Purpose:
*     Determine how to convert between two coordinate systems.

*  Type:
*     Protected virtual function.

*  Synopsis:
*     #include "frame.h"
*     AstFrameSet *astConvertX( AstFrame *to, AstFrame *from,
*                               const char *domainlist )

*  Class Membership:
*     Frame method.

*  Description:
*     This function performs the processing for the public astConvert
*     method and has exactly the same interface except that the order
*     of the first two arguments is swapped. This is a trick to allow
*     the astConvert method to be over-ridden by derived classes on
*     the basis of the class of either of its first two arguments.
*
*     See the astConvert method for details of the interface.
*-

*  Implementation Deficiencies:
*     - This function's job is basically to negotiate with two Frames
*     to try and find a mutually agreeable coordinate system for
*     conversion.  Ideally, it should be able to juggle the number of
*     axes, etc. to do this.  At present, however, the implementation
*     is much simpler. This is adequate for the Frame classes which
*     exist at the time of writing, but the implementation may need
*     beefing up in future.
*     - One likely problem is with attributes which default in both
*     the source and destination Frames. This means they also default
*     in the common coordinate system. If these default values were to
*     differ when matching different target Frames, however, we would
*     be in trouble, because the common coordinate system would not
*     then be remaining constant. The longer-term solution to this is
*     probably to provide some mechanism to "fix" all attribute values
*     for a Frame, by taking any attributes that are un-set and
*     explicitly setting a firm value (equal to the default) so they
*     cannot then change.
*/

/* Local Variables: */
   AstFrameSet *result;          /* Pointer to Mapping to be returned */
   AstMapping **map1_address;    /* Location of first Mapping pointer */
   AstMapping **map2_address;    /* Location of second Mapping pointer */
   AstMapping *common0;          /* Initial common coordinate system */
   AstMapping *common1;          /* Improved common coordinate system */
   AstMapping *common2;          /* Final common coordinate system */
   AstMapping *frame1;           /* Pointer to Frame for first match */
   AstMapping *frame2;           /* Pointer to Frame for second match */
   AstMapping *from_map;         /* Pointer to "from" Mapping */
   AstMapping *map;              /* Pointer to conversion Mapping */
   AstMapping *result_map;       /* Pointer to result Mapping */
   AstMapping *tmp;              /* Temporary Maping pointer */
   AstMapping *to_map;           /* Pointer to "to" Mapping */
   char *domain;                 /* Pointer to result domain */
   char *domain_end;             /* Pointer to null at end of domain */
   char *domainlist_copy;        /* Pointer to copy of domainlist */
   int *axes1;                   /* Pointer to axis assignments */
   int *axes2;                   /* Pointer to axis assignments */
   int best_score;               /* Score assigned to best match */
   int icom;                     /* Common coordinate system loop counter */
   int match1;                   /* First match succeeded? */
   int match2;                   /* Second match succeeded? */
   int match;                    /* Overall match found? */
   int perfect;                  /* Perfect match found? */
   int score;                    /* Score assigned to match */

/* Initialise. */
   result = NULL;

/* Check the global error status. */
   if ( !astOK ) return result;

/* Further initialisation. */
   result_map = NULL;

/* Make a temporary copy of the domains list. */
   domainlist_copy = astStore( NULL, domainlist,
                               strlen( domainlist ) + (size_t) 1 );
   if ( astOK ) {

/* Loop to inspect each comma-separated field in the domains list
   until an error occurs, all the domains are used up, or a match is
   found. */
      domain = domainlist_copy;
      match = 0;
      while ( astOK && domain && !match ) {

/* Change the comma at the end of each field to a null to terminate
   the domain. Then convert the domain to upper case and eliminate
   white space. */
         if ( ( domain_end = strchr( domain, ',' ) ) ) *domain_end = '\0';
         CleanDomain( domain );

/* For any given domain, we will ignore imperfect matches in favour of
   better ones by assigning a score to each match. Initialise the best
   score value for the current domain. */
         best_score = -1;

/* Loop to consider both the "to" and "from" Frames in turn as the
   basis of a possible common coordinate system. Quit looping early if
   an error occurs or a perfect match is found. */
         perfect = 0;
         for ( icom = 0; astOK && !perfect && ( icom <= 1 ); icom++ ) {

/* Make a copy of the Frame representing the initial guess at a common
   coordinate system. We will use this to probe the other
   Frame. Ensure that axes are not preserved (so that we convert to
   the common axis number/order). */
            common0 = astCopy( icom ? from : to );
            astSetPreserveAxes( common0, 0 );

/* Also, if the current domain is not blank, set the Domain attribute (so
   we will only find coordinate systems which match the current
   "domainlist" field). */
            if ( *domain ) astSetDomain( common0, domain );

/* Obtain a pointer to the other Frame. */
            frame1 = astClone( icom ? to : from );

/* Set the address at which to store the resulting Mapping pointer. */
            map1_address = icom ? &to_map : &from_map;

/* See if conversion from "frame1" to the common coordinate system is
   possible.  If successful, this results in a new approximation
   ("common1") to the possible common coordinate system. */
            match1 = astMatch( common0, frame1, &axes1, &axes2,
                               map1_address, &common1 );

/* If successful, free memory allocated for the axis association
   arrays, which are not needed. */
            if ( astOK && match1 ) {
               axes1 = astFree( axes1 );
               axes2 = astFree( axes2 );

/* Using the improved approximation to the common coordinate system,
   now test if conversion from the alternative Frame "frame2" is
   possible. */
               frame2 = astClone( icom ? from : to );
               map2_address = icom ? &from_map : &to_map;
               astSetPreserveAxes( common1, 0 );
               match2 = astMatch( common1, frame2, &axes1, &axes2,
                                  map2_address, &common2 );

/* If successful, free memory allocated for the axis association
   arrays, which are not needed. */
               if ( astOK && match2 ) {
                  axes1 = astFree( axes1 );
                  axes2 = astFree( axes2 );

/* Invert the "to" Mapping and concatenate the two Mappings to
   describe the conversion between the "from" and "to" Frames. Then
   simplify the result. */
                  astInvert( to_map );
                  tmp = (AstMapping *)
                        astCmpMap( from_map, to_map, 1, "" );
                  map = astSimplify( tmp );
                  tmp = astAnnul( tmp );

/* Assign a score that favours Mappings with both transformations
   available over those with only one, and Mappings with only a
   forward transformation over those with only an inverse
   transformation. */
                  score = ( astGetTranForward( map ) ? 2 : 0 ) +
                          ( astGetTranInverse( map ) ? 1 : 0 );

/* If the new score is better than the previous one (or is the first
   one), note that we have a possible match. */
                  if ( astOK && ( score > best_score ) ) {
                     match = 1;

/* Update the best score and note if it indicates a perfect match (in
   which case we can stop searching at this point). */
                     best_score = score;
                     perfect = ( best_score >= 3 );

/* Annul any previous result Mapping pointer and replace it with this
   better one. */
                     if ( result_map ) result_map = astAnnul( result_map );
                     result_map = astClone( map );
                  }

/* Annul pointers to all the intermediate Objects. */
                  map = astAnnul( map );
                  *map2_address = astAnnul( *map2_address );
                  common2 = astAnnul( common2 );
               }
               *map1_address = astAnnul( *map1_address );
               common1 = astAnnul( common1 );
            }
            common0 = astAnnul( common0 );
         }

/* Go on to consider the next field in the domains list. */
         domain = domain_end ? domain_end + 1 : NULL;
      }
   }

/* Free the domain list copy. */
   domainlist_copy = astFree( domainlist_copy );

/* If returning a result, build the result FrameSet. Then annul the
   result Mapping pointer. */
   if ( result_map ) {
      result = astFrameSet( from, "" );
      astAddFrame( result, AST__BASE, result_map, to );
      result_map = astAnnul( result_map );
   }

/* If an error occurred, annul the result FrameSet pointer. */
   if ( !astOK ) result = astAnnul( result );

/* Return the result. */
   return result;
}

static int DefaultMaxAxes( AstFrame *this ) {
/*
*  Name:
*     DefaultMaxAxes

*  Purpose:
*     Obtain the MaxAxes attribute from a Frame structure, with defaulting.

*  Type:
*     Private function.

*  Synopsis:
*     #include "frame.h"
*     int DefaultMaxAxes( AstFrame *this ) {

*  Class Membership:
*     Frame member function.

*  Description:
*     This function inspects the max_axes component of a Frame structure and
*     derives a value for the MaxAxes attribute. If the component's value
*     indicates that the attribute has not been set, a suitable default is
*     returned which is consistent with the state of the Frames's MinAxes
*     attribute.

*  Parameters:
*     this
*        Pointer to the Frame.

*  Returned Value:
*     The value to be used for the MaxAxes attribute.
*/

/* Local Variables. */
   int result;                   /* Result to be returned */
   int min_axes;                 /* Value of MinAxes attribute */

/* Check the global error status. */
   if ( !astOK ) return 0;

/* If the Frame's max_axes component is set, return its value. */
   if ( this->max_axes != -INT_MAX ) {
      result = this->max_axes;

/* Otherwise, the preferred default value is the number of Frame axes. */
   } else {
      result = astGetNaxes( this );

/* Before returning this value, check if the MinAxes attribute is set. If it
   is, obtain its value. */
      if ( astTestMinAxes( this ) ) {
         min_axes = astGetMinAxes( this );

/* If necessary, increase the MaxAxes default value so that it is not less than
   MinAxes. */
         if ( result < min_axes ) result = min_axes;
      }
   }

/* If an error occurred, clear the result value. */
   if ( !astOK ) result = 0;

/* Return the result. */
   return result;
}

static int DefaultMinAxes( AstFrame *this ) {
/*
*  Name:
*     DefaultMinAxes

*  Purpose:
*     Obtain the MinAxes attribute from a Frame structure, with defaulting.

*  Type:
*     Private function.

*  Synopsis:
*     #include "frame.h"
*     int DefaultMinAxes( AstFrame *this ) {

*  Class Membership:
*     Frame member function.

*  Description:
*     This function inspects the min_axes component of a Frame structure and
*     derives a value for the MinAxes attribute. If the component's value
*     indicates that the attribute has not been set, a suitable default is
*     returned which is consistent with the state of the Frames's MaxAxes
*     attribute.

*  Parameters:
*     this
*        Pointer to the Frame.

*  Returned Value:
*     The value to be used for the MinAxes attribute.
*/

/* Local Variables: */
   int result;                   /* Result to be returned */
   int max_axes;                 /* Value of MaxAxes attribute */

/* Check the global error status. */
   if ( !astOK ) return 0;

/* If the Frame's min_axes component is set, return its value. */
   if ( this->min_axes != -INT_MAX ) {
      result = this->min_axes;

/* Otherwise, the preferred default value is the number of Frame axes. */
   } else {
      result = astGetNaxes( this );

/* Before returning this value, check if the MaxAxes attribute is set. If it
   is, obtain its value. */
      if ( astTestMaxAxes( this ) ) {
         max_axes = astGetMaxAxes( this );

/* If necessary, reduce the MinAxes default value so that it does not exceed
   MaxAxes. */
         if ( result > max_axes ) result = max_axes;
      }
   }

/* If an error occurred, clear the result value. */
   if ( !astOK ) result = 0;

/* Return the result. */
   return result;
}

static double Distance( AstFrame *this,
                        const double point1[], const double point2[] ) {
/*
*++
*  Name:
c     astDistance
f     AST_DISTANCE

*  Purpose:
*     Calculate the distance between two points in a Frame.

*  Type:
*     Public virtual function.

*  Synopsis:
c     #include "frame.h"
c     double astDistance( AstFrame *this,
c                         const double point1[], const double point2[] )
f     RESULT = AST_DISTANCE( THIS, POINT1, POINT2, STATUS )

*  Class Membership:
*     Frame method.

*  Description:
*     This function finds the distance between two points whose Frame
*     coordinates are given. The distance calculated is that along
*     the geodesic curve that joins the two points.
*
*     For example, in a basic Frame, the distance calculated will be
*     the Cartesian distance along the straight line joining the two
*     points. For a more specialised Frame describing a sky coordinate
*     system, however, it would be the distance along the great circle
*     passing through two sky positions.

*  Parameters:
c     this
f     THIS = INTEGER (Given)
*        Pointer to the Frame.
c     point1
f     POINT1( * ) = DOUBLE PRECISION (Given)
c        An array of double, with one element for each Frame axis
f        An array with one element for each Frame axis
*        (Naxes attribute) containing the coordinates of the first point.
c     point2
f     POINT2( * ) = DOUBLE PRECISION (Given)
c        An array of double, with one element for each Frame axis
f        An array with one element for each Frame axis
*        containing the coordinates of the second point.
f     STATUS = INTEGER (Given and Returned)
f        The global status.

*  Returned Value:
c     astDistance
f     AST_DISTANCE = DOUBLE PRECISION
*        The distance between the two points.

*  Notes:
*     - This function will return a "bad" result value (AST__BAD) if
*     any of the input coordinates has this value.
*     - A "bad" value will also be returned if this function is
c     invoked with the AST error status set, or if it should fail for
f     invoked with STATUS set to an error value, or if it should fail for
*     any reason.
*--
*/

/* Local Variables: */
   double delta;                 /* Separation along an axis */
   double result;                /* Result value to return */
   int axis;                     /* Loop counter for axes */
   int naxes;                    /* Number of Frame axes */

/* Initialise. */
   result = AST__BAD;

/* Check the global error status. */
   if ( !astOK ) return result;

/* Obtain the number of Frame axes. */
   naxes = astGetNaxes( this );
   if ( astOK ) {

/* Loop to determine the Cartesian distance between the points. */
      result = 0.0;
      for ( axis = 0; axis < naxes; axis++ ) {

/* If any of the coordinates supplied is bad, set the distance to be
   bad and quit looping. */
         if ( ( point1[ axis ] == AST__BAD ) ||
              ( point2[ axis ] == AST__BAD ) ) {
            result = AST__BAD;
            break;

/* Otherwise, accumulate the sum of squared separations along each
   axis. */
         } else {
            delta = point1[ axis ] - point2[ axis ];
            result += ( delta * delta );
         }
      }

/* Take the square root to find the distance (if valid). */
      if ( result != AST__BAD ) result = sqrt( result );
   }

/* Return the result. */
   return result;
}

static AstFrameSet *FindFrame( AstFrame *target, AstFrame *template,
                               const char *domainlist ) {
/*
*++
*  Name:
c     astFindFrame
f     AST_FINDFRAME

*  Purpose:
*     Find a coordinate system with specified characteristics.

*  Type:
*     Public virtual function.

*  Synopsis:
c     #include "frame.h"
c     AstFrameSet *astFindFrame( AstFrame *target, AstFrame *template,
c                                const char *domainlist )
f     RESULT = AST_FINDFRAME( TARGET, TEMPLATE, DOMAINLIST, STATUS )

*  Class Membership:
*     Frame method.

*  Description:
*     This function uses a "template" Frame to search another Frame
*     (or FrameSet) to identify a coordinate system which has a
*     specified set of characteristics. If a suitable coordinate
*     system can be found, the function returns a pointer to a
*     FrameSet which describes the required coordinate system and how
*     to convert coordinates to and from it.
*
*     This function is provided to help answer general questions about
*     coordinate systems, such as typically arise when coordinate
*     information is imported into a program as part of an initially
*     unknown dataset. For example:
*     - Is there a wavelength scale?
*     - Is there a 2-dimensional coordinate system?
*     - Is there a celestial coordinate system?
*     - Can I plot the data in ecliptic coordinates?
*
*     You can also use this function as a means of reconciling a
*     user's preference for a particular coordinate system (for
*     example, what type of axes to draw) with what is actually
*     possible given the coordinate information available.
*
*     To perform a search, you supply a "target" Frame (or FrameSet)
*     which represents the set of coordinate systems to be searched.
*     If a basic Frame is given as the target, this set of coordinate
*     systems consists of the one described by this Frame, plus all
*     other "virtual" coordinate systems which can potentially be
*     reached from it by applying built-in conversions (for example,
*     any of the celestial coordinate conversions known to the AST
*     library would constitute a "built-in" conversion). If a FrameSet
*     is given as the target, the set of coordinate systems to be
*     searched consists of the union of those represented by all the
*     individual Frames within it.
*
*     To select from this large set of possible coordinate systems,
*     you supply a "template" Frame which is an instance of the type
*     of Frame you are looking for. Effectively, you then ask the
*     function to "find a coordinate system that looks like this".
*
*     You can make your request more or less specific by setting
*     attribute values for the template Frame. If a particular
*     attribute is set in the template, then the function will only
*     find coordinate systems which have exactly the same value for
*     that attribute.  If you leave a template attribute un-set,
*     however, then the function has discretion about the value the
*     attribute should have in any coordinate system it finds. The
*     attribute will then take its value from one of the actual
*     (rather than virtual) coordinate systems in the target. If the
*     target is a FrameSet, its Current attribute will be modified to
*     indicate which of its Frames was used for this purpose.
*
*     The result of this process is a coordinate system represented by
*     a hybrid Frame which acquires some attributes from the template
*     (but only if they were set) and the remainder from the
*     target. This represents the "best compromise" between what you
*     asked for and what was available. A Mapping is then generated
*     which converts from the target coordinate system to this hybrid
*     one, and the returned FrameSet encapsulates all of this
*     information.

*  Parameters:
c     target
f     TARGET = INTEGER (Given)
*        Pointer to the target Frame (or FrameSet).
*
*        Note that if a FrameSet is supplied (and a suitable
*        coordinate system is found), then its Current attribute will
*        be modified to indicate which Frame was used to obtain
*        attribute values which were not specified by the template.
*        This Frame will, in some sense, represent the "closest"
*        non-virtual coordinate system to the one you requested.
c     template
f     TEMPLATE = INTEGER (Given)
*        Pointer to the template Frame, which should be an instance of
*        the type of Frame you wish to find. If you wanted to find a
*        Frame describing a celestial coordinate system, for example,
*        then you might use a SkyFrame here. See the "Examples"
*        section for more ideas.
c     domainlist
f     DOMAINLIST = CHARACTER * ( * ) (Given)
c        Pointer to a null-terminated character string containing a
f        A character string containing a
*        comma-separated list of Frame domains. This may be used to
*        establish a priority order for the different types of
*        coordinate system that might be found.
*
*        The function will first try to find a suitable coordinate
*        system whose Domain attribute equals the first domain in this
*        list. If this fails, the second domain in the list will be
*        used, and so on, until a result is obtained. A blank domain
*        (e.g. two consecutive commas) indicates that any coordinate
*        system is acceptable (subject to the template) regardless of
*        its domain.
*
*        This list is case-insensitive and all white space is ignored.
*        If you do not wish to restrict the domain in this way,
c        you should supply an empty string.
f        you should supply a blank string.
f     STATUS = INTEGER (Given and Returned)
f        The global status.

*  Returned Value:
c     astFindFrame()
f     AST_FINDFRAME = INTEGER
*        If the search is successful, the function returns a pointer
*        to a FrameSet which contains the Frame found and a
*        description of how to convert to (and from) the coordinate
*        system it represents. Otherwise, a null Object pointer
*        (AST__NULL) is returned without error.
*
*        If a FrameSet is returned, it will contain two Frames. Frame
*        number 1 (its base Frame) represents the target coordinate
*        system and will be the same as the (base Frame of the)
*        target. Frame number 2 (its current Frame) will be a Frame
*        representing the coordinate system which the function
*        found. The Mapping which inter-relates these two Frames will
*        describe how to convert between their respective coordinate
*        systems.
*
*        Note that a FrameSet may be used both as a Mapping and as a
*        Frame. If the result is used as a Mapping (e.g. with
*        astTran2), then it provides a means of converting coordinates
*        from the target coordinate system into the new coordinate
*        system that was found (and vice versa if its inverse
*        transformation is selected). If it is used as a Frame, its
*        attributes will describe the new coordinate system.

*  Applicability:
*     Frame
*        This function applies to all Frames.
*     FrameSet
*        If the target is a FrameSet, the possibility exists that
*        several of the Frames within it might be matched by the
*        template.  Unless the choice is sufficiently restricted by
c        the "domainlist" string, the sequence in which Frames are
f        the DOMAINLIST string, the sequence in which Frames are
*        searched can then become important. In this case, the search
*        proceeds as follows:
c        - Each field in the "domainlist" string is considered in turn.
f        - Each field in the DOMAINLIST string is considered in turn.
*        - An attempt is made to match the template to each of the
*        target's Frames in the order: (1) the current Frame, (2) the
*        base Frame, (3) each remaining Frame in the order of being
*        added to the target FrameSet.
*        - Generally, the first match found is used. However, the
*        Mapping between the target coordinate system and the
*        resulting Frame is also examined. Preference is given to
*        cases where both the forward and inverse transformations are
*        defined (as indicated by the TranForward and TranInverse
*        attributes). If only one transformation is defined, the
*        forward one is preferred.
*        - If a match is found and the domain of the resulting Frame also
c        matches the current "domainlist" field, it is
f        matches the current DOMAINLIST field, it is
c        accepted. Otherwise, the next "domainlist" field is considered
f        accepted. Otherwise, the next DOMAINLIST field is considered
*        and the process repeated.
*
*        If a suitable coordinate system is found, the Current
*        attribute of the target FrameSet will be modified on exit to
*        identify the Frame whose match with the target was eventually
*        accepted.

*  Examples:
c     result = astFindFrame( target, astFrame( 3, "" ), "" );
f     RESULT = AST_FINDFRAME( TARGET, AST_FRAME( 3, ' ', STATUS ), ' ', STATUS )
*        Searches for a 3-dimensional coordinate system in the target
*        Frame (or FrameSet). No attributes have been set in the
c        template Frame (created by astFrame), so no restriction has
f        template Frame (created by AST_FRAME), so no restriction has
*        been placed on the required coordinate system, other than
*        that it should have 3 dimensions. The first suitable Frame
c        found will be returned as part of the "result" FrameSet.
f        found will be returned as part of the RESULT FrameSet.
c     result = astFindFrame( target, astSkyFrame( "" ), "" );
f     RESULT = AST_FINDFRAME( TARGET, AST_SKYFRAME( ' ', STATUS ), ' ', STATUS )
*        Searches for a celestial coordinate system in the target
*        Frame (or FrameSet). The type of celestial coordinate system
c        is unspecified, so astFindFrame will return the first one
f        is unspecified, so AST_FINDFRAME will return the first one
c        found as part of the "result" FrameSet. If the target is
f        found as part of the RESULT FrameSet. If the target is
*        a FrameSet, then its Current attribute will be updated to
*        identify the Frame that was used.
*
*        If no celestial coordinate system can be found, a value of
*        AST__NULL will be returned without error.
c     result = astFindFrame( target, astSkyFrame( "System=FK5" ), "" );
f     RESULT = AST_FINDFRAME( TARGET, AST_SKYFRAME( 'System=FK5', STATUS ), ' ', STATUS )
*        Searches for an equatorial (FK5) coordinate system in the
*        target. The Equinox value for the coordinate system has not
*        been specified, so will be obtained from the target. If the
*        target is a FrameSet, its Current attribute will be updated
*        to indicate which SkyFrame was used to obtain this value.
c     result = astFindFrame( target, astFrame( 2, "" ), "sky,pixel," );
f     RESULT = AST_FINDFRAME( TARGET, AST_FRAME( 2, ' ', STATUS ), 'SKY,PIXEL,', STATUS )
*        Searches for a 2-dimensional coordinate system in the
*        target. Initially, a search is made for a suitable coordinate
*        system whose Domain attribute has the value "SKY". If this
*        search fails, a search is then made for one with the domain
*        "PIXEL". If this also fails, then any 2-dimensional
c        coordinate system is returned as part of the "result"
f        coordinate system is returned as part of the RESULT
*        FrameSet.
*
*        Only if no 2-dimensional coordinate systems can be reached by
*        applying built-in conversions to any of the Frames in the
*        target will a value of AST__NULL be returned.
c     result = astFindFrame( target, astFrame( 1, "Domain=WAVELENGTH" ), "" );
f     RESULT = AST_FINDFRAME( TARGET, AST_FRAME( 1, 'Domain=WAVELENGTH', STATUS ), ' ', STATUS )
*        Searches for any 1-dimensional coordinate system in the
*        target which has the domain "WAVELENGTH".
c     result = astFindFrame( target, astFrame( 1, "" ), "wavelength" );
f     RESULT = AST_FINDFRAME( TARGET, AST_FRAME( 1, ' ', STATUS ), 'WAVELENGTH', STATUS )
*        This example has exactly the same effect as that above. It
*        illustrates the equivalence of the template's Domain attribute
c        and the fields in the "domainlist" string.
f        and the fields in the DOMAINLIST string.
c     result = astFindFrame( target, astFrame( 1, "MaxAxes=3" ), "" );
f     RESULT = AST_FINDFRAME( TARGET, AST_FRAME( 1, 'MaxAxes=3', STATUS ), ' ', STATUS )
*        This is a more advanced example which will search for any
*        coordinate system in the target having 1, 2 or 3
c        dimensions. The Frame returned (as part of the "result"
f        dimensions. The Frame returned (as part of the RESULT
*        FrameSet) will always be 1-dimensional, but will be related
*        to the coordinate system that was found by a suitable Mapping
*        (e.g. a PermMap) which simply extracts the first axis.
*
*        If we had wanted a Frame representing the actual (1, 2 or
*        3-dimensional) coordinate system found, we could set the
*        PreserveAxes attribute to a non-zero value in the template.
c     result = astFindFrame( target, astSkyFrame( "Permute=0" ), "" );
f     RESULT = AST_FINDFRAME( TARGET, AST_SKYFRAME( 'Permute=0', STATUS ), ' ', STATUS )
*        Searches for any celestial coordinate system in the target,
*        but only finds one if its axes are in the conventional
*        (longitude,latitude) order and have not been permuted
c        (e.g. with astPermAxes).
f        (e.g. with AST_PERMAXES).

*  Notes:
*     - Beware of setting the Domain attribute of the template and then
c     using a "domainlist" string which does not include the template's domain
f     using a DOMAINLIST string which does not include the template's domain
*     (or a blank field). If you do so, no coordinate system will be
*     found.
*     - A null Object pointer (AST__NULL) will be returned if this
c     function is invoked with the AST error status set, or if it
f     function is invoked with STATUS set to an error value, or if it
*     should fail for any reason.

*  More on Using Templates:
*     A Frame (describing a coordinate system) will be found by this
*     function if (a) it is "matched" by the template you supply, and
c     (b) the value of its Domain attribute appears in the "domainlist"
f     (b) the value of its Domain attribute appears in the DOMAINLIST
*     string (except that a blank field in this string permits any
*     domain). A successful match by the template depends on a number
*     of criteria, as outlined below:
*     - In general, a template will only match another Frame which
*     belongs to the same class as the template, or to a derived (more
*     specialised) class. For example, a SkyFrame template will match
*     any other SkyFrame, but will not match a basic
*     Frame. Conversely, a basic Frame template will match any class
*     of Frame.
*     - If a template has a value set for any of its attributes, then
*     it will only match Frames which have an identical value for that
*     attribute (or which can be transformed, using a built-in
*     conversion, so that they have the required value for that
*     attribute). If any attribute in the template is un-set, however,
*     then Frames are matched regardless of the value they may have
*     for that attribute. You may therefore make a template more or
*     less specific by choosing the attributes for which you set
*     values.
*     - An important application of this principle involves the Domain
*     attribute. Setting the Domain attribute of the template has the
*     effect of restricting the search to a particular type of Frame
*     (with the domain you specify).  Conversely, if the Domain
*     attribute is not set in the template, then the domain of the
*     Frame found is not relevant, so all Frames are searched.  Note
*     that the
c     "domainlist" string provides an alternative way of restricting the
f     DOMAINLIST string provides an alternative way of restricting the
*     search in the same manner, but is a more convenient interface if
*     you wish to search automatically for another domain if the first
*     search fails.
*     - Normally, a template will only match a Frame which has the
*     same number of axes as itself. However, for some classes of
*     template, this default behaviour may be changed by means of the
*     MinAxes, MaxAxes and MatchEnd attributes. In addition, the
*     behaviour of a template may be influenced by its Permute and
*     PreserveAxes attributes, which control whether it matches Frames
*     whose axes have been permuted, and whether this permutation is
*     retained in the Frame which is returned (as opposed to returning
*     the axes in the order specified in the template, which is the
*     default behaviour). You should consult the descriptions of these
*     attributes for details of this more advanced use of templates.
*--
*/

/* Local Variables: */
   AstFrame *frame;              /* Pointer to result Frame */
   AstFrameSet *result;          /* Pointer to result FrameSet */
   AstMapping *map;              /* Pointer to result Mapping */
   AstMapping *tmp;              /* Temporary Mapping pointer */
   char *domain_copy;            /* Pointer to copy of result domain */
   char *domainlist_copy;        /* Pointer to copy of domains list */
   const char *domain;           /* Pointer to result Domain value */
   int *target_axes;             /* Pointer to target axis assignments */
   int *template_axes;           /* Pointer to template axis assignments */
   int i;                        /* Loop counter for characters */
   int j;                        /* Character index */
   int match;                    /* Template matched target? */

/* Initialise. */
   result = NULL;

/* Check the global error status. */
   if ( !astOK ) return result;

/* Allocate space to store a copy of the domains list, with added
   commas. */
   domainlist_copy = astMalloc( strlen( domainlist ) + (size_t) 3 );
   if ( astOK ) {

/* Make a copy of the domains list, with an extra comma added at each
   end. Also remove all white space and convert to upper case. */
      domainlist_copy[ 0 ] = ',';
      for ( i = 0, j = 1; domainlist[ i ]; i++ ) {
         if ( !isspace( domainlist[ i ] ) ) {
            domainlist_copy[ j++ ] = toupper( domainlist[ i ] );
         }
      }
      domainlist_copy[ j++ ] = ',';
      domainlist_copy[ j ] = '\0';

/* Invoke the protected astMatch method associated with the template
   Frame. This matches the template to the target and returns
   information about how to convert between the target and the Frame
   it found (if any). */
      match = astMatch( template, target,
                        &template_axes, &target_axes, &map, &frame );

/* If successful, obtain a pointer to the Domain string of the result
   Frame. Allocate space for a copy of this string, with added
   commas. */
      if ( match && astOK ) {
         domain = astGetDomain( frame );
         if ( astOK ) {
            domain_copy = astMalloc( strlen( domain ) + (size_t) 3 );
            if ( astOK ) {

/* Make a copy of the domain, adding an extra comma at each end. */
               domain_copy[ 0 ] = ',';
               for ( i = 0, j = 1; domain[ i ]; i++ ) {
                  domain_copy[ j++ ] = domain[ i ];
               }
               domain_copy[ j++ ] = ',';
               domain_copy[ j ] = '\0';
            
/* Test if the domain appears in the domains list (with added
   commas). If not, test if a blank domain (which permits the result
   Frame to have any Domain) appears instead. */
               if ( strstr( domainlist_copy, domain_copy ) ||
                    strstr( domainlist_copy, ",," ) ) {

/* If the result Frame is acceptable, simplify the result Mapping. */
                  tmp = astSimplify( map );
                  map = astAnnul( map );
                  map = tmp;

/* Build the result FrameSet. */
                  result = astFrameSet( target, "" );
                  astAddFrame( result, AST__BASE, map, frame );
               }
            }

/* Free the copy of the result domain. */
            domain_copy = astFree( domain_copy );
         }

/* Free space and annul pointers allocated by astMatch. */
         template_axes = astFree( template_axes );
         target_axes = astFree( target_axes );
         map = astAnnul( map );
         frame = astAnnul( frame );
      }
   }

/* Free the copy of the domains list. */
   domainlist_copy = astFree( domainlist_copy );

/* If an error occurred, annul any result pointer. */
   if ( !astOK && result ) result = astAnnul( result );

/* Return the result. */
   return result;
}

static const char *Format( AstFrame *this, int axis, double value ) {
/*
*+
*  Name:
*     astFormat

*  Purpose:
*     Format a coordinate value for a Frame axis.

*  Type:
*     Protected virtual function.

*  Synopsis:
*     #include "frame.h"
*     const char *astFormat( AstFrame *this, int axis, double value )

*  Class Membership:
*     Frame method.

*  Description:
*     This function returns a pointer to a string containing the
*     formatted (character) version of a coordinate value for a Frame
*     axis. The formatting applied is determined by the Frame's
*     attributes and, in particular, by any Format attribute string
*     that has been set for the axis. A suitable default format will
*     be applied if necessary.

*  Parameters:
*     this
*        Pointer to the Frame.
*     axis
*        The number of the Frame axis for which formatting is to be
*        performed (axis numbering starts at zero for the first axis).
*     value
*        The coordinate value to be formatted.

*  Returned Value:
*     A pointer to a null-terminated string containing the formatted
*     value.

*  Notes:
*     - The returned string pointer may point at memory allocated
*     within the Frame, or at static memory. The contents of the
*     string may be over-written or the pointer may become invalid
*     following a further invocation of the same function or any
*     modification of the Frame. A copy of the string should
*     therefore be made if necessary.
*     - A NULL pointer will be returned if this function is invoked
*     with the global error status set, or if it should fail for any
*     reason.
*-

*  Implementation Notes:
*     - This function implements the basic astFormat method available
*     via the protected interface to the Frame class. The public
*     interface to this method is provided by the astFormatId_
*     function.
*/

/* Local Variables: */
   AstAxis *ax;                  /* Pointer to Axis object */
   const char *result;           /* Pointer value to return */
   int digits_set;               /* Axis Digits attribute set? */

/* Check the global error status. */
   if ( !astOK ) return NULL;

/* Validate the axis index and obtain a pointer to the required Axis. */
   (void) astValidateAxis( this, axis, "astFormat" );
   ax = astGetAxis( this, axis );

/* Test if any Axis attributes which may affect the result are undefined (i.e.
   have not been explicitly set). If so, we over-ride them, giving them
   temporary values dictated by the Frame. Only the Digits attribute is
   relevant here. */
   digits_set = astTestAxisDigits( ax );
   if ( !digits_set ) astSetAxisDigits( ax, astGetDigits( this ) );

/* Format the value. */
   result = astAxisFormat( ax, value );

/* Clear any Axis attributes that were temporarily over-ridden. */
   if ( !digits_set ) astClearAxisDigits( ax );

/* Annul the Axis pointer. */
   ax = astAnnul( ax );

/* If an error occurred, clear the result value. */
   if ( !astOK ) result = NULL;

/* Return the result. */
   return result;
}

static double Gap( AstFrame *this, int axis, double gap, int *ntick ) {
/*
*+
*  Name:
*     astGap

*  Purpose:
*     Find a "nice" gap for tabulating Frame axis values.

*  Type:
*     Protected virtual function.

*  Synopsis:
*     #include "frame.h"
*     double astGap( AstFrame *this, int axis, double gap, int *ntick )

*  Class Membership:
*     Frame method.

*  Description:
*     This function returns a gap size which produces a nicely spaced
*     series of formatted values for a Frame axis, the returned gap
*     size being as close as possible to the supplied target gap
*     size. It also returns a convenient number of divisions into
*     which the gap can be divided.

*  Parameters:
*     this
*        Pointer to the Frame.
*     axis
*        The number of the axis (zero-based) for which a gap is to be found.
*     gap
*        The target gap size.
*     ntick
*        Address of an int in which to return a convenient number of
*        divisions into which the gap can be divided.

*  Returned Value:
*     The nice gap size.

*  Notes:
*     - A value of zero is returned if the target gap size is zero.
*     - A negative gap size is returned if the supplied gap size is negative.
*     - A value of zero will be returned if this function is invoked
*     with the global error status set, or if it should fail for any
*     reason.
*-
*/

/* Local Variables: */
   AstAxis *ax;                  /* Pointer to Axis object */
   double result;                /* The nice gap value */

/* Check the global error status. */
   if ( !astOK ) return 0.0;

/* Validate the axis index and obtain a pointer to the required
   Axis. */
   (void) astValidateAxis( this, axis, "astGap" );
   ax = astGetAxis( this, axis );

/* Find the gap. */
   result = astAxisGap( ax, gap, ntick );

/* Annul the Axis pointer. */
   ax = astAnnul( ax );

/* If an error occurred, clear the result value. */
   if ( !astOK ) result = 0.0;

/* Return the result. */
   return result;
}

static const char *GetAttrib( AstObject *this_object, const char *attrib ) {
/*
*  Name:
*     GetAttrib

*  Purpose:
*     Get the value of a specified attribute for a Frame.

*  Type:
*     Private function.

*  Synopsis:
*     #include "frame.h"
*     const char *GetAttrib( AstObject *this, const char *attrib )

*  Class Membership:
*     Frame member function (over-rides the protected astGetAttrib
*     method inherited from the Mapping class).

*  Description:
*     This function returns a pointer to the value of a specified
*     attribute for a Frame, formatted as a character string.

*  Parameters:
*     this
*        Pointer to the Frame.
*     attrib
*        Pointer to a null-terminated string containing the name of
*        the attribute whose value is required. This name should be in
*        lower case, with all white space removed.

*  Returned Value:
*     - Pointer to a null-terminated string containing the attribute
*     value.

*  Notes:
*     - This function uses one-based axis numbering so that it is
*     suitable for external (public) use.
*     - The returned string pointer may point at memory allocated
*     within the Frame, or at static memory. The contents of the
*     string may be over-written or the pointer may become invalid
*     following a further invocation of the same function or any
*     modification of the Frame. A copy of the string should
*     therefore be made if necessary.
*     - A NULL pointer will be returned if this function is invoked
*     with the global error status set, or if it should fail for any
*     reason.
*/

/* Local Constants: */
#define BUFF_LEN 50              /* Max. characters in result buffer */

/* Local Variables: */
   AstAxis *ax;                  /* Pointer to Axis */
   AstFrame *this;               /* Pointer to the Frame structure */
   char *axis_attrib;            /* Pointer to axis attribute name */
   const char *result;           /* Pointer value to return */
   int axis;                     /* Frame axis number */
   int axis_nc;                  /* No. characters in axis attribute name */
   int digits;                   /* Digits attribute value */
   int direction;                /* Direction attribute value */
   int len;                      /* Length of attrib string */
   int match_end;                /* MatchEnd attribute value */
   int max_axes;                 /* MaxAxes attribute value */
   int min_axes;                 /* MinAxes attribute value */
   int naxes;                    /* Naxes attribute value */
   int nc;                       /* No. characters read by sscanf */
   int permute;                  /* Permute attribute value */
   int preserve_axes;            /* PreserveAxes attribute value */
   static char buff[ BUFF_LEN + 1 ]; /* Buffer for string result */

/* Initialise. */
   result = NULL;

/* Check the global error status. */   
   if ( !astOK ) return result;

/* Obtain a pointer to the Frame structure. */
   this = (AstFrame *) this_object;

/* Obtain the length of the attrib string. */
   len = strlen( attrib );

/* Compare "attrib" with each recognised attribute name in turn,
   obtaining the value of the required attribute. If necessary, write
   the value into "buff" as a null-terminated string in an appropriate
   format.  Set "result" to point at the result string. */

/* Digits. */
/* ------- */
   if ( !strcmp( attrib, "digits" ) ) {
      digits = astGetDigits( this );
      if ( astOK ) {
         (void) sprintf( buff, "%d", digits );
         result = buff;
      }

/* Digits(axis). */
/* ------------- */
   } else if ( nc = 0,
               ( 1 == sscanf( attrib, "digits(%d)%n", &axis, &nc ) )
               && ( nc >= len ) ) {

/* There is no function to obtain the Digits attribute value for an
   axis directly, so obtain a pointer to the Axis and use this to
   obtain the value. Use the Frame's Digits attribute instead if the
   Axis attribute value is not set. */
      (void) astValidateAxis( this, axis - 1, "astGetDigits(axis)" );
      ax = astGetAxis( this, axis - 1 );
      if ( astTestAxisDigits( ax ) ) {
         digits = astGetAxisDigits( ax );
      } else {
         digits = astGetDigits( this );
      }
      ax = astAnnul( ax );
      if ( astOK ) {
         (void) sprintf( buff, "%d", digits );
         result = buff;
      }


/* Direction(axis). */
/* ---------------- */
   } else if ( nc = 0,
               ( 1 == sscanf( attrib, "direction(%d)%n", &axis, &nc ) )
               && ( nc >= len ) ) {
      direction = astGetDirection( this, axis - 1 );
      if ( astOK ) {
         (void) sprintf( buff, "%d", direction );
         result = buff;
      }

/* Domain. */
/* ------- */
   } else if ( !strcmp( attrib, "domain" ) ) {
      result = astGetDomain( this );

/* Format(axis). */
/* ------------- */
   } else if ( nc = 0,
               ( 1 == sscanf( attrib, "format(%d)%n", &axis, &nc ) )
               && ( nc >= len ) ) {
      result = astGetFormat( this, axis - 1 );

/* Label(axis). */
/* ------------ */
   } else if ( nc = 0,
               ( 1 == sscanf( attrib, "label(%d)%n", &axis, &nc ) )
               && ( nc >= len ) ) {
      result = astGetLabel( this, axis - 1 );

/* MatchEnd. */
/* --------- */
   } else if ( !strcmp( attrib, "matchend" ) ) {
      match_end = astGetMatchEnd( this );
      if ( astOK ) {
         (void) sprintf( buff, "%d", match_end );
         result = buff;
      }

/* MaxAxes. */
/* -------- */
   } else if ( !strcmp( attrib, "maxaxes" ) ) {
      max_axes = astGetMaxAxes( this );
      if ( astOK ) {
         (void) sprintf( buff, "%d", max_axes );
         result = buff;
      }

/* MinAxes. */
/* -------- */
   } else if ( !strcmp( attrib, "minaxes" ) ) {
      min_axes = astGetMinAxes( this );
      if ( astOK ) {
         (void) sprintf( buff, "%d", min_axes );
         result = buff;
      }

/* Naxes. */
/* -----_ */
   } else if ( !strcmp( attrib, "naxes" ) ) {
      naxes = astGetNaxes( this );
      if ( astOK ) {
         (void) sprintf( buff, "%d", naxes );
         result = buff;
      }

/* Permute. */
/* -------- */
   } else if ( !strcmp( attrib, "permute" ) ) {
      permute = astGetPermute( this );
      if ( astOK ) {
         (void) sprintf( buff, "%d", permute );
         result = buff;
      }

/* PreserveAxes. */
/* ------------- */
   } else if ( !strcmp( attrib, "preserveaxes" ) ) {
      preserve_axes = astGetPreserveAxes( this );
      if ( astOK ) {
         (void) sprintf( buff, "%d", preserve_axes );
         result = buff;
      }

/* Symbol(axis). */
/* ------------- */
   } else if ( nc = 0,
               ( 1 == sscanf( attrib, "symbol(%d)%n", &axis, &nc ) )
               && ( nc >= len ) ) {
      result = astGetSymbol( this, axis - 1 );

/* Title. */
/* ------ */
   } else if ( !strcmp( attrib, "title" ) ) {
      result = astGetTitle( this );

/* Unit(axis). */
/* ----------- */
   } else if ( nc = 0,
               ( 1 == sscanf( attrib, "unit(%d)%n", &axis, &nc ) )
               && ( nc >= len ) ) {
      result = astGetUnit( this, axis - 1 );

/* Other axis attributes. */
/* ---------------------- */
/* If the attribute was not identified above, but appears to refer to
   a Frame axis, then it may refer to an Axis object of a derived type
   (which has additional attributes not recognised here). */
   } else if ( nc = 0,
               ( 1 == sscanf( attrib, "%*[^()]%n(%d)%n",
                                      &axis_nc, &axis, &nc ) )
               && ( nc >= len ) ) {

/* Validate the axis index and extract the attribute name. */
      (void) astValidateAxis( this, axis - 1, "astGet" );
      axis_attrib = astString( attrib, axis_nc );

/* Obtain a pointer to the Axis object and use its astGetAttrib method
   to obtain the result. */
      ax = astGetAxis( this, axis - 1 );
      result = astGetAttrib( ax, axis_attrib );

/* Annul the Axis pointer and free the memory holding the attribute
   name. */
      ax = astAnnul( ax );
      axis_attrib = astFree( axis_attrib );

/* Not recognised. */
/* --------------- */
/* If the attribute name was not recognised, pass it on to the parent
   method for further interpretation. */
   } else {
      result = (*parent_getattrib)( this_object, attrib );
   }

/* Return the result. */
   return result;

/* Undefine macros local to this function. */
#undef BUFF_LEN
}

static AstAxis *GetAxis( AstFrame *this, int axis ) {
/*
*+
*  Name:
*     astGetAxis

*  Purpose:
*     Obtain a pointer to a specified Axis from a Frame.

*  Type:
*     Protected virtual function.

*  Synopsis:
*     #include "frame.h"
*     AstAxis *astGetAxis( AstFrame *this, int axis )

*  Class Membership:
*     Frame method.

*  Description:
*     This function returns a pointer to the Axis object associated
*     with one of the axes of a Frame. This object describes the
*     quantity which is represented along that axis.

*  Parameters:
*     this
*        Pointer to the Frame.
*     axis
*        The number of the axis (zero-based) for which an Axis pointer is
*        required.

*  Returned Value:
*     A pointer to the requested Axis object.

*  Notes:
*     - The reference count of the requested Axis object will be
*     incremented by one to reflect the additional pointer returned by
*     this function.
*     - A NULL pointer will be returned if this function is invoked
*     with the global error status set, or if it should fail for any
*     reason.
*-
*/

/* Local Variables: */
   AstAxis *result;              /* Pointer to Axis */

/* Check the global error status. */
   if ( !astOK ) return NULL;

/* Initialise. */
   result = NULL;

/* Validate and permute the axis index. */
   axis = astValidateAxis( this, axis, "astGetAxis" );

/* If OK, clone the required Axis pointer. */
   if ( astOK ) result = astClone( this->axis[ axis ] );

/* Return the result. */
   return result;
}

/*
*att++
*  Name:
*     Naxes

*  Purpose:
*     Number of Frame axes.

*  Type:
*     Public attribute.

*  Synopsis:
*     Integer, read-only.

*  Description:
*     This is a read-only attribute giving the number of axes in a
*     Frame (i.e. the number of dimensions of the coordinate space
*     which the Frame describes). This value is determined when the
*     Frame is created.

*  Applicability:
*     Frame
*        All Frames have this attribute.
*     FrameSet
*        The Naxes attribute of a FrameSet is the same as that of its
*        current Frame (as specified by the Current attribute).
*     CmpFrame
*        The Naxes attribute of a CmpFrame is equal to the sum of the
*        Naxes values of its two component Frames.
*att--
*/

static int GetNaxes( AstFrame *this ) {
/*
*+
*  Name:
*     astGetNaxes

*  Purpose:
*     Determine how many axes a Frame has.

*  Type:
*     Protected virtual function.

*  Synopsis:
*     #include "frame.h"
*     int astGetNaxes( AstFrame *this )

*  Class Membership:
*     Frame method.

*  Description:
*     This function returns the number of axes for a Frame.

*  Parameters:
*     this
*        Pointer to the Frame.

*  Returned Value:
*     The number of Frame axes.

*  Notes:
*     - A value of zero will be returned if this function is invoked
*     with the global error status set, or if it should fail for any
*     reason.
*-
*/

/* Check the global error status. */
   if ( !astOK ) return 0;

/* Return the number of Frame axes. */
   return this->naxes;
}

static int GetNin( AstMapping *this_mapping ) {
/*
*  Name:
*     GetNin

*  Purpose:
*     Get the number of input coordinates for a Frame.

*  Type:
*     Private function.

*  Synopsis:
*     #include "frame.h"
*     int GetNin( AstMapping *this )

*  Class Membership:
*     Frame member function (over-rides the astGetNin method inherited
*     from the Mapping class).

*  Description:
*     This function returns the number of input coordinate values
*     required per point by a Frame, when used as a Mapping (i.e. the
*     number of dimensions of the space in which input points reside).

*  Parameters:
*     this
*        Pointer to the Frame.

*  Returned Value:
*     Number of coordinate values required.

*  Notes:
*     - A value of zero will be returned if this function is invoked
*     with the global error status set, or if it should fail for any
*     reason.
*/

/* Local Variables: */
   AstFrame *this;               /* Pointer to Frame structure */
   int result;                   /* Result value to return */

/* Initialise. */
   result = 0;

/* Check the global error status. */
   if ( !astOK ) return result;

/* Obtain a pointer to the Frame structure. */
   this = (AstFrame *) this_mapping;

/* Return the number of Frame axes. */
   result = astGetNaxes( this );

/* Return the result. */
   return result;
}

static int GetNout( AstMapping *this_mapping ) {
/*
*  Name:
*     GetNout

*  Purpose:
*     Get the number of output coordinates for a Frame.

*  Type:
*     Private function.

*  Synopsis:
*     #include "frame.h"
*     int GetNout( AstMapping *this )

*  Class Membership:
*     Frame member function (over-rides the astGetNout method
*     inherited from the Mapping class).

*  Description:
*     This function returns the number of output coordinate values
*     generated per point by a Frame, when used as a Mapping (i.e. the
*     number of dimensions of the space in which output points
*     reside).

*  Parameters:
*     this
*        Pointer to the Frame.

*  Returned Value:
*     Number of coordinate values generated.

*  Notes:
*     - A value of zero will be returned if this function is invoked
*     with the global error status set, or if it should fail for any
*     reason.
*/

/* Local Variables: */
   AstFrame *this;               /* Pointer to Frame structure */
   int result;                   /* Result value to return */

/* Initialise. */
   result = 0;

/* Check the global error status. */
   if ( !astOK ) return result;

/* Obtain a pointer to the Frame structure. */
   this = (AstFrame *) this_mapping;

/* Return the number of Frame axes. */
   result = astGetNaxes( this );

/* Return the result. */
   return result;
}

static const int *GetPerm( AstFrame *this ) {
/*
*+
*  Name:
*     astGetPerm

*  Purpose:
*     Access the axis permutation array for a Frame.

*  Type:
*     Protected virtual function.

*  Synopsis:
*     #include "frame.h"
*     const int *astGetPerm( AstFrame *this )

*  Class Membership:
*     Frame method.

*  Description:
*     This function returns a pointer to the axis permutation array
*     for a Frame. This array constitutes a lookup-table that converts
*     between an axis number supplied externally and the corresponding
*     index in the Frame's internal axis arrays.

*  Parameters:
*     this
*        Pointer to the Frame.

*  Returned Value:
*     Pointer to the Frame's axis permutation array (a constant array
*     of int).  Each element of this contains the (zero-based)
*     internal axis index to be used in place of the external index
*     which is used to address the permutation array. If the Frame has
*     zero axes, this pointer will be NULL.

*  Notes:
*     - This protected method is provided to assist class
*     implementations which need to implement axis-dependent
*     extensions to Frame methods, and which therefore need to know
*     how a Frames's external axis index is converted for internal
*     use.
*     - The pointer returned by this function gives direct access to
*     data internal to the Frame object. It remains valid only so long
*     as the Frame exists. The permutation array contents may be
*     modified by other functions which operate on the Frame and this
*     may render the returned pointer invalid.
*     - A NULL pointer will be returned if this function is invoked
*     with the global error status set, or if it should fail for any
*     reason.
*-
*/

/* Check the global error status. */
   if ( !astOK ) return NULL;

/* Return a pointer to the axis permutation array. */
   return this->perm;
}

static void InitVtab( AstFrameVtab *vtab ) {
/*
*  Name:
*     InitVtab

*  Purpose:
*     Initialise a virtual function table for a Frame.

*  Type:
*     Private function.

*  Synopsis:
*     #include "frame.h"
*     void InitVtab( AstFrameVtab *vtab )

*  Class Membership:
*     Frame member function.

*  Description:
*     This function initialises the component of a virtual function
*     table which is used by the Frame class.

*  Parameters:
*     vtab
*        Pointer to the virtual function table. The components used by
*        all ancestral classes should already have been initialised.
*/

/* Local Variables: */
   AstObjectVtab *object;        /* Pointer to Object component of Vtab */
   AstMappingVtab *mapping;      /* Pointer to Mapping component of Vtab */

/* Check the local error status. */
   if ( !astOK ) return;

/* Store a unique "magic" value in the virtual function table. This
   will be used (by astIsAFrame ) to determine if an object belongs
   to this class.  We can conveniently use the address of the (static)
   class_init variable to generate this unique value. */
   vtab->check = &class_init;

/* Initialise member function pointers. */
/* ------------------------------------ */
/* Store pointers to the member functions (implemented here) that provide
   virtual methods for this class. */
   vtab->Abbrev = Abbrev;
   vtab->CheckPerm = CheckPerm;
   vtab->ClearDigits = ClearDigits;
   vtab->ClearDirection = ClearDirection;
   vtab->ClearDomain = ClearDomain;
   vtab->ClearFormat = ClearFormat;
   vtab->ClearLabel = ClearLabel;
   vtab->ClearMatchEnd = ClearMatchEnd;
   vtab->ClearMaxAxes = ClearMaxAxes;
   vtab->ClearMinAxes = ClearMinAxes;
   vtab->ClearPermute = ClearPermute;
   vtab->ClearPreserveAxes = ClearPreserveAxes;
   vtab->ClearSymbol = ClearSymbol;
   vtab->ClearTitle = ClearTitle;
   vtab->ClearUnit = ClearUnit;
   vtab->Convert = Convert;
   vtab->ConvertX = ConvertX;
   vtab->Distance = Distance;
   vtab->FindFrame = FindFrame;
   vtab->Format = Format;
   vtab->Gap = Gap;
   vtab->GetAxis = GetAxis;
   vtab->GetDigits = GetDigits;
   vtab->GetDirection = GetDirection;
   vtab->GetDomain = GetDomain;
   vtab->GetFormat = GetFormat;
   vtab->GetLabel = GetLabel;
   vtab->GetMatchEnd = GetMatchEnd;
   vtab->GetMaxAxes = GetMaxAxes;
   vtab->GetMinAxes = GetMinAxes;
   vtab->GetNaxes = GetNaxes;
   vtab->GetPerm = GetPerm;
   vtab->GetPermute = GetPermute;
   vtab->GetPreserveAxes = GetPreserveAxes;
   vtab->GetSymbol = GetSymbol;
   vtab->GetTitle = GetTitle;
   vtab->GetUnit = GetUnit;
   vtab->Match = Match;
   vtab->Norm = Norm;
   vtab->Offset = Offset;
   vtab->Overlay = Overlay;
   vtab->PermAxes = PermAxes;
   vtab->PickAxes = PickAxes;
   vtab->PrimaryFrame = PrimaryFrame;
   vtab->SetAxis = SetAxis;
   vtab->SetDigits = SetDigits;
   vtab->SetDirection = SetDirection;
   vtab->SetDomain = SetDomain;
   vtab->SetFormat = SetFormat;
   vtab->SetLabel = SetLabel;
   vtab->SetMatchEnd = SetMatchEnd;
   vtab->SetMaxAxes = SetMaxAxes;
   vtab->SetMinAxes = SetMinAxes;
   vtab->SetPermute = SetPermute;
   vtab->SetPreserveAxes = SetPreserveAxes;
   vtab->SetSymbol = SetSymbol;
   vtab->SetTitle = SetTitle;
   vtab->SetUnit = SetUnit;
   vtab->SubFrame = SubFrame;
   vtab->TestDigits = TestDigits;
   vtab->TestDirection = TestDirection;
   vtab->TestDomain = TestDomain;
   vtab->TestFormat = TestFormat;
   vtab->TestLabel = TestLabel;
   vtab->TestMatchEnd = TestMatchEnd;
   vtab->TestMaxAxes = TestMaxAxes;
   vtab->TestMinAxes = TestMinAxes;
   vtab->TestPermute = TestPermute;
   vtab->TestPreserveAxes = TestPreserveAxes;
   vtab->TestSymbol = TestSymbol;
   vtab->TestTitle = TestTitle;
   vtab->TestUnit = TestUnit;
   vtab->Unformat = Unformat;
   vtab->ValidateAxis = ValidateAxis;
   vtab->ValidateAxisSelection = ValidateAxisSelection;

/* Save the inherited pointers to methods that will be extended, and
   replace them with pointers to the new member functions. */
   object = (AstObjectVtab *) vtab;

   parent_clearattrib = object->ClearAttrib;
   object->ClearAttrib = ClearAttrib;
   parent_getattrib = object->GetAttrib;
   object->GetAttrib = GetAttrib;
   parent_setattrib = object->SetAttrib;
   object->SetAttrib = SetAttrib;
   parent_testattrib = object->TestAttrib;
   object->TestAttrib = TestAttrib;

/* Store replacement pointers for methods which will be over-ridden by
   new member functions implemented here. */
   mapping = (AstMappingVtab *) vtab;

   mapping->GetNin = GetNin;
   mapping->GetNout = GetNout;
   mapping->ReportPoints = ReportPoints;
   mapping->Transform = Transform;

/* Declare the copy constructor, destructor and class dump
   function. */
   astSetCopy( vtab, Copy );
   astSetDelete( vtab, Delete );
   astSetDump( vtab, Dump, "Frame", "Coordinate system description" );
}

static int Match( AstFrame *template, AstFrame *target,
                  int **template_axes, int **target_axes,
                  AstMapping **map, AstFrame **result ) {
/*
*+
*  Name:
*     astMatch

*  Purpose:
*     Determine if conversion is possible between two coordinate systems.

*  Type:
*     Protected virtual function.

*  Synopsis:
*     #include "frame.h"
*     int astMatch( AstFrame *template, AstFrame *target,
*                   int **template_axes, int **target_axes,
*                   AstMapping **map, AstFrame **result )

*  Class Membership:
*     Frame method.

*  Description:
*     This function matches a "template" frame to a "target" frame and
*     determines whether it is possible to convert coordinates between
*     them.  If it is, a mapping that performs the transformation is
*     returned along with a new Frame that describes the coordinate
*     system that results when this mapping is applied to the "target"
*     coordinate system. In addition, information is returned to allow
*     the axes in this "result" Frame to be associated with the
*     corresponding axes in the "target" and "template" Frames from
*     which they are derived.

*  Parameters:
*     template
*        Pointer to the template Frame. This describes the coordinate
*        system (or set of possible coordinate systems) into which we
*        wish to convert our coordinates.
*     target
*        Pointer to the target Frame. This describes the coordinate
*        system in which we already have coordinates.
*     template_axes
*        Address of a location where a pointer to int will be returned
*        if the requested coordinate conversion is possible. This
*        pointer will point at a dynamically allocated array of
*        integers with one element for each axis of the "result" Frame
*        (see below). It must be freed by the caller (using astFree)
*        when no longer required.
*
*        For each axis in the result Frame, the corresponding element
*        of this array will return the (zero-based) index of the
*        template Frame axis from which it is derived. If it is not
*        derived from any template frame axis, a value of -1 will be
*        returned instead.
*     target_axes
*        Address of a location where a pointer to int will be returned
*        if the requested coordinate conversion is possible. This
*        pointer will point at a dynamically allocated array of
*        integers with one element for each axis of the "result" Frame
*        (see below). It must be freed by the caller (using astFree)
*        when no longer required.
*
*        For each axis in the result Frame, the corresponding element
*        of this array will return the (zero-based) index of the
*        target Frame axis from which it is derived. If it is not
*        derived from any target Frame axis, a value of -1 will be
*        returned instead.
*     map
*        Address of a location where a pointer to a new Mapping will
*        be returned if the requested coordinate conversion is
*        possible. If returned, the forward transformation of this
*        Mapping may be used to convert coordinates between the
*        "target" Frame and the "result" Frame (see below) and the
*        inverse transformation will convert in the opposite
*        direction.
*     result
*        Address of a location where a pointer to a new Frame will be
*        returned if the requested coordinate conversion is
*        possible. If returned, this Frame describes the coordinate
*        system that results from applying the returned Mapping
*        (above) to the "target" coordinate system. In general, this
*        Frame will combine attributes from (and will therefore be
*        more specific than) both the target and the template
*        Frames. In particular, when the template allows the
*        possibility of transformaing to any one of a set of
*        alternative coordinate systems, the "result" Frame will
*        indicate which of the alternatives was used.

*  Returned Value:
*     A non-zero value is returned if the requested coordinate
*     conversion is possible. Otherwise zero is returned (this will
*     not in itself result in an error condition).

*  Notes:
*     - By default, the "result" frame will have its number of axes
*     and axis order determined by the "template" Frame. However, if
*     the PreserveAxes attribute of the template frame is non-zero,
*     then the axis count and axis order of the "target" frame will be
*     used instead.
*     - The template_axes and target_axes arrays are provided so that
*     if the caller needs to permute the target and/or template axes
*     before invoking this function, it is possible to deduce how the
*     result axes should be permuted so as to correspond with the
*     original template/target axis order.
*     - For result axes that do not correspond with a template and/or
*     target axis (where a value of -1 is returned in the
*     template_axes and/or target_axes arrays), the caller has no
*     clear way of knowing where these axes should appear in any
*     permuted order. In this case, the relative position of these
*     axes within the result Frame (with respect to axes that do have
*     template/target axis associations) will be used to convey this
*     information. Such axes should be taken to be associated either
*     with the next preceding or following axis (depending on the
*     AST__MATCHEND flag of the template frame) which does have an
*     association.
*     - If the result Frame has zero axes, then NULL pointer values
*     will be returned for *template_axes and *target_axes.
*     - A value of zero will be returned if this function is invoked
*     with the global error status set, or if it should fail for any
*     reason.
*-

*  Implementation Notes:
*     This implementation addresses the matching of a Frame class
*     object to other types of Frames (i.e. the target may be from a
*     derived class). A Frame will match any other type of Frame with
*     an acceptable number of axes but will not distinguish axis order
*     (i.e. it will match the axes in whatever order they are
*     given). If the template Frame has a value set for its Domain
*     attribute, then it will only match another Frame with the same
*     Domain.
*/

/* Local Variables: */
   char *template_domain;        /* Pointer to copy of template domain */
   const char *ptr;              /* Pointer to domain string */
   const char *target_domain;    /* Pointer to target domain string */
   int match;                    /* Template matches target? */
   int match_end;                /* Match final axes of target? */
   int max_axes;                 /* Maximum acceptable number of axes */
   int min_axes;                 /* Minimum acceptable nu,ber of axes */
   int preserve_axes;            /* Preserve target axes? */
   int result_axis;              /* Loop counter for result axes */
   int result_naxes;             /* Number of result axes */
   int target_naxes;             /* Number of target axes */
   int template_naxes;           /* Number of template axes */

/* Initialise the returned values. */
   *template_axes = NULL;
   *target_axes = NULL;
   *map = NULL;
   *result = NULL;
   match = 0;

/* Check the global error status. */
   if ( !astOK ) return match;

/* The first requirement for a match is that the target object is a Frame. This
   is already known to be true, as it forms part of the argument validation
   for this function. */

/* The second requirement is that the number of target axes is acceptable.
   Obtain the number of target axes and the minimum and maximum number of axes
   that the template will match. */
   target_naxes = astGetNaxes( target );
   min_axes = astGetMinAxes( template );
   max_axes = astGetMaxAxes( template );

/* Test if the number of target axes is acceptable. */
   if ( astOK ) {
      match = ( ( target_naxes >= min_axes ) && ( target_naxes <= max_axes ) );
   }

/* The third requirement is that if the template has its Domain
   attribute defined, then the target must also have the same Domain
   (although it need not be set - the default will do). First check if
   the template has a domain. */
   if ( astOK && match ) {
      if ( astTestDomain( template ) ) {

/* Obtain a pointer to the template domain. Then allocate memory and
   make a copy of it (this is necessary as we will next inquire the
   domain of the target and may over-write the buffer holding the
   template's domain). */
         ptr = astGetDomain( template );
         if ( astOK ) {
            template_domain = astStore( NULL, ptr,
                                        strlen( ptr ) + (size_t) 1 );

/* Obtain a pointer to the target domain. */
            target_domain = astGetDomain( target );
            
/* Compare the domain strings for equality. Then free the memory
   allocated above. */
            match = astOK && !strcmp( template_domain, target_domain );
            template_domain = astFree( template_domain );
         }
      }
   }

/* If the template matches, obtain the values of the template's PreserveAxes
   and MatchEnd attributes and determine the number of template axes. */
   if ( astOK && match ) {
      preserve_axes = astGetPreserveAxes( template );
      match_end = astGetMatchEnd( template );
      template_naxes = astGetNaxes( template );

/* If the PreserveAxes attribute is non-zero, the target axes should be
   preserved, so the number of result axes equals the number of target axes.
   Otherwise the number of template axes is used. */
      result_naxes = preserve_axes ? target_naxes : template_naxes;

/* Allocate memory for the arrays of axis associations to be returned. */
      *template_axes = astMalloc( sizeof( int ) * (size_t) result_naxes );
      *target_axes = astMalloc( sizeof( int ) * (size_t) result_naxes );
      if ( astOK ) {

/* Loop through each of the result axes. */
         for ( result_axis = 0; result_axis < result_naxes; result_axis++ ) {

/* Set up the axis associations. By default, associate the first result axis 
   with the first template/target axis. */
            (*template_axes)[ result_axis ] = result_axis;
            (*target_axes)[ result_axis ] = result_axis;

/* However, if the MatchEnd attribute is non-zero, associate the last result
   axis with the last template/target axis (this only makes a difference if
   there is a difference in the number of axes). */
            if ( match_end ) {
               (*template_axes)[ result_axis ] +=
                                               template_naxes - result_naxes;
               (*target_axes)[ result_axis ] += target_naxes - result_naxes;
            }

/* If any of the associations would be with a template/target axis that doesn't
   exist, then use an axis index of -1 for the association instead. */
            if ( ( (*template_axes)[ result_axis ] < 0 ) ||
                 ( (*template_axes)[ result_axis ] >= template_naxes ) ) {
               (*template_axes)[ result_axis ] = -1;
	    }
            if ( ( (*target_axes)[ result_axis ] < 0 ) ||
                 ( (*target_axes)[ result_axis ] >= target_naxes ) ) {
               (*target_axes)[ result_axis ] = -1;
	    }
         }

/* Use the target's astSubFrame method to select the required axes from it,
   overlaying the template's attributes on to the resulting Frame. This process
   also generates the required Mapping between the target and result Frames. */
         match = astSubFrame( target, template,
                              result_naxes, *target_axes, *template_axes,
                              map, result );
      }
   }

/* If an error occurred, free any allocated memory and reset the result. */
   if ( !astOK || !match ) {
      *template_axes = astFree( *template_axes );
      *target_axes = astFree( *target_axes );
      match = 0;
   }

/* Return the result. */
   return match;
}

static void Norm( AstFrame *this, double value[] ) {
/*
*++
*  Name:
c     astNorm
f     AST_NORM

*  Purpose:
*     Normalise a set of Frame coordinates.

*  Type:
*     Public virtual function.

*  Synopsis:
c     #include "frame.h"
c     void astNorm( AstFrame *this, double value[] )
f     CALL AST_NORM( THIS, VALUE, STATUS )

*  Class Membership:
*     Frame method.

*  Description:
c     This function normalises a set of Frame coordinate values which
f     This routine normalises a set of Frame coordinate values which
*     might be unsuitable for display (e.g. may lie outside the
*     expected range) into a set of acceptable values suitable for
*     display.

*  Parameters:
c     this
f     THIS = INTEGER (Given)
*        Pointer to the Frame.
c     value
f     VALUE( * ) = DOUBLE PRECISION (Given and Returned)
c        An array of double, with one element for each Frame axis
f        An array with one element for each Frame axis
*        (Naxes attribute). Initially, this should contain a set of
*        coordinate values representing a point in the space which the
*        Frame describes. If these values lie outside the expected
*        range for the Frame, they will be replaced with more
*        acceptable (normalised) values. Otherwise, they will be
*        returned unchanged.
f     STATUS = INTEGER (Given and Returned)
f        The global status.

*  Notes:
*     - For some classes of Frame, whose coordinate values are not
*     constrained, this function will never modify the values
*     supplied. However, for Frames whose axes represent cyclic
*     quantities (such as angles or positions on the sky), coordinates
*     will typically be wrapped into an appropriate standard range,
*     such as zero to 2*pi.
*     - It is intended to be possible to put any set of coordinates
*     into a form suitable for display by using this function to
*     normalise them, followed by appropriate formatting
c     (using astFormat).
f     (using AST_FORMAT).
*--
*/

/* Local Variables: */
   AstAxis *ax;                  /* Pointer to Axis object */
   int axis;                     /* Loop counter for axes */
   int naxes;                    /* Number of Frame axes */

/* Check the global error status. */
   if ( !astOK ) return;

/* Obtain the number of Frame axes. */
   naxes = astGetNaxes( this );

/* Loop to process the coordinate for each axis in turn. */
   for ( axis = 0; axis < naxes; axis++ ) {

/* Obtain a pointer to the relevant Frame Axis. */
      ax = astGetAxis( this, axis );

/* Normalise the coordinate for this axis. */
      astAxisNorm( ax, value + axis );

/* Annul the pointer to the Axis. */
      ax = astAnnul( ax );

/* Quit looping if an error occurs. */
      if ( !astOK ) break;
   }
}

static void Offset( AstFrame *this, const double point1[],
                    const double point2[], double offset, double point3[] ) {
/*
*++
*  Name:
c     astOffset
f     AST_OFFSET

*  Purpose:
*     Calculate an offset along a geodesic curve.

*  Type:
*     Public virtual function.

*  Synopsis:
c     #include "frame.h"
c     void astOffset( AstFrame *this,
c                     const double point1[], const double point2[],
c                     double offset, double point3[] )
f     CALL AST_OFFSET( THIS, POINT1, POINT2, OFFSET, POINT3, STATUS )

*  Class Membership:
*     Frame method.

*  Description:
c     This function finds the Frame coordinate values of a point which
f     This routine finds the Frame coordinate values of a point which
*     is offset a specified distance along the geodesic curve between
*     two other points.
*
*     For example, in a basic Frame, this offset will be along the
*     straight line joining two points. For a more specialised Frame
*     describing a sky coordinate system, however, it would be along
*     the great circle passing through two sky positions.

*  Parameters:
c     this
f     THIS = INTEGER (Given)
*        Pointer to the Frame.
c     point1
f     POINT1( * ) = DOUBLE PRECISION (Given)
c        An array of double, with one element for each Frame axis
f        An array with one element for each Frame axis
*        (Naxes attribute). This should contain the coordinates of the
*        point marking the start of the geodesic curve.
c     point2
f     POINT2( * ) = DOUBLE PRECISION (Given)
c        An array of double, with one element for each Frame axis
f        An array with one element for each Frame axis.
*        This should contain the coordinates of the point marking the
*        end of the geodesic curve.
c     offset
f     OFFSET = DOUBLE PRECISION
*        The required offset from the first point along the geodesic
*        curve. If this is positive, it will be towards the second
*        point. If it is negative, it will be in the opposite
*        direction. This offset need not imply a position lying
*        between the two points given, as the curve will be
*        extrapolated if necessary.
c     point3
f     POINT3( * ) = DOUBLE PRECISION (Returned)
c        An array of double, with one element for each Frame axis
f        An array with one element for each Frame axis
*        in which the coordinates of the required point will be returned.
f     STATUS = INTEGER (Given and Returned)
f        The global status.

*  Notes:
c     - The geodesic curve used by this function is the path of
f     - The geodesic curve used by this routine is the path of
*     shortest distance between two points, as defined by the
c     astDistance function.
f     AST_DISTANCE function.
*     - This function will return "bad" coordinate values (AST__BAD)
*     if any of the input coordinates has this value.
*     - "Bad" coordinate values will also be returned if the two
*     points supplied are coincident (or otherwise fail to uniquely
*     specify a geodesic curve) but the requested offset is non-zero.
*--
*/

/* Local Variables: */
   double delta;                 /* Displacement along axis */
   double dist;                  /* Distance between points */
   double fract;                 /* Fraction of distance required */
   int axis;                     /* Loop counter for axes */
   int naxes;                    /* Number of Frame axes */

/* Check the global error status. */
   if ( !astOK ) return;

/* Determine the number of Frame axes. */
   naxes = astGetNaxes( this );
   if ( astOK ) {

/* Loop to determine the Cartesian distance between points 1 and 2. */
      dist = 0.0;
      for ( axis = 0; axis < naxes; axis++ ) {

/* If any of the coordinates supplied is bad, set the distance to be
   bad and quit looping. */
         if ( ( point1[ axis ] == AST__BAD ) ||
              ( point2[ axis ] == AST__BAD ) ) {
            dist = AST__BAD;
            break;

/* Otherwise, accumulate the sum of squared displacements along each
   axis. */
         } else {
            delta = point1[ axis ] - point2[ axis ];
            dist += ( delta * delta );
         }
      }

/* Take the square root to find the distance (if valid). */
      if ( dist != AST__BAD ) dist = sqrt( dist );

/* If the distance between the points cannot be found, or the distance
   is zero but the required offset is non-zero, then set the result
   coordinates to be bad. */
      if ( ( dist == AST__BAD ) ||
           ( ( dist == 0.0 ) && ( offset != 0.0 ) ) ) {
         for ( axis = 0; axis < naxes; axis++ ) {
            point3[ axis ] = AST__BAD;
         }

/* Otherwise, calculate what fraction of the distance between the
   points we need to move, and apply this fraction of the displacement
   along each axis. */
      } else {
         fract = ( dist == 0.0 ) ? 0.0 : offset / dist;
         for ( axis = 0; axis < naxes; axis++ ) {
            point3[ axis ] = point1[ axis ] +
                             fract * ( point2[ axis ] - point1[ axis ] );
         }
      }
   }
}

static void Overlay( AstFrame *template, const int *template_axes,
                     AstFrame *result ) {
/*
*+
*  Name:
*     astOverlay

*  Purpose:
*     Overlay the attributes of a template Frame on to another Frame.

*  Type:
*     Protected virtual function.

*  Synopsis:
*     #include "frame.h"
*     void astOverlay( AstFrame *template, const int *template_axes,
*                      AstFrame *result )

*  Class Membership:
*     Frame method.

*  Description:
*     This function overlays attributes of one Frame (the "template") on to
*     another Frame, so as to over-ride selected attributes of that second
*     Frame. Normally only those attributes which have been specifically set
*     in the template will be transferred. This implements a form of
*     defaulting, in which a Frame acquires attributes from the template, but
*     retains its original attributes (as the default) if new values have not
*     previously been explicitly set in the template.

*  Parameters:
*     template
*        Pointer to the template Frame, for which values should have been
*        explicitly set for any attribute which is to be transferred.
*     template_axes
*        Pointer to an array of int, with one element for each axis of the
*        "result" Frame (see below). For each axis in the result frame, the
*        corresponding element of this array should contain the (zero-based)
*        index of the template axis to which it corresponds. This array is used
*        to establish from which template axis any axis-dependent attributes
*        should be obtained.
*
*        If any axis in the result Frame is not associated with a template
*        axis, the corresponding element of this array should be set to -1.
*     result
*        Pointer to the Frame which is to receive the new attribute values.
*-
*/

/* Local Variables: */
   AstAxis *result_ax;           /* Pointer to result Axis object */
   AstAxis *template_ax;         /* Pointer to template Axis object */
   int result_axis;              /* Loop counter for result Frame axes */
   int result_naxes;             /* Number of result Frame axes */
   int template_axis;            /* Index of template Frame axis */
   int template_naxes;           /* Number of template Frame axes */

/* Check the global error status. */
   if ( !astOK ) return;

/* Define a macro that tests whether an attribute is set in the template and,
   if so, transfers its value to the target. */
#define OVERLAY(attribute) \
   if ( astTest##attribute( template ) ) { \
      astSet##attribute( result, astGet##attribute( template ) ); \
   }

/* Use the macro to transfer each Frame attribute in turn. */
   OVERLAY(Digits);
   OVERLAY(Domain);
   OVERLAY(Title);

/* Now transfer attributes associated with individual axes. Obtain the number
   of axes in the template and result Frames. */
   template_naxes = astGetNaxes( template );
   result_naxes = astGetNaxes( result );
   if ( astOK ) {

/* Loop through all the axes in the result Frame and determine to which
   template axis each one corresponds. Check that the resulting axis index is
   valid. If not, then the axis will not receive new attributes. */
      for ( result_axis = 0; result_axis < result_naxes; result_axis++ ) {
         template_axis = template_axes[ result_axis ];
         if ( ( template_axis >= 0 ) && ( template_axis < template_naxes ) ) {

/* Obtain pointers to the relevant Axis objects of each Frame and use the
   astAxisOverlay method of the template Axis to overlay attributes on to
   the result Axis. Annul the Axis pointers afterwards. */
            template_ax = astGetAxis( template, template_axis );
            result_ax = astGetAxis( result, result_axis );
            astAxisOverlay( template_ax, result_ax );
            template_ax = astAnnul( template_ax );
            result_ax = astAnnul( result_ax );

/* Quit looping if an error occurs. */
            if ( !astOK ) break;
         }
      }
   }

/* Undefine macros local to this function. */
#undef OVERLAY
}

static void PermAxes( AstFrame *this, const int perm[] ) {
/*
*+
*  Name:
*     astPermAxes

*  Purpose:
*     Permute the order of a Frame's axes.

*  Type:
*     Protected virtual function.

*  Synopsis:
*     #include "frame.h"
*     void astPermAxes( AstFrame *this, const int perm[] )

*  Class Membership:
*     Frame method.

*  Description:
*     This function permutes the order in which a Frame's axes occur.

*  Parameters:
*     this
*        Pointer to the Frame.
*     perm
*        An array of int (with one element for each axis of the Frame)
*        which lists the axes in their new order. Each element of this
*        array should be a (zero-based) axis index identifying the
*        axes according to their old (un-permuted) order.

*  Notes:
*     - Only genuine permutations of the axis order are permitted, so
*     each axis must be referenced exactly once in the "perm" array.
*     - If more than one axis permutation is applied to a Frame, the
*     effects are cumulative.
*-

*  Implementation Notes:
*     - This function implements the basic astPermAxes method which is
*     available via the protected interface to the Frame class. A
*     public interface to this method is provided by the
*     astPermAxesId_ function.
*/

/* Local Variables: */
   int *old;                     /* Pointer to copy of old permutation array */
   int axis;                     /* Loop counter for Frame axes */
   int naxes;                    /* Number of Frame axes */

/* Check the global error status. */
   if ( !astOK ) return;

/* Validate the permutation array, to check that it describes a genuine
   permutation. */
   astCheckPerm( this, perm, "astPermAxes" );

/* Obtain the number of Frame axes. */
   naxes = astGetNaxes( this );

/* Allocate memory and use it to store a copy of the old permutation array for
   the Frame. */
   old = astStore( NULL, this->perm, sizeof( int ) * (size_t) naxes );

/* Apply the new axis permutation cumulatively to the old one and store the
   result in the Frame. */
   if ( astOK ) {
      for ( axis = 0; axis < naxes; axis++ ) {
         this->perm[ axis ] = old[ perm[ axis ] ];
      }
   }

/* Free the temporary copy of the old array. */
   old = astFree( old );
}

static AstFrame *PickAxes( AstFrame *this, int naxes, const int axes[],
                           AstMapping **map ) {
/*
*+
*  Name:
*     astPickAxes

*  Purpose:
*     Create a new Frame by picking axes from an existing one.

*  Type:
*     Protected virtual function.

*  Synopsis:
*     #include "frame.h"
*     AstFrame *PickAxes( AstFrame *this, int naxes, const int axes[],
*                         AstMapping **map )

*  Class Membership:
*     Frame method.

*  Description:
*     This function creates a new Frame whose axes are copies of axes
*     picked from an existing Frame. Other Frame attributes are also
*     copied to the new Frame. Zero or more of the original axes may
*     be picked in any order, but each can be used only
*     once. Additional axes (with default characteristics) may be
*     included in the new Frame if required.
*
*     Optionally, a Mapping that converts between the original Frame's
*     axes and those of the new Frame may also be returned.

*  Parameters:
*     this
*        Pointer to the original Frame.
*     naxes
*        The number of axes required in the new Frame.
*     axes
*        Pointer to an array of int with naxes elements. This should
*        contain (zero based) axis indices specifying the axes which
*        are to be included in the new Frame, in the order
*        required. Each axis index may occur only once.
*
*        If additional (default) axes are also to be included, the
*        corresponding elements of this array should be set to -1.
*     map
*        Address of a location to receive a pointer to a new
*        Mapping. This will be a PermMap (or a UnitMap as a special
*        case) that describes the axis permutation that has taken
*        place between the original and new Frames.  The forward
*        transformation will convert from the original Frame's axes to
*        the new one's, and vice versa.
*
*        If this Mapping is not required, a NULL value may be supplied
*        for this parameter.

*  Returned Value:
*     Pointer to the new Frame.

*  Notes:
*     - The class of object returned may differ from that of the
*     original Frame, depending on which axes are selected. For
*     example, if a single axis is picked from a SkyFrame (which
*     always has two axes), the resulting Frame cannot be a valid
*     SkyFrame, so will revert to the parent class (Frame) instead.
*     - The new Frame contains a deep copy of all the data selected
*     from the original Frame. Modifying the new Frame will therefore
*     not affect the original one.
*     - A NULL pointer will be returned if this function is invoked
*     with the global error status set, or if it should fail for any
*     reason.
*-

*  Implementation Notes:
*     - This function implements the basic astPickAxes method
*     available via the protected interface to the Frame class. The
*     public interface to this method is provided by the
*     astPickAxesId_ function.
*/

/* Local Variables: */
   AstFrame *frame;              /* Pointer to Frame to be returned */
   AstMapping *mapping;          /* Pointer to Mapping to be returned */

/* Check the global error status. */
   if ( !astOK ) return NULL;

/* Initialise the returned pointers. */
   frame = NULL;
   if ( map ) *map = NULL;

/* Check that a valid set of axes is being selected . */
   astValidateAxisSelection( this, naxes, axes, "astPickAxes" );

/* Create the required new Frame by selecting the axes. This also returns a
   Mapping which transforms between the original Frame and the new one. */
   astSubFrame( this, NULL, naxes, axes, NULL, &mapping, &frame );
   if ( astOK ) {

/* Return the Mapping pointer if required. */
      if ( map ) {
         *map = mapping;

/* Otherwise annul the Mapping. If an error occurs, also annul the Frame. */
      } else {
         mapping = astAnnul( mapping );
         if ( !astOK ) frame = astAnnul( frame );
      }
   }

/* Return the pointer to the new Frame. */
   return frame;
}

static void PrimaryFrame( AstFrame *this, int axis1,
                          AstFrame **frame, int *axis2 ) {
/*
*+
*  Name:
*     astPrimaryFrame

*  Purpose:
*     Uniquely identify a primary Frame and one of its axes.

*  Type:
*     Protected virtual function.

*  Synopsis:
*     #include "frame.h"
*     void astPrimaryFrame( AstFrame *this, int axis1, AstFrame **frame,
*                           int *axis2 )

*  Class Membership:
*     Frame method.

*  Description:
*     This function returns information about the underlying (primary) Frame
*     corresponding to a specified axis, when given what may be a compound
*     Frame composed of more than one simpler one.

*  Parameters:
*     this
*        Pointer to the Frame.
*     axis1
*        An axis index (zero-based) identifying the Frame axis for which
*        information is required.
*     frame
*        Address of a location to receive a pointer to the underlying (primary)
*        frame to which the requested axis belongs (i.e. this will not be a
*        compound Frame).
*     axis2
*        Pointer to an int which is to receive the (zero-based) axis
*        index within "frame" which identifies the axis being referred
*        to, using the axis order that applied when the primary Frame
*        was originally constructed (i.e. this function undoes all
*        subsequent axis pemutations and the effects of combining
*        Frames, in order to reveal the original underlying axis
*        order).

*  Notes:
*     -  This protected method is provided so that class implementations can
*     distinguish the axes of frames from one another (e.g. can distinguish
*     a longitude axis as being different from a latitide axis) even after
*     their order has been permuted and they have been combined with axes from
*     other Frames.
*     -  The reference count of the primary Frame will be incremented by one to
*     reflect the new pointer returned.
*-
*/

/* Check the global error status. */
   if ( !astOK ) return;

/* Initialise the returned values. */
   *frame = NULL;
   *axis2 = 0;

/* Validate and permute the axis index supplied. */
   axis1 = astValidateAxis( this, axis1, "astPrimaryFrame" );

/* Since "this" is a primary Frame (i.e. is not compound), simply clone a
   pointer to it. */
   if ( astOK ) *frame = astClone( this );

/* Return the permuted axis index, which refers to the original axis order. */
   if ( astOK ) *axis2 = axis1;
}

static void ReportPoints( AstMapping *this_mapping, int forward,
                          AstPointSet *in_points, AstPointSet *out_points ) {
/*
*  Name:
*     ReportPoints

*  Purpose:
*     Report the effect of transforming a set of points using a Frame.

*  Type:
*     Private function.

*  Synopsis:
*     #include "mapping.h"
*     void ReportPoints( AstMapping *this, int forward,
*                        AstPointSet *in_points, AstPointSet *out_points )

*  Class Membership:
*     Frame member function (over-rides the protected astReportPoints
*     method inherited from the Mapping class).

*  Description:
*     This function reports the coordinates of a set of points before
*     and after being transformed by a Frame, by writing them to
*     standard output.

*  Parameters:
*     this
*        Pointer to the Frame.
*     forward
*        A non-zero value indicates that the Frame's forward
*        coordinate transformation has been applied, while a zero
*        value indicates the inverse transformation.
*     in_points
*        Pointer to a PointSet which is associated with the
*        coordinates of a set of points before the Frame was applied.
*     out_points
*        Pointer to a PointSet which is associated with the
*        coordinates of the same set of points after the Frame has
*        been applied.
*/

/* Local Variables: */
   AstFrame *this;               /* Pointer to the Frame structure */
   double **ptr_in;              /* Pointer to array of input data pointers */
   double **ptr_out;             /* Pointer to array of output data pointers */
   int coord;                    /* Loop counter for coordinates */
   int ncoord_in;                /* Number of input coordinates per point */
   int ncoord_out;               /* Number of output coordinates per point */
   int npoint;                   /* Number of points to report */
   int npoint_in;                /* Number of input points */
   int npoint_out;               /* Number of output points */
   int point;                    /* Loop counter for points */

/* Check the global error status. */
   if ( !astOK ) return;

/* Obtain a pointer to the Frame structure. */
   this = (AstFrame *) this_mapping;

/* Obtain the numbers of points and coordinates associated with each
   PointSet. */ 
   npoint_in = astGetNpoint( in_points );
   npoint_out = astGetNpoint( out_points );
   ncoord_in = astGetNcoord( in_points );
   ncoord_out = astGetNcoord( out_points );

/* Obtain the pointers that give access to the coordinate data
   associated with each PointSet. */
   ptr_in = astGetPoints( in_points );
   ptr_out = astGetPoints( out_points );

/* In the event that both PointSets don't contain equal numbers of
   points (this shouldn't actually happen), simply use the minimum
   number. */
   npoint = ( npoint_in < npoint_out ) ? npoint_in : npoint_out;

/* Loop to report the effect of the transformation on each point in
   turn. */
   for ( point = 0; point < npoint; point++ ) {

/* Report the input coordinates (in parentheses and separated by
   commas). Format each value for display using the Frame's astFormat
   method. */
      printf( "(" );
      for ( coord = 0; coord < ncoord_in; coord++ ) {
         printf( "%s%s", coord ? ", " : "",
                 astFormat( this, coord, ptr_in[ coord ][ point ] ) );
      }

/* Similarly report the output coordinates. */
      printf( ") --> (" );
      for ( coord = 0; coord < ncoord_out; coord++ ) {
         printf( "%s%s", coord ? ", " : "",
                 astFormat( this, coord, ptr_out[ coord ][ point ] ) );
      }
      printf( ")\n" );
   }
}

static void SetAttrib( AstObject *this_object, const char *setting ) {
/*
*  Name:
*     SetAttrib

*  Purpose:
*     Set an attribute value for a Frame.

*  Type:
*     Private function.

*  Synopsis:
*     #include "frame.h"
*     void SetAttrib( AstObject *this, const char *setting )

*  Class Membership:
*     Frame member function (over-rides the astSetAttrib method inherited
*     from the Mapping class).

*  Description:
*     This function assigns an attribute value for a Frame, the
*     attribute and its value being specified by means of a string of
*     the form:
*
*        "attribute= value "
*
*     Here, "attribute" specifies the attribute name and should be in
*     lower case with no white space present. The value to the right
*     of the "=" should be a suitable textual representation of the
*     value to be assigned and this will be interpreted according to
*     the attribute's data type.  White space surrounding the value is
*     only significant for string attributes.

*  Parameters:
*     this
*        Pointer to the Frame.
*     setting
*        Pointer to a null terminated string specifying the new attribute
*        value.

*  Notes:
*     - This function uses one-based axis numbering so that it is
*     suitable for external (public) use.
*/

/* Local Vaiables: */
   AstAxis *ax;                  /* Pointer to Axis */
   AstFrame *this;               /* Pointer to the Frame structure */
   char *axis_setting;           /* Pointer to axis attribute setting string */
   int axis;                     /* Index for the Frame axis */
   int axis_nc;                  /* No. characters in axis attribute name */
   int axis_value;               /* Offset of value to be assigned to axis */
   int digits;                   /* Number of digits of precision */
   int direction;                /* Axis direction flag */
   int domain;                   /* Offset of Domain string */
   int format;                   /* Offset of axis Format string */
   int label;                    /* Offset of axis Label string */
   int len;                      /* Length of setting string */
   int match_end;                /* Match final axes of target? */
   int max_axes;                 /* Maximum number of axes matched */
   int min_axes;                 /* Minimum number of axes matched */
   int nc;                       /* Number of characters read by sscanf */
   int permute;                  /* Permute axes in order to match? */
   int preserve_axes;            /* Preserve matched target axes? */
   int symbol;                   /* Offset of axis Symbol string */
   int title;                    /* Offset of Title string */
   int unit;                     /* Offset of axis Unit string */

/* Check the global error status. */
   if ( !astOK ) return;

/* Obtain a pointer to the Frame structure. */
   this = (AstFrame *) this_object;

/* Obtain the length of the setting string. */
   len = strlen( setting );

/* Test for each recognised attribute in turn, using "sscanf" to parse the
   setting string and extract the attribute value (or an offset to it in the
   case of string values). In each case, use the value set in "nc" to check
   that the entire string was matched. Once a value has been obtained, use the
   appropriate method to set it. */

/* Digits. */
/* ------- */
   if ( nc = 0,
        ( 1 == sscanf( setting, "digits= %d %n", &digits, &nc ) )
        && ( nc >= len ) ) {
      astSetDigits( this, digits );

/* Digits(axis). */
/* ------------- */
   } else if ( nc = 0,
               ( 2 == sscanf( setting, "digits(%d)= %d %n",
                              &axis, &digits, &nc ) )
               && ( nc >= len ) ) {

/* There is no function to set the Digits attribute value for an axis
   directly, so obtain a pointer to the Axis and use this to set the
   attribute. */
      (void) astValidateAxis( this, axis - 1, "astSetDigits(axis)" );
      ax = astGetAxis( this, axis - 1 );
      astSetAxisDigits( ax, digits );
      ax = astAnnul( ax );

/* Direction(axis). */
/* ---------------- */
   } else if ( nc = 0,
               ( 2 == sscanf( setting, "direction(%d)= %d %n",
                              &axis, &direction, &nc ) )
               && ( nc >= len ) ) {
      astSetDirection( this, axis - 1, direction );

/* Domain. */
/* ------- */
   } else if ( nc = 0,
               ( 0 == sscanf( setting, "domain=%n%*[^\n]%n", &domain, &nc ) )
               && ( nc >= len ) ) {
      astSetDomain( this, setting + domain );

/* Format(axis). */
/* ------------- */
   } else if ( nc = 0,
               ( 1 == sscanf( setting, "format(%d)=%n%*[^\n]%n",
                              &axis, &format, &nc ) )
               && ( nc >= len ) ) {
      astSetFormat( this, axis - 1, setting + format );

/* Label(axis). */
/* ------------ */
   } else if ( nc = 0,
               ( 1 == sscanf( setting, "label(%d)=%n%*[^\n]%n",
                              &axis, &label, &nc ) )
               && ( nc >= len ) ) {
      astSetLabel( this, axis - 1, setting + label );

/* MatchEnd. */
/* --------- */
   } else if ( nc = 0,
               ( 1 == sscanf( setting, "matchend= %d %n", &match_end, &nc ) )
               && ( nc >= len ) ) {
      astSetMatchEnd( this, match_end );

/* MaxAxes. */
/* -------- */
   } else if ( nc = 0,
               ( 1 == sscanf( setting, "maxaxes= %d %n", &max_axes, &nc ) )
               && ( nc >= len ) ) {
      astSetMaxAxes( this, max_axes );

/* MinAxes. */
/* -------- */
   } else if ( nc = 0,
               ( 1 == sscanf( setting, "minaxes= %d %n", &min_axes, &nc ) )
               && ( nc >= len ) ) {
      astSetMinAxes( this, min_axes );

/* Permute. */
/* -------- */
   } else if ( nc = 0,
               ( 1 == sscanf( setting, "permute= %d %n", &permute, &nc ) )
               && ( nc >= len ) ) {
      astSetPermute( this, permute );

/* PreserveAxes. */
/* ------------- */
   } else if ( nc = 0,
               ( 1 == sscanf( setting, "preserveaxes= %d %n",
                              &preserve_axes, &nc ) )
               && ( nc >= len ) ) {
      astSetPreserveAxes( this, preserve_axes );

/* Symbol(axis). */
/* ------------- */
   } else if ( nc = 0,
               ( 1 == sscanf( setting, "symbol(%d)=%n%*[^\n]%n",
                              &axis, &symbol, &nc ) )
               && ( nc >= len ) ) {
      astSetSymbol( this, axis - 1, setting + symbol );

/* Title. */
/* ------ */
   } else if ( nc = 0,
               ( 0 == sscanf( setting, "title=%n%*[^\n]%n", &title, &nc ) )
               && ( nc >= len ) ) {
      astSetTitle( this, setting + title );

/* Unit(axis). */
/* ----------- */
   } else if ( nc = 0,
               ( 1 == sscanf( setting, "unit(%d)=%n%*[^\n]%n",
                              &axis, &unit, &nc ) )
               & ( nc >= len ) ) {
      astSetUnit( this, axis - 1, setting + unit );


/* Read-only attributes. */
/* --------------------- */
/* Define a macro to see if the setting string matches any of the
   read-only attributes of this class. */
#define MATCH(attrib) \
        ( nc = 0, ( 0 == sscanf( setting, attrib "=%*[^\n]%n", &nc ) ) && \
                  ( nc >= len ) )

/* Use this macro to report an error if a read-only attribute has been
   specified. */
   } else if ( MATCH( "naxes" ) ) {
      astError( AST__NOWRT, "astSet: The setting \"%s\" is invalid for a %s.",
                setting, astGetClass( this ) );
      astError( AST__NOWRT, "This is a read-only attribute." );

/* Other axis attributes. */
/* ---------------------- */
/* If the attribute was not identified above, but appears to refer to
   a Frame axis, then it may refer to an Axis object of a derived type
   (which has additional attributes not recognised here). */
   } else if ( nc = 0,
               ( 1 == sscanf( setting, "%*[^()]%n(%d)%n=%*[^\n]%n",
                                       &axis_nc, &axis, &axis_value, &nc ) )
               && ( nc >= len ) ) {

/* Validate the axis index and copy the attribute setting string. */
      (void) astValidateAxis( this, axis - 1, "astSet" );
      axis_setting = astString( setting, len );
      if ( astOK ) {

/* Over-write the axis index in the copy with the value to be
   assigned. */
         (void) strcpy( axis_setting + axis_nc, setting + axis_value );

/* Obtain a pointer to the Axis object and use its astSetAttrib method
   to set the value. */
         ax = astGetAxis( this, axis - 1 );
         astSetAttrib( ax, axis_setting );

/* Annul the Axis pointer and free the memory holding the attribute
   setting. */
         ax = astAnnul( ax );
      }
      axis_setting = astFree( axis_setting );

/* Not recognised. */
/* --------------- */
/* If the attribute is still not recognised, pass it on to the parent
   method for further interpretation. */
   } else {
      (*parent_setattrib)( this_object, setting );
   }

/* Undefine macros local to this function. */
#undef MATCH
}

static void SetAxis( AstFrame *this, int axis, AstAxis *newaxis ) {
/*
*+
*  Name:
*     astSetAxis

*  Purpose:
*     Set a new Axis for a Frame.

*  Type:
*     Protected virtual function.

*  Synopsis:
*     #include "frame.h"
*     void astSetAxis( AstFrame *this, int axis, AstAxis *newaxis )

*  Class Membership:
*     Frame method.

*  Description:
*     This function allows a new Axis object to be associated with one
*     of the axes of a Frame, replacing the previous one. Each Axis
*     object contains a description of the quantity represented along
*     one of the Frame's axes, so this function allows this
*     description to be exchanged for another one.

*  Parameters:
*     this
*        Pointer to the Frame.
*     axis
*        The index (zero-based) of the axis whose associated Axis object is to
*        be replaced.
*     newaxis
*        Pointer to the new Axis object.
*-
*/

/* Check the global error status. */
   if ( !astOK ) return;

/* Validate and permute the axis index supplied. */
   axis = astValidateAxis( this, axis, "astSetAxis" );

/* If OK, annul the Frame's pointer to the old Axis object and clone a pointer
   to the new one to replace it. */
   if ( astOK ) {
      this->axis[ axis ] = astAnnul( this->axis[ axis ] );
      this->axis[ axis ] = astClone( newaxis );
   }
}

static int SubFrame( AstFrame *target, AstFrame *template,
                     int result_naxes, const int *target_axes,
                     const int *template_axes, AstMapping **map,
                     AstFrame **result ) {
/*
*+
*  Name:
*     astSubFrame

*  Purpose:
*     Select axes from a Frame and convert to the new coordinate system.

*  Type:
*     Protected virtual function.

*  Synopsis:
*     #include "frame.h"
*     int astSubFrame( AstFrame *target, AstFrame *template,
*                      int result_naxes, const int *target_axes,
*                      const int *template_axes, AstMapping **map,
*                      AstFrame **result )

*  Class Membership:
*     Frame method.

*  Description:
*     This function selects a requested sub-set (or super-set) of the axes from
*     a "target" Frame and creates a new Frame with copies of the selected
*     axes assembled in the requested order. It then optionally overlays the
*     attributes of a "template" Frame on to the result. It returns both the
*     resulting Frame and a Mapping that describes how to convert between the
*     coordinate systems described by the target and result Frames. If
*     necessary, this Mapping takes account of any differences in the Frames'
*     attributes due to the influence of the template.

*  Parameters:
*     target
*        Pointer to the target Frame, from which axes are to be selected.
*     template
*        Pointer to the template Frame, from which new attributes for the
*        result Frame are to be obtained. Optionally, this may be NULL, in
*        which case no overlaying of template attributes will be performed.
*     result_naxes
*        Number of axes to be selected from the target Frame. This number may
*        be greater than or less than the number of axes in this Frame (or
*        equal).
*     target_axes
*        Pointer to an array of int with result_naxes elements, giving a list
*        of the (zero-based) axis indices of the axes to be selected from the
*        target Frame. The order in which these are given determines the order
*        in which the axes appear in the result Frame. If any of the values in
*        this array is set to -1, the corresponding result axis will not be
*        derived from the target Frame, but will be assigned default attributes
*        instead.
*     template_axes
*        Pointer to an array of int with result_naxes elements. This should
*        contain a list of the template axes (given as zero-based axis indices)
*        with which the axes of the result Frame are to be associated. This
*        array determines which axes are used when overlaying axis-dependent
*        attributes of the template on to the result. If any element of this
*        array is set to -1, the corresponding result axis will not receive any
*        template attributes.
*
*        If the template argument is given as NULL, this array is not used and
*        a NULL pointer may also be supplied here.
*     map
*        Address of a location to receive a pointer to the returned Mapping.
*        The forward transformation of this Mapping will describe how to
*        convert coordinates from the coordinate system described by the target
*        Frame to that described by the result Frame. The inverse
*        transformation will convert in the opposite direction.
*     result
*        Address of a location to receive a pointer to the result Frame.

*  Returned Value:
*     A non-zero value is returned if coordinate conversion is
*     possible between the target and the result Frame. Otherwise zero
*     is returned and *map and *result are returned as NULL (but this
*     will not in itself result in an error condition). In general,
*     coordinate conversion should always be possible if no template
*     Frame is supplied but may not always be possible otherwise.

*  Notes:
*     - A value of zero will be returned if this function is invoked
*     with the global error status set, or if it should fail for any
*     reason.

*  Implementation Deficiencies:
*     - Any axis selection is currently permitted. Probably this
*     should be restricted so that each axis can only be selected
*     once. The astValidateAxisSelection method will do this but
*     currently there are bugs in the CmpFrame class that cause axis
*     selections which will not pass this test. Install the validation
*     when these are fixed.
*-

*  Implementation Notes:
*     - This implementation addresses the selection of axes from a
*     Frame class object. This simply results in another object of the
*     same class and a Mapping which describes an axis permutation (or
*     a unit Mapping as a special case). Changes of Frame attributes
*     have no significance for coordinate values in this class, so do
*     not affect the Mapping returned.
*/

/* Local Variables: */
   AstAxis *newaxis;             /* Pointer to new Axis object */
   AstFrame *tempframe;          /* Pointer to temporary Frame */
   int *inperm;                  /* Pointer to permutation array */
   int *outperm;                 /* Pointer to permutation array */
   int match;                    /* Coordinate conversion possible? */
   int result_axis;              /* Result Frame axis index */
   int target_axis;              /* Target Frame axis index */
   int target_naxes;             /* Number of target Frame axes */
   int unit;                     /* Unit Mapping appropriate? */

/* Initialise the returned values. */
   *map = NULL;
   *result = NULL;
   match = 0;

/* Check the global error status. */
   if ( !astOK ) return match;

/* Obtain the number of target Frame axes. */
   target_naxes = astGetNaxes( target );

/* Ensure we do not attempt to use a negative number of result axes. */
   if ( result_naxes < 0 ) result_naxes = 0;

/* Create a temporary new Frame with the required number of axes. This will
   have a default Axis object associated with each of its axes. We will
   replace these where necessary with copies of the actual Axis objects we
   require. */
   tempframe = astFrame( result_naxes, "" );

/* Allocate memory to store two permutation arrays. These will be used to
   construct the Mapping that relates the target and result Frames. */
   inperm = astMalloc( sizeof( int ) * (size_t) target_naxes );
   outperm = astMalloc( sizeof( int ) * (size_t) result_naxes );
   if ( astOK ) {

/* Initialise the array that associates each target axis with the corresponding
   result axis (filling it with the value -1 initially signifies no
   associations). */
      for ( target_axis = 0; target_axis < target_naxes; target_axis++ ) {
         inperm[ target_axis ] = -1;
      }

/* Loop through each axis in the result Frame and obtain the index of the axis
   in the target Frame from which it is to be derived. */
      for ( result_axis = 0; result_axis < result_naxes; result_axis++ ) {
         target_axis = target_axes[ result_axis ];

/* Check if the resulting axis index is valid. If not, this result axis is not
   to be derived from any target axis, and it will therefore be left with its
   default attributes. Make an entry in the appropriate permutation array to
   indicate that this result axis is unassociated. */
         if ( ( target_axis < 0 ) || ( target_axis >= target_naxes ) ) {
            outperm[ result_axis ] = -1;

/* Otherwise, obtain a pointer to the target Axis object and modify the
   temporary Frame so that its axis is associated with the same Axis object.
   Annul the Axis pointer afterwards. */
         } else {
            newaxis = astGetAxis( target, target_axis );
            astSetAxis( tempframe, result_axis, newaxis );
            newaxis = astAnnul( newaxis );

/* Update both permutation arrays to record the association between the target
   and result axes. */
            outperm[ result_axis ] = target_axis;
            inperm[ target_axis ] = result_axis;
         }

/* Quit looping if an error occurs. */
         if ( !astOK ) break;
      }

/* So far, we have only modified pointers in the temporary Frame to refer to
   the target Frame's Axis objects. Since we will next modify these objects'
   attributes, we must make a deep copy of the entire temporary Frame so that
   we do not modify the target's axes. This copy now becomes our result Frame.
   Annul the temporary one. */
      if ( astOK ) {
         *result = astCopy( tempframe );
         tempframe = astAnnul( tempframe );

/* Invoke the target "astOverlay" method to overlay any remaining
   attributes from the target Frame which are not associated with
   individual axes (e.g.  the Frame's Title and Domain). */
         astOverlay( target, target_axes, *result );

/* If a template Frame was supplied, also invoke its astOverlay method to
   overlay its attributes on the result Frame. (Note that in this particular
   case this has no effect other than transferring attributes. In general,
   however, i.e. in derived classes, this process is vital to determining the
   mapping below, whose main purpose is to convert between the target and
   result Frames. These will have different attributes as a result of the
   influence that the template has here.) */
         if ( template ) astOverlay( template, template_axes, *result );

/* We will next generate the Mapping that relates the target and result
   Frames. If appropriate this should be a unit Mapping (UnitMap), so test if
   the number of axes in both Frames is equal. */
         unit = ( target_naxes == result_naxes );

/* If so, check the contents of one of the permutation arrays to see if all
   result axes are associated with the corresponding target axis (the converse
   then also follows). If not, note this fact and quit checking. */
         if ( unit ) {
            for ( result_axis = 0; result_axis < result_naxes;
                                   result_axis++ ) {
               if ( outperm[ result_axis ] != result_axis ) {
                  unit = 0;
                  break;
	       }
	    }
	 }

/* If a unit Mapping is appropriate, then construct it. */
         if ( unit ) {
            *map = (AstMapping *) astUnitMap( result_naxes, "" );

/* Otherwise, construct a Mapping describing the axis permutation we have
   produced. */
         } else {
            *map = (AstMapping *) astPermMap( target_naxes, inperm,
                                              result_naxes, outperm, NULL,
                                              "" );
         }

/* Note that coordinate conversion is possible (this will always be the case
   here unless an error occurs). */
         match = 1;
      }
   }

/* Free the memory used for the permutation arrays. */
   inperm = astFree( inperm );
   outperm = astFree( outperm );

/* If an error occurred, annul the returned objects and reset the returned
   value. */
   if ( !astOK ) {
      *map = astAnnul( *map );
      *result = astAnnul( *result );
      match = 0;
   }

/* Return the result. */
   return match;
}

static int TestAttrib( AstObject *this_object, const char *attrib ) {
/*
*  Name:
*     TestAttrib

*  Purpose:
*     Test if a specified attribute value is set for a Frame.

*  Type:
*     Private function.

*  Synopsis:
*     #include "frame.h"
*     int TestAttrib( AstObject *this, const char *attrib )

*  Class Membership:
*     Frame member function (over-rides the astTestAttrib protected
*     method inherited from the Mapping class).

*  Description:
*     This function returns a boolean result (0 or 1) to indicate whether
*     a value has been set for one of a Frame's attributes.

*  Parameters:
*     this
*        Pointer to the Frame.
*     attrib
*        Pointer to a null terminated string specifying the attribute
*        name.  This should be in lower case with no surrounding white
*        space.

*  Returned Value:
*     One if a value has been set, otherwise zero.

*  Notes:
*     - This function uses one-based axis numbering so that it is
*     suitable for external (public) use.
*     - A value of zero will be returned if this function is invoked
*     with the global status set, or if it should fail for any reason.
*/

/* Local Variables: */
   AstAxis *ax;                  /* Pointer to Axis */
   AstFrame *this;               /* Pointer to the Frame structure */
   char *axis_attrib;            /* Pointer to axis attribute name */
   int axis;                     /* Frame axis number */
   int axis_nc;                  /* No. characters in axis attribute name */
   int len;                      /* Length of attrib string */
   int nc;                       /* No. characters read by sscanf */
   int result;                   /* Result value to return */

/* Initialise. */
   result = 0;

/* Check the global error status. */
   if ( !astOK ) return result;

/* Obtain a pointer to the Frame structure. */
   this = (AstFrame *) this_object;

/* Obtain the length of the attrib string. */
   len = strlen( attrib );

/* Check the attribute name and test the appropriate attribute. */

/* Digits. */
/* ------- */
   if ( !strcmp( attrib, "digits" ) ) {
      result = astTestDigits( this );

/* Digits(axis). */
/* ------------- */
   } else if ( nc = 0,
               ( 1 == sscanf( attrib, "digits(%d)%n", &axis, &nc ) )
               && ( nc >= len ) ) {

/* There is no function to test the Digits attribute for an axis
   directly, so obtain a pointer to the Axis and use this to test the
   attribute. */
      (void) astValidateAxis( this, axis - 1, "astTestDigits(axis)" );
      ax = astGetAxis( this, axis - 1 );
      astTestAxisDigits( ax );
      ax = astAnnul( ax );

/* Direction(axis). */
/* ---------------- */
   } else if ( nc = 0,
               ( 1 == sscanf( attrib, "direction(%d)%n", &axis, &nc ) )
               && ( nc >= len ) ) {
      result = astTestDirection( this, axis - 1 );

/* Domain. */
/* ------- */
   } else if ( !strcmp( attrib, "domain" ) ) {
      result = astTestDomain( this );

/* Format(axis). */
/* ------------- */
   } else if ( nc = 0,
               ( 1 == sscanf( attrib, "format(%d)%n", &axis, &nc ) )
               && ( nc >= len ) ) {
      result = astTestFormat( this, axis - 1 );

/* Label(axis). */
/* ------------ */
   } else if ( nc = 0,
               ( 1 == sscanf( attrib, "label(%d)%n", &axis, &nc ) )
               && ( nc >= len ) ) {
      result = astTestLabel( this, axis - 1 );

/* MatchEnd. */
/* --------- */
   } else if ( !strcmp( attrib, "matchend" ) ) {
      result = astTestMatchEnd( this );

/* MaxAxes. */
/* -------- */
   } else if ( !strcmp( attrib, "maxaxes" ) ) {
      result = astTestMaxAxes( this );

/* MinAxes. */
/* -------- */
   } else if ( !strcmp( attrib, "minaxes" ) ) {
      result = astTestMinAxes( this );

/* Permute. */
/* -------- */
   } else if ( !strcmp( attrib, "permute" ) ) {
      result = astTestPermute( this );

/* PreserveAxes. */
/* ------------- */
   } else if ( !strcmp( attrib, "preserveaxes" ) ) {
      result = astTestPreserveAxes( this );

/* Symbol(axis). */
/* ------------- */
   } else if ( nc = 0,
               ( 1 == sscanf( attrib, "symbol(%d)%n", &axis, &nc ) )
               && ( nc >= len ) ) {
      result = astTestSymbol( this, axis - 1 );

/* Title. */
/* ------ */
   } else if ( !strcmp( attrib, "title" ) ) {
      result = astTestTitle( this );

/* Unit(axis). */
/* ----------- */
   } else if ( nc = 0,
               ( 1 == sscanf( attrib, "unit(%d)%n", &axis, &nc ) )
               && ( nc >= len ) ) {
      result = astTestUnit( this, axis - 1 );

/* Read-only attributes. */
/* --------------------- */
/* Test if the attribute name matches any of the read-only attributes
   of this class. If it does, then return zero. */
   } else if ( !strcmp( attrib, "naxes" ) ) {
      result = 0;

/* Other axis attributes. */
/* ---------------------- */
/* If the attribute was not identified above, but appears to refer to
   a Frame axis, then it may refer to an Axis object of a derived type
   (which has additional attributes not recognised here). */
   } else if ( nc = 0,
               ( 1 == sscanf( attrib, "%*[^()]%n(%d)%n",
                                      &axis_nc, &axis, &nc ) )
               && ( nc >= len ) ) {

/* Validate the axis index and extract the attribute name. */
      (void) astValidateAxis( this, axis - 1, "astTest" );
      axis_attrib = astString( attrib, axis_nc );

/* Obtain a pointer to the Axis object and use its astTestAttrib
   method to test the attribute value. */
      ax = astGetAxis( this, axis - 1 );
      result = astTestAttrib( ax, axis_attrib );

/* Annul the Axis pointer and free the memory holding the attribute
   name. */
      ax = astAnnul( ax );
      axis_attrib = astFree( axis_attrib );

/* Not recognised. */
/* --------------- */
/* If the attribute is still not recognised, pass it on to the parent
   method for further interpretation. */
   } else {
      result = (*parent_testattrib)( this_object, attrib );
   }

/* Return the result, */
   return result;
}

static AstPointSet *Transform( AstMapping *this_mapping, AstPointSet *in,
                               int forward, AstPointSet *out ) {
/*
*  Name:
*     Transform

*  Purpose:
*     Use a Frame to transform a set of points.

*  Type:
*     Private function.

*  Synopsis:
*     #include "frame.h"
*     AstPointSet *Transform( AstMapping *this, AstPointSet *in,
*                             int forward, AstPointSet *out )

*  Class Membership:
*     Frame member function (over-rides the astTransform method inherited
*     from the Mapping class).

*  Description:
*     This function takes a Frame and a set of points encapsulated in a
*     PointSet and transforms the points so as to perform the identity
*     transformation (i.e. simply copies the coordinate values).

*  Parameters:
*     this
*        Pointer to the Frame.
*     in
*        Pointer to the PointSet holding the input coordinate data.
*     forward
*        A non-zero value indicates that the forward coordinate transformation
*        should be applied, while a zero value requests the inverse
*        transformation. In this case, both transformations are equivalent.
*     out
*        Pointer to a PointSet which will hold the transformed (output)
*        coordinate values. A NULL value may also be given, in which case a
*        new PointSet will be created by this function.

*  Returned Value:
*     Pointer to the output (possibly new) PointSet.

*  Notes:
*     -  A null pointer will be returned if this function is invoked with the
*     global error status set, or if it should fail for any reason.
*     -  The number of coordinate values per point in the input PointSet must
*     match the number of coordinates for the Frame being applied. This number
*     will be equal to the number of Frame axes.
*     -  If an output PointSet is supplied, it must have space for sufficient
*     number of points and coordinate values per point to accommodate the
*     result. Any excess space will be ignored.
*/

/* Local Variables: */
   AstFrame *this;               /* Pointer to the Frame structure */
   AstPointSet *result;          /* Pointer value to be returned */
   AstUnitMap *unitmap;          /* Pointer to temporary UnitMap */

/* Check the global error status. */
   if ( !astOK ) return NULL;

/* Obtain a pointer to the Frame structure. */
   this = (AstFrame *) this_mapping;

/* Create a unit Mapping with one coordinate for each Frame axis. */
   unitmap = astUnitMap( astGetNaxes( this ), "" );

/* Use the Mapping to transform (i.e. copy) the coordinate values. */   
   result = astTransform( unitmap, in, forward, out );

/* Annul the Mapping. */
   unitmap = astAnnul( unitmap );

/* If an error occurred and a new PointSet may have been created, then annul
   the result. In any case, ensure that a NULL pointer is returned. */
   if ( !astOK ) {
      if ( !out ) result = astAnnul( result );
      result = NULL;
   }

/* Return the result pointer. */
   return result;
}

static int Unformat( AstFrame *this, int axis, const char *string,
                     double *value ) {
/*
*+
*  Name:
*     astUnformat

*  Purpose:
*     Read a formatted coordinate value for a Frame axis.

*  Type:
*     Protected virtual function.

*  Synopsis:
*     #include "frame.h"
*     int astUnformat( AstFrame *this, int axis, const char *string,
*                      double *value )

*  Class Membership:
*     Frame method.

*  Description:
*     This function reads a formatted coordinate value for a Frame
*     axis (supplied as a string) and returns the equivalent numerical
*     value as a double. It also returns the number of characters read
*     from the string.

*  Parameters:
*     this
*        Pointer to the Frame.
*     axis
*        The number of the Frame axis for which the coordinate value
*        is to be read (axis numbering starts at zero for the first
*        axis).
*     string
*        Pointer to a constant null-terminated string containing the
*        formatted coordinate value.
*     value
*        Pointer to a double in which the coordinate value read will be
*        returned.

*  Returned Value:
*     The number of characters read from the string to obtain the
*     coordinate value.

*  Notes:
*     - Any white space at the beginning of the string will be
*     skipped, as also will any trailing white space following the
*     coordinate value read. The function's return value will reflect
*     this.
*     - A function value of zero (and no coordinate value) will be
*     returned, without error, if the string supplied does not contain
*     a suitably formatted value.
*     - The string "<bad>" is recognised as a special case and will
*     generate the value AST__BAD, without error. The test for this
*     string is case-insensitive and permits embedded white space.
*     - A function result of zero will be returned and no coordinate
*     value will be returned via the "value" pointer if this function
*     is invoked with the global error status set, or if it should
*     fail for any reason.
*-

*  Implementation Notes:
*     - This function implements the basic astUnformat method
*     available via the protected interface to the Frame class. The
*     public interface to this method is provided by the
*     astUnformatId_ function.
*/

/* Local Variables: */
   AstAxis *ax;                  /* Pointer to Axis object */
   const char *label;            /* Pointer to axis label string */
   double coord;                 /* Coordinate value read */
   int digits_set;               /* Axis Digits attribute set? */
   int nc;                       /* Number of characters read */
   int status;                   /* AST error status */

/* Initialise. */
   nc = 0;

/* Check the global error status. */
   if ( !astOK ) return nc;

/* Validate the axis index and obtain a pointer to the required Axis. */
   (void) astValidateAxis( this, axis, "astUnformat" );
   ax = astGetAxis( this, axis );

/* Test if any Axis attributes which may affect the result are
   undefined (i.e. have not been explicitly set). If so, we over-ride
   them, giving them temporary values dictated by the Frame. Only the
   Digits attribute is potentially relevant here. */
   digits_set = astTestAxisDigits( ax );
   if ( !digits_set ) astSetAxisDigits( ax, astGetDigits( this ) );

/* Read the coordinate value. */
   if ( astOK ) {
      nc = astAxisUnformat( ax, string, &coord );

/* If an error occurred, save and temporarily clear the global error
   status while the axis Label string is obtained. Then restore the
   original error status value afterwards. */
      if ( !astOK ) {
         status = astStatus;
         astClearStatus;
         label = astGetLabel( this, axis );
         astSetStatus( status );

/* Report a contextual error message containing the axis label. */
         astError( status, "astUnformat(%s): Unable to read \"%s\" value.",
                   astGetClass( this ), label );
      }
   }

/* Clear any Axis attributes that were temporarily over-ridden. */
   if ( !digits_set ) astClearAxisDigits( ax );

/* Annul the Axis pointer. */
   ax = astAnnul( ax );

/* If an error occurred, clear the count of characters read. */
   if ( !astOK ) {
      nc = 0;

/* Otherwise, if characters were read, return the coordinate value. */
   } else if ( nc ) {
      *value = coord;
   }

/* Return the number of characters read. */
   return nc;
}

static int ValidateAxis( AstFrame *this, int axis, const char *method ) {
/*
*+
*  Name:
*     astValidateAxis

*  Purpose:
*     Validate and permute a Frame's axis index.

*  Type:
*     Protected virtual function.

*  Synopsis:
*     #include "frame.h"
*     int astValidateAxis( AstFrame *this, int axis, const char *method )

*  Class Membership:
*     Frame method.

*  Description:
*     This function checks the validity of an index (zero-based) which
*     is to be used to address one of the coordinate axes of a
*     Frame. If the index is valid, it is permuted using the axis
*     permutation array associated with the Frame and the (zero-based)
*     permuted axis index is returned.  This gives the location of the
*     required axis information within the Frame's internal arrays. If
*     the axis index supplied is not valid, an error is reported and
*     the global error status is set.

*  Parameters:
*     this
*        Pointer to the Frame.
*     axis
*        The axis index (zero-based) to be checked. To be valid, it
*        must lie between zero and (naxes-1) inclusive, where "naxes"
*        is the number of coordinate axes associated with the Frame.
*     method
*        Pointer to a constant null-terminated character string
*        containing the name of the method that invoked this function
*        to validate an axis index. This method name is used solely
*        for constructing error messages.

*  Returned Value:
*     The permuted axis index.

*  Notes:
*     - A value of zero will be returned if this function is invoked
*     with the global error status set, or if it should fail for any
*     reason.
*     - Error messages issued by this function refer to the external
*     (public) numbering system used for axes (which is one-based),
*     whereas zero-based axis indices are used internally.
*-
*/

/* Local Variables: */
   const int *perm;              /* Pointer to axis permutation array */
   int naxes;                    /* Number of Frame axes */
   int result;                   /* Permuted axis index */

/* Initialise. */
   result = 0;

/* Check the global error status. */
   if ( !astOK ) return result;

/* Determine the number of Frame axes. */
   naxes = astGetNaxes( this );
   if ( astOK ) {

/* If the Frame has no axes, report an error (note we convert to
   one-based axis numbering in the error message). */
      if ( naxes == 0 ) {
         astError( AST__AXIIN, "%s(%s): Invalid attempt to use an axis index "
                   "(%d) for a %s which has no axes.", method,
                   astGetClass( this ), axis + 1, astGetClass( this ) );

/* Otherwise, check the axis index for validity and report an error if
   it is not valid (again, use one-based axis numbering). */
      } else if ( ( axis < 0 ) || ( axis >= naxes ) ) {
         astError( AST__AXIIN, "%s(%s): Axis index (%d) invalid - it should "
                   "be in the range 1 to %d.", method, astGetClass( this ),
                   axis + 1, naxes );

/* If the axis index was valid, obtain the axis permutation array and
   use this to generate the permuted axis value. */
      } else {
         perm = astGetPerm( this );
         if ( astOK ) result = perm[ axis ];
      }
   }

/* Return the result. */
   return result;
}

static void ValidateAxisSelection( AstFrame *this, int naxes, const int *axes,
                                   const char *method ) {
/*
*+
*  Name:
*     astValidateAxisSelection

*  Purpose:
*     Check that a set of axes selected from a Frame is valid.

*  Type:
*     Protected virtual function.

*  Synopsis:
*     #include "frame.h"
*     void astValidateAxisSelection( AstFrame *this, int naxes,
*                                    const int *axes, const char *method )

*  Class Membership:
*     Frame method.

*  Description:
*     This function checks the validity of an array of (zero-based)
*     axis indices that specify a set of axes to be selected from a
*     Frame. To be valid, no axis should be selected more than
*     once. In assessing this, any axis indices that do not refer to
*     valid Frame axes (e.g. are set to -1) are ignored.
*
*     If the axis selection is valid, this function returns without further
*     action. Otherwise, an error is reported and the global error status is
*     set.

*  Parameters:
*     this
*        Pointer to the Frame.
*     naxes
*        The number of axes to be selected (may be zero).
*     axes
*        Pointer to an array of int with naxes elements that contains the
*        (zero based) axis indices to be checked.
*     method
*        Pointer to a constant null-terminated character string
*        containing the name of the method that invoked this function
*        to validate an axis selection. This method name is used
*        solely for constructing error messages.
*-
*/

/* Local Variables: */
   int *count;                   /* Pointer to temporary array of counts */
   int axis;                     /* Loop counter for selected axes */
   int frame_axis;               /* Loop counter for Frame axes */
   int frame_naxes;              /* Number of Frame axes */
   int valid;                    /* Axis selection valid? */

/* Check the global error status. */
   if ( !astOK ) return;
   
/* Check to see if no axes have been selected. If so, there is nothing to
   do. */
   if ( naxes ) {

/* Initialise. */
      valid = 1;

/* Obtain the number of Frame axes and allocate an array of int with
   one element for each Frame axis. This will store a count of the
   number of times each axis is selected. */
      frame_naxes = astGetNaxes( this );
      count = astMalloc( sizeof( int ) * (size_t) frame_naxes );
      if ( astOK ) {

/* Initialise the array of counts to zero. */
         for ( frame_axis = 0; frame_axis < frame_naxes; frame_axis++ ) {
            count[ frame_axis ] = 0;
         }

/* Loop through each selected axis. */
         for ( axis = 0; axis < naxes; axis++ ) {
            frame_axis = axes[ axis ];

/* Check if the selected axis index is valid for the Frame. If so, increment
   the selection count for that Frame axis. */
            if ( ( frame_axis >= 0 ) && ( frame_axis < frame_naxes ) ) {
               count[ frame_axis ]++;
            }
         }

/* Loop through the count array and check that no Frame axis was selected
   more than once. If it was, clear the "valid" flag and quit checking. */
         for ( frame_axis = 0; frame_axis < frame_naxes; frame_axis++ ) {
            if ( count[ frame_axis ] > 1 ) {
               valid = 0;
               break;
            }
         }
      }

/* Free the temporary count array. */
      count = astFree( count );

/* If no error has occurred, but the axis selection is not valid, then report
   an error. */
      if ( astOK && !valid ) {
         astError( AST__SELIN, "%s(%s): Invalid axis selection - each axis "
                   "may be selected only once.", method, astGetClass( this ) );
      }
   }
}

/* Functions which access class attributes. */
/* ---------------------------------------- */
/* Implement member functions to access the attributes associated with
   the axes of a Frame using the private macros defined for this
   purpose at the start of this file. */
/*
*att++
*  Name:
*     Direction(axis)

*  Purpose:
*     Display axis in conventional direction?

*  Type:
*     Public attribute.

*  Synopsis:
*     Integer (boolean).

*  Description:
*     This attribute is a boolean value which suggests how the axes of
*     a Frame should be displayed (e.g.) in graphical output. By
*     default, it has the value one, indicating that they should be
*     shown in the conventional sense (increasing left to right for an
*     abscissa, and bottom to top for an ordinate). If set to zero,
*     this attribute indicates that the direction should be reversed,
*     as would often be done for an astronomical magnitude or a right
*     ascension axis.

*  Applicability:
*     Frame
*        The default Direction value supplied by the Frame class is 1,
*        indicating that all axes should be displayed in the
*        conventional direction.
*     SkyFrame
*        The SkyFrame class re-defines the default Direction value to
*        suggest that certain axes (e.g. right ascension) should be
*        plotted in reverse when appropriate.
*     FrameSet
*        The Direction attribute of a FrameSet axis is the same as
*        that of its current Frame (as specified by the Current
*        attribute).

*  Notes:
*     - When specifying this attribute by name, it should be
*     subscripted with the number of the Frame axis to which it
*     applies.
*     - The Direction attribute does not directly affect the behaviour
*     of the AST library. Instead, it serves as a hint to applications
*     programs about the orientation in which they may wish to display
*     any data associated with the Frame. Applications are free to
*     ignore this hint if they wish.
*att--
*/
/* This simply provides an interface to the Axis methods for accessing
   the Direction flag. */
MAKE_CLEAR(Direction)
MAKE_GET(Direction,int,0,0,0)
MAKE_SET(Direction,int)
MAKE_TEST(Direction)

/*
*att++
*  Name:
*     Format(axis)

*  Purpose:
*     Format specification for axis values.

*  Type:
*     Public attribute.

*  Synopsis:
*     String.

*  Description:
*     This attribute specifies the format to be used when displaying
*     coordinate values associated with a particular Frame axis
*     (i.e. to convert values from binary to character form). It is
c     interpreted by the astFormat function and determines the
f     interpreted by the AST_FORMAT function and determines the
*     formatting which it applies.
*
*     If no Format value is set for a Frame axis, a default value is
*     supplied instead. This is based on the value of the Digits, or
*     Digits(axis), attribute and is chosen so that it displays the
*     requested number of digits of precision.

*  Applicability:
*     Frame
*        The Frame class interprets this attribute as a format
*        specification string to be passed to the C "printf" function
*        (e.g. "%1.7G") in order to format a single coordinate value
*        (supplied as a double precision number).
c
c        When supplying a value for this attribute, beware that the
c        "%" character may be interpreted directly as a format
c        specification by some printf-like functions (such as
c        astSet). You may need to double it (i.e. use "%%") to avoid
c        this.
*     SkyFrame
*        The SkyFrame class re-defines the syntax and default value of
*        the Format string to allow the formatting of sexagesimal
*        values as appropriate for the particular celestial coordinate
*        system being represented. The syntax of SkyFrame Format
*        strings is described (below) in the "SkyFrame Formats"
*        section.
*     FrameSet
*        The Format attribute of a FrameSet axis is the same as that
*        of its current Frame (as specified by the Current
*        attribute). Note that the syntax of the Format string is also
*        determined by the current Frame.

*  SkyFrame Formats:
*     The Format string supplied for a SkyFrame should contain zero or
*     more of the following characters. These may occur in any order,
*     but the following is recommended for clarity:
*
*     - "+": Indicates that a plus sign should be prefixed to positive
*     values. By default, no plus sign is used.
*
*     - "z": Indicates that leading zeros should be prefixed to the
*     value so that the first field is of constant width, as would be
*     required in a fixed-width table (leading zeros are always
*     prefixed to any fields that follow). By default, no leading
*     zeros are added.
*
*     - "i": Use the standard ISO field separator (a colon) between
*     fields. This is the default behaviour.
*
*     - "b": Use a blank to separate fields.
*
*     - "l": Use a letter ("h"/"d", "m" or "s" as appropriate) to
*     separate fields.
*
*     - "d": Include a degrees field. Expressing the angle purely in
*     degrees is also the default if none of "h", "m", "s" or "t" are
*     given.
*
*     - "h": Express the angle as a time and include an hours field
*     (where 24 hours correspond to 360 degrees). Expressing the angle
*     purely in hours is also the default if "t" is given without
*     either "m" or "s".
*
*     - "m": Include a minutes field. By default this is not included.
*
*     - "s": Include a seconds field. By default this is not included.
*     This request is ignored if "d" or "h" is given, unless a minutes
*     field is also included.
*
*     - "t": Express the angle as a time (where 24 hours correspond to
*     360 degrees). This option is ignored if either "d" or "h" is
*     given and is intended for use where the value is to be expressed
*     purely in minutes and/or seconds of time (with no hours
*     field). If "t" is given without "d", "h", "m" or "s" being
*     present, then it is equivalent to "h".
*
*     - ".": Indicates that decimal places are to be given for the
*     final field in the formatted string (whichever field this
*     is). The "." should be followed immediately by an unsigned
*     integer which gives the number of decimal places required. By
*     default, no decimal places are given.
*
*     All of the above format specifiers are case-insensitive. If
*     several characters make conflicting requests (e.g. if both "i"
*     and "b" appear), then the character occurring last takes
*     precedence, except that "d" and "h" always override "t".

*  Notes:
*     - When specifying this attribute by name, it should be
*     subscripted with the number of the Frame axis to which it
*     applies.
*att--
*/
/* This simply provides an interface to the Axis methods for accessing
   the Format string. */
MAKE_CLEAR(Format)
MAKE_GET(Format,const char *,NULL,0,0)
MAKE_SET(Format,const char *)
MAKE_TEST(Format)

/*
*att++
*  Name:
*     Label(axis)

*  Purpose:
*     Axis label.

*  Type:
*     Public attribute.

*  Synopsis:
*     String.

*  Description:
*     This attribute specifies a label to be attached to each axis of
*     a Frame when it is represented (e.g.) in graphical output.
*
*     If a Label value has not been set for a Frame axis, then a
*     suitable default is supplied.

*  Applicability:
*     Frame
*        The default supplied by the Frame class is the string "Axis
*        <n>", where <n> is 1, 2, etc. for each successive axis.
*     SkyFrame
*        The SkyFrame class re-defines the default Label value
*        (e.g. to "Right ascension" or "Galactic latitude") as
*        appropriate for the particular celestial coordinate system
*        being represented.
*     FrameSet
*        The Label attribute of a FrameSet axis is the same as that of
*        its current Frame (as specified by the Current attribute).

*  Notes:
*     - Axis labels are intended purely for interpretation by human
*     readers and not by software.
*     - When specifying this attribute by name, it should be
*     subscripted with the number of the Frame axis to which it
*     applies.
*att--
*/
/* This provides an interface to the Axis methods for accessing the
   Label string, but provides an alternative default Label based on
   the axis number.  This default string is written to the static
   "label_buff" buffer and a pointer to this is returned if
   required. */
MAKE_CLEAR(Label)
MAKE_GET(Label,const char *,NULL,1,( sprintf( label_buff, "Axis %d",
                                              axis + 1 ), label_buff ))
MAKE_SET(Label,const char *)
MAKE_TEST(Label)

/*
*att++
*  Name:
*     Symbol(axis)

*  Purpose:
*     Axis symbol.

*  Type:
*     Public attribute.

*  Synopsis:
*     String.

*  Description:
*     This attribute specifies a short-form symbol to be used to
*     represent coordinate values for a particular axis of a
*     Frame. This might be used (e.g.) in algebraic expressions where
*     a full description of the axis would be inappropriate. Examples
*     include "RA" and "Dec" (for Right Ascension and Declination).
*
*     If a Symbol value has not been set for a Frame axis, then a
*     suitable default is supplied.

*  Applicability:
*     Frame
*        The default Symbol value supplied by the Frame class is the
*        string "<Domain><n>", where <n> is 1, 2, etc. for successive
*        axes, and <Domain> is the value of the Frame's Domain
*        attribute (truncated if necessary so that the final string
*        does not exceed 15 characters). If no Domain value has been
*        set, "x" is used as the <Domain> value in constructing this
*        default string.
*     SkyFrame
*        The SkyFrame class re-defines the default Symbol value
*        (e.g. to "RA" or "Dec") as appropriate for the particular
*        celestial coordinate system being represented.
*     FrameSet
*        The Symbol attribute of a FrameSet axis is the same as that
*        of its current Frame (as specified by the Current attribute).

*  Notes:
*     - When specifying this attribute by name, it should be
*     subscripted with the number of the Frame axis to which it
*     applies.
*att--
*/
/* This provides an interface to the Axis methods for accessing the
   Symbol string, but provides an alternative default Symbol based on
   the axis number and the Frame's Domain (if defined, otherwise "x"
   is used). This default string is written to the static
   "symbol_buff" buffer and a pointer to this is returned if
   required. */
MAKE_CLEAR(Symbol)
MAKE_GET(Symbol,const char *,NULL,1,(

/* Note we use "sprintf" once to determine how many characters are
   produced by the "%d" format string and then limit the number of
   characters used from the Domain string in the second invocation of
   "sprintf" so that the total length of the default Symbol string
   does not exceed SYMBOL_BUFF_LEN characters. */
         sprintf( symbol_buff, "%.*s%d",
                  SYMBOL_BUFF_LEN - sprintf( symbol_buff, "%d", axis + 1 ),
                  astTestDomain( this ) ? astGetDomain( this ) : "x",
                  axis + 1 ),

/* Use the AddUnderscores function to replace any white space in the Symbol
   string with underscore characters. */
                  AddUnderscores( symbol_buff ),
                  symbol_buff ))
MAKE_SET(Symbol,const char *)
MAKE_TEST(Symbol)

/*
*att++
*  Name:
*     Unit(axis)

*  Purpose:
*     Axis physical units.

*  Type:
*     Public attribute.

*  Synopsis:
*     String.

*  Description:
*     This attribute contains a textual representation of the physical
*     units used to represent coordinate values on a particular axis
*     of a Frame.

*  Applicability:
*     Frame
*        The default supplied by the Frame class is an empty string.
*     SkyFrame
*        The SkyFrame class re-defines the default Unit value (e.g. to
*        "hh:mm:ss.sss") to describe the character string returned by
c        the astFormat function when formatting coordinate values.
f        the AST_FORMAT function when formatting coordinate values.
*     FrameSet
*        The Unit attribute of a FrameSet axis is the same as that of
*        its current Frame (as specified by the Current attribute).

*  Notes:
*     - When specifying this attribute by name, it should be
*     subscripted with the number of the Frame axis to which it
*     applies.
*att--
*/
/* This simply provides an interface to the Axis methods for accessing
   the Unit string. */
MAKE_CLEAR(Unit)
MAKE_GET(Unit,const char *,NULL,0,0)
MAKE_SET(Unit,const char *)
MAKE_TEST(Unit)

/* Implement member functions to access the attributes associated with
   the Frame as a whole using the macros defined for this purpose in
   the "object.h" file. */

/*
*att++
*  Name:
*     Digits/Digits(axis)

*  Purpose:
*     Number of digits of precision.

*  Type:
*     Public attribute.

*  Synopsis:
*     Integer.

*  Description:
*     This attribute specifies how many digits of precision are
*     required by default when a coordinate value is formatted for a
c     Frame axis (e.g. using astFormat). Its value may be set either
f     Frame axis (e.g. using AST_FORMAT). Its value may be set either
*     for a Frame as a whole, or (by subscripting the attribute name
*     with the number of an axis) for each axis individually. Any
*     value set for an individual axis will over-ride the value for
*     the Frame as a whole.
*
*     Note that the Digits value acts only as a means of determining a
*     default Format string. Its effects are over-ridden if a Format
*     string is set explicitly for an axis.

*  Applicability:
*     Frame
*        The default Digits value supplied by the Frame class is 7. If
*        a value less than 1 is supplied, then 1 is used instead.
*     FrameSet
*        The Digits attribute of a FrameSet (or one of its axes) is
*        the same as that of its current Frame (as specified by the
*        Current attribute).
*att--
*/
/* Clear the Digits value by setting it to -INT_MAX. */
astMAKE_CLEAR(Frame,Digits,digits,-INT_MAX)

/* Supply a default of 7 digits if no value has been set. */
astMAKE_GET(Frame,Digits,int,0,( ( this->digits != -INT_MAX ) ? this->digits :
                                                                7 ))

/* Constrain the Digits value being set to be at least 1. */
astMAKE_SET(Frame,Digits,int,digits,( value > 1 ? value : 1 ))

/* The Digits value is set if it is not -INT_MAX. */
astMAKE_TEST(Frame,Digits,( this->digits != -INT_MAX ))

/*
*att++
*  Name:
*     MatchEnd

*  Purpose:
*     Match trailing axes?

*  Type:
*     Public attribute.

*  Synopsis:
*     Integer (boolean).

*  Description:
*     This attribute is a boolean value which controls how a Frame
c     behaves when it is used (by astFindFrame) as a template to match
f     behaves when it is used (by AST_FINDFRAME) as a template to match
*     another (target) Frame. It applies only in the case where a
*     match occurs between template and target Frames with different
*     numbers of axes.
*
*     If the MatchEnd value of the template Frame is zero, then the
*     axes which occur first in the target Frame will be matched and
*     any trailing axes (in either the target or template) will be
*     disregarded. If it is non-zero, the final axes in each Frame
*     will be matched and any un-matched leading axes will be
*     disregarded instead.

*  Applicability:
*     Frame
*        The default MatchEnd value for a Frame is zero, so that
*        trailing axes are disregarded.
*     FrameSet
*        The MatchEnd attribute of a FrameSet is the same as that of
*        its current Frame (as specified by the Current attribute).
*att--
*/
/* Clear the MatchEnd value by setting it to -INT_MAX. */
astMAKE_CLEAR(Frame,MatchEnd,match_end,-INT_MAX)

/* Supply a default of 0 if no MatchEnd value has been set. */
astMAKE_GET(Frame,MatchEnd,int,0,( ( this->match_end != -INT_MAX ) ?
                                   this->match_end : 0 ))

/* Set a MatchEnd value of 1 if any non-zero value is supplied. */
astMAKE_SET(Frame,MatchEnd,int,match_end,( value != 0 ))

/* The MatchEnd value is set if it is not -INT_MAX. */
astMAKE_TEST(Frame,MatchEnd,( this->match_end != -INT_MAX ))

/*
*att++
*  Name:
*     MaxAxes

*  Purpose:
*     Maximum number of Frame axes to match.

*  Type:
*     Public attribute.

*  Synopsis:
*     Integer.

*  Description:
*     This attribute controls how a Frame behaves when it is used (by
c     astFindFrame) as a template to match another (target) Frame. It
f     AST_FINDFRAME) as a template to match another (target) Frame. It
*     specifies the maximum number of axes that the target Frame may
*     have in order to match the template.
*
*     Normally, this value will equal the number of Frame axes, so
*     that a template Frame will only match another Frame with the
*     same number of axes as itself. By setting a different value,
*     however, the matching process may be used to identify Frames
*     with specified numbers of axes.

*  Applicability:
*     Frame
*        The default MaxAxes value for a Frame is equal to the number
*        of Frame axes (Naxes attribute).
*     SkyFrame
*        The SkyFrame class constrains the MaxAxes value to be 2.  Any
*        attempt to alter this value is simply ignored.
*     CmpFrame
*        The MaxAxes attribute of a CmpFrame is equal to the sum of
*        the MaxAxes values of its two component Frames. Any attempt
*        to alter this value (other than through the component Frames)
*        is simply ignored.
*     FrameSet
*        The MaxAxes attribute of a FrameSet is the same as that of
*        its current Frame (as specified by the Current attribute).

*  Notes:
*     - When setting a MaxAxes value, the value of the MinAxes
*     attribute may also be silently changed so that it remains
*     consistent with (i.e. does not exceed) the new value. The
*     default MaxAxes value may also be reduced to remain consistent
*     with the MinAxes value.
*     - If a template Frame is used to match a target with a different
*     number of axes, the MatchEnd attribute of the template is used
*     to determine how the individual axes of each Frame should match.
*att--
*/
/* Clear the MaxAxes value by setting it to -INT_MAX. */
astMAKE_CLEAR(Frame,MaxAxes,max_axes,-INT_MAX)

/* Use the DefaultMaxAxes and ConsistentMaxAxes functions (defined earlier) for
   the Get and Set operations to ensure that MinAxes and MaxAxes values remain
   consistent. */
astMAKE_GET(Frame,MaxAxes,int,0,DefaultMaxAxes( this ))
astMAKE_SET(Frame,MaxAxes,int,max_axes,ConsistentMaxAxes( this, value ))

/* The MaxAxes value is set if it is not -INT_MAX. */
astMAKE_TEST(Frame,MaxAxes,( this->max_axes != -INT_MAX ))

/*
*att++
*  Name:
*     MinAxes

*  Purpose:
*     Minimum number of Frame axes to match.

*  Type:
*     Public attribute.

*  Synopsis:
*     Integer.

*  Description:
*     This attribute controls how a Frame behaves when it is used (by
c     astFindFrame) as a template to match another (target) Frame. It
f     AST_FINDFRAME) as a template to match another (target) Frame. It
*     specifies the minimum number of axes that the target Frame may
*     have in order to match the template.
*
*     Normally, this value will equal the number of Frame axes, so
*     that a template Frame will only match another Frame with the
*     same number of axes as itself. By setting a different value,
*     however, the matching process may be used to identify Frames
*     with specified numbers of axes.

*  Applicability:
*     Frame
*        The default MinAxes value for a Frame is equal to the number
*        of Frame axes (Naxes attribute).
*     SkyFrame
*        The SkyFrame class constrains the MinAxes value to be 2. Any
*        attempt to alter this value is simply ignored.
*     CmpFrame
*        The MinAxes attribute of a CmpFrame is equal to the sum of
*        the MinAxes values of its two component Frames. Any attempt
*        to alter this value (other than through the component Frames)
*        is simply ignored.
*     FrameSet
*        The MinAxes attribute of a FrameSet is the same as that of
*        its current Frame (as specified by the Current attribute).

*  Notes:
*     - When setting a MinAxes value, the value of the MaxAxes
*     attribute may also be silently changed so that it remains
*     consistent with (i.e. is not less than) the new value. The
*     default MinAxes value may also be reduced to remain consistent
*     with the MaxAxes value.
*     - If a template Frame is used to match a target with a different
*     number of axes, the MatchEnd attribute of the template is used
*     to determine how the individual axes of each Frame should match.
*att--
*/
/* Clear the MinAxes value by setting it to -INT_MAX. */
astMAKE_CLEAR(Frame,MinAxes,min_axes,-INT_MAX)

/* Use the DefaultMinAxes and ConsistentMinAxes functions (defined earlier) for
   the Get and Set operations to ensure that MinAxes and MaxAxes values remain
   consistent. */
astMAKE_GET(Frame,MinAxes,int,0,DefaultMinAxes( this ))
astMAKE_SET(Frame,MinAxes,int,min_axes,ConsistentMinAxes( this, value ))

/* The MinAxes value is set if it is not -INT_MAX. */
astMAKE_TEST(Frame,MinAxes,( this->min_axes != -INT_MAX ))

/*
*att++
*  Name:
*     Domain

*  Purpose:
*     Coordinate system domain.

*  Type:
*     Public attribute.

*  Synopsis:
*     String.

*  Description:
*     This attribute contains a string which identifies the physical
*     domain of the coordinate system that a Frame describes.
*
*     The Domain attribute also controls how a Frame behaves when it is
c     used (by astFindFrame) as a template to match another (target)
f     used (by AST_FINDFRAME) as a template to match another (target)
*     Frame. It does this by specifying the Domain that the target
*     Frame should have in order to match the template. If the Domain
*     value in the template Frame is set, then only targets with the
*     same Domain value will be matched. If the template's Domain
*     value is not set, however, then the target's Domain will be
*     ignored.

*  Applicability:
*     Frame
*        The default Domain value supplied by the Frame class is an
*        empty string.
*     SkyFrame
*        The SkyFrame class re-defines the default Domain value to be
*        "SKY".
*     CmpFrame
*        The CmpFrame class re-defines the default Domain value to be
*        "CMP".
*     FrameSet
*        The Domain attribute of a FrameSet is the same as that of its
*        current Frame (as specified by the Current attribute).

*  Notes:
*     - All Domain values are converted to upper case and white space
*     is removed before use.
*att--
*/
/* Clear the Domain value by freeing the allocated memory and
   assigning a NULL pointer. */
astMAKE_CLEAR(Frame,Domain,domain,astFree( this->domain ))

/* If the Domain value is not set, supply a default in the form of a
   pointer to the constant string "". */
astMAKE_GET(Frame,Domain,const char *,NULL,( this->domain ? this->domain :
                                                            "" ))

/* Set a Domain value by freeing any previously allocated memory,
   allocating new memory, storing the string, removing white space,
   converting to upper case and saving the pointer to the cleaned
   copy. */
astMAKE_SET(Frame,Domain,const char *,domain,CleanDomain(
                                                astStore( this->domain,
                                    value, strlen( value ) + (size_t) 1 ) ))

/* The Domain value is set if the pointer to it is not NULL. */
astMAKE_TEST(Frame,Domain,( this->domain != NULL ))

/*
*att++
*  Name:
*     Permute

*  Purpose:
*     Permute axis order?

*  Type:
*     Public attribute.

*  Synopsis:
*     String.

*  Description:
*     This attribute is a boolean value which controls how a Frame
c     behaves when it is used (by astFindFrame) as a template to match
f     behaves when it is used (by AST_FINDFRAME) as a template to match
*     another (target) Frame. It specifies whether the axis order of
*     the target Frame may be permuted in order to obtain a match.
*
*     If the template's Permute value is zero, it will match a target
*     only if it can do so without changing the order of its
*     axes. Otherwise, it will attempt to permute the target's axes as
*     necessary.
*
*     The default value is 1, so that axis permutation will be attempted.

*  Applicability:
*     Frame
*        All Frames have this attribute. However, the Frame class
*        effectively ignores this attribute and behaves as if it has
*        the value 1. This is because the axes of a basic Frame are
*        not distinguishable and will always match any other Frame
*        whatever their order.
*     SkyFrame
*        Unlike a basic Frame, the SkyFrame class makes use of this
*        attribute.
*     FrameSet
*        The Permute attribute of a FrameSet is the same as that of
*        its current Frame (as specified by the Current attribute).
*att--
*/
/* Clear the Permute value by setting it to -INT_MAX. */
astMAKE_CLEAR(Frame,Permute,permute,-INT_MAX)

/* Supply a default of 1 if no Permute value has been set. */
astMAKE_GET(Frame,Permute,int,0,( ( this->permute != -INT_MAX ) ?
                                  this->permute : 1 ))

/* Set a Permute value of 1 if any non-zero value is supplied. */
astMAKE_SET(Frame,Permute,int,permute,( value != 0 ))

/* The Permute value is set if it is not -INT_MAX. */
astMAKE_TEST(Frame,Permute,( this->permute != -INT_MAX ))

/*
*att++
*  Name:
*     PreserveAxes

*  Purpose:
*     Preserve axes?

*  Type:
*     Public attribute.

*  Synopsis:
*     Integer (boolean).

*  Description:
*     This attribute controls how a Frame behaves when it is used (by
c     astFindFrame) as a template to match another (target) Frame. It
f     AST_FINDFRAME) as a template to match another (target) Frame. It
*     determines which axes appear (and in what order) in the "result"
*     Frame produced.
*
*     If PreserveAxes is zero in the template Frame, then the result
*     Frame will have the same number (and order) of axes as the
*     template. If it is non-zero, however, the axes of the target
*     Frame will be preserved, so that the result Frame will have the
*     same number (and order) of axes as the target.
*
*     The default value is zero, so that target axes are not preserved
*     and the result Frame resembles the template.

*  Applicability:
*     Frame
*        All Frames have this attribute.
*     FrameSet
*        The PreserveAxes attribute of a FrameSet is the same as that
*        of its current Frame (as specified by the Current attribute).
*att--
*/
/* Clear the PreserveAxes value by setting it to -INT_MAX. */
astMAKE_CLEAR(Frame,PreserveAxes,preserve_axes,-INT_MAX)

/* Supply a default of 0 if no PreserveAxes value has been set. */
astMAKE_GET(Frame,PreserveAxes,int,0,( ( this->preserve_axes != -INT_MAX ) ?
                                       this->preserve_axes : 0 ))

/* Set a PreserveAxes value of 1 if any non-zero value is supplied. */
astMAKE_SET(Frame,PreserveAxes,int,preserve_axes,( value != 0 ))

/* The PreserveAxes value is set if it is not -INT_MAX. */
astMAKE_TEST(Frame,PreserveAxes,( this->preserve_axes != -INT_MAX ))

/*
*att++
*  Name:
*     Title

*  Purpose:
*     Frame title.

*  Type:
*     Public attribute.

*  Synopsis:
*     String.

*  Description:
*     This attribute holds a string which is used as a title in (e.g.)
*     graphical output to describe the coordinate system which a Frame
*     represents. Examples might be "Detector Coordinates" or
*     "Galactic Coordinates".
*
*     If a Title value has not been set for a Frame, then a suitable
*     default is supplied.

*  Applicability:
*     Frame
*        The default supplied by the Frame class is "<n>-d coordinate
*        system", where <n> is the number of Frame axes (Naxes
*        attribute).
*     SkyFrame
*        The SkyFrame class re-defines the default Title value
*        (e.g. to "FK5 equatorial coordinates, mean equinox J2000.0")
*        as appropriate to the particular celestial coordinate system
*        being represented.
*     CmpFrame
*        The CmpFrame class re-defines the default Title value to be
*        "<n>-d compound coordinate system", where <n> is the number
*        of CmpFrame axes (Naxes attribute).
*     FrameSet
*        The Title attribute of a FrameSet is the same as that of its
*        current Frame (as specified by the Current attribute).

*  Notes:
*     - A Frame's Title is intended purely for interpretation by human
*     readers and not by software.
*att--
*/
/* Clear the Title value by freeing the allocated memory and assigning
   a NULL pointer. */
astMAKE_CLEAR(Frame,Title,title,astFree( this->title ))

/* If the Title value is not set, write a default based on the number of Frame
   axes into the static "title_buff" buffer, and return a pointer to this
   buffer. */
astMAKE_GET(Frame,Title,const char *,NULL,( this->title ?
                                            this->title :
            ( sprintf( title_buff, "%d-d coordinate system",
                       astGetNaxes( this ) ), title_buff ) ))

/* Set a Title value by freeing any previously allocated memory, allocating
   new memory, storing the string and saving the pointer to the copy. */
astMAKE_SET(Frame,Title,const char *,title,astStore( this->title, value,
                                           strlen( value ) + (size_t) 1 ))

/* The Title value is set if the pointer to it is not NULL. */
astMAKE_TEST(Frame,Title,( this->title != NULL ))

/* Copy constructor. */
/* ----------------- */
static void Copy( const AstObject *objin, AstObject *objout ) {
/*
*  Name:
*     Copy

*  Purpose:
*     Copy constructor for Frame objects.

*  Type:
*     Private function.

*  Synopsis:
*     void Copy( const AstObject *objin, AstObject *objout )

*  Description:
*     This function implements the copy constructor for Frame objects.

*  Parameters:
*     objin
*        Pointer to the object to be copied.
*     objout
*        Pointer to the object being constructed.

*  Notes:
*     -  This constructor makes a deep copy.
*/

/* Local Variables: */
   AstFrame *in;                 /* Pointer to input Frame */
   AstFrame *out;                /* Pointer to output Frame */
   int axis;                     /* Loop counter for axes */

/* Check the global error status. */
   if ( !astOK ) return;

/* Obtain pointers to the input and output Frames. */
   in = (AstFrame *) objin;
   out = (AstFrame *) objout;

/* For safety, first clear any references to the input memory from
   the output Frame. */
   out->axis = NULL;
   out->domain = NULL;
   out->perm = NULL;
   out->title = NULL;

/* If necessary, allocate memory in the output Frame and store a copy of the
   input Title and Domain strings. */
   if ( in->title ) out->title = astStore( NULL, in->title,
                                           strlen( in->title ) + (size_t) 1 );
   if ( in->domain ) out->domain = astStore( NULL, in->domain,
                                             strlen( in->domain ) +
                                             (size_t) 1 );

/* Allocate memory to hold the output Frame's Axis object pointers and its axis
   permutation array. */
   out->axis = astMalloc( sizeof( AstAxis * ) * (size_t) in->naxes );
   out->perm = astMalloc( sizeof( int ) * (size_t) in->naxes );

/* Make a copy of each of the input Frame's Axis objects, storing the pointer
   to each new Axis in the memory just allocated. Also copy the axis
   permutation array. */
   if ( astOK ) {
      for ( axis = 0; axis < in->naxes; axis++ ) {
         out->axis[ axis ] = astCopy( in->axis[ axis ] );
         out->perm[ axis ] = in->perm[ axis ];
      }

/* If an error occurred while copying the Axis objects, then loop through the
   resulting array of pointers and make sure that all of them are properly
   annulled. */
      if ( !astOK ) {
         for ( axis = 0; axis < in->naxes; axis++ ) {
            out->axis[ axis ] = astAnnul( out->axis[ axis ] );
         }
      }
   }

/* If an error occurred, free any allocated memory. */
   if ( !astOK ) {
      out->axis = astFree( out->axis );
      out->domain = astFree( out->domain );
      out->perm = astFree( out->perm );
      out->title = astFree( out->title );
   }
}

/* Destructor. */
/* ----------- */
static void Delete( AstObject *obj ) {
/*
*  Name:
*     Delete

*  Purpose:
*     Destructor for Frame objects.

*  Type:
*     Private function.

*  Synopsis:
*     void Delete( AstObject *obj )

*  Description:
*     This function implements the destructor for Frame objects.

*  Parameters:
*     obj
*        Pointer to the object to be deleted.

*  Notes:
*     This function attempts to execute even if the global error status is
*     set.
*/

/* Local Variables: */
   AstFrame *this;               /* Pointer to Frame */
   int axis;                     /* Loop counter for Frame axes */

/* Obtain a pointer to the Frame structure. */
   this = (AstFrame *) obj;

/* Free the memory used for the Title and Domain strings if necessary. */
   this->title = astFree( this->title );
   this->domain = astFree( this->domain );

/* If memory has been allocated to store pointers to the Frame's Axis objects,
   annul each of these pointers and then free the memory. */
   if ( this->axis ) {
      for ( axis = 0; axis < this->naxes; axis++ ) {
         this->axis[ axis ] = astAnnul( this->axis[ axis ] );
      }
      this->axis = astFree( this->axis );
   }

/* Free memory used for the axis permutation array if necessary. */
   this->perm = astFree( this->perm );
}

/* Dump function. */
/* -------------- */
static void Dump( AstObject *this_object, AstChannel *channel ) {
/*
*  Name:
*     Dump

*  Purpose:
*     Dump function for Frame objects.

*  Type:
*     Private function.

*  Synopsis:
*     void Dump( AstObject *this, AstChannel *channel )

*  Description:
*     This function implements the Dump function which writes out data
*     for the Frame class to an output Channel.

*  Parameters:
*     this
*        Pointer to the Frame whose data are being written.
*     channel
*        Pointer to the Channel to which the data are being written.
*/

/* Local Constants: */
#define COMMENT_LEN 150          /* Maximum length of a comment string */
#define KEY_LEN 50               /* Maximum length of a keyword */

/* Local Variables: */
   AstAxis *ax;                  /* Pointer to Axis */
   AstFrame *this;               /* Pointer to the Frame structure */
   char comment[ COMMENT_LEN + 1 ]; /* Buffer for comment strings */
   char key[ KEY_LEN + 1 ];      /* Buffer for keywords */
   const char *sval;             /* Pointer to string value */
   const int *perm;              /* Pointer to axis permutation array */
   int *invperm;                 /* Pointer to inverse permutation array */
   int axis;                     /* Loop counter for Frame axes */
   int digits_set;               /* Digits set explicitly for any axis? */
   int full;                     /* Full attribute value */
   int full_set;                 /* Full attribute set? */
   int helpful;                  /* Helpful to show value even if not set? */
   int ival;                     /* Integer value */
   int naxes;                    /* Number of Frame axes */
   int set;                      /* Attribute value set? */

/* Check the global error status. */
   if ( !astOK ) return;

/* Obtain a pointer to the Frame structure. */
   this = (AstFrame *) this_object;

/* Determine the number of Frame axes and a pointer to the Frame's
   axis permutation array (using methods, to allow for any over-ride
   by a derived class). */
   naxes = astGetNaxes( this );
   perm = astGetPerm( this );

/* Allocate memory to hold an inverse axis permutation array and
   generate this array from the forward permutation values. This will
   be used to determine which axis should be enquired about (using
   possibly over-ridden methods) to obtain data to correspond with a
   particular internal value (i.e. instance variable) relating to an
   axis. This step is needed so that the effect of any axis
   permutation can be un-done before values are written out, as output
   values are written by this function in un-permuted order. */
   invperm = astMalloc( sizeof( int ) * (size_t) naxes );
   if ( astOK ) {
      for ( axis = 0; axis < naxes; axis++ ) invperm[ perm[ axis ] ] = axis;

/* Write out values representing the instance variables for the Frame
   class.  Accompany these with appropriate comment strings, possibly
   depending on the values being written.*/

/* In the case of attributes, we first use the appropriate (private)
   Test...  member function to see if they are set. If so, we then use
   the (private) Get... function to obtain the value to be written
   out.

   For attributes which are not set, we use the astGet... method to
   obtain the value instead. This will supply a default value
   (possibly provided by a derived class which over-rides this method)
   which is more useful to a human reader as it corresponds to the
   actual default attribute value.  Since "set" will be zero, these
   values are for information only and will not be read back. */

/* Title. */
/* ------ */
      set = TestTitle( this );
      sval = set ? GetTitle( this ) : astGetTitle( this );
      astWriteString( channel, "Title", set, 1, sval,
                      "Title of coordinate system" );

/* Naxes. */
/* ------ */
      set = ( this->naxes != 0 );
      ival = set ? this->naxes : naxes;
      astWriteInt( channel, "Naxes", set, 1, ival,
                   "Number of coordinate axes" );

/* Domain. */
/* ------- */
      set = TestDomain( this );
      sval = set ? GetDomain( this ) : astGetDomain( this );

/* Don't show an un-set Domain value if it is blank. */
      helpful = ( sval && *sval );
      astWriteString( channel, "Domain", set, helpful, sval,
                      "Coordinate system domain" );

/* Label. */
/* ------ */
/* This, and some other, attributes are stored internally by the
   Frame's Axis objects, but are "re-packaged" by the Frame class to
   appear as Frame attributes. We treat them here like Frame
   attributes that are "un-set". There is a Label value for each Frame
   axis. */
      for ( axis = 0; axis < naxes; axis++ ) {

/* The inverse axis permutation array is used to obtain the axis index
   for astGetLabel. This reverses the effect of the Frame's axis
   permutation array and yields a default value appropriate to the
   axis with internal index "axis". */
         sval = astGetLabel( this, invperm[ axis ] );

/* Create keyword and comment strings appropriate to each axis
   (converting to 1-based axis numbering) and write out the Label
   values. */
         (void) sprintf( key, "Lbl%d", axis + 1 );
         (void) sprintf( comment, "Label for axis %d", axis + 1 );
         astWriteString( channel, key, 0, 1, sval, comment );
      }

/* Symbol. */
/* ------- */
/* There is a Symbol value for each Frame axis. These are handled in
   the same way as the Label values. */
      for ( axis = 0; axis < naxes; axis++ ) {
         sval = astGetSymbol( this, invperm[ axis ] );
         (void) sprintf( key, "Sym%d", axis + 1 );
         (void) sprintf( comment, "Symbol for axis %d", axis + 1 );
         astWriteString( channel, key, 0, 0, sval, comment );
      }

/* Unit. */
/* ----- */
/* There is a Unit value for each axis. */
      for ( axis = 0; axis < naxes; axis++ ) {
         sval = astGetUnit( this, invperm[ axis ] );

/* Show the Unit value if it is not blank. */
         helpful = ( sval && *sval );
         (void) sprintf( key, "Uni%d", axis + 1 );
         (void) sprintf( comment, "Units for axis %d", axis + 1 );
         astWriteString( channel, key, 0, helpful, sval, comment );
      }

/* Digits. */
/* ------- */
/* There is a Digits value for each axis... */
      digits_set = 0;
      for ( axis = 0; axis < naxes; axis++ ) {

/* Obtain the axis Digits value, using the Frame's Digits value as a
   default. */
         ax = astGetAxis( this, invperm[ axis ] );
         set = astTestAxisDigits( ax );
         ival = set ? astGetAxisDigits( ax ) : astGetDigits( this );
         ax = astAnnul( ax );

/* Show the value if it is set for the axis (i.e. if it differs from
   the default for the whole Frame) and note if any such value is
   set. */
         helpful = set;
         if ( set ) digits_set = 1;
         (void) sprintf( key, "Dig%d", axis + 1 );
         (void) sprintf( comment, "Individual precision for axis %d",
                         axis + 1 );
         astWriteInt( channel, key, 0, helpful, ival, comment );
      }

/* There is also a Digits value for the Frame as a whole... */
      set = TestDigits( this );

/* Show the value (even if not set) if an explicit Digits value has
   been set for any axis (above). */
      helpful = digits_set;
      ival = set ? GetDigits( this ) : astGetDigits( this );
      astWriteInt( channel, "Digits", set, helpful, ival,
                   "Default formatting precision" );

/* Format. */
/* ------- */
/* There is a Format value for each axis. */
      for ( axis = 0; axis < naxes; axis++ ) {
         sval = astGetFormat( this, invperm[ axis ] );

/* Show the Format value if the Digits value is set for an individual
   axis. */
         ax = astGetAxis( this, invperm[ axis ] );
         helpful = astTestAxisDigits( ax );
         ax = astAnnul( ax );
         (void) sprintf( key, "Fmt%d", axis + 1 );
         (void) sprintf( comment, "Format specifier for axis %d", axis + 1 );
         astWriteString( channel, key, 0, helpful, sval, comment );
      }

/* Direction. */
/* ---------- */
/* There is a Direction value for each axis. */
      for ( axis = 0; axis < naxes; axis++ ) {
         ival = astGetDirection( this, invperm[ axis ] );

/* Show the value if it is zero. */
         helpful = ( ival == 0 );
         (void) sprintf( key, "Dir%d", axis + 1 );
         (void) sprintf( comment,
                         ival ? "Plot axis %d in conventional direction" :
                                "Plot axis %d in reverse direction",
                         axis + 1 );
         astWriteInt( channel, key, 0, helpful, ival, comment );
      }

/* PreserveAxes. */
/* ------------- */
      set = TestPreserveAxes( this );
      ival = set ? GetPreserveAxes( this ) : astGetPreserveAxes( this );
      astWriteInt( channel, "Presrv", set, 0, ival,
                   ival ? "Preserve target axes" :
                          "Don't preserve target axes" );

/* Permute. */
/* -------- */
      set = TestPermute( this );
      ival = set ? GetPermute( this ) : astGetPermute( this );
      astWriteInt( channel, "Permut", set, 0, ival,
                   ival ? "Axes may be permuted to match" :
                          "Axes may not be permuted match" );

/* MinAxes. */
/* -------- */
      set = TestMinAxes( this );
      ival = set ? GetMinAxes( this ) : astGetMinAxes( this );
      astWriteInt( channel, "MinAx", set, 0, ival,
                   "Minimum number of axes to match" );

/* MaxAxes. */
/* -------- */
      set = TestMaxAxes( this );
      ival = set ? GetMaxAxes( this ) : astGetMaxAxes( this );
      astWriteInt( channel, "MaxAx", set, 0, ival,
                   "Maximum number of axes to match" );

/* MatchEnd. */
/* --------- */
      set = TestMatchEnd( this );
      ival = set ? GetMatchEnd( this ) : astGetMatchEnd( this );
      astWriteInt( channel, "MchEnd", set, 0, ival,
                   ival ? "Match final target axes" :
                          "Match initial target axes" );

/* Axis permutation array. */
/* ----------------------- */
/* Write out the axis permutation array value for each axis,
   converting to 1-based axis numbering. */
      for ( axis = 0; axis < this->naxes; axis++ ) {
         set = ( this->perm[ axis ] != axis );
         ival = this->perm[ axis ] + 1;

/* Create a keyword and comment appropriate to the axis. */
         (void) sprintf( key, "Prm%d", axis + 1 );
         if ( set ) {
            (void) sprintf( comment,
                            "Axis %d permuted to use internal axis %d",
                            axis + 1, ival );
         } else {
            (void) sprintf( comment, "Axis %d not permuted", axis + 1 );
         }
         astWriteInt( channel, key, set, 0, ival, comment );
      }

/* Axis Objects. */
/* ------------- */
/* Temporarily set the Channel's Full attribute to -1 (unless it is +1
   to start with), remembering the original setting. This prevents any
   unnecessary "un-set" Axis values being output that would otherwise
   simply duplicate the Frame's attributes which have already been
   written. "Set" Axis values are still written, however (and all
   values are written if Full is set to 1). */
      full_set = astTestFull( channel );
      full = astGetFull( channel );
      if ( full <= 0 ) astSetFull( channel, -1 );

/* Handle each axis in turn. */
      for ( axis = 0; axis < this->naxes; axis++ ) {

/* Create a keyword and comment appropriate to the axis (converting to
   1-based axis numbering). */
         (void) sprintf( key, "Ax%d", axis + 1 );
         (void) sprintf( comment, "Axis number %d", axis + 1 );

/* Write out the axis Object description. */
         astWriteObject( channel, key, 1, 0, this->axis[ axis ], comment );
      }

/* Restore the Channel's original Full attribute setting. */
      if ( full_set ) {
         astSetFull( channel, full );
      } else {
         astClearFull( channel );
      }

/* Free the inverse axis permutation array. */
      invperm = astFree( invperm );
   }

/* Undefine macros local to this function. */
#undef COMMENT_LEN
#undef KEY_LEN
}

/* Standard class functions. */
/* ========================= */
/* Implement the astIsAFrame and astCheckFrame functions using the macros
   defined for this purpose in the "object.h" header file. */
astMAKE_ISA(Frame,Object,check,&class_init)
astMAKE_CHECK(Frame)

AstFrame *astFrame_( int naxes, const char *options, ... ) {
/*
*+
*  Name:
*     astFrame

*  Purpose:
*     Create a Frame.

*  Type:
*     Protected function.

*  Synopsis:
*     #include "frame.h"
*     AstFrame *astFrame( int naxes, const char *options, ... )

*  Class Membership:
*     Frame constructor.

*  Description:
*     This function creates a new Frame and optionally initialises its
*     attributes.

*  Parameters:
*     naxes
*        The number of Frame axes.
*     options
*        Pointer to a null terminated string containing an optional
*        comma-separated list of attribute assignments to be used for
*        initialising the new Frame. The syntax used is the same as
*        for the astSet method and may include "printf" format
*        specifiers identified by "%" symbols in the normal way.
*     ...
*        If the "options" string contains "%" format specifiers, then
*        an optional list of arguments may follow it in order to
*        supply values to be substituted for these specifiers. The
*        rules for supplying these are identical to those for the
*        astSet method (and for the C "printf" function).

*  Returned Value:
*     A pointer to the new Frame.

*  Notes:
*     - A NULL pointer will be returned if this function is invoked
*     with the global error status set, or if it should fail for any
*     reason.
*-

*  Implementation Notes:
*     - This function implements the basic Frame constructor which is
*     available via the protected interface to the Frame class.  A
*     public interface is provided by the astFrameId_ function.
*/

/* Local Variables: */
   AstFrame *new;                /* Pointer to new Frame */
   va_list args;                 /* Variable argument list */

/* Check the global error status. */
   if ( !astOK ) return NULL;

/* Initialise the Frame, allocating memory and initialising the virtual
   function table as well if necessary. */
   new = astInitFrame( NULL, sizeof( AstFrame ), !class_init, &class_vtab,
                       "Frame", naxes );

/* If successful, note that the virtual function table has been initialised. */
   if ( astOK ) {
      class_init = 1;

/* Obtain the variable argument list and pass it along with the options string
   to the astVSet method to initialise the new Frame's attributes. */
      va_start( args, options );
      astVSet( new, options, args );
      va_end( args );

/* If an error occurred, clean up by deleting the new object. */
      if ( !astOK ) new = astDelete( new );
   }

/* Return a pointer to the new Frame. */
   return new;
}

AstFrame *astInitFrame_( void *mem, size_t size, int init,
                         AstFrameVtab *vtab, const char *name,
                         int naxes ) {
/*
*+
*  Name:
*     astInitFrame

*  Purpose:
*     Initialise a Frame.

*  Type:
*     Protected function.

*  Synopsis:
*     #include "frame.h"
*     AstFrame *astInitFrame( void *mem, size_t size, int init,
*                             AstFrameVtab *vtab, const char *name,
*                             int naxes )

*  Class Membership:
*     Frame initialiser.

*  Description:
*     This function is provided for use by class implementations to initialise
*     a new Frame object. It allocates memory (if necessary) to accommodate
*     the Frame plus any additional data associated with the derived class.
*     It then initialises a Frame structure at the start of this memory. If
*     the "init" flag is set, it also initialises the contents of a virtual
*     function table for a Frame at the start of the memory passed via the
*     "vtab" parameter.

*  Parameters:
*     mem
*        A pointer to the memory in which the Frame is to be created. This
*        must be of sufficient size to accommodate the Frame data
*        (sizeof(Frame)) plus any data used by the derived class. If a value
*        of NULL is given, this function will allocate the memory itself using
*        the "size" parameter to determine its size.
*     size
*        The amount of memory used by the Frame (plus derived class data).
*        This will be used to allocate memory if a value of NULL is given for
*        the "mem" parameter. This value is also stored in the Frame
*        structure, so a valid value must be supplied even if not required for
*        allocating memory.
*     init
*        A logical flag indicating if the Frame's virtual function table is
*        to be initialised. If this value is non-zero, the virtual function
*        table will be initialised by this function.
*     vtab
*        Pointer to the start of the virtual function table to be associated
*        with the new Frame.
*     name
*        Pointer to a constant null-terminated character string which contains
*        the name of the class to which the new object belongs (it is this
*        pointer value that will subsequently be returned by the astGetClass
*        method).
*     naxes
*        The number of Frame axes.

*  Returned Value:
*     A pointer to the new Frame.

*  Notes:
*     -  A null pointer will be returned if this function is invoked with the
*     global error status set, or if it should fail for any reason.
*-
*/

/* Local Variables: */
   AstFrame *new;                /* Pointer to new Frame */
   int axis;                     /* Loop counter for Frame axes */

/* Check the global status. */
   if ( !astOK ) return NULL;

/* Initialise. */
   new = NULL;

/* Check the number of axes for validity, reporting an error if necessary. */
   if ( naxes < 0 ) {
      astError( AST__NAXIN, "astInitFrame(%s): Number of axes (%d) is "
                "invalid - this number should not be negative.", name, naxes );

/* Initialise a Mapping structure (the parent class) as the first
   component within the Frame structure, allocating memory if
   necessary. Set the number of input/output coordinates to zero (the
   astGetNin and astGetNout methods are over-ridden by the Frame class
   to provide values for these that are equal to the number of Frame
   axes). */
   } else {
      new = (AstFrame *) astInitMapping( mem, size, init,
                                         (AstMappingVtab *) vtab, name,
                                         0, 0, 1, 1 );

/* If necessary, initialise the virtual function table. */
/* ---------------------------------------------------- */
      if ( init ) InitVtab( vtab );
      if ( astOK ) {

/* Initialise the Frame data. */
/* ----------------------------- */
/* Set the number of Frame axes. */
         new->naxes = naxes;

/* Initialise all attributes to their "undefined" values. */
         new->digits = -INT_MAX;
         new->domain = NULL;
         new->match_end = -INT_MAX;
         new->max_axes = -INT_MAX;
         new->min_axes = -INT_MAX;
         new->permute = -INT_MAX;
         new->preserve_axes = -INT_MAX;
         new->title = NULL;

/* Allocate memory to store pointers to the Frame's Axis objects and to store
   its axis permutation array. */
         new->axis = astMalloc( sizeof( AstAxis * ) * (size_t) naxes );
         new->perm = astMalloc( sizeof( int ) * (size_t) naxes );

/* Create a new Axis object to describe each axis of the Frame and store the
   resulting pointers in the memory allocated above. Also initialise the
   axis permutation array so that the axes appear in their natural order. */
         if ( astOK ) {
            for ( axis = 0; axis < naxes; axis++ ) {
               new->axis[ axis ] = astAxis( "" );
               new->perm[ axis ] = axis;
	    }

/* If an error occurred while creating the Axis objects, scan through the array
   of pointers to them again to ensure that they are all correctly annulled. */
            if ( !astOK ) {
               for ( axis = 0; axis < naxes; axis++ ) {
                  new->axis[ axis ] = astAnnul( new->axis[ axis ] );
   	       }
	    }
	 }

/* If an error occurred, clean up by deleting the new object. */
         if ( !astOK ) new = astDelete( new );
      }
   }

/* Return a pointer to the new object. */
   return new;
}

AstFrame *astLoadFrame_( void *mem, size_t size, int init,
                         AstFrameVtab *vtab, const char *name,
                         AstChannel *channel ) {
/*
*+
*  Name:
*     astLoadFrame

*  Purpose:
*     Load a Frame.

*  Type:
*     Protected function.

*  Synopsis:
*     #include "frame.h"
*     AstFrame *astLoadFrame_( void *mem, size_t size, int init,
*                              AstFrameVtab *vtab, const char *name,
*                              AstChannel *channel )

*  Class Membership:
*     Frame loader.

*  Description:
*     This function is provided to load a new Frame using data read
*     from a Channel. It first loads the data used by the parent class
*     (which allocates memory if necessary) and then initialises a
*     Frame structure in this memory, using data read from the input
*     Channel.
*
*     If the "init" flag is set, it also initialises the contents of a
*     virtual function table for a Frame at the start of the memory
*     passed via the "vtab" parameter.

*  Parameters:
*     mem
*        A pointer to the memory into which the Frame is to be loaded.
*        This must be of sufficient size to accommodate the Frame data
*        (sizeof(Frame)) plus any data used by derived classes. If a
*        value of NULL is given, this function will allocate the
*        memory itself using the "size" parameter to determine its
*        size.
*     size
*        The amount of memory used by the Frame (plus derived class
*        data).  This will be used to allocate memory if a value of
*        NULL is given for the "mem" parameter. This value is also
*        stored in the Frame structure, so a valid value must be
*        supplied even if not required for allocating memory.
*
*        If the "vtab" parameter is NULL, the "size" value is ignored
*        and sizeof(AstFrame) is used instead.
*     init
*        A boolean flag indicating if the Frame's virtual function
*        table is to be initialised. If this value is non-zero, the
*        virtual function table will be initialised by this function.
*
*        If the "vtab" parameter is NULL, the "init" value is ignored
*        and the (static) virtual function table initialisation flag
*        for the Frame class is used instead.
*     vtab
*        Pointer to the start of the virtual function table to be
*        associated with the new Frame. If this is NULL, a pointer to
*        the (static) virtual function table for the Frame class is
*        used instead.
*     name
*        Pointer to a constant null-terminated character string which
*        contains the name of the class to which the new object
*        belongs (it is this pointer value that will subsequently be
*        returned by the astGetClass method).
*
*        If the "vtab" parameter is NULL, the "name" value is ignored
*        and a pointer to the string "Frame" is used instead.

*  Returned Value:
*     A pointer to the new Frame.

*  Notes:
*     - A null pointer will be returned if this function is invoked
*     with the global error status set, or if it should fail for any
*     reason.
*-
*/

/* Local Constants: */
#define KEY_LEN 50               /* Maximum length of a keyword */

/* Local Variables: */
   AstFrame *new;                /* Pointer to the new Frame */
   char *sval;                   /* Pointer to string value */
   char key[ KEY_LEN + 1 ];      /* Buffer for keywords */
   int axis;                     /* Loop counter for axes */
   int ival;                     /* Integer value */

/* Initialise. */
   new = NULL;

/* Check the global error status. */
   if ( !astOK ) return new;

/* If a NULL virtual function table has been supplied, then this is
   the first loader to be invoked for this Frame. In this case the
   Frame belongs to this class, so supply appropriate values to be
   passed to the parent class loader (and its parent, etc.). */
   if ( !vtab ) {
      size = sizeof( AstFrame );
      init = !class_init;
      vtab = &class_vtab;
      name = "Frame";
   }

/* Invoke the parent class loader to load data for all the ancestral
   classes of the current one, returning a pointer to the resulting
   partly-built Frame. */
   new = astLoadMapping( mem, size, init, (AstMappingVtab *) vtab, name,
                         channel );

/* If required, initialise the part of the virtual function table used
   by this class. */
   if ( init ) InitVtab( vtab );

/* Note if we have successfully initialised the (static) virtual
   function table owned by this class (so that this is done only
   once). */
   if ( astOK ) {
      if ( ( vtab == &class_vtab ) && init ) class_init = 1;

/* Read input data. */
/* ================ */
/* Request the input Channel to read all the input data appropriate to
   this class into the internal "values list". */
      astReadClassData( channel, "Frame" );

/* Now read each individual data item from this list and use it to
   initialise the appropriate instance variable(s) for this class. */

/* In the case of attributes, we first read the "raw" input value,
   supplying the "unset" value as the default. If a "set" value is
   obtained, we then use the appropriate (private) Set... member
   function to validate and set the value properly. */

/* Naxes. */
/* ------ */
/* Obtain the number of Frame axes and allocate memory for the arrays
   which hold axis information. */
      new->naxes = astReadInt( channel, "naxes", 0 );
      if ( new->naxes < 0 ) new->naxes = 0;
      new->perm = astMalloc( sizeof( int ) * (size_t) new->naxes );
      new->axis = astMalloc( sizeof( AstAxis * ) * (size_t) new->naxes );

/* If an error occurred, ensure that any allocated memory is freed. */
      if ( !astOK ) {
         new->perm = astFree( new->perm );
         new->axis = astFree( new->axis );

/* Otherwise, initialise the array of Axis pointers. */
      } else {
         for ( axis = 0; axis < new->naxes; axis++ ) new->axis[ axis ] = NULL;

/* Now obtain those input values which are required for each axis... */
         for ( axis = 0; axis < new->naxes; axis++ ) {

/* Axis object. */
/* ------------ */
/* This must be read first, so that it can hold the other axis values
   obtained below. */

/* Create a keyword appropriate to this axis. */
            (void) sprintf( key, "ax%d", axis + 1 );

/* Read the Axis object. If none was read, provide a default Axis
   instead. */
            new->axis[ axis ] = astReadObject( channel, key, NULL );
            if ( !new->axis[ axis ] ) new->axis[ axis ] = astAxis( "" );

/* Label. */
/* ------ */
/* Read the Label string for each axis. If a value is obtained, use
   it to set the Label attribute for the axis. Free the memory holding
   the string when no longer needed. */
            (void) sprintf( key, "lbl%d", axis + 1 );
            sval = astReadString( channel, key, NULL );
            if ( sval ) {
               astSetAxisLabel( new->axis[ axis ], sval );
               sval = astFree( sval );
            }

/* Symbol. */
/* ------- */
            (void) sprintf( key, "sym%d", axis + 1 );
            sval = astReadString( channel, key, NULL );
            if ( sval ) {
               astSetAxisSymbol( new->axis[ axis ], sval );
               sval = astFree( sval );
            }

/* Format. */
/* ------- */
            (void) sprintf( key, "fmt%d", axis + 1 );
            sval = astReadString( channel, key, NULL );
            if ( sval ) {
               astSetAxisFormat( new->axis[ axis ], sval );
               sval = astFree( sval );
            }

/* Unit. */
/* ----- */
            (void) sprintf( key, "uni%d", axis + 1 );
            sval = astReadString( channel, key, NULL );
            if ( sval ) {
               astSetAxisUnit( new->axis[ axis ], sval );
               sval = astFree( sval );
            }

/* Direction. */
/* ---------- */
            (void) sprintf( key, "dir%d", axis + 1 );
            ival = astReadInt( channel, key, -INT_MAX );
            if ( ival != -INT_MAX ) {
               astSetAxisDirection( new->axis[ axis ], ival );
            }
            
/* Digits. */
/* ------- */
            (void) sprintf( key, "dig%d", axis + 1 );
            ival = astReadInt( channel, key, -INT_MAX );
            if ( ival != -INT_MAX ) {
               astSetAxisDigits( new->axis[ axis ], ival );
            }

/* Axis permutation array. */
/* ----------------------- */
/* Convert from 1-based to zero-based axis numbering at this
   point. The default is the "un-permuted" value. */
            sprintf( key, "prm%d", axis + 1 );
            new->perm[ axis ] = astReadInt( channel, key, axis + 1 ) - 1;

/* Quit looping if an error occurs. */
            if ( !astOK ) break;
         }

/* The remaining values are not associated with particular axes... */

/* Title. */
/* ------ */
         new->title = astReadString( channel, "title", NULL );

/* Domain. */
/* ------- */
         new->domain = astReadString( channel, "domain", NULL );

/* Digits. */
/* ------- */
/* This is the value that applies to the Frame as a whole. */
         new->digits = astReadInt( channel, "digits", -INT_MAX );
         if ( TestDigits( new ) ) SetDigits( new, new->digits );

/* PreserveAxes. */
/* ------------- */
         new->preserve_axes = astReadInt( channel, "presrv", -INT_MAX );
         if ( TestPreserveAxes( new ) ) {
            SetPreserveAxes( new, new->preserve_axes );
         }

/* Permute. */
/* -------- */
         new->permute = astReadInt( channel, "permut", -INT_MAX );
         if ( TestPermute( new ) ) SetPermute( new, new->permute );

/* MinAxes. */
/* -------- */
         new->min_axes = astReadInt( channel, "minax", -INT_MAX );
         if ( TestMinAxes( new ) ) SetMinAxes( new, new->min_axes );

/* MaxAxes. */
/* -------- */
         new->max_axes = astReadInt( channel, "maxax", -INT_MAX );
         if ( TestMaxAxes( new ) ) SetMaxAxes( new, new->max_axes );

/* MatchEnd. */
/* --------- */
         new->match_end = astReadInt( channel, "mchend", -INT_MAX );
         if ( TestMatchEnd( new ) ) SetMatchEnd( new, new->match_end );
      }

/* If an error occurred, clean up by deleting the new Frame. */
      if ( !astOK ) new = astDelete( new );
   }

/* Return the new Frame pointer. */
   return new;

/* Undefine macros local to this function. */
#undef KEY_LEN
}

/* Virtual function interfaces. */
/* ============================ */
/* These provide the external interface to the virtual functions defined by
   this class. Each simply checks the global error status and then locates and
   executes the appropriate member function, using the function pointer stored
   in the object's virtual function table (this pointer is located using the
   astMEMBER macro defined in "object.h").

   Note that the member function may not be the one defined here, as it may
   have been over-ridden by a derived class. However, it should still have the
   same interface. */
const char *astAbbrev_( AstFrame *this, int axis,
                        const char *str1, const char *str2 ) {
   if ( !astOK ) return str2;
   return (**astMEMBER(this,Frame,Abbrev))( this, axis, str1, str2 );
}
void astCheckPerm_( AstFrame *this, const int *perm, const char *method ) {
   if ( !astOK ) return;
   (**astMEMBER(this,Frame,CheckPerm))( this, perm, method );
}
AstFrameSet *astConvert_( AstFrame *from, AstFrame *to,
                          const char *domainlist ) {
   if ( !astOK ) return NULL;
   return (**astMEMBER(from,Frame,Convert))( from, to, domainlist );
}
AstFrameSet *astConvertX_( AstFrame *to, AstFrame *from,
                           const char *domainlist ) {
   if ( !astOK ) return NULL;
   return (**astMEMBER(to,Frame,ConvertX))( to, from, domainlist );
}
double astDistance_( AstFrame *this,
                     const double point1[], const double point2[] ) {
   if ( !astOK ) return AST__BAD;
   return (**astMEMBER(this,Frame,Distance))( this, point1, point2 );
}
AstFrameSet *astFindFrame_( AstFrame *target, AstFrame *template,
                            const char *domainlist ) {
   if ( !astOK ) return NULL;
   return (**astMEMBER(target,Frame,FindFrame))( target, template, domainlist );
}
const char *astFormat_( AstFrame *this, int axis, double value ) {
   if ( !astOK ) return NULL;
   return (**astMEMBER(this,Frame,Format))( this, axis, value );
}
double astGap_( AstFrame *this, int axis, double gap, int *ntick ) {
   if ( !astOK ) return 0.0;
   return (**astMEMBER(this,Frame,Gap))( this, axis, gap, ntick );
}
AstAxis *astGetAxis_( AstFrame *this, int axis ) {
   if ( !astOK ) return NULL;
   return (**astMEMBER(this,Frame,GetAxis))( this, axis );
}
int astGetNaxes_( AstFrame *this ) {
   if ( !astOK ) return 0;
   return (**astMEMBER(this,Frame,GetNaxes))( this );
}
const int *astGetPerm_( AstFrame *this ) {
   if ( !astOK ) return NULL;
   return (**astMEMBER(this,Frame,GetPerm))( this );
}
int astMatch_( AstFrame *this, AstFrame *target,
               int **template_axes, int **target_axes,
               AstMapping **map, AstFrame **result ) {
   if ( !astOK ) return 0;
   return (**astMEMBER(this,Frame,Match))( this, target,
                                           template_axes, target_axes,
                                           map, result );
}
void astNorm_( AstFrame *this, double value[] ) {
   if ( !astOK ) return;
   (**astMEMBER(this,Frame,Norm))( this, value );
}
void astOffset_( AstFrame *this, const double point1[], const double point2[],
                 double offset, double point3[] ) {
   if ( !astOK ) return;
   (**astMEMBER(this,Frame,Offset))( this, point1, point2, offset, point3 );
}
void astOverlay_( AstFrame *template, const int *template_axes,
                  AstFrame *result ) {
   if ( !astOK ) return;
   (**astMEMBER(template,Frame,Overlay))( template, template_axes, result );
}
void astPermAxes_( AstFrame *this, const int perm[] ) {
   if ( !astOK ) return;
   (**astMEMBER(this,Frame,PermAxes))( this, perm );
}
AstFrame *astPickAxes_( AstFrame *this, int naxes, const int axes[],
                        AstMapping **map ) {
   if ( !astOK ) return NULL;
   return (**astMEMBER(this,Frame,PickAxes))( this, naxes, axes, map );
}
void astPrimaryFrame_( AstFrame *this, int axis1,
                      AstFrame **frame, int *axis2 ) {
   if ( !astOK ) return;
   (**astMEMBER(this,Frame,PrimaryFrame))( this, axis1, frame, axis2 );
}
void astSetAxis_( AstFrame *this, int axis, AstAxis *newaxis ) {
   if ( !astOK ) return;
   (**astMEMBER(this,Frame,SetAxis))( this, axis, newaxis );
}
int astSubFrame_( AstFrame *target, AstFrame *template, int result_naxes,
                  const int *target_axes, const int *template_axes,
                  AstMapping **map, AstFrame **result ) {
   if ( !astOK ) return 0;
   return (**astMEMBER(target,Frame,SubFrame))( target, template, result_naxes,
                                                target_axes, template_axes,
                                                map, result );
}
int astUnformat_( AstFrame *this, int axis, const char *string,
                  double *value ) {
   if ( !astOK ) return 0;
   return (**astMEMBER(this,Frame,Unformat))( this, axis, string, value );
}
int astValidateAxis_( AstFrame *this, int axis, const char *method ) {
   if ( !astOK ) return 0;
   return (**astMEMBER(this,Frame,ValidateAxis))( this, axis, method );
}
void astValidateAxisSelection_( AstFrame *this, int naxes, const int *axes,
                                const char *method ) {
   if ( !astOK ) return;
   (**astMEMBER(this,Frame,ValidateAxisSelection))( this, naxes, axes,
                                                    method );
}

/* Special public interface functions. */
/* =================================== */
/* These provide the public interface to certain special functions
   whose public interface cannot be handled using macros (such as
   astINVOKE) alone. In general, they are named after the
   corresponding protected version of the function, but with "Id"
   appended to the name. */

/* Public Interface Function Prototypes. */
/* ------------------------------------- */
/* The following functions have public prototypes only (i.e. no
   protected prototypes), so we must provide local prototypes for use
   within this module. */
AstFrame *PickAxesId_( AstFrame *, int, const int[], AstMapping ** );
AstFrame *astFrameId_( int, const char *, ... );
const char *astFormatId_( AstFrame *, int, double );
int astUnformatId_( AstFrame *, int, const char *, double * );
void astPermAxesId_( AstFrame *, const int[] );

/* Special interface function implementations. */
/* ------------------------------------------- */
const char *astFormatId_( AstFrame *this, int axis, double value ) {
/*
*++
*  Name:
c     astFormat
f     AST_FORMAT

*  Purpose:
*     Format a coordinate value for a Frame axis.

*  Type:
*     Public virtual function.

*  Synopsis:
c     #include "frame.h"
c     const char *astFormat( AstFrame *this, int axis, double value )
f     RESULT = AST_FORMAT( THIS, AXIS, VALUE, STATUS )

*  Class Membership:
*     Frame method.

*  Description:
c     This function returns a pointer to a string containing the
f     This function returns a character string containing the
*     formatted (character) version of a coordinate value for a Frame
*     axis. The formatting applied is determined by the Frame's
*     attributes and, in particular, by any Format attribute string
*     that has been set for the axis. A suitable default format (based
*     on the Digits attribute value) will be applied if necessary.

*  Parameters:
c     this
f     THIS = INTEGER (given)
*        Pointer to the Frame.
c     axis
f     AXIS = INTEGER (Given)
*        The number of the Frame axis for which formatting is to be
*        performed (axis numbering starts at 1 for the first axis).
c     value
f     VALUE = DOUBLE PRECISION (Given)
*        The coordinate value to be formatted.
f     STATUS = INTEGER (Given and Returned)
f        The global status.

*  Returned Value:
c     astFormat()
c        A pointer to a null-terminated string containing the formatted
c        value.
f     AST_FORMAT = CHARACTER * ( AST__SZCHR )
f        The formatted value.

*  Notes:
c     - The returned pointer is guaranteed to remain valid and the
c     string to which it points will not be over-written for a total
c     of 50 successive invocations of this function. After this, the
c     memory containing the string may be re-used, so a copy of the
c     string should be made if it is needed for longer than this.
c     - A formatted value may be converted back into a numerical (double)
c     value using astUnformat.
f     - A formatted value may be converted back into a numerical
f     (double precision) value using AST_UNFORMAT.
c     - A NULL pointer will be returned if this function is invoked
c     with the AST error status set, or if it should fail for any
c     reason.
f     - A blank string will be returned if this function is invoked
f     with STATUS set to an error value, or if it should fail for any
f     reason.
*--

*  Implementation Notes:
*     This function implements the public interface for the astFormat
*     method. It is identical to astFormat_ except that:
*
*     - The axis index is decremented by 1 before use. This allows the
*     public interface to use 1-based axis numbers (whereas internally
*     axis numbers are zero-based).
*
*     - The returned string value is buffered in dynamically allocated
*     memory so that it will remain valid for a guaranteed number of
*     function invocations.
*/

/* Local Constants: */
#define MAX_STRINGS 50           /* Number of string values to buffer */

/* Local Variables: */
   const char *fvalue;           /* Pointer to formatted value */
   const char *result;           /* Pointer value to return */
   int i;                        /* Loop counter for initialisation */
   static char *strings[ MAX_STRINGS ]; /* Pointers to string buffers */
   static int init = 0;          /* "strings" array initialised? */
   static int istr = 0;          /* Offset of next string in "strings" */

/* Initialise. */
   result = NULL;

/* Check the global error status. */
   if ( !astOK ) return result;

/* If the "strings" array has not been initialised, fill it with NULL
   pointers. */
   if ( !init ) {
      init = 1;
      for ( i = 0; i < MAX_STRINGS; i++ ) strings[ i ] = NULL;
   }

/* Invoke the normal astFormat_ function to obtain a pointer to the
   required formatted value, adjusting the axis index to become
   zero-based. */
   fvalue = astFormat_( this, axis - 1, value );

/* If OK, store a copy of the resulting string in dynamically
   allocated memory, putting a pointer to the copy into the next
   element of the "strings" array.  (This process also de-allocates
   any previously allocated memory pointed at by this "strings"
   element, so the earlier string is effectively replaced by the new
   one.) */
   if ( astOK ) {
      strings[ istr ] = astStore( strings[ istr ], fvalue,
                                  strlen( fvalue ) + (size_t) 1 );

/* If OK, return a pointer to the copy and increment "istr" to use the
   next element of "strings" on the next invocation. Recycle "istr" to
   zero when all elements have been used. */
      if ( astOK ) {
         result = strings[ istr++ ];
         if ( istr == ( MAX_STRINGS - 1 ) ) istr = 0;
      }
   }

/* Return the result. */
   return result;

/* Undefine macros local to this function. */
#undef MAX_STRINGS
}

AstFrame *astFrameId_( int naxes, const char *options, ... ) {
/*
*++
*  Name:
c     astFrame
f     AST_FRAME

*  Purpose:
*     Create a Frame.

*  Type:
*     Public function.

*  Synopsis:
c     #include "frame.h"
c     AstFrame *astFrame( int naxes, const char *options, ... )
f     RESULT = AST_FRAME( NAXES, OPTIONS, STATUS )

*  Class Membership:
*     Frame constructor.

*  Description:
*     This function creates a new Frame and optionally initialises its
*     attributes.
*
*     A Frame is used to represent a coordinate system. It does this
*     in rather the same way that a frame around a graph describes the
*     coordinate space in which data are plotted. Consequently, a
*     Frame has a Title (string) attribute, which describes the
*     coordinate space, and contains axes which in turn hold
*     information such as Label and Units strings which are used for
*     labelling (e.g.) graphical output. In general, however, the
*     number of axes is not restricted to two.
*
*     Functions are available for converting Frame coordinate values
*     into a form suitable for display, and also for calculating
*     distances and offsets between positions within the Frame.
*
*     Frames may also contain knowledge of how to transform to and
*     from related coordinate systems.

*  Parameters:
c     naxes
f     NAXES = INTEGER (Given)
*        The number of Frame axes (i.e. the number of dimensions of
*        the coordinate space which the Frame describes).
c     options
f     OPTIONS = CHARACTER * ( * ) (Given)
c        Pointer to a null-terminated string containing an optional
c        comma-separated list of attribute assignments to be used for
c        initialising the new Frame. The syntax used is identical to
c        that for the astSet function and may include "printf" format
c        specifiers identified by "%" symbols in the normal way.
c        If no initialisation is required, a zero-length string may be
c        supplied.
f        A character string containing an optional comma-separated
f        list of attribute assignments to be used for initialising the
f        new Frame. The syntax used is identical to that for the
f        AST_SET routine. If no initialisation is required, a blank
f        value may be supplied.
c     ...
c        If the "options" string contains "%" format specifiers, then
c        an optional list of additional arguments may follow it in
c        order to supply values to be substituted for these
c        specifiers. The rules for supplying these are identical to
c        those for the astSet function (and for the C "printf"
c        function).
f     STATUS = INTEGER (Given and Returned)
f        The global status.

*  Returned Value:
c     astFrame()
f     AST_FRAME = INTEGER
*        A pointer to the new Frame.

*  Examples:
c     frame = astFrame( 2, "Title=Energy Spectrum: Plot %d", n );
c        Creates a new 2-dimensional Frame and initialises its Title
c        attribute to the string "Energy Spectrum: Plot <n>", where
c        <n> takes the value of the int variable "n".
c     frame = astFrame( 2, "Label(1)=Energy, Label(2)=Response" );
c        Creates a new 2-dimensional Frame and initialises its axis
c        Label attributes to suitable string values.
f     FRAME = AST_FRAME( 2, 'Title=Energy Spectrum', STATUS );
f        Creates a new 2-dimensional Frame and initialises its Title
f        attribute to the string "Energy Spectrum".
f     FRAME = AST_FRAME( 2, 'Label(1)=Energy, Label(2)=Response', STATUS );
f        Creates a new 2-dimensional Frame and initialises its axis
f        Label attributes to suitable string values.

*  Notes:
*     - A null Object pointer (AST__NULL) will be returned if this
c     function is invoked with the AST error status set, or if it
f     function is invoked with STATUS set to an error value, or if it
*     should fail for any reason.
*--

*  Implementation Notes:
*     - This function implements the external (public) interface to
*     the astFrame constructor function. It returns an ID value
*     (instead of a true C pointer) to external users, and must be
*     provided because astFrame_ has a variable argument list which
*     cannot be encapsulated in a macro (where this conversion would
*     otherwise occur).
*     - The variable argument list also prevents this function from
*     invoking astFrame_ directly, so it must be a re-implementation
*     of it in all respects, except for the final conversion of the
*     result to an ID value.
*/

/* Local Variables: */
   AstFrame *new;                /* Pointer to new Frame */
   va_list args;                 /* Variable argument list */

/* Check the global error status. */
   if ( !astOK ) return NULL;

/* Initialise the Frame, allocating memory and initialising the virtual
   function table as well if necessary. */
   new = astInitFrame( NULL, sizeof( AstFrame ), !class_init, &class_vtab,
                       "Frame", naxes );

/* If successful, note that the virtual function table has been initialised. */
   if ( astOK ) {
      class_init = 1;

/* Obtain the variable argument list and pass it along with the options string
   to the astVSet method to initialise the new Frame's attributes. */
      va_start( args, options );
      astVSet( new, options, args );
      va_end( args );

/* If an error occurred, clean up by deleting the new object. */
      if ( !astOK ) new = astDelete( new );
   }

/* Return an ID value for the new Frame. */
   return astMakeId( new );
}

void astPermAxesId_( AstFrame *this, const int perm[] ) {
/*
*++
*  Name:
c     astPermAxes
f     AST_PERMAXES

*  Purpose:
*     Permute the axis order in a Frame.

*  Type:
*     Public virtual function.

*  Synopsis:
c     #include "frame.h"
c     void astPermAxes( AstFrame *this, const int perm[] )
f     CALL AST_PERMAXES( THIS, PERM, STATUS )

*  Class Membership:
*     Frame method.

*  Description:
c     This function permutes the order in which a Frame's axes occur.
f     This routine permutes the order in which a Frame's axes occur.

*  Parameters:
c     this
f     THIS = INTEGER (Given)
*        Pointer to the Frame.
c     perm
f     PERM( * ) = INTEGER (Given)
*        An array with one element for each axis of the Frame (Naxes
*        attribute). This should list the axes in their new order,
*        using the original axis numbering (which starts at 1 for the
*        first axis).
f     STATUS = INTEGER (Given and Returned)
f        The global status.

*  Notes:
*     - Only genuine permutations of the axis order are permitted, so
c     each axis must be referenced exactly once in the "perm" array.
f     each axis must be referenced exactly once in the PERM array.
*     - If successive axis permutations are applied to a Frame, then
*     the effects are cumulative.
*--

*  Implementation Notes:
*     This function implements the public interface for the
*     astPermAxes method. It is identical to astPermAxes_ except that
*     the axis numbers in the "perm" array are decremented by 1 before
*     use. This is to allow the public interface to use one-based axis
*     numbering (internally, zero-based axis numbering is used).
*/

/* Local Variables: */
   int *perm1;                   /* Pointer to modified perm array */
   int axis;                     /* Loop counter for Frame axes */
   int naxes;                    /* Number of Frame axes */

/* Check the global error status. */
   if ( !astOK ) return;

/* Obtain the number of Frame axes. */
   naxes = astGetNaxes( this );

/* Allocate an array to hold a modified version of the "perm"
   array. */
   perm1 = astMalloc( sizeof( int ) * (size_t) naxes );
   if ( astOK ) {

/* Make a modified copy of the "perm" array by subtracting one from
   each element. This allows the public interface to use one-based
   axis numbering, whereas all internal code is zero-based. */
      for ( axis = 0; axis < naxes; axis++ ) perm1[ axis ] = perm[ axis ] - 1;

/* Invoke the normal astPermAxes_ function to permute the Frame's axes. */
      astPermAxes_( this, perm1 );
   }

/* Free the temporary array. */
   perm1 = astFree( perm1 );
}

AstFrame *astPickAxesId_( AstFrame *this, int naxes, const int axes[],
                          AstMapping **map ) {
/*
*++
*  Name:
c     astPickAxes
f     AST_PICKAXES

*  Purpose:
*     Create a new Frame by picking axes from an existing one.

*  Type:
*     Public virtual function.

*  Synopsis:
c     #include "frame.h"
c     AstFrame *PickAxes( AstFrame *this, int naxes, const int axes[],
c                         AstMapping **map )
f     RESULT = AST_PICKAXES( THIS, NAXES, AXES, MAP, STATUS )

*  Class Membership:
*     Frame method.

*  Description:
*     This function creates a new Frame whose axes are copied from an
*     existing Frame along with other Frame attributes, such as its
*     Title. Any number (zero or more) of the original Frame's axes
*     may be copied, in any order, and additional axes with default
*     attributes may also be included in the new Frame.
*
c     Optionally, a Mapping that converts between the coordinate
f     A Mapping that converts between the coordinate
*     systems described by the two Frames will also be returned.

*  Parameters:
c     this
f     THIS = INTEGER (Given)
*        Pointer to the original Frame.
c     naxes
f     NAXES = INTEGER (Given)
*        The number of axes required in the new Frame.
c     axes
f     AXES( NAXES ) = INTEGER (Given)
c        An array, with "naxes" elements, which lists the axes to be
f        An array which lists the axes to be
*        copied. These should be given in the order required in the
*        new Frame, using the axis numbering in the original Frame
*        (which starts at 1 for the first axis). Axes may be selected
*        in any order, but each may only be used once.  If additional
*        (default) axes are also to be included, the corresponding
*        elements of this array should be set to zero.
c     map
f     MAP = INTEGER (Returned)
c        Address of a location in which to return a pointer to a new
f        A pointer to a new
*        Mapping. This will be a PermMap (or a UnitMap as a special
*        case) that describes the axis permutation that has taken
*        place between the original and new Frames. The Mapping's
*        forward transformation will convert coordinates from the
*        original Frame into the new one, and vice versa.
c
c        If this Mapping is not required, a NULL value may be supplied
c        for this parameter.
f     STATUS = INTEGER (Given and Returned)
f        The global status.

*  Returned Value:
c     astPickAxes()
f     AST_PICKAXES = INTEGER
*        A pointer to the new Frame.

*  Notes:
*     - The class of Frame returned may differ from that of the
*     original Frame, depending on which axes are selected. For
*     example, if a single axis is picked from a SkyFrame (which must
*     always have two axes) then the resulting Frame cannot be a valid
*     SkyFrame, so will revert to the parent class (Frame) instead.
c     - The new Frame will contain a "deep" copy (c.f. astCopy) of all
f     - The new Frame will contain a "deep" copy (c.f. AST_COPY) of all
*     the data selected from the original Frame. Modifying any aspect
*     of the new Frame will therefore not affect the original one.
*     - A null Object pointer (AST__NULL) will be returned if this
c     function is invoked with the AST error status set, or if it
f     function is invoked with STATUS set to an error value, or if it
*     should fail for any reason.
*--

*  Implementation Notes:
*     This function implements the public interface for the
*     astPickAxes method. It is identical to astPickAxes_ except for
*     the following:
*
*     - The axis numbers in the "axes" array are decremented by 1 before
*     use. This is to allow the public interface to use one-based axis
*     numbering (internally, zero-based axis numbering is used).
*
*     - An ID value is returned via the "map" parameter (if used)
*     instead of a true C pointer. This is required because this
*     conversion cannot be performed by the macro that invokes the
*     function.
*/

/* Local Variables: */
   AstFrame *result;             /* Pointer to result Frame */
   int *axes1;                   /* Pointer to modified axes array */
   int axis;                     /* Loop counter for axes */

/* Initialise. */
   result = NULL;

/* Check the global error status. */
   if ( !astOK ) return result;

/* Allocate an array to hold a modified version of the "axes" array
   (check that "naxes" is valid first - if not, this error will be
   reported by astPickAxes_ below). */
   axes1 = ( naxes >= 0 ) ? astMalloc( sizeof( int ) * (size_t) naxes ) :
                            NULL;
   if ( astOK ) {

/* Make a modified copy of the "axes" array by subtracting one from
   each element. This allows the public interface to use one-based
   axis numbering, whereas all internal code is zero-based. */
      for ( axis = 0; axis < naxes; axis++ ) axes1[ axis ] = axes[ axis ] - 1;

/* Invoke the normal astPickAxes_ function to select the required axes. */
      result = astPickAxes_( this, naxes, axes1, map );
   }

/* Free the temporary array. */
   axes1 = astFree( axes1 );

/* If required, return an ID value for the Mapping. */
   if ( map ) *map = astMakeId( *map );

/* Return the result. */
   return result;
}

int astUnformatId_( AstFrame *this, int axis, const char *string,
                    double *value ) {
/*
*++
*  Name:
c     astUnformat
f     AST_UNFORMAT

*  Purpose:
*     Read a formatted coordinate value for a Frame axis.

*  Type:
*     Public virtual function.

*  Synopsis:
c     #include "frame.h"
c     int astUnformat( AstFrame *this, int axis, const char *string,
c                      double *value )
f     RESULT = AST_UNFORMAT( THIS, AXIS, STRING, VALUE, STATUS )

*  Class Membership:
*     Frame method.

*  Description:
c     This function reads a formatted coordinate value (given as a
c     character string) for a Frame axis and returns the equivalent
c     numerical (double) value. It also returns the number of
c     characters read from the string.
f     This function reads a formatted coordinate value (given as a
f     character string) for a Frame axis and returns the equivalent
f     numerical (double precision) value. It also returns the number
f     of characters read from the string.
*
c     The principle use of this function is in decoding user-supplied
c     input which contains formatted coordinate values. Free-format
c     input is supported as far as possible. If input is ambiguous, it
c     is interpreted with reference to the Frame's attributes (in
c     particular, the Format string associated with the Frame's
c     axis). This function is, in essence, the inverse of astFormat.
f     The principle use of this function is in decoding user-supplied
f     input which contains formatted coordinate values. Free-format
f     input is supported as far as possible. If input is ambiguous, it
f     is interpreted with reference to the Frame's attributes (in
f     particular, the Format string associated with the Frame's
f     axis). This function is, in essence, the inverse of AST_FORMAT.

*  Parameters:
c     this
f     THIS = INTEGER (Given)
*        Pointer to the Frame.
c     axis
f     AXIS = INTEGER (Given)
*        The number of the Frame axis for which a coordinate value is to
*        be read (axis numbering starts at 1 for the first axis).
c     string
f     STRING = CHARACTER * ( * ) (Given)
c        Pointer to a null-terminated character string containing the
c        formatted coordinate value.
f        A character string containing the formatted coordinate value.
*        This string may contain additional information following the
*        value to be read, in which case reading stops at the first
*        character which cannot be interpreted as part of the value.
*        Any white space before or after the value is discarded.
c     value
f     VALUE = DOUBLE PRECISION (Returned)
c        Pointer to a double in which the coordinate value read will be
c        returned.
f        The coordinate value read.
f     STATUS = INTEGER (Given and Returned)
f        The global status.

*  Returned Value:
c     astUnformat()
f     AST_UNFORMAT = INTEGER
*        The number of characters read from the string in order to
*        obtain the coordinate value. This will include any white
*        space which occurs before or after the value.

*  Applicability:
*     Frame
*        This function applies to all Frames. See the "Frame Input
*        Format" section below for details of the input formats
*        accepted by a basic Frame.
*     SkyFrame
*        The SkyFrame class re-defines the input format to be suitable
*        for representing angles and times, with the resulting
*        coordinate value returned in radians.  See the "SkyFrame
*        Input Format" section below for details of the formats
*        accepted.
*     FrameSet
*        The input formats accepted by a FrameSet are determined by
*        its current Frame (as specified by the Current attribute).

*  Frame Input Format
*     The input format accepted for a basic Frame axis is as follows:
*     - An optional sign, followed by:
*     - A sequence of one or more digits possibly containing a decimal point,
*     followed by:
*     - An optional exponent field.
*     - The exponent field, if present, consists of "E" or "e"
*     followed by a possibly signed integer.
*
*     Examples of acceptable Frame input formats include:
*     - 99
*     - 1.25
*     - -1.6
*     - 1E8
*     - -.99e-17
*     - <bad>

*  SkyFrame Input Format
*     The input format accepted for a SkyFrame axis is as follows:
*     - An optional sign, followed by between one and three fields
*     representing either degrees, arc-minutes, arc-seconds or hours,
*     minutes, seconds (e.g. "-12 42 03").
*     - Each field should consist of a sequence of one or more digits,
*     which may include leading zeros. At most one field may contain a
*     decimal point, in which case it is taken to be the final field
*     (e.g. decimal degrees might be given as "124.707", while degrees
*     and decimal arc-minutes might be given as "-13 33.8").
*     - The first field given may take any value, allowing angles and
*     times outside the conventional ranges to be
*     represented. However, subsequent fields must have values of less
*     than 60 (e.g. "720 45 31" is valid, whereas "11 45 61" is not).
*     - Fields may be separated by white space or by ":" (colon), but
*     the choice of separator must be used consistently throughout the
*     value. Additional white space may be present around fields and
*     separators (e.g. "- 2: 04 : 7.1").
*     - The following field identification characters may be used as
*     separators to replace either of those above (or may be appended
*     to the final field), in order to identify the field to which
*     they are appended: "d"---degrees; "h"---hours; "m"---minutes of
*     arc or time; "s"---seconds of arc or time; "'" (single
*     quote)---minutes of arc; """ (double quote)---seconds of arc.
*     Either lower or upper case may be used.  Fields must be given in
*     order of decreasing significance (e.g. "-11D 3' 14.4"" or
*     "22h14m11.2s").
*     - The presence of any of the field identification characters
*     "d", "'" (single quote) or """ (double quote) indicates that the
*     value is to be interpreted as an angle. Conversely, the presence
*     of "h" indicates that it is to be interpreted as a time (with 24
*     hours corresponding to 360 degrees). Incompatible angle/time
*     identification characters may not be mixed (e.g. "10h14'3"" is
*     not valid).  The remaining field identification characters and
*     separators do not specify a preference for an angle or a time
*     and may be used with either.
c     - If no preference for an angle or a time is expressed anywhere
c     within the value, it is interpreted as an angle if the Format
c     attribute string associated with the SkyFrame axis generates an
c     angle and as a time otherwise. This ensures that values produced
c     by astFormat are correctly interpreted by astUnformat.
f     - If no preference for an angle or a time is expressed anywhere
f     within the value, it is interpreted as an angle if the Format
f     attribute string associated with the SkyFrame axis generates an
f     angle and as a time otherwise. This ensures that values produced
f     by AST_FORMAT are correctly interpreted by AST_UNFORMAT.
*     - Fields may be omitted, in which case they default to zero. The
*     remaining fields may be identified by using appropriate field
*     identification characters (see above) and/or by adding extra
*     colon separators (e.g. "-05m13s" is equivalent to "-:05:13"). If
*     a field is not identified explicitly, it is assumed that
*     adjacent fields have been given, after taking account of any
*     extra separator characters (e.g. "14:25.4s" specifies minutes
*     and seconds, while "14::25.4s" specifies degrees and seconds).
c     - If fields are omitted in such a way that the remaining ones
c     cannot be identified uniquely (e.g. "01:02"), then the first
c     field (either given explicitly or implied by an extra leading
c     colon separator) is taken to be the most significant field that
c     astFormat would produce when formatting a value (using the
c     Format attribute associated with the SkyFrame axis).  By
c     default, this means that the first field will normally be
c     interpreted as degrees or hours. However, if this does not
c     result in consistent field identification, then the last field
c     (either given explicitly or implied by an extra trailing colon
c     separator) is taken to to be the least significant field that
c     astFormat would produce.
f     - If fields are omitted in such a way that the remaining ones
f     cannot be identified uniquely (e.g. "01:02"), then the first
f     field (either given explicitly or implied by an extra leading
f     colon separator) is taken to be the most significant field that
f     AST_FORMAT would produce when formatting a value (using the
f     Format attribute associated with the SkyFrame axis).  By
f     default, this means that the first field will normally be
f     interpreted as degrees or hours. However, if this does not
f     result in consistent field identification, then the last field
f     (either given explicitly or implied by an extra trailing colon
f     separator) is taken to to be the least significant field that
f     AST_FORMAT would produce.
*
c     This final convention is intended to ensure that values formatted
c     by astFormat which contain less than three fields will be
c     correctly interpreted if read back using astUnformat, even if
c     they do not contain field identification characters.
f     This final convention is intended to ensure that values formatted
f     by AST_FORMAT which contain less than three fields will be
f     correctly interpreted if read back using AST_UNFORMAT, even if
f     they do not contain field identification characters.
*
*     Examples of acceptable SkyFrame input formats (with
*     interpretation in parentheses) include:
*     - -14d 13m 22.2s (-14d 13' 22.2")
*     - + 12:34:56.7 (12d 34' 56.7" or 12h 34m 56.7s)
*     - 001 : 02 : 03.4 (1d 02' 03.4" or 1h 02m 03.4s)
*     - 22h 30 (22h 30m 00s)
*     - 136::10" (136d 00' 10" or 136h 00m 10s)
*     - -14M 27S (-0d 14' 27" or -0h 14m 27s)
*     - -:14: (-0d 14' 00" or -0h 14m 00s)
*     - -::4.1 (-0d 00' 04.1" or -0h 00m 04.1s)
*     - .9" (0d 00' 00.9")
*     - d12m (0d 12' 00")
*     - H 12:22.3s (0h 12m 22.3s)
*     - <bad> (AST__BAD)
*
*     Where alternative interpretations are shown, the choice of angle or
*     time depends on the associated Format(axis) attribute.

*  Notes:
*     - A function value of zero (and no coordinate value) will be
*     returned, without error, if the string supplied does not contain
*     a suitably formatted value.
c     - Beware that it is possible for a formatting error part-way
c     through an input string to terminate input before it has been
c     completely read, but to yield a coordinate value that appears
c     valid. For example, if a user types "1.5r6" instead of "1.5e6",
c     the "r" will terminate input, giving an incorrect coordinate
c     value of 1.5. It is therefore most important to check the return
c     value of this function to ensure that the correct number of
c     characters have been read.
f     - Beware that it is possible for a formatting error part-way
f     through an input string to terminate input before it has been
f     completely read, but to yield a coordinate value that appears
f     valid. For example, if a user types "1.5R6" instead of "1.5E6",
f     the "R" will terminate input, giving an incorrect coordinate
f     value of 1.5. It is therefore most important to check the return
f     value of this function to ensure that the correct number of
f     characters have been read.
*     - An error will result if a value is read which appears to have
*     the correct format, but which cannot be converted into a valid
*     coordinate value (for instance, because the value of one or more
*     of its fields is invalid).
*     - The string "<bad>" is recognised as a special case and will
*     yield the coordinate value AST__BAD without error. The test for
*     this string is case-insensitive and also permits embedded white
*     space.
c     - A function result of zero will be returned and no coordinate
c     value will be returned via the "value" pointer if this function
c     is invoked with the AST error status set, or if it should fail
c     for any reason.
f     - A function result of zero will be returned and no coordinate
f     value will be returned via the VALUE argument if this function
f     is invoked with the AST error status set, or if it should fail
f     for any reason.
*--

*  Implementation Notes:
*     This function implements the public interface for the
*     astUnformat method. It is identical to astUnformat_ except that:
*
*     - The axis index is decremented by 1 before use. This allows the
*     public interface to use 1-based axis numbers (whereas internally
*     axis numbers are zero-based).
*/

/* Invoke the normal astUnformat_ function, adjusting the axis index
   to become zero-based. */
   return astUnformat_( this, axis - 1, string, value );
}
