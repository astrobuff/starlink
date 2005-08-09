      SUBROUTINE KPG1_ASMRG( IWCS1, IWCS2, DOMAIN, QUIET, IND, STATUS )
*+
*  Name:
*     KPG1_ASMRG

*  Purpose:
*     Merge two FrameSets by aligning them in a common Frame.

*  Language:
*     Starlink Fortran 77

*  Invocation:
*     CALL KPG1_ASMRG( IWCS1, IWCS2, DOMAIN, QUIET, IND, STATUS )

*  Description:
*     This routine merges two FrameSet by aligning them in a suitable
*     common Frame. The Current Frame in the second FrameSet becomes the 
*     Current Frame in the merged FrameSet. The domain search order for 
*     finding a suitable Frame is:
*
*     1) The domain of the Current Frame in IWCS2, if not blank.
*     2) "SKY"
*     3) "SPECTRUM"
*     4) "PIXEL"
*     5) "GRID"
*     6) The domain specified by argument DOMAIN, if not blank. If DOMAIN
*        is blank, "AGI_WORLD" is used.
*     7) Any other suitable Frame.
*
*     For each of these Domains, the current Frame is checked first. An error 
*     is reported if alignment is not possible, and a message identifying the 
*     alignment Frame is displayed if alignment is possible.
*
*     If either FrameSet contains a second Frame with the same Domain as
*     the alignment Frame then a warning is issued.

*  Arguments:
*     IWCS1 = INTEGER (Given)
*        An AST pointer to the first FrameSet. This is modified by adding
*        all the Frames from IWCS2 into it. The Current Frame on exit is
*        inherited from IWCS2.
*     IWCS2 = INTEGER (Given)
*        An AST pointer to the second FrameSet. The Current and Base Frames 
*        are unchanged on exit.
*     DOMAIN = CHARACTER * ( * ) (Given)
*        A comma separated list of domains in which alignment of the FrameSets 
*        should be attempted if alignment is not possible in the Current Frame 
*        of the second FrameSet, or SKY, SPECTRUM, PIXEL or GRID.
*     QUIET = LOGICAL (Given)
*        Suppress the message identifying the alignment Frame?
*     IND = INTEGER (Given)
*        The alignment message is padded with IND leading spaces.
*     STATUS = INTEGER (Given and Returned)
*        The global status.

*  Authors:
*     DSB: David S. Berry (STARLINK)
*     {enter_new_authors_here}

*  History:
*     17-SEP-1998 (DSB):
*        Original version.
*     22-JUN-1999 (DSB):
*        Added SKY and PIXEL before GRID is Domain search path.
*     8-JAN-2003 (DSB):
*        Added SPECTRUM to Domain search path.
*     11-FEB-2005 (DSB):
*        - Check current Frames first.
*        - Issue warning if more than one Frame with the alignment Domain
*          exists in either FrameSet.
*     9-AUG-2005 (DSB):
*        Add warning about differing dimensionalities if no alignment is
*        possible.
*     {enter_changes_here}

*  Bugs:
*     {note_any_bugs_here}

*-
      
*  Type Definitions:
      IMPLICIT NONE              ! No implicit typing

*  Global Constants:
      INCLUDE 'SAE_PAR'          ! Standard SAE constants
      INCLUDE 'AST_PAR'          ! AST constants and function declarations

*  Arguments Given:
      INTEGER IWCS1
      INTEGER IWCS2
      CHARACTER DOMAIN*(*)
      LOGICAL QUIET
      INTEGER IND

*  Status:
      INTEGER STATUS             ! Global status

*  Local Variables:
      CHARACTER DOM*80           ! Domain of Current Frame in FrameSet
      CHARACTER DOMLST*255       ! Domain search list
      CHARACTER TEXT*30          ! General text string
      INTEGER I                  ! Frame Index
      INTEGER IAT                ! No. of characters in string
      INTEGER IBASE1             ! Index of original Base Frame in IWCS1
      INTEGER IBASE2             ! Index of original Base Frame in IWCS2
      INTEGER ICURR1             ! Index of Current Frame in IWCS1
      INTEGER ICURR2             ! Index of Current Frame in IWCS2
      INTEGER IMAT1              ! Index of alignment Frame in IWCS1 
      INTEGER IMAT2              ! Index of alignment Frame in IWCS2
      INTEGER MAP                ! Simplified mapping between two Frames
      INTEGER NAXC1              ! Number of axies in current Frame of IWCS1
      INTEGER NAXC2              ! Number of axies in current Frame of IWCS2
      INTEGER NFRM1              ! No. of Frames supplied in IWCS1
      INTEGER TEMP               ! AST pointer to a FrameSet
      LOGICAL WARNED             ! Warning of duplicate Frames issued?
*.

*  Check the inherited status. 
      IF ( STATUS .NE. SAI__OK ) RETURN

*  Begin an AST context.
      CALL AST_BEGIN( STATUS )

*  Note the number of axes in the two current Frames.
      NAXC1 = AST_GETI( IWCS1, 'NAXES', status )
      NAXC2 = AST_GETI( IWCS2, 'NAXES', status )

*  Note the indices of the Base and Currrent Frames in the two FrameSets so 
*  that they can be re-instated after AST_CONVERT has changed them.
      IBASE1 = AST_GETI( IWCS1, 'BASE', STATUS )
      IBASE2 = AST_GETI( IWCS2, 'BASE', STATUS )

      ICURR1 = AST_GETI( IWCS1, 'CURRENT', STATUS )
      ICURR2 = AST_GETI( IWCS2, 'CURRENT', STATUS )

*  Note the number of Frames supplied in IWCS1.
      NFRM1 = AST_GETI( IWCS1, 'NFRAME', STATUS )

*  Get the name of the Domain in which the Current Frame of the second 
*  FrameSet lives.
      DOM = AST_GETC( IWCS2, 'DOMAIN', STATUS )

*  Create a list of preferences for the Domain in which alignment should
*  occur. First use the Domain of the Current Frame in IWCS2, then try 
*  SKY, SPECTRUM, PIXEL and GRID, then try the supplied Domain (if any), 
*  then try any other Domain. 
      DOMLST = ' '
      IAT = 0

      IF( DOM .NE. ' ' ) THEN
         CALL CHR_APPND( DOM, DOMLST, IAT )
         CALL CHR_APPND( ',', DOMLST, IAT )
      END IF

      CALL CHR_APPND( 'SKY,SPECTRUM,PIXEL,GRID,', DOMLST, IAT )

      IF( DOMAIN .NE. ' ' ) THEN
         CALL CHR_APPND( DOMAIN, DOMLST, IAT )
         CALL CHR_APPND( ',', DOMLST, IAT )
      ELSE
         CALL CHR_APPND( 'AGI_WORLD,', DOMLST, IAT )
      END IF

*  If a FrameSet contains two Frames with the same Domain name, one of
*  which is the current Frame, we want to give preference to the current
*  Frame. To do this, ensure that the current Frame is considered first by
*  setting the base Frame in each FrameSet to be the current Frame.
      CALL AST_SETI( IWCS1, 'BASE', ICURR1, STATUS )
      CALL AST_SETI( IWCS2, 'BASE', ICURR2, STATUS )

*  Attempt to align the FrameSets. If succesfull, a new FrameSet is 
*  returned describing the relationship between the Current Frames in 
*  IWCS2 and IWCS1, and the Base Frames are changed to indicate 
*  the Frames in which alignment occurred.
      TEMP = AST_CONVERT( IWCS1, IWCS2, DOMLST( : IAT ), STATUS ) 

*  Issue a fatal error if alignment was not possible in any Domain.
      IF( TEMP .EQ. AST__NULL .AND. STATUS .EQ. SAI__OK ) THEN
         IF( AST_ISAPLOT( IWCS1, STATUS ) ) THEN          
            STATUS = SAI__ERROR
            CALL ERR_REP( 'KPG1_ASMRG_2', 'Could not align data '//
     :                    'with displayed picture.', STATUS )

         ELSE 
            STATUS = SAI__ERROR
            CALL ERR_REP( 'KPG1_ASMRG_2', 'Could not align supplied '//
     :                    'positions.', STATUS )
         END IF

*  Warn the user about any different dimensionalities in the two current Frames
         IF( NAXC1 .NE. NAXC2 ) THEN
            CALL MSG_SETI( 'NC1', NAXC1 )
            CALL MSG_SETI( 'NC2', NAXC2 )
            CALL ERR_REP( 'KPG1_ASMRG_2', 'The two current coordinate'//
     :                    ' Frames have different numbers of axes '//
     :                    '(^NC1 and ^NC2).', STATUS )
         END IF

         GO TO 999

      END IF

*  Note indices of Frames in which alignment occurred.
      IMAT1 = AST_GETI( IWCS1, 'BASE', STATUS )
      IMAT2 = AST_GETI( IWCS2, 'BASE', STATUS )

*  We now repeat the call to AST_CONVERT, but before we do so we make sure
*  that the Current Frame in each FrameSet is the one in which alignment
*  occurred. The Mapping returned is then a lot simpler and avoids the
*  possibility of back-to-back PermMaps throwing away redundant axis
*  information. First set the Current Frames in the the two FrameSets to
*  be the matching Frames.
      CALL AST_SETI( IWCS1, 'CURRENT', IMAT1, STATUS )
      CALL AST_SETI( IWCS2, 'CURRENT', IMAT2, STATUS )

*  Now call AST_CONVERT again.
      TEMP = AST_CONVERT( IWCS1, IWCS2, DOMLST( : IAT ), STATUS ) 

*  Issue a fatal error if alignment was not possible. This shouldn't happen.
      IF( TEMP .EQ. AST__NULL .AND. STATUS .EQ. SAI__OK ) THEN
         IF( AST_ISAPLOT( IWCS1, STATUS ) ) THEN          
            STATUS = SAI__ERROR
            CALL ERR_REP( 'KPG1_ASMRG_2', 'Could not align data '//
     :                    'with displayed picture.', STATUS )

         ELSE 
            STATUS = SAI__ERROR
            CALL ERR_REP( 'KPG1_ASMRG_2', 'Could not align supplied '//
     :                    'positions.', STATUS )
         END IF

         GO TO 999

      END IF

*  If succesful, tell the user what Domain alignment was performed in.
      IF( .NOT. QUIET ) THEN
         DOM = AST_GETC( AST_GETFRAME( IWCS2, AST__BASE, STATUS ),
     :                   'Domain', STATUS ) 

         TEXT = ' '
         IAT = IND
         CALL CHR_APPND( 'Alignment', TEXT, IAT )
         CALL MSG_SETC( 'AL', TEXT( : IAT ) )
         CALL MSG_SETC( 'DOM', DOM )
         CALL MSG_OUT( 'KPG1_ASMRG_1', '^AL has occurred '//
     :                  'within the ^DOM Domain.', STATUS )
         CALL MSG_BLANK( STATUS )

*  Issue a warning if either FrameSet contains another Frame with the
*  same Domain.
         WARNED = .FALSE.
         DO I = 1, AST_GETI( IWCS1, 'NFRAME', STATUS )
            IF( I .NE. IMAT1 .AND. .NOT. WARNED ) THEN
               IF( AST_GETC( AST_GETFRAME( IWCS1, I, STATUS ),
     :                       'Domain', STATUS ) .EQ. DOM ) THEN
                  CALL MSG_OUT( 'KPG1_ASMRG_2', 'WARNING: More than '//
     :               'one Frame with this Domain was found in the '//
     :               'data and so alignment is ambiguous and may be '//
     :               'incorrect.', STATUS )
                  WARNED = .TRUE.
               END IF
            END IF
         END DO

         DO I = 1, AST_GETI( IWCS2, 'NFRAME', STATUS )
            IF( I .NE. IMAT2 .AND. .NOT. WARNED ) THEN
               IF( AST_GETC( AST_GETFRAME( IWCS2, I, STATUS ),
     :                       'Domain', STATUS ) .EQ. DOM ) THEN
                  CALL MSG_OUT( 'KPG1_ASMRG_3', 'WARNING: More than '//
     :               'one Frame with this Domain was found in the '//
     :               'data and so alignment is ambiguous and may be '//
     :               'incorrect.', STATUS )
                  WARNED = .TRUE.
               END IF
            END IF
         END DO

      END IF

*  Get a simplified Mapping connecting the two Frames.
      MAP = AST_SIMPLIFY( AST_GETMAPPING( TEMP, AST__BASE,
     :                                    AST__CURRENT, STATUS ),
     :                    STATUS ) 

*  Merge the second FrameSet into the first FrameSet using the Mapping 
*  returned by AST_CONVERT to join the two matching Frames. 
      CALL AST_ADDFRAME( IWCS1, IMAT1, MAP, IWCS2, STATUS )

*  Re-instate the two Base Frames which were modified by AST_CONVERT.
      CALL AST_SETI( IWCS1, 'BASE', IBASE1, STATUS )
      CALL AST_SETI( IWCS2, 'BASE', IBASE2, STATUS )

*  Re-instate the original Current Frame in IWCS2.
      CALL AST_SETI( IWCS2, 'CURRENT', ICURR2, STATUS )

*  Set the Current Frame in the merged FrameSet so that it corresponds to
*  the originalCurrent Frame in IWCS2.
      CALL AST_SETI( IWCS1, 'CURRENT', ICURR2 + NFRM1, STATUS )

 999  CONTINUE

*  End the AST context.
      CALL AST_END( STATUS )

      END
