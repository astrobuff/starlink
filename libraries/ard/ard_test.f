      PROGRAM ARD_TEST
*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
*  ARD installation test program
*
*-----------------------------------------------------------------------
      IMPLICIT NONE

      INCLUDE 'SAE_PAR'
      INCLUDE 'PRM_PAR'
      INCLUDE 'GRP_PAR'
      INCLUDE 'CNF_PAR'

      INTEGER NDIM
      PARAMETER ( NDIM = 2 )
 
      INTEGER I, J, LBND( NDIM ), UBND( NDIM ), LBNDE( NDIM ), 
     :        UBNDE( NDIM ), LBNDI( NDIM ), UBNDI( NDIM ), 
     :        IP, EL, IGRP, STATUS, INDEX
      REAL C( 1 )
      LOGICAL FLAG

      DATA LBND /1,1/,
     :     UBND /40,40/

*  Initialise the inherited status.
      STATUS = SAI__OK

*  Work out the number of pixels in the mask.
      EL = 1
      DO I = 1, NDIM
         EL = EL*( UBND( I ) - LBND( I ) + 1 )
      END DO

*  Get space to hold the mask.
      CALL PSX_CALLOC( EL, '_INTEGER', IP, STATUS )

*  Indicate that a unit application transformation is to be used.
      C( 1 ) = VAL__BADR

*  Store the ARD description.
      IGRP = GRP__NOID
      CALL ARD_GRPEX( 'CIR(0,0,20)OFF(10,0)CIR(0,0,20)', GRP__NOID, 
     :                IGRP, FLAG, STATUS )

*  Create the mask.
      INDEX = 2
      CALL ARD_WORK( IGRP, NDIM, LBND, UBND, C, .FALSE., INDEX, 
     :               %VAL( CNF_PVAL( IP ) ), 
     :               LBNDI, UBNDI, LBNDE, UBNDE, STATUS )

*  Check the returned index is corect.
      IF( INDEX .NE. 4 ) THEN
         WRITE(*,*) 'ARD installation test failed...'
         WRITE(*,*) 'Region index returned by ARD_WORK was ',INDEX

*  Check the retuned bounding boxes are corect.
      ELSE IF( LBNDI( 1 ) .NE. 1 .OR. UBNDI( 1 ) .NE. 30 .OR.
     :         LBNDI( 2 ) .NE. 1 .OR. UBNDI( 2 ) .NE. 20 .OR.
     :         LBNDE( 1 ) .NE. 1 .OR. UBNDE( 1 ) .NE. 40 .OR.
     :         LBNDE( 2 ) .NE. 1 .OR. UBNDE( 2 ) .NE. 40 ) THEN

*  If the test has failed, display the bounding boxes.
         WRITE(*,*) 'ARD installation test failed...'
         WRITE(*,*) 'Internal bounding box:'
   
         IF( LBNDI(1) .GT. UBNDI(1) ) THEN
            WRITE(*,*) '... null'
   
         ELSE      
            DO I = 1, NDIM
               WRITE(*,*) I,'; ',LBNDI(I),':',UBNDI(I)
            END DO
   
         END IF
   
         WRITE(*,*)
         WRITE(*,*) 'External bounding box:'
   
         IF( LBNDE(1) .GT. UBNDE(1) ) THEN
            WRITE(*,*) '... null'
   
         ELSE      
            DO I = 1, NDIM
               WRITE(*,*) I,'; ',LBNDE(I),':',UBNDE(I)
            END DO
   
         END IF

* Otherwise, indicate that the test has been passed.
      ELSE
         WRITE(*,*) 'ARD installation test succeeded'

      END IF
      
*  Delete the GRP group.
      CALL GRP_DELET( IGRP, STATUS )

*  Free the mask.
      CALL PSX_FREE( IP, STATUS )

      END
