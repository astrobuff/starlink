      SUBROUTINE CCG1_FMR1R( STACK, NPIX, NLINES, VARS, MINPIX, 
     :                         COVEC, NMAT, RESULT, RESVAR, WRK1, WRK2, 
     :                         NCON, POINT, USED, STATUS )
*+
*  Name:
*     CCG1_FMR1

*  Purpose:
*     Combines data lines using a fast median.

*  Language:
*     Starlink Fortran 77

*  Invocation:
*     CALL CCG1_FMR1R( STACK, NPIX, NLINES, VARS, MINPIX, COVEC, NMAT, 
*                        RESULT, RESVAR, WRK1, WRK2, NCON, POINT, USED, 
*                        STATUS )

*  Description:
*     This routine accepts an array consisting a series of (vectorised)
*     lines of data. The data values in the lines are then combined to
*     form a (fast) median line. This method uses no weights and returns
*     the estimated variances based on order stats for the population
*     sizes used at each output pixel.
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
*     VARS( NPIX, NLINES ) = REAL (Given)
*        The data variances.
*     MINPIX = INTEGER (Given)
*        The minimum number of pixels required to contribute to an
*        output pixel.
*     COVEC( NMAT, NLINES ) = DOUBLE PRECISION (Given)
*        The packed variance-covariance matrix of the order statistics
*        from a normal distribution of sizes up to NLINES, produced by
*        CCD1_ORVAR. This is only used when median is derived from
*        the central-2 values.
*     NMAT = INTEGER (Given)
*        Size of the first dimension of COVEC.
*     RESULT( NPIX ) = REAL (Returned)
*        The output line of data.
*     RESVAR( NPIX ) = REAL (Returned)
*        The output variance.
*     WRK1( NLINES ) = REAL (Given and Returned)
*        Workspace for calculations.
*     WRK2( NLINES ) = REAL (Given and Returned)
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
*     16-NOV-1998 (PDRAPER):
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
      INTEGER NMAT
      REAL STACK( NPIX, NLINES )
      REAL VARS( NPIX, NLINES )
      DOUBLE PRECISION COVEC( NMAT, NLINES )

*  Arguments Given and Returned:
      DOUBLE PRECISION NCON( NLINES )
      REAL WRK1( NLINES )
      REAL WRK2( NLINES )

*  Arguments Returned:
      REAL RESULT( NPIX )
      REAL RESVAR( NPIX )
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
      DOUBLE PRECISION VAR      ! Final variance
      DOUBLE PRECISION VSUM     ! Variance/covariance sum
      INTEGER I                 ! Loop variable
      INTEGER K                 ! Loop variable
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
            IF( STACK( I, J ) .NE. VAL__BADR .AND.
     :           VARS( I, J ) .NE. VAL__BADR ) THEN

*  Increment good value counter.
               NGOOD = NGOOD + 1

*  Update the pointers to the positions of the unextracted data.
               POINT( NGOOD ) = J

*  Convert input type floating point value.
               WRK1( NGOOD ) = NUM_RTOR( STACK( I, J ) )

*  Change variance to weight, increment sums.
               VAR = 1.0D0 / NUM_RTOD( VARS( I, J ) )
               SUM = SUM + VAR
               WRK2( NGOOD ) = REAL( VAR )

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
               VAR = WRK2( 1 )
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

*  Use order statistics to derive the correct modification of the
*  population variances.
                  VSUM = 0.0D0
                  DO 3 K = IMED, IMED + 1
                     DO 4 J = K, IMED + 1
                        
*  Sum variances and twice covariances ( off diagonal elements ).
                        IF( K .EQ. J ) THEN
                           VSUM = VSUM + 
     :                     COVEC( K + J * ( J - 1 ) / 2, NGOOD )
                        ELSE
                           VSUM = VSUM +
     :                     2.0D0 * COVEC( K + J * ( J - 1 )/ 2, NGOOD )
                        END IF
 4                   CONTINUE
 3                CONTINUE
                  VAR = ( 1.0D0 / SUM ) * VSUM * DBLE( NGOOD ) * 0.25
               ELSE

*  Straight-forward case.
                  CALL CCG1_KTHR( NGOOD, IMED, WRK1, POINT, RVAL,
     :                            STATUS )
                  NCON( POINT( IMED ) ) = NCON( POINT( IMED ) ) + 1.0D0
                  VAL = DBLE( RVAL )

*  Variance is PI/2 of population value.
                  VAR = ( 1.0D0 / SUM ) * DBLE( NGOOD ) * 
     :                  COVEC( IMED + IMED * ( IMED - 1 ) / 2, NGOOD ) 
               END IF
            END IF
         END IF

*  If there are sufficient good pixels output the result.
         IF ( NGOOD .GE. MINPIX ) THEN
            RESULT( I ) = REAL( VAL )
            RESVAR( I ) = REAL( VAR )

*  Trap numeric errors.
            IF ( NUM_ERROR .NE. SAI__OK ) THEN
               RESULT( I ) = VAL__BADR
               RESVAR( I ) = VAL__BADR
            END IF
         ELSE

*  Not enough contributing pixels, set output invalid.
            RESULT( I ) = VAL__BADR
            RESVAR( I ) = VAL__BADR
         END IF
 1    CONTINUE

*  Remove the numerical error handler.
      CALL NUM_REVRT

      END
* $Id$
