      SUBROUTINE CCG1_FMR3R( STACK, NPIX, NLINES, MINPIX, RESULT, 
     :                         WRK1, NCON, POINT, USED, STATUS )
*+
*  Name:
*     CCG1_FMR3

*  Purpose:
*     Combines data lines using a fast median.

*  Language:
*     Starlink Fortran 77

*  Invocation:
*     CALL CCG1_FMR3R( STACK, NPIX, NLINES, MINPIX, RESULT, 
*                        WRK1, NCON, POINT, USED, STATUS )

*  Description:
*     This routine accepts an array consisting a series of (vectorised)
*     lines of data. The data values in the lines are then combined to
*     form a (fast) median line. This method uses no weights and returns
*     no variances.
*
*     The method used is based on Wirth's algorithm for selecting the
*     Kth value, which is very fast compared to a full sort.
*
*     The output medians are returned in the array RESULT.

*  Arguments:
*     STACK( NPIX, NLINES ) = REAL (Given)
*        The array of lines which are to be combined into a single line.
*     NPIX = INTEGER (Given)
*        The number of pixels in a line of data.
*     NLINES = INTEGER (Given)
*        The number of lines of data in the stack.
*     MINPIX = INTEGER (Given)
*        The minimum number of pixels required to contribute to an
*        output pixel.
*     RESULT( NPIX ) = REAL (Returned)
*        The output line of data.
*     WRK1( NLINES ) = REAL (Given and Returned)
*        Workspace for calculations.
*     NCON( NLINES ) = DOUBLE PRECISION (Given and Returned)
*        The actual number of contributing pixels.
*     POINT( NLINES ) = INTEGER (Given and Returned)
*        Workspace to hold pointers to the original positions of the
*        data before extraction and conversion in to the WRK1 array.
*     USED( NLINES ) = LOGICAL (Given and Returned)
*        Workspace used to indicate which values have been used in
*        estimating a resultant value.
*     STATUS = INTEGER (Given and Returned)
*        The global status.

*  Authors:
*     PDRAPER: Peter Draper (STARLINK)
*     {enter_new_authors_here}

*  History:
*     18-NOV-1998 (PDRAPER):
*        Original version.
*     {enter_changes_here}

*  Bugs:
*     {note_any_bugs_here}

*-

*  Type Definitions:
      IMPLICIT NONE              ! No implicit typing

*  Global Constants:
      INCLUDE 'SAE_PAR'          ! Standard SAE constants
      INCLUDE 'PRM_PAR'          ! PRIMDAT constants

*  Arguments Given:
      INTEGER NPIX
      INTEGER NLINES
      INTEGER MINPIX
      REAL STACK( NPIX, NLINES )

*  Arguments Given and Returned:
      DOUBLE PRECISION NCON( NLINES )
      REAL WRK1( NLINES )

*  Arguments Returned:
      REAL RESULT( NPIX )
      LOGICAL USED( NLINES )
      INTEGER POINT( NLINES )


*  Status:
      INTEGER STATUS             ! Global status

*  Global Variables:
      INCLUDE 'NUM_CMN'          ! Numerical error flag

*  External References:
      EXTERNAL NUM_TRAP
      INTEGER NUM_TRAP           ! Numerical error handler

*  Local Constants:
      DOUBLE PRECISION PIBY2
      PARAMETER ( PIBY2 = 1.57079632679  ) ! PI/2.0

*  Local Variables:
      DOUBLE PRECISION SUM      ! Variance sum
      DOUBLE PRECISION VAL      ! Weighted median
      INTEGER I                 ! Loop variable
      INTEGER IMED              ! Position of median value
      INTEGER J                 ! Loop variable
      INTEGER NGOOD             ! Number of good pixels
      REAL  RVAL                ! Real variable

*  Internal References:
      INCLUDE 'NUM_DEC_CVT'      ! NUM_ type conversion functions
      INCLUDE 'NUM_DEF_CVT'      ! Define functions...

*.

*  Check inherited global status.
      IF ( STATUS .NE. SAI__OK ) RETURN

*  Set the numeric error and set error flag value.
      CALL NUM_HANDL( NUM_TRAP )

*  Loop over for all possible output pixels.
      DO 1 I = 1, NPIX
         NGOOD = 0
         NUM_ERROR = SAI__OK
         SUM = 0.0D0

*  Loop over all possible contributing pixels.
         DO 2 J = 1, NLINES
            IF( STACK( I, J ) .NE. VAL__BADR ) THEN

*  Increment good value counter.
               NGOOD = NGOOD + 1

*  Update the pointers to the positions of the unextracted data.
               POINT( NGOOD ) = J

*  Convert input type floating point value.
               WRK1( NGOOD ) = NUM_RTOR( STACK( I, J ) )

*  Trap conversion failures.
               IF ( NUM_ERROR .NE. SAI__OK ) THEN

*  Decrement NGOOD, do not let it go below zero.
                  NGOOD = MAX( 0, NGOOD - 1 )
                  NUM_ERROR = SAI__OK
               END IF
            END IF
 2       CONTINUE

*  Continue if more than one good value.
         IF ( NGOOD .GT. 0 ) THEN

*  If only one value is available then this is the median.
            IF ( NGOOD .EQ. 1 ) THEN
               VAL = WRK1( 1 )
            ELSE

*  Get position at median, or one above (for even)
               IMED = NGOOD / 2 + 1

*  If number is even we need the central two values.
               IF ( MOD( NGOOD, 2 ) .EQ. 0 ) THEN

*  Select the two values needed for median, update the used counters.
                  CALL CCG1_KTHR( NGOOD, IMED, WRK1, POINT, RVAL,
     :                            STATUS )
                  VAL = DBLE( RVAL )
                  NCON( POINT( IMED ) ) = NCON( POINT( IMED ) ) + 1.0D0

                  IMED = IMED - 1
                  CALL CCG1_KTHR( NGOOD, IMED, WRK1, POINT, RVAL,
     :                            STATUS )
                  NCON( POINT( IMED ) ) = NCON( POINT( IMED ) ) + 1.0D0

*  Median is mean of these two values.
                  VAL = ( VAL + DBLE( RVAL ) ) * 0.5D0
               ELSE

*  Straight-forward case.
                  CALL CCG1_KTHR( NGOOD, IMED, WRK1, POINT, RVAL,
     :                            STATUS )
                  NCON( POINT( IMED ) ) = NCON( POINT( IMED ) ) + 1.0D0
                  VAL = DBLE( RVAL )
               END IF
            END IF
         END IF

*  If there are sufficient good pixels output the result.
         IF ( NGOOD .GE. MINPIX ) THEN
            RESULT( I ) = REAL( VAL )

*  Trap numeric errors.
            IF ( NUM_ERROR .NE. SAI__OK ) THEN
               RESULT( I ) = VAL__BADR
            END IF
         ELSE

*  Not enough contributing pixels, set output invalid.
            RESULT( I ) = VAL__BADR
         END IF
 1    CONTINUE

*  Remove the numerical error handler.
      CALL NUM_REVRT

      END
* $Id$
