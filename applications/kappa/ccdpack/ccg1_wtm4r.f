      SUBROUTINE CCG1_WTM4R( ORDDAT, WEIGHT, VAR, LBND, UBND, NENT,
     :                         USED, COVAR, RESULT, RESVAR, STATUS )
*+
*  Name:
*     CCG1_WTM4R

*  Purpose:
*     To form the weighted median of a list of ordered data values.
*     Incrementing the contributing pixel buffers and estimating the
*     variance change.

*  Language:
*     Starlink Fortran 77

*  Invocation:
*      CALL CCG1_WTM4R( ORDDAT, WEIGHT, VAR, LBND, UBND, NENT, USED,
*                         COVAR, RESULT, RESVAR, STATUS )

*  Description:
*     This routine finds a value which can be associated with the half-
*     weight value. It sums all weights then finds a value for the
*     half-weight. The comparison with the half-weight value proceeds
*     in halves of the weights for each data point (half of the first
*     weight, then the second half of the first weight and the first
*     half of the second weight etc.) until the half weight is
*     exceeded. The data values around this half weight position are
*     then found and a linear interpolation of these values is the
*     weighted median. The values which contribute to the result are
*     flagged and passed through the USED array. This routine also
*     uses the order statistic covariance array (for a population NENT
*     big) to estimate the change in the variance from a optimal
*     measurement from the given population, returning the adjusted
*     variance.
*
*     This version is specialised to accept bounds for the values that
*     can be used in ARR, these are from ARR(LBND:UBND). It is designed
*     for use when some outliers of the population have already been
*     flagged for removal (say by some clipping algorithm and the
*     variance still needs to be determined from the original population
*     size).

*  Arguments:
*     ARR( NENT ) = REAL (Given)
*        The list of ordered data for which the weighted median is
*        required
*     WEIGHT( NENT ) = REAL (Given)
*        The weights of the values.
*     VAR( NENT ) = DOUBLE PRECISION (Given)
*        The variance of the unordered sample now ordered in ARR.
*     LBND = INTEGER (Given)
*        Lower bound of data to be considered in ARR.
*     UBND = INTEGER (Given)
*        Upper bound of data to be considered in ARR.
*     NENT = INTEGER (Given)
*        The number of entries in the data array.
*     USED = LOGICAL (Returned)
*        If a value contributes to the median value it is flagged as
*        true in this array, otherwise the array is set to false.
*     COVAR( * ) = DOUBLE PRECISION (Given)
*        The packed variance-covariance matrix of the order statistics
*        from a normal distribution of size NENT.
*     RESULT = DOUBLE PRECISION (Returned)
*        The weighted median
*     RESVAR = DOUBLE PRECISION (Returned)
*        The variance of result.
*     STATUS = INTEGER (Given and Returned)
*        The global status.

*  Prior Requirements:
*     - The input data must be ordered increasing. No BAD values may be
*     present, although you may restrict the range (LBND, UBND).

*  Notes:
*     - the routine should only be used at real or better precisions.

*  Authors:
*     PDRAPER: Peter Draper (STARLINK)
*     {enter_new_authors_here}

*  History:
*     30-JAN-1998 (PDRAPER):
*        Original version.
*     {enter_further_changes_here}

*  Bugs:
*     {note_any_bugs_here}

*-

*  Type Definitions:
      IMPLICIT NONE              ! No implicit typing

*  Global Constants:
      INCLUDE 'SAE_PAR'          ! Standard SAE constants

*  Arguments Given:
      INTEGER NENT
      INTEGER LBND
      INTEGER UBND
      REAL ORDDAT( NENT )
      REAL WEIGHT( NENT )
      DOUBLE PRECISION VAR
      DOUBLE PRECISION COVAR( * )

*  Arguments Returned:
      DOUBLE PRECISION RESULT
      DOUBLE PRECISION RESVAR
      LOGICAL USED( NENT )

*  Status:
      INTEGER STATUS             ! Global status

*  Local Variables:
      DOUBLE PRECISION TOTWT    ! The total value of weights
      INTEGER I                 ! Loop variable
      INTEGER J                 ! Loop variable
      INTEGER K                 ! Loop variable
      INTEGER LOWER             ! Lower sum bound
      INTEGER UPPER             ! Upper sum bound
      DOUBLE PRECISION WTSUM    ! Current sum of weights
      DOUBLE PRECISION WTINC    ! Increment in current wtsum
      DOUBLE PRECISION D1       !
      DOUBLE PRECISION D2       ! Values around half weight
      DOUBLE PRECISION W1       ! Fractional weight of value
      DOUBLE PRECISION W2       ! Fractional weight of value
      DOUBLE PRECISION IW       ! Dummy
      DOUBLE PRECISION JW       ! Dummy
      DOUBLE PRECISION VSUM     ! Covariance sum

*  Internal References:
      INCLUDE 'NUM_DEC_CVT'      ! Numeric conversion definitions
      INCLUDE 'NUM_DEF_CVT'      !
*.

*  Check inherited global status.
      IF ( STATUS .NE. SAI__OK ) RETURN

*  If only one input value has been given, do things quickly. Note
*  we are ignoring LBND and UBND in the case of NENT = 1.
      IF ( NENT .EQ. 1 ) THEN

*  Just one value, copy the input values to the output values.
         RESULT = NUM_RTOD( ORDDAT( 1 ) )
         RESVAR = VAR
         USED( 1 ) = .TRUE.
      ELSE IF ( LBND .EQ. UBND ) THEN
         RESULT = NUM_RTOD( ORDDAT( LBND ) )
         RESVAR = VAR
         USED( LBND ) = .TRUE.
      ELSE

*  Set the used array to false.
         DO 5 I = 1, NENT
            USED( I ) =.FALSE.
 5       CONTINUE

*  More than one value so process in earnest... Sum weights.
         TOTWT = 0.0D0
         DO 1 I = LBND, UBND
            TOTWT = TOTWT + NUM_RTOD( WEIGHT( I ) )
 1       CONTINUE

* Search for median weight.
         TOTWT = TOTWT * 0.5D0
         WTSUM = 0.0D0
         DO 2 I = LBND, UBND
            IF ( I .EQ. LBND ) THEN
               WTINC = NUM_RTOD( WEIGHT( I ) ) * 0.5D0
            ELSE
               WTINC = ( NUM_RTOD( WEIGHT( I ) + WEIGHT( I - 1 ) ) )
     :                 *  0.5D0
            END IF
            WTSUM = WTSUM + WTINC
            IF ( WTSUM .GT. TOTWT ) GO TO 66
 2       CONTINUE
 66      I = MIN( I, UBND )

*  Set the used array
         USED( I ) = .TRUE.

*  Bounds are present value and previous one.
         LOWER = I - 1
         UPPER = I
         D1 = NUM_RTOD( ORDDAT( LOWER ) )
         D2 = NUM_RTOD( ORDDAT( UPPER ) )

*  Set the used array extra value.
         USED( LOWER ) = .TRUE.

*  Set weights factors
         W1 = ( WTSUM - TOTWT ) / MAX( WTINC, 1.0D-20 )
         W2 = 1.0D0 - W1

* Interpolate between data values
         RESULT = D1 * W1 + D2 * W2

*  Sum the relevant ordered statistic variances and covariances.
*  weighting accordingly.
         VSUM = 0.0D0
         DO 3 K = LOWER, UPPER
            IF( K .EQ. LOWER ) THEN
               IW = W1
            ELSE
               IW = W2
            END IF
            DO 4 J = K, UPPER
               IF( J .EQ. LOWER ) THEN
                  JW = W1
               ELSE
                  JW = W2
               END IF

*  Sum variances and twice covariances (off diagonal elements).
               IF( K .EQ. J ) THEN
                  VSUM = VSUM + IW * JW * COVAR( K + J * ( J - 1 )/ 2 )
               ELSE
                  VSUM = VSUM +
     :                   2.0D0 * IW * JW * COVAR( K + J * ( J - 1 )/ 2 )
               END IF
 4          CONTINUE
 3       CONTINUE

*  Right make the new variance estimate. Use the sum of variances
*  and covariances of the order statistic of the `trimmed' sample size
*  Sample variance changes to NENT * VAR to represent total variance
*  of original data.
         RESVAR = VAR * NENT * VSUM
      END IF
      END
* $Id$
