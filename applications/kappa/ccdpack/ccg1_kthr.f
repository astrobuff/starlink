      SUBROUTINE CCG1_KTHR( N, K, ARRAY, ANCDAT, VALUE, STATUS )
*+
*  Name:
*     CCG1_KTHR

*  Purpose:
*     Return the Kth smallest value in an array.

*  Language:
*     Fortran-77

*  Invocation:
*     CALL CCG1_KTHR( N, K, ARRAY, ANCDAT, VALUE, STATUS )

*  Description:
*     This routine returns the value of the Kth smallest element in the
*     given array. It is an implementation of an algorithm of Niklaus
*     Wirth from the book "Algorithms + data structures = programs".

*  Arguments:
*     N = INTEGER (Given)
*        The number of elements in the input array.
*     K = INTEGER (Given and Returned)
*        On entry the ordered value to be returned, on exit index of
*        ARRAY that contain the Kth smallest value.
*     ARRAY( N ) = REAL (Given and Returned)
*        The array of values to be processed. Note this is modified on 
*        exit.
*     ANCDAT( N ) = INTEGER (Given and Returned)
*        Array of data whose association with ARRAY is to be preserved 
*        (i.e. a list of pointers to the original positions with ARRAY).
*     VALUE = REAL (Returned)
*         The Kth smallest value in the input array.
*     STATUS = INTEGER (Given)
*        The global status.

*  Authors:
*     PDRAPER: Peter Draper (STARLINK - Durham University)
*     {enter_new_authors_here}

*  History:
*     16-NOV-1998 (PDRAPER):
*        Original version.
*     {enter_changes_here}

*  Bugs:
*     {note_any_bugs_here}

*-

*  Type Definitions:
      IMPLICIT NONE             ! No implicit typing

*  Global Constants:
      INCLUDE 'SAE_PAR'         ! Standard SAE constants
      
*  Arguments Given:
      INTEGER N 
      INTEGER K
      
*  Arguments Given and Returned:
      REAL ARRAY( N ) 
      INTEGER ANCDAT( N )
      
*  Arguments Returned:
      REAL VALUE
      
*  Status:
      INTEGER STATUS             ! Global status

*  Local Variables:
      INTEGER I, J, L, M
      REAL LVAL, TMP
      INTEGER IVAL, ITMP
      
*.

*  Check inherited global status.
      IF ( STATUS .NE. SAI__OK ) RETURN
      
*  Initialization.
      L = 1
      M = N

*  Do while L is less than M
 1    CONTINUE
      IF ( L .LT. M ) THEN 
         LVAL = ARRAY( K )
         IVAL = ANCDAT( K )
         I = L
         J = M

*  Do until I is greater than J.
 2       CONTINUE

*  Do while ARRAY( I ) is less than LVAL
 3       CONTINUE
         IF ( ARRAY( I ) .LT. LVAL ) THEN 
            I = I + 1
            GO TO 3
         END IF

*  Do while LVAL is less than ARRAY( J ).
 4       CONTINUE
         IF ( LVAL .LT. ARRAY( J ) ) THEN 
            J = J - 1
            GO TO 4
         END IF

*  Swap values at I and J if needed.
         IF ( I .LE. J ) THEN 
            TMP = ARRAY( I )
            ARRAY( I ) = ARRAY( J )
            ARRAY( J ) = TMP

            ITMP = ANCDAT( I )
            ANCDAT( I ) = ANCDAT( J )
            ANCDAT( J ) = ITMP

            I = I + 1
            J = J - 1
         END IF

*  End of do until I is greater than J.
         IF ( I .LE. J ) GO TO 2
         IF ( J .LT. K ) L = I
         IF ( K .LT. I ) M = J
         GO TO 1
      END IF

*  Return Kth smallest value.
      VALUE = ARRAY( K )
      END
