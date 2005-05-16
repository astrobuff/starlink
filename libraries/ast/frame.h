#if !defined( FRAME_INCLUDED )   /* Include this file only once */
#define FRAME_INCLUDED
/*
*+
*  Name:
*     frame.h

*  Type:
*     C include file.

*  Purpose:
*     Define the interface to the Frame class.

*  Invocation:
*     #include "frame.h"

*  Description:
*     This include file defines the interface to the Frame class and
*     provides the type definitions, function prototypes and macros, etc.
*     needed to use this class.
*
*     A Frame object encapsulates information about a coordinate
*     system, including its axes. It may also act as a "template" to
*     be matched against another Frame object. This process determines
*     whether it is possible to perform a coordinate transformation
*     between the two coordinates systems they describe.

*  Inheritance:
*     The Frame class inherits from the Mapping class.

*  Attributes Over-Ridden:
*     Nin (integer, readonly)
*        The Frame class sets this value to be equal to the number of
*        Frame axes.
*     Nout (integer, readonly)
*        The Frame class sets this value to be equal to the number of
*        Frame axes.

*  New Attributes Defined:
*     AlignSystem (string)
*        This attribute takes a value to identify the coordinate system
*        in which the Frame should be aligned with other Frames.
*     Digits [or Digits(axis)] (integer)
*        Specifies how many digits of precision are required by
*        default when a coordinate value for a Frame is formatted
*        (e.g. using the astFormat method). The Digits value acts as a
*        default only and is over-ridden if a Format string is
*        specified for an axis explicitly.
*
*        The default Digits value for a Frame is 7. This attribute
*        normally applies to all the Frame's axes, but reference may
*        be made to a particular axis by adding an axis subscript
*        (e.g. Digits(1)). If a value is set for an individual axis,
*        it will over-ride the Digits value for the Frame as a whole
*        when formatting values for that axis.
*     Direction(axis) (integer)
*        A boolean value which specifies how coordinate values for
*        each Frame axis should be displayed (e.g. in graphs). By
*        default, it has the value one, indicating that they should be
*        shown in the conventional sense (i.e. increasing left to
*        right for an abscissa and bottom to top for an ordinate). If
*        set to zero, this attribute indicates that the direction
*        should be reversed (as would often be done for an
*        astronomical magnitude or a right ascension axis, for
*        example).
*     Epoch (double)
*        This value is used to qualify coordinate systems by
*        giving the moment in time when the coordinates are known to
*        be correct. Often, this will be the date of observation.
*     Format(axis) (string)
*        Specifies the format to be used to display coordinate values
*        for each Frame axis (i.e. to convert them from binary to
*        character form). The interpretation of this string (e.g. by
*        derived classes) is left to the astFormat method which, in
*        turn will invoke the astAxisFormat for the Axis object that
*        describes each axis. By default, the Frame class supplies an
*        Axis class object for each axis and this will interpret this
*        parameter as a C "printf" format string which should be
*        capable of formatting a single coordinate value stored as a
*        double (e.g. "%1.7G"). If no Format string is set, the
*        default format is based on the value of the Digits attribute.
*     Label(axis) (string)
*        Specifies the label to be attached to each Frame axis when it
*        is represented in (e.g.) a graph. It is intended purely for
*        interpretation by human readers and not by software. The
*        default supplied by the Frame class is the string "Axis <n>",
*        where <n> is 1, 2, etc. for each successive axis.
*     MatchEnd (integer)
*        A boolean value that controls how a Frame behaves when used
*        as a template to match another Frame. If it is zero and a
*        template Frame matches a target frame which has a different
*        number of axes, then the axes wich occur first in the target
*        frame will be matched and any trailing axes in either the
*        target or template will be discarded (if necessary). If it is
*        non-zero, however, the last axes in each frame will be
*        matched and any un-matched leading axes will be discarded
*        instead. The default value supplied by the Frame class is
*        zero.
*     MaxAxes (integer)
*        Specifies the maximum number of axes in a target Frame that
*        can be matched when using the Frame as a template. Normally,
*        by default, this value is equal to the number of Frame axes,
*        so that a Frame will only match another Frame with the same
*        number of axes as itself. By setting a different value,
*        however, Frames with different numbers of axes may be matched
*        (the MatchEnd attribute then determines which of the
*        individual axes are matched).
*
*        When setting this value, the value of the MinAxes attribute
*        may be silently changed so that it remains consistent with
*        (i.e. does not exceed) the new value. The default value may
*        also be reduced if necessary to remain consistent with the
*        MinAxes value.
*     MinAxes (integer)
*        Specifies the minimum number of axes in a target Frame that
*        can be matched when using the Frame as a template. Normally,
*        by default, this value is equal to the number of Frame axes,
*        so that a Frame will only match another Frame with the same
*        number of axes as itself. By setting a different value,
*        however, Frames with different numbers of axes may be matched
*        (the MatchEnd attribute then determines which of the
*        individual axes are matched).
*
*        When setting this value, the value of the MaxAxes attribute
*        may be silently changed so that it remains consistent with
*        (i.e. is not less than) the new value. The default value may
*        also be reduced if necessary to remain consistent with the
*        MaxAxes value.
*     Domain (string)
*        A string which may be used to identify the physical domain to
*        which a Frame applies and used as an additional key when
*        matching a target Frame with a template. If the Domain
*        attribute in the template Frame is set, then only target
*        frames with the same Domain value will be matched. If a
*        Domain is not set in the template Frame, the target Frame's
*        Domain value will be ignored and has no effect on
*        matching. The default value supplied by the Frame class is an
*        empty string. Domain values are automatically converted to
*        upper case and all white space is removed before use.
*     Naxes (integer)
*        A read-only attribute that gives the number of axes in a
*        Frame (i.e.  the number of dimensions of the space which the
*        Frame describes). This value is determined when the Frame is
*        created.
*     Permute (integer)
*        A boolean value which specifies whether the axis order of a
*        target Frame may be permuted in order to obtain a match with
*        a template. If this value is set to zero in the template
*        Frame, it will only match a target if it can do so without
*        changing the order of its axes. The default value supplied by
*        the Frame class is 1 (i.e. allow axis permutations).
*     PreserveAxes (integer)
*        A boolean value which determines how the "result" Frame is
*        produced whan a target frame is matched by a template. If
*        this value is zero in the template Frame, then the result
*        Frame will have the same number of axes as the template. If
*        it is non-zero, however, the axes of the target Frame will be
*        preserved, so that the result Frame will have the same number
*        of axes as the target. The default supplied by the Frame
*        class is zero (i.e. target axes are not preserved).
*
*        The main use for this attribute is when the MaxAxes and/or
*        MinAxes attributes have been set to search for a Frame which
*        may have a different number of axes from the template. For
*        example, if a 2-dimensional template Frame matches a
*        3-dimensional target Frame, then by default the result is
*        2-dimensional and the last axis (normally) will be
*        discarded. However, if the template's PreserveAxes value is
*        non-zero, the result will instead be 3-dimensional to
*        correspond with the target Frame.
*     Symbol(axis) (string)
*        Specifies the symbol to be used to represent coordinate
*        values for each Frame axis in "short form", such as in
*        algebraic expressions where a full description of the axis
*        would be inappropriate. Examples include "RA" and "Dec" (for
*        Right Ascension and Declination).
*
*        The default supplied by the Frame class is the string
*        "<Domain><n>", where <n> is 1, 2, etc. for successive axes,
*        and <Domain> is the value of the Frame's Domain attribute
*        (with any white space replaced by underscores and truncated
*        if necessary so that the final string does not exceed 15
*        characters). If no Domain value has been set, "x" is used as
*        the <Domain> value in constructing this default string.
*     System (string)
*        This attribute takes a value to identify the coordinate system
*        used to describe positions within the domain of the Frame.
*     Title (string)
*        Specifies a string to be used as a title on (e.g.) graphs to
*        describe the coordinate system which the Frame
*        represents. Examples would be "Detector Coordinates" or
*        "Galactic Coordinates". This string is intended solely for
*        interpretation by human readers and not by software. The
*        default supplied by the Frame class is "<n>-D Coordinate
*        System", where <n> is the number of Frame axes.
*     Unit(axis) (string)
*        Describes the units used to represent coordinate values on
*        each Frame axis. The default supplied by the Frame class is
*        an empty string.

*  Methods Over-Ridden:
*     Public:
*        astGetNin
*           Get the number of input coordinates for a Frame.
*        astGetNout
*           Get the number of output coordinates for a Frame.
*        astTransform
*           Use a Frame to transform a set of points.

*     Protected:
*        astClearAttrib
*           Clear an attribute value for a Frame.
*        astGetAttrib
*           Get an attribute value for a Frame.
*        astReportPoints
*           Report the effect of transforming a set of points using a Frame.
*        astSetAttrib
*           Set an attribute value for a Frame.
*        astTestAttrib
*           Test if an attribute value has been set for a Frame.

*  New Methods Defined:
*     Public:
*        astAngle
*           Calculate the angle between three points.
*        astAxAngle
*           Find the angle from an axis to a line through two points.
*        astAxDistance
*           Calculate the distance between two axis values
*        astAxOffset
*           Calculate an offset along an axis
*        astConvert
*           Determine how to convert between two coordinate systems.
*        astDistance
*           Calculate the distance between two points.
*        astFindFrame
*           Find a coordinate system with specified characteristics
*        astFormat
*           Format a coordinate value for a Frame axis.
*        astNorm
*           Normalise a set of Frame coordinates.
*        astOffset
*           Calculate an offset along a geodesic curve.
*        astOffset2
*           Calculate an offset along a geodesic curve for a 2D Frame.
*        astPermAxes
*           Permute the order of a Frame's axes.
*        astPickAxes
*           Create a new Frame by picking axes from an existing one.
*        astResolve
*           Resolve a vector into two orthogonal components.
*        astUnformat
*           Read a formatted coordinate value for a Frame axis.

*     Protected:
*        astAbbrev
*           Abbreviate a formatted Frame axis value by skipping
*           leading fields.
*        astCheckPerm
*           Check that an array contains a valid permutation.
*        astClearDigits
*           Clear the Digits attribute for a Frame.
*        astClearDirection
*           Clear the Direction attribute for a Frame axis.
*        astClearDomain
*           Clear the Domain attribute for a Frame.
*        astClearFormat
*           Clear the Format attribute for a Frame axis.
*        astClearLabel
*           Clear the Label attribute for a Frame axis.
*        astClearMatchEnd
*           Clear the MatchEnd attribute for a Frame.
*        astClearMaxAxes
*           Clear the MaxAxes attribute for a Frame.
*        astClearMinAxes
*           Clear the MinAxes attribute for a Frame.
*        astClearPermute
*           Clear the Permute attribute for a Frame.
*        astClearPreserveAxes
*           Clear the PreserveAxes attribute for a Frame.
*        astClearSymbol
*           Clear the Symbol attribute for a Frame axis.
*        astClearSystem
*           Clear the value of the System attribute for a Frame.
*        astClearTitle
*           Clear the Title attribute for a Frame.
*        astClearUnit
*           Clear the Unit attribute for a Frame axis.
*        astConvertX
*           Determine how to convert between two coordinate systems.
*        astFields
*           Identify the fields within a formatted Frame axis value.
*        astGap
*           Find a "nice" gap for tabulating Frame axis values.
*        astGetAxis
*           Obtain a pointer to a specified Axis from a Frame.
*        astGetDigits
*           Get the value of the Digits attribute for a Frame.
*        astGetDirection
*           Get the value of the Direction attribute for a Frame axis.
*        astGetDomain
*           Get a pointer to the Domain attribute for a Frame.
*        astGetFormat
*           Get a pointer to the Format attribute for a Frame axis.
*        astGetLabel
*           Get a pointer to the Label attribute for a Frame axis.
*        astGetMatchEnd
*           Get the value of the MatchEnd attribute for a Frame.
*        astGetMaxAxes
*           Get the value of the MaxAxes attribute for a Frame.
*        astGetMinAxes
*           Get the value of the MinAxes attribute for a Frame.
*        astGetNaxes
*           Determine how many axes a Frame has.
*        astGetPerm
*           Access the axis permutation array for a Frame.
*        astGetPermute
*           Get the value of the Permute attribute for a Frame.
*        astGetPreserveAxes
*           Get the value of the PreserveAxes attribute for a Frame.
*        astGetSymbol
*           Get a pointer to the Symbol attribute for a Frame axis.
*        astGetSystem
*           Get the value of the System attribute for a Frame.
*        astGetTitle
*           Get a pointer to the Title attribute for a Frame.
*        astGetUnit
*           Get a pointer to the Unit attribute for a Frame axis.
*        astIsUnitFrame
*           Returns a flag indicating if a Frame is equivalent to a UnitMap.
*        astMatch
*           Determine if conversion is possible between two coordinate systems.
*        astOverlay
*           Overlay the attributes of a template Frame on to another Frame.
*        astPrimaryFrame
*           Uniquely identify a primary Frame and one of its axes.
*        astResolvePoints
*           Resolve many vectors into two orthogonal components.
*        astSetAxis
*           Set a new Axis for a Frame.
*        astSetDigits
*           Set the value of the Digits attribute for a Frame.
*        astSetDirection
*           Set the value of the Direction attribute for a Frame axis.
*        astSetDomain
*           Set the value of the Domain attribute for a Frame.
*        astSetFormat
*           Set the value of the Format attribute for a Frame axis.
*        astSetLabel
*           Set the value of the Label attribute for a Frame axis.
*        astSetMatchEnd
*           Set the value of the MatchEnd attribute for a Frame.
*        astSetMaxAxes
*           Set the value of the MaxAxes attribute for a Frame.
*        astSetMinAxes
*           Set the value of the MinAxes attribute for a Frame.
*        astSetPermute
*           Set the value of the Permute attribute for a Frame.
*        astSetPreserveAxes
*           Set the value of the PreserveAxes attribute for a Frame.
*        astSetSymbol
*           Set the value of the Symbol attribute for a Frame axis.
*        astSetSystem
*           Set the value of the System attribute for a Frame.
*        astSetTitle
*           Set the value of the Title attribute for a Frame.
*        astSetUnit
*           Set the value of the Unit attribute for a Frame axis.
*        astSubFrame
*           Select axes from a Frame and convert to the new coordinate system.
*        astTestDigits
*           Test whether a value has been set for the Digits attribute of a
*           Frame.
*        astTestDirection
*           Test whether a value has been set for the Direction attribute of a
*           Frame axis.
*        astTestDomain
*           Test whether a value has been set for the Domain attribute of a
*           Frame.
*        astTestFormat
*           Test whether a value has been set for the Format attribute of a
*           Frame axis.
*        astTestLabel
*           Test whether a value has been set for the Label attribute of a
*           Frame axis.
*        astTestMatchEnd
*           Test whether a value has been set for the MatchEnd attribute of a
*           Frame.
*        astTestMaxAxes
*           Test whether a value has been set for the MaxAxes attribute of a
*           Frame.
*        astTestMinAxes
*           Test whether a value has been set for the MinAxes attribute of a
*           Frame.
*        astTestPermute
*           Test whether a value has been set for the Permute attribute of a
*           Frame.
*        astTestPreserveAxes
*           Test whether a value has been set for the PreserveAxes attribute of
*           a Frame.
*        astTestSymbol
*           Test whether a value has been set for the Symbol attribute of a
*           Frame axis.
*        astTestSystem
*           Test whether a value has been set for the System attribute of a
*           Frame.
*        astTestTitle
*           Test whether a value has been set for the Title attribute of a
*           Frame.
*        astTestUnit
*           Test whether a value has been set for the Unit attribute of a Frame
*           axis.
*        astValidateAxis
*           Validate and permute a Frame's axis index.
*        astValidateAxisSelection
*           Check that a set of axes selected from a Frame is valid.
*        astValidateSystem
*           Validate a Frame's System attribute.
*        astSystemString
*           Return a string representation of a System code.
*        astSystemCode
*           Return a code for a string representation of a System value

*  Other Class Functions:
*     Public:
*        astFrame
*           Create a Frame.
*        astIsAFrame
*           Test class membership.

*     Protected:
*        astCheckFrame
*           Validate class membership.
*        astInitFrame
*           Initialise a Frame.
*        astInitFrameVtab
*           Initialise the virtual function table for the Frame class.
*        astLoadFrame
*           Load a Frame.

*  Macros:
*     Public:
*        None.

*     Protected:
*        AST__BADSYSTEM
*           A "bad" (undefined) value for the System attribute.

*  Type Definitions:
*     Public:
*        AstFrame
*           Frame object type.

*     Protected:
*        AstFrameVtab
*           Frame virtual function table type.
*        AstSystemType
*           Enumerated type used for the System attribute.

*  Feature Test Macros:
*     astCLASS
*        If the astCLASS macro is undefined, only public symbols are
*        made available, otherwise protected symbols (for use in other
*        class implementations) are defined. This macro also affects
*        the reporting of error context information, which is only
*        provided for external calls to the AST library.

*  Copyright:
*     <COPYRIGHT_STATEMENT>

*  Authors:
*     RFWS: R.F. Warren-Smith (Starlink)
*     DSB: B.S. Berry (Starlink)

*  History:
*     1-MAR-1996 (RFWS):
*        Original version.
*     25-APR-1996 (RFWS):
*        Tidied up, etc.
*     11-SEP-1996 (RFWS):
*        Added astGap (written by DSB).
*     10-JUN-1997 (RFWS):
*        Revised astConvert and added astFindFrame.
*     15-FEB-1998 (RFWS):
*        Added astUnformat.
*     21-JUN-2001 (DSB):
*        Added astAngle and astOffset2.
*     29-AUG-2001 (DSB):
*        Added astAxDistance and astAxOffset.
*     4-SEP-2001 (DSB):
*        Added astResolve.
*     9-SEP-2001 (DSB):
*        Added astBear.
*     21-SEP-2001 (DSB):
*        Replace astBear with astAxAngle.
*     15-NOV-2002 (DSB):
*        Moved System and Epoch attributes from SkyFrame into this class.
*        Added AlignSystem attribute.
*     8-JAN-2003 (DSB):
*        Added protected astInitFrameVtab method.
*     24-JAN-2004 (DSB):
*        o  Added astFields.
*        o  Added argument "fmt" to astAbbrev.
*     24-JUN-2004 (DSB):
*        Remove unused entry "void (* SetMatchRange)( AstFrame *, int, int );"
*        from AstFrameVtab structure.
*     9-NOV-2004 (DSB):
*        Added protected astIsAUnitFrame method.
*-
*/

/* Include files. */
/* ============== */
/* Interface definitions. */
/* ---------------------- */
#include "object.h"              /* Base Object class */
#include "axis.h"                /* Coordinate Axis class */
#include "mapping.h"             /* Coordinate mappings (parent class) */

#if defined(astCLASS)            /* Protected */
#include "channel.h"             /* I/O channels */
#endif

/* C header files. */
/* --------------- */
#include <stddef.h>

/* Macros. */
/* ------- */
#if defined(astCLASS)            /* Protected */

/* A bad value for the System attribute. */
#define AST__BADSYSTEM -1

/* The legal System values recognized by this class of Frame. */
#define AST__CART 0

#endif

/* Type Definitions. */
/* ================= */
/* Integer type used to store the System attribute values. */
typedef int AstSystemType;

/* Frame structure. */
/* ------------------- */
/* This structure contains all information that is unique to each object in
   the class (e.g. its instance variables). */
typedef struct AstFrame {

/* Attributes inherited from the parent class. */
   AstMapping mapping;           /* Parent class structure */

/* Attributes specific to objects in this class. */
   AstAxis **axis;               /* Pointer to array of Axis objects */
   char *domain;                 /* Pointer to Domain string */
   char *title;                  /* Pointer to Title string */
   double epoch;                 /* Epoch as Modified Julian Date */
   int *perm;                    /* Pointer to axis permutation array */
   int digits;                   /* Default digits of precision */
   int match_end;                /* Match final axes of target? */
   int active_unit;              /* Use Unit when aligning Frames? */
   int max_axes;                 /* Minimum no. axes matched */
   int min_axes;                 /* Max. no. axes matched */
   int naxes;                    /* Number of axes */
   int permute;                  /* Permute axes in order to match? */
   int preserve_axes;            /* Preserve target axes? */
   AstSystemType system;         /* Code identifying coordinate system */
   AstSystemType alignsystem;    /* Code for Alignment coordinate system */
} AstFrame;

/* Cached Line structure. */
/* ---------------------- */
/* This structure contains information describing a line segment within a
   2D Frame. It is used by other classes to store intermediate cached values
   relating to the line in order to speed up repeated operations on the
   line. */

typedef struct AstLineDef {
   AstFrame *frame;            /* Pointer to Frame in which the line is defined */
   double length;              /* Line length */
   double start[2];            /* Frame axis values at line start */
   double end[2];              /* Frame axis values at line end */
   double dir[2];              /* Unit vector defining line direction */
   double q[2];                /* Unit vector perpendicular to line */
} AstLineDef;

/* Virtual function table. */
/* ----------------------- */
/* The virtual function table makes a forward reference to the
   AstFrameSet structure which is not defined until "frameset.h" is
   included (below). Hence make a preliminary definition available
   now. */
struct AstFrameSet;

/* This table contains all information that is the same for all objects in the
   class (e.g. pointers to its virtual functions). */
#if defined(astCLASS)            /* Protected */
typedef struct AstFrameVtab {

/* Properties (e.g. methods) inherited from the parent class. */
   AstMappingVtab mapping;       /* Parent class virtual function table */

/* Unique flag value to determine class membership. */
   int *check;                   /* Check value */

/* Properties (e.g. methods) specific to this class. */
   AstAxis *(* GetAxis)( AstFrame *, int );
   AstFrame *(* PickAxes)( AstFrame *, int, const int[], AstMapping ** );
   AstLineDef *(* LineDef)( AstFrame *, const double[2], const double[2] );
   AstPointSet *(* ResolvePoints)( AstFrame *, const double [], const double [], AstPointSet *, AstPointSet * );
   const char *(* Abbrev)( AstFrame *, int, const char *, const char *, const char * );
   const char *(* Format)( AstFrame *, int, double );
   const char *(* GetDomain)( AstFrame * );
   const char *(* GetFormat)( AstFrame *, int );
   const char *(* GetLabel)( AstFrame *, int );
   const char *(* GetSymbol)( AstFrame *, int );
   const char *(* GetTitle)( AstFrame * );
   const char *(* GetUnit)( AstFrame *, int );
   const int *(* GetPerm)( AstFrame * );
   double (* Angle)( AstFrame *, const double[], const double[], const double[] );
   double (* Distance)( AstFrame *, const double[], const double[] );
   double (* Gap)( AstFrame *, int, double, int * );
   int (* Fields)( AstFrame *, int, const char *, const char *, int, char **, int *, double * );
   double (* AxDistance)( AstFrame *, int, double, double );
   double (* AxOffset)( AstFrame *, int, double, double );
   int (* AxIn)( AstFrame *, int, double, double, double, int );
   int (* GetDigits)( AstFrame * );
   int (* GetDirection)( AstFrame *, int );
   int (* GetMatchEnd)( AstFrame * );
   int (* GetMaxAxes)( AstFrame * );
   int (* GetMinAxes)( AstFrame * );
   int (* GetNaxes)( AstFrame * );
   int (* GetPermute)( AstFrame * );
   int (* GetPreserveAxes)( AstFrame * );
   int (* IsUnitFrame)( AstFrame * );
   int (* LineCrossing)( AstFrame *, AstLineDef *, AstLineDef *, double ** );
   int (* LineContains)( AstFrame *, AstLineDef *, int, double * );
   int (* Match)( AstFrame *, AstFrame *, int **, int **, AstMapping **, AstFrame ** );
   int (* SubFrame)( AstFrame *, AstFrame *, int, const int *, const int *, AstMapping **, AstFrame ** );
   int (* TestDigits)( AstFrame * );
   int (* TestDirection)( AstFrame *, int );
   int (* TestDomain)( AstFrame * );
   int (* TestFormat)( AstFrame *, int );
   int (* TestLabel)( AstFrame *, int );
   int (* TestMatchEnd)( AstFrame * );
   int (* TestMaxAxes)( AstFrame * );
   int (* TestMinAxes)( AstFrame * );
   int (* TestPermute)( AstFrame * );
   int (* TestPreserveAxes)( AstFrame * );
   int (* TestSymbol)( AstFrame *, int );
   int (* TestTitle)( AstFrame * );
   int (* TestUnit)( AstFrame *, int );
   int (* Unformat)( AstFrame *, int, const char *, double * );
   int (* ValidateAxis)( AstFrame *, int, const char * );
   AstSystemType (* ValidateSystem)( AstFrame *, AstSystemType, const char * );
   AstSystemType (* SystemCode)( AstFrame *, const char * );
   const char *(* SystemString)( AstFrame *, AstSystemType );
   struct AstFrameSet *(* Convert)( AstFrame *, AstFrame *, const char * );
   struct AstFrameSet *(* ConvertX)( AstFrame *, AstFrame *, const char * );
   struct AstFrameSet *(* FindFrame)( AstFrame *, AstFrame *, const char * );
   void (* CheckPerm)( AstFrame *, const int *, const char * );
   void (* ClearDigits)( AstFrame * );
   void (* ClearDirection)( AstFrame *, int );
   void (* ClearDomain)( AstFrame * );
   void (* ClearFormat)( AstFrame *, int );
   void (* ClearLabel)( AstFrame *, int );
   void (* ClearMatchEnd)( AstFrame * );
   void (* ClearMaxAxes)( AstFrame * );
   void (* ClearMinAxes)( AstFrame * );
   void (* ClearPermute)( AstFrame * );
   void (* ClearPreserveAxes)( AstFrame * );
   void (* ClearSymbol)( AstFrame *, int );
   void (* ClearTitle)( AstFrame * );
   void (* ClearUnit)( AstFrame *, int );
   void (* Norm)( AstFrame *, double[] );
   void (* NormBox)( AstFrame *, double *, double *, AstMapping * );
   void (* Offset)( AstFrame *, const double[], const double[], double, double[] );
   double (* AxAngle)( AstFrame *, const double[2], const double[2], int );
   double (* Offset2)( AstFrame *, const double[2], double, double, double[2] );
   void (* Overlay)( AstFrame *, const int *, AstFrame * );
   void (* PermAxes)( AstFrame *, const int[] );
   void (* PrimaryFrame)( AstFrame *, int, AstFrame **, int * );
   void (* Resolve)( AstFrame *, const double [], const double [], const double [], double [], double *, double * );
   void (* SetAxis)( AstFrame *, int, AstAxis * );
   void (* SetDigits)( AstFrame *, int );
   void (* SetDirection)( AstFrame *, int, int );
   void (* SetDomain)( AstFrame *, const char * );
   void (* SetFormat)( AstFrame *, int, const char * );
   void (* SetLabel)( AstFrame *, int, const char * );
   void (* SetMatchEnd)( AstFrame *, int );
   void (* SetMaxAxes)( AstFrame *, int );
   void (* SetMinAxes)( AstFrame *, int );
   void (* SetPermute)( AstFrame *, int );
   void (* SetPreserveAxes)( AstFrame *, int );
   void (* SetSymbol)( AstFrame *, int, const char * );
   void (* SetTitle)( AstFrame *, const char * );
   void (* SetUnit)( AstFrame *, int, const char * );
   void (* ValidateAxisSelection)( AstFrame *, int, const int *, const char * );
   void (* LineOffset)( AstFrame *, AstLineDef *, double, double, double[2] );

   double (* GetTop)( AstFrame *, int );
   int (* TestTop)( AstFrame *, int );
   void (* ClearTop)( AstFrame *, int );
   void (* SetTop)( AstFrame *, int, double );

   double (* GetBottom)( AstFrame *, int );
   int (* TestBottom)( AstFrame *, int );
   void (* ClearBottom)( AstFrame *, int );
   void (* SetBottom)( AstFrame *, int, double );

   AstSystemType (* GetSystem)( AstFrame * );
   int (* TestSystem)( AstFrame * );
   void (* ClearSystem)( AstFrame * );
   void (* SetSystem)( AstFrame *, AstSystemType );

   AstSystemType (* GetAlignSystem)( AstFrame * );
   int (* TestAlignSystem)( AstFrame * );
   void (* ClearAlignSystem)( AstFrame * );
   void (* SetAlignSystem)( AstFrame *, AstSystemType );

   double (* GetEpoch)( AstFrame * );
   int (* TestEpoch)( AstFrame * );
   void (* ClearEpoch)( AstFrame * );
   void (* SetEpoch)( AstFrame *, double );

   int (* TestActiveUnit)( AstFrame * );
   int (* GetActiveUnit)( AstFrame * );
   void (* SetActiveUnit)( AstFrame *, int );

} AstFrameVtab;


#endif

/* More include files. */
/* =================== */
/* The interface to the FrameSet class must be included here (after
   the type definitions for the Frame class) because "frameset.h"
   itself includes this file ("frame.h"), although "frameset.h" refers
   to the AstFrameSet structure above. This seems a little strange at
   first, but is simply analogous to making a forward reference to a
   structure type when recursively defining a normal C structure
   (except that here the definitions happen to be in separate include
   files). */
#include "frameset.h"

/* Function prototypes. */
/* ==================== */
/* Prototypes for standard class functions. */
/* ---------------------------------------- */
astPROTO_CHECK(Frame)            /* Check class membership */
astPROTO_ISA(Frame)              /* Test class membership */

/* Constructor. */
#if defined(astCLASS)            /* Protected */
AstFrame *astFrame_( int, const char *, ... );
#else
AstFrame *astFrameId_( int, const char *, ... );
#endif

#if defined(astCLASS)            /* Protected */

/* Initialiser. */
AstFrame *astInitFrame_( void *, size_t, int, AstFrameVtab *, const char *,
                         int );

/* Vtab initialiser. */
void astInitFrameVtab_( AstFrameVtab *, const char * );

/* Loader. */
AstFrame *astLoadFrame_( void *, size_t, AstFrameVtab *,
                         const char *, AstChannel *channel );
#endif

/* Prototypes for member functions. */
/* -------------------------------- */
AstFrameSet *astConvert_( AstFrame *, AstFrame *, const char * );
AstFrameSet *astFindFrame_( AstFrame *, AstFrame *, const char * );
double astAngle_( AstFrame *, const double[], const double[], const double[] );
double astDistance_( AstFrame *, const double[], const double[] );
void astNorm_( AstFrame *, double[] );
double astAxDistance_( AstFrame *, int, double, double );
double astAxOffset_( AstFrame *, int, double, double );
double astAxAngle_( AstFrame *, const double[2], const double[2], int );
double astOffset2_( AstFrame *, const double[2], double, double, double[2] );
void astOffset_( AstFrame *, const double[], const double[], double, double[] );
void astResolve_( AstFrame *, const double [], const double [], const double [], double [], double *, double * );
int astGetActiveUnit_( AstFrame * );
void astSetActiveUnit_( AstFrame *, int );

#if defined(astCLASS)            /* Protected */
void astNormBox_( AstFrame *, double *, double *, AstMapping * );
AstFrame *astPickAxes_( AstFrame *, int, const int[], AstMapping ** );
const char *astFormat_( AstFrame *, int, double );
int astUnformat_( AstFrame *, int, const char *, double * );
void astPermAxes_( AstFrame *, const int[] );
#else
AstFrame *astPickAxesId_( AstFrame *, int, const int[], AstMapping ** );
const char *astFormatId_( AstFrame *, int, double );
int astUnformatId_( AstFrame *, int, const char *, double * );
void astPermAxesId_( AstFrame *, const int[] );
#endif

#if defined(astCLASS)            /* Protected */
int astAxIn_( AstFrame *, int, double, double, double, int );
AstAxis * astGetAxis_( AstFrame *, int );
AstFrameSet *astConvertX_( AstFrame *, AstFrame *, const char * );
AstLineDef *astLineDef_( AstFrame *, const double[2], const double[2] );
AstPointSet *astResolvePoints_( AstFrame *, const double [], const double [], AstPointSet *, AstPointSet * );
const char *astAbbrev_( AstFrame *, int, const char *, const char *, const char * );
const char *astGetDomain_( AstFrame * );
const char *astGetFormat_( AstFrame *, int );
const char *astGetLabel_( AstFrame *, int );
const char *astGetSymbol_( AstFrame *, int );
const char *astGetTitle_( AstFrame * );
const char *astGetUnit_( AstFrame *, int );
const int *astGetPerm_( AstFrame * );
double astGap_( AstFrame *, int, double, int * );
int astFields_( AstFrame *, int, const char *, const char *, int, char **, int *, double * );
int astGetDigits_( AstFrame * );
int astGetDirection_( AstFrame *, int );
int astGetMatchEnd_( AstFrame * );
int astGetMaxAxes_( AstFrame * );
int astGetMinAxes_( AstFrame * );
int astGetNaxes_( AstFrame * );
int astGetPermute_( AstFrame * );
int astGetPreserveAxes_( AstFrame * );
int astIsUnitFrame_( AstFrame * );
int astLineCrossing_( AstFrame *, AstLineDef *, AstLineDef *, double **);
int astLineContains_( AstFrame *, AstLineDef *, int, double * );
int astMatch_( AstFrame *, AstFrame *, int **, int **, AstMapping **, AstFrame ** );
int astSubFrame_( AstFrame *, AstFrame *, int, const int *, const int *, AstMapping **, AstFrame ** );
int astTestDigits_( AstFrame * );
int astTestDirection_( AstFrame *, int );
int astTestDomain_( AstFrame * );
int astTestFormat_( AstFrame *, int );
int astTestLabel_( AstFrame *, int );
int astTestMatchEnd_( AstFrame * );
int astTestMaxAxes_( AstFrame * );
int astTestMinAxes_( AstFrame * );
int astTestPermute_( AstFrame * );
int astTestPreserveAxes_( AstFrame * );
int astTestSymbol_( AstFrame *, int );
int astTestTitle_( AstFrame * );
int astTestUnit_( AstFrame *, int );
int astValidateAxis_( AstFrame *, int, const char * );
AstSystemType astValidateSystem_( AstFrame *, AstSystemType, const char * );
AstSystemType astSystemCode_( AstFrame *, const char * );
const char *astSystemString_( AstFrame *, AstSystemType );
void astCheckPerm_( AstFrame *, const int *, const char * );
void astClearDigits_( AstFrame * );
void astClearDirection_( AstFrame *, int );
void astClearDomain_( AstFrame * );
void astClearFormat_( AstFrame *, int );
void astClearLabel_( AstFrame *, int );
void astClearMatchEnd_( AstFrame * );
void astClearMaxAxes_( AstFrame * );
void astClearMinAxes_( AstFrame * );
void astClearPermute_( AstFrame * );
void astClearPreserveAxes_( AstFrame * );
void astClearSymbol_( AstFrame *, int );
void astClearTitle_( AstFrame * );
void astClearUnit_( AstFrame *, int );
void astOverlay_( AstFrame *, const int *, AstFrame * );
void astPrimaryFrame_( AstFrame *, int, AstFrame **, int * );
void astSetAxis_( AstFrame *, int, AstAxis * );
void astSetDigits_( AstFrame *, int );
void astSetDirection_( AstFrame *, int, int );
void astSetDomain_( AstFrame *, const char * );
void astSetFormat_( AstFrame *, int, const char * );
void astSetLabel_( AstFrame *, int, const char * );
void astSetMatchEnd_( AstFrame *, int );
void astSetMaxAxes_( AstFrame *, int );
void astSetMinAxes_( AstFrame *, int );
void astSetPermute_( AstFrame *, int );
void astSetPreserveAxes_( AstFrame *, int );
void astSetSymbol_( AstFrame *, int, const char * );
void astSetTitle_( AstFrame *, const char * );
void astSetUnit_( AstFrame *, int, const char * );
void astValidateAxisSelection_( AstFrame *, int, const int *, const char * );
double astReadDateTime_( const char * );
const char *astFmtDecimalYr_( double, int);
void astLineOffset_( AstFrame *, AstLineDef *, double, double, double[2] );

double astGetTop_( AstFrame *, int );
int astTestTop_( AstFrame *, int );
void astClearTop_( AstFrame *, int );
void astSetTop_( AstFrame *, int, double );

double astGetBottom_( AstFrame *, int );
int astTestBottom_( AstFrame *, int );
void astClearBottom_( AstFrame *, int );
void astSetBottom_( AstFrame *, int, double );

AstSystemType astGetSystem_( AstFrame * );
int astTestSystem_( AstFrame * );
void astClearSystem_( AstFrame * );
void astSetSystem_( AstFrame *, AstSystemType );

AstSystemType astGetAlignSystem_( AstFrame * );
int astTestAlignSystem_( AstFrame * );
void astClearAlignSystem_( AstFrame * );
void astSetAlignSystem_( AstFrame *, AstSystemType );

double astGetEpoch_( AstFrame * );
int astTestEpoch_( AstFrame * );
void astClearEpoch_( AstFrame * );
void astSetEpoch_( AstFrame *, double );

int astTestActiveUnit_( AstFrame * );
#endif

/* Function interfaces. */
/* ==================== */
/* These macros are wrap-ups for the functions defined by this class to make
   them easier to invoke (e.g. to avoid type mis-matches when passing pointers
   to objects from derived classes). */

/* Interfaces to standard class functions. */
/* --------------------------------------- */
/* Some of these functions provide validation, so we cannot use them to
   validate their own arguments. We must use a cast when passing object
   pointers (so that they can accept objects from derived classes). */

/* Check class membership. */
#define astCheckFrame(this) astINVOKE_CHECK(Frame,this)

/* Test class membership. */
#define astIsAFrame(this) astINVOKE_ISA(Frame,this)

/* Constructor. */
#if defined(astCLASS)            /* Protected */
#define astFrame astINVOKE(F,astFrame_)
#else
#define astFrame astINVOKE(F,astFrameId_)
#endif

#if defined(astCLASS)            /* Protected */

/* Initialiser. */
#define astInitFrame(mem,size,init,vtab,name,naxes) \
astINVOKE(O,astInitFrame_(mem,size,init,vtab,name,naxes))

/* Vtab Initialiser. */
#define astInitFrameVtab(vtab,name) astINVOKE(V,astInitFrameVtab_(vtab,name))
/* Loader. */
#define astLoadFrame(mem,size,vtab,name,channel) \
astINVOKE(O,astLoadFrame_(mem,size,vtab,name,astCheckChannel(channel)))
#endif

/* Interfaces to public member functions. */
/* -------------------------------------- */
/* Here we make use of astCheckFrame to validate Frame pointers before
   use. This provides a contextual error report if a pointer to the
   wrong sort of Object is supplied. */
#define astConvert(from,to,domainlist) \
astINVOKE(O,astConvert_(astCheckFrame(from),astCheckFrame(to),domainlist))
#define astAngle(this,a,b,c) \
astINVOKE(V,astAngle_(astCheckFrame(this),a,b,c))
#define astDistance(this,point1,point2) \
astINVOKE(V,astDistance_(astCheckFrame(this),point1,point2))
#define astFindFrame(target,template,domainlist) \
astINVOKE(O,astFindFrame_(astCheckFrame(target),astCheckFrame(template),domainlist))
#define astNorm(this,value) \
astINVOKE(V,astNorm_(astCheckFrame(this),value))
#define astAxDistance(this,axis,v1,v2) \
astINVOKE(V,astAxDistance_(astCheckFrame(this),axis,v1,v2))
#define astAxOffset(this,axis,v1,dist) \
astINVOKE(V,astAxOffset_(astCheckFrame(this),axis,v1,dist))
#define astOffset(this,point1,point2,offset,point3) \
astINVOKE(V,astOffset_(astCheckFrame(this),point1,point2,offset,point3))
#define astAxAngle(this,a,b,axis) \
astINVOKE(V,astAxAngle_(astCheckFrame(this),a,b,axis))
#define astOffset2(this,point1,angle,offset,point2) \
astINVOKE(V,astOffset2_(astCheckFrame(this),point1,angle,offset,point2))
#define astResolve(this,point1,point2,point3,point4,d1,d2) \
astINVOKE(V,astResolve_(astCheckFrame(this),point1,point2,point3,point4,d1,d2))
#define astGetActiveUnit(this) \
astINVOKE(V,astGetActiveUnit_(astCheckFrame(this)))
#define astSetActiveUnit(this,value) \
astINVOKE(V,astSetActiveUnit_(astCheckFrame(this),value))

#if defined(astCLASS)            /* Protected */
#define astNormBox(this,lbnd,ubnd,reg) \
astINVOKE(V,astNormBox_(astCheckFrame(this),lbnd,ubnd,astCheckMapping(reg)))
#define astFormat(this,axis,value) \
astINVOKE(V,astFormat_(astCheckFrame(this),axis,value))
#define astPermAxes(this,perm) \
astINVOKE(V,astPermAxes_(astCheckFrame(this),perm))
#define astPickAxes(this,naxes,axes,map) \
astINVOKE(O,astPickAxes_(astCheckFrame(this),naxes,axes,(AstMapping **)(map)))
#define astUnformat(this,axis,string,value) \
astINVOKE(V,astUnformat_(astCheckFrame(this),axis,string,value))
#else
#define astFormat(this,axis,value) \
astINVOKE(V,astFormatId_(astCheckFrame(this),axis,value))
#define astPermAxes(this,perm) \
astINVOKE(V,astPermAxesId_(astCheckFrame(this),perm))
#define astPickAxes(this,naxes,axes,map) \
astINVOKE(O,astPickAxesId_(astCheckFrame(this),naxes,axes,(AstMapping **)(map)))
#define astUnformat(this,axis,string,value) \
astINVOKE(V,astUnformatId_(astCheckFrame(this),axis,string,value))
#endif

#if defined(astCLASS)            /* Protected */
#define astAxIn(this,axis,lo,hi,val,closed) \
astINVOKE(V,astAxIn_(astCheckFrame(this),axis,lo,hi,val,closed))
#define astAbbrev(this,axis,fmt,str1,str2) \
astINVOKE(V,astAbbrev_(astCheckFrame(this),axis,fmt,str1,str2))
#define astFields(this,axis,fmt,str,maxfld,fields,nc,val) \
astINVOKE(V,astFields_(astCheckFrame(this),axis,fmt,str,maxfld,fields,nc,val))
#define astCheckPerm(this,perm,method) \
astINVOKE(V,astCheckPerm_(astCheckFrame(this),perm,method))
#define astResolvePoints(this,p1,p2,in,out) \
astINVOKE(O,astResolvePoints_(astCheckFrame(this),p1,p2,astCheckPointSet(in),((out)?astCheckPointSet(out):NULL)))
#define astLineDef(this,p1,p2) \
astINVOKE(V,astLineDef_(astCheckFrame(this),p1,p2))
#define astLineOffset(this,line,par,prp,point) \
astINVOKE(V,astLineOffset_(astCheckFrame(this),line,par,prp,point))
#define astLineCrossing(this,l1,l2,cross) \
astINVOKE(V,astLineCrossing_(astCheckFrame(this),l1,l2,cross))
#define astLineContains(this,l,def,point) \
astINVOKE(V,astLineContains_(astCheckFrame(this),l,def,point))
#define astClearDigits(this) \
astINVOKE(V,astClearDigits_(astCheckFrame(this)))
#define astClearDirection(this,axis) \
astINVOKE(V,astClearDirection_(astCheckFrame(this),axis))
#define astClearDomain(this) \
astINVOKE(V,astClearDomain_(astCheckFrame(this)))
#define astClearFormat(this,axis) \
astINVOKE(V,astClearFormat_(astCheckFrame(this),axis))
#define astClearLabel(this,axis) \
astINVOKE(V,astClearLabel_(astCheckFrame(this),axis))
#define astClearMatchEnd(this) \
astINVOKE(V,astClearMatchEnd_(astCheckFrame(this)))
#define astClearMaxAxes(this) \
astINVOKE(V,astClearMaxAxes_(astCheckFrame(this)))
#define astClearMinAxes(this) \
astINVOKE(V,astClearMinAxes_(astCheckFrame(this)))
#define astClearPermute(this) \
astINVOKE(V,astClearPermute_(astCheckFrame(this)))
#define astClearPreserveAxes(this) \
astINVOKE(V,astClearPreserveAxes_(astCheckFrame(this)))
#define astClearSymbol(this,axis) \
astINVOKE(V,astClearSymbol_(astCheckFrame(this),axis))
#define astClearTitle(this) \
astINVOKE(V,astClearTitle_(astCheckFrame(this)))
#define astClearUnit(this,axis) \
astINVOKE(V,astClearUnit_(astCheckFrame(this),axis))
#define astConvertX(to,from,domainlist) \
astINVOKE(O,astConvertX_(astCheckFrame(to),astCheckFrame(from),domainlist))
#define astGap(this,axis,gap,ntick) \
astINVOKE(V,astGap_(astCheckFrame(this),axis,gap,ntick))
#define astGetAxis(this,axis) \
astINVOKE(O,astGetAxis_(astCheckFrame(this),axis))
#define astGetDigits(this) \
astINVOKE(V,astGetDigits_(astCheckFrame(this)))
#define astGetDirection(this,axis) \
astINVOKE(V,astGetDirection_(astCheckFrame(this),axis))
#define astGetDomain(this) \
astINVOKE(V,astGetDomain_(astCheckFrame(this)))
#define astGetFormat(this,axis) \
astINVOKE(V,astGetFormat_(astCheckFrame(this),axis))
#define astGetLabel(this,axis) \
astINVOKE(V,astGetLabel_(astCheckFrame(this),axis))
#define astGetMatchEnd(this) \
astINVOKE(V,astGetMatchEnd_(astCheckFrame(this)))
#define astGetMaxAxes(this) \
astINVOKE(V,astGetMaxAxes_(astCheckFrame(this)))
#define astGetMinAxes(this) \
astINVOKE(V,astGetMinAxes_(astCheckFrame(this)))
#define astGetNaxes(this) \
astINVOKE(V,astGetNaxes_(astCheckFrame(this)))
#define astGetPerm(this) \
astINVOKE(V,astGetPerm_(astCheckFrame(this)))
#define astGetPermute(this) \
astINVOKE(V,astGetPermute_(astCheckFrame(this)))
#define astGetPreserveAxes(this) \
astINVOKE(V,astGetPreserveAxes_(astCheckFrame(this)))
#define astGetSymbol(this,axis) \
astINVOKE(V,astGetSymbol_(astCheckFrame(this),axis))
#define astGetTitle(this) \
astINVOKE(V,astGetTitle_(astCheckFrame(this)))
#define astGetUnit(this,axis) \
astINVOKE(V,astGetUnit_(astCheckFrame(this),axis))
#define astMatch(template,target,template_axes,target_axes,map,result) \
astINVOKE(V,astMatch_(astCheckFrame(template),astCheckFrame(target),template_axes,target_axes,(AstMapping **)(map),(AstFrame **)(result)))
#define astIsUnitFrame(this) \
astINVOKE(V,astIsUnitFrame_(astCheckFrame(this)))
#define astOverlay(template,template_axes,result) \
astINVOKE(V,astOverlay_(astCheckFrame(template),template_axes,astCheckFrame(result)))
#define astPrimaryFrame(this,axis1,frame,axis2) \
astINVOKE(V,astPrimaryFrame_(astCheckFrame(this),axis1,(AstFrame **)(frame),axis2))
#define astSetAxis(this,axis,newaxis) \
astINVOKE(V,astSetAxis_(astCheckFrame(this),axis,astCheckAxis(newaxis)))
#define astSetDigits(this,digits) \
astINVOKE(V,astSetDigits_(astCheckFrame(this),digits))
#define astSetDirection(this,axis,direction) \
astINVOKE(V,astSetDirection_(astCheckFrame(this),axis,direction))
#define astSetDomain(this,domain) \
astINVOKE(V,astSetDomain_(astCheckFrame(this),domain))
#define astSetFormat(this,axis,format) \
astINVOKE(V,astSetFormat_(astCheckFrame(this),axis,format))
#define astSetLabel(this,axis,label) \
astINVOKE(V,astSetLabel_(astCheckFrame(this),axis,label))
#define astSetMatchEnd(this,value) \
astINVOKE(V,astSetMatchEnd_(astCheckFrame(this),value))
#define astSetMaxAxes(this,value) \
astINVOKE(V,astSetMaxAxes_(astCheckFrame(this),value))
#define astSetMinAxes(this,value) \
astINVOKE(V,astSetMinAxes_(astCheckFrame(this),value))
#define astSetPermute(this,value) \
astINVOKE(V,astSetPermute_(astCheckFrame(this),value))
#define astSetPreserveAxes(this,value) \
astINVOKE(V,astSetPreserveAxes_(astCheckFrame(this),value))
#define astSetSymbol(this,axis,symbol) \
astINVOKE(V,astSetSymbol_(astCheckFrame(this),axis,symbol))
#define astSetTitle(this,title) \
astINVOKE(V,astSetTitle_(astCheckFrame(this),title))
#define astSetUnit(this,axis,unit) \
astINVOKE(V,astSetUnit_(astCheckFrame(this),axis,unit))
#define astSubFrame(target,template,result_naxes,target_axes,template_axes,map,result) \
astINVOKE(V,astSubFrame_(astCheckFrame(target),template?astCheckFrame(template):NULL,result_naxes,target_axes,template_axes,(AstMapping **)(map),(AstFrame **)(result)))
#define astTestDigits(this) \
astINVOKE(V,astTestDigits_(astCheckFrame(this)))
#define astTestDirection(this,axis) \
astINVOKE(V,astTestDirection_(astCheckFrame(this),axis))
#define astTestDomain(this) \
astINVOKE(V,astTestDomain_(astCheckFrame(this)))
#define astTestFormat(this,axis) \
astINVOKE(V,astTestFormat_(astCheckFrame(this),axis))
#define astTestLabel(this,axis) \
astINVOKE(V,astTestLabel_(astCheckFrame(this),axis))
#define astTestMatchEnd(this) \
astINVOKE(V,astTestMatchEnd_(astCheckFrame(this)))
#define astTestMaxAxes(this) \
astINVOKE(V,astTestMaxAxes_(astCheckFrame(this)))
#define astTestMinAxes(this) \
astINVOKE(V,astTestMinAxes_(astCheckFrame(this)))
#define astTestPermute(this) \
astINVOKE(V,astTestPermute_(astCheckFrame(this)))
#define astTestPreserveAxes(this) \
astINVOKE(V,astTestPreserveAxes_(astCheckFrame(this)))
#define astTestSymbol(this,axis) \
astINVOKE(V,astTestSymbol_(astCheckFrame(this),axis))
#define astTestTitle(this) \
astINVOKE(V,astTestTitle_(astCheckFrame(this)))
#define astTestUnit(this,axis) \
astINVOKE(V,astTestUnit_(astCheckFrame(this),axis))
#define astValidateAxis(this,axis,method) \
astINVOKE(V,astValidateAxis_(astCheckFrame(this),axis,method))
#define astValidateAxisSelection(this,naxes,axes,method) \
astINVOKE(V,astValidateAxisSelection_(astCheckFrame(this),naxes,axes,method))

#define astFmtDecimalYr astFmtDecimalYr_
#define astReadDateTime astReadDateTime_

#define astValidateSystem(this,system,method) \
astINVOKE(V,astValidateSystem_(astCheckFrame(this),system,method))
#define astSystemString(this,system) \
astINVOKE(V,astSystemString_(astCheckFrame(this),system))
#define astSystemCode(this,system) \
astINVOKE(V,astSystemCode_(astCheckFrame(this),system))

#define astClearTop(this,axis) \
astINVOKE(V,astClearTop_(astCheckFrame(this),axis))
#define astGetTop(this,axis) \
astINVOKE(V,astGetTop_(astCheckFrame(this),axis))
#define astSetTop(this,axis,value) \
astINVOKE(V,astSetTop_(astCheckFrame(this),axis,value))
#define astTestTop(this,axis) \
astINVOKE(V,astTestTop_(astCheckFrame(this),axis))

#define astClearBottom(this,axis) \
astINVOKE(V,astClearBottom_(astCheckFrame(this),axis))
#define astGetBottom(this,axis) \
astINVOKE(V,astGetBottom_(astCheckFrame(this),axis))
#define astSetBottom(this,axis,value) \
astINVOKE(V,astSetBottom_(astCheckFrame(this),axis,value))
#define astTestBottom(this,axis) \
astINVOKE(V,astTestBottom_(astCheckFrame(this),axis))

#define astClearSystem(this) \
astINVOKE(V,astClearSystem_(astCheckFrame(this)))
#define astGetSystem(this) \
astINVOKE(V,astGetSystem_(astCheckFrame(this)))
#define astSetSystem(this,value) \
astINVOKE(V,astSetSystem_(astCheckFrame(this),value))
#define astTestSystem(this) \
astINVOKE(V,astTestSystem_(astCheckFrame(this)))

#define astClearAlignSystem(this) \
astINVOKE(V,astClearAlignSystem_(astCheckFrame(this)))
#define astGetAlignSystem(this) \
astINVOKE(V,astGetAlignSystem_(astCheckFrame(this)))
#define astSetAlignSystem(this,value) \
astINVOKE(V,astSetAlignSystem_(astCheckFrame(this),value))
#define astTestAlignSystem(this) \
astINVOKE(V,astTestAlignSystem_(astCheckFrame(this)))

#define astClearEpoch(this) \
astINVOKE(V,astClearEpoch_(astCheckFrame(this)))
#define astGetEpoch(this) \
astINVOKE(V,astGetEpoch_(astCheckFrame(this)))
#define astSetEpoch(this,value) \
astINVOKE(V,astSetEpoch_(astCheckFrame(this),value))
#define astTestEpoch(this) \
astINVOKE(V,astTestEpoch_(astCheckFrame(this)))

#define astTestActiveUnit(this) \
astINVOKE(V,astTestActiveUnit_(astCheckFrame(this)))

#endif
#endif
