*+  RED4_SIGMA_TO_SESQ - Convert standard deviation to standard error squared.
      SUBROUTINE RED4_SIGMA_TO_SESQ( NELM, SIGMA, NADDS, SESQ,
     :   STATUS)
*    Description :
*     This routine calculates the "standard error squared" for each
*     pixel in a data set, from the standard deviation and the number
*     of coadds making up the data.
*
*     The routine is used for converting the standard deviation array
*     generated by the ADP in NDR mode into standard error squared.
*    Invocation :
*     CALL RED4_SIGMA_TO_SESQ( NELM, SIGMA, NADDS, SESQ, STATUS)
*    Parameters :
*     NELM            = INTEGER( READ )
*           The number of elements in the arrays.
*     SIGMA( NELM )   = REAL( READ )
*           The standard deviation of the data values.
*     NADDS           = INTEGER( READ )
*           The number of coadds which have been combined to obtain
*           the standard deviation.
*     SESQ( NELM )    = REAL( WRITE )
*           The "standard error squared" for the data.
*    Method :
*     The standard deviation is squared to obtain the variance, and
*     this is divided by NADDS to obtain standard error squared.
*    Deficiencies :
*    Bugs :
*    Authors :
*     S.M.Beard (REVAD::SMB)
*     P.N.Daly  (JACH::PND)
*    History :
*      4-Oct-1990: Original version.                            (SMB)
*     23-Feb-1993: Conform to error strategy                    (PND)
*    endhistory
*    Type Definitions :
      IMPLICIT NONE
*    Global constants :
      INCLUDE 'ADAMDEFNS'
      INCLUDE 'ADAMERRS'
*    Import :
      INTEGER
     :  NELM,                       ! The number of elements in the arrays
     :  NADDS                       ! The number of coadds making up the data
      REAL
     :  SIGMA( NELM )               ! The standard deviation array
*    Export:
      REAL
     :  SESQ( NELM )                ! The "standard error squared" array
*    Status :
      INTEGER
     :  STATUS                      ! Global status
*    Local Constants :
*    Local variables :
      INTEGER
     :  I                           ! Loop index
*-

*   Check for error on entry
      IF ( STATUS .NE. ADAM__OK ) RETURN

*   Check that the number coadds is at least 1 (so there is not a
*   divide by zero error).
      IF ( NADDS .GT. 0 ) THEN

*      Loop through each of the array elements
         DO I = 1, NELM

*         Calculate the "standard error squared"
            SESQ(I) = SIGMA(I) * SIGMA(I) / REAL( NADDS )
         END DO
      ELSE

*      The number of coadds is zero or smaller. Return a zero standard
*      error squared array.
         DO I = 1, NELM

            SESQ(I) = 0.0
         END DO
      END IF

      END
