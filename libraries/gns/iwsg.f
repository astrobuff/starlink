      SUBROUTINE GNS_IWSG ( IWKID, WSSCA, STATUS )

*+
*  Name:
*     GNS_IWSG

*  Purpose:
*     Inquire workstation scale

*  Invocation:
*     CALL GNS_IWSG( IWKID, WSSCA, STATUS )

*  Description:
*     The size in metres of 'device units' for the specified GKS
*     workstation is returned.
*
*     Note that device units as defined by GKS are not necessarily the
*     same as the device resolution. On devices with a well defined actual
*     size such as plotters and printers device units are metres and a
*     value of 1.0 will be returned.

*  Arguments:
*     IWKID = INTEGER (Given)
*        GKS workstation identifier
*     WSSCA = REAL (Returned)
*        Size in metres of device units
*     STATUS = INTEGER (Given and Returned)
*        The global status

*  Authors:
*     DLT: D.L. Terrett (STARLINK)
*     NE: Nick Eaton (Durham University)

*  History:
*      7-JUN-1988 (DLT):
*        Original version.
*      9-JUL-1990 (NE):
*        Added error reporting
*      1-SEP-1992 (NE):
*        Updated prologue.
*-
      
*  Type Definitions:
      IMPLICIT NONE

*  Global Variables:
      INCLUDE 'GNS_ERR'
      INCLUDE 'GNS_PAR'
      INCLUDE 'gns.cmn'
      INCLUDE 'GKS_PAR'

*  Arguments Given:
      INTEGER IWKID

*  Arguments Returned:
      REAL WSSCA

*  Status:
      INTEGER STATUS

*  Local Variables:
      INTEGER IERR, ICON, ITYPE, IUNIT, LX, LY
      REAL RX, RY
*.

      IF (STATUS.EQ.0) THEN

*     Convert the workstation id to a type
         CALL GQWKC(IWKID,IERR,ICON,ITYPE)
         IF (IERR.EQ.0) THEN

*        See if device units are metres
            CALL GQDSP(ITYPE,IERR,IUNIT,RX,RY,LX,LY)

            IF (IUNIT.EQ.GMETRE) THEN
               WSSCA = 1.0
            ELSE

*           Make sure that the common block contains data for this type
               CALL GNS_1RDWST(ITYPE, STATUS)

               IF (STATUS.EQ.0) THEN
                  WSSCA = SCALE
               ELSE
                  WSSCA = 0.05
               END IF
            END IF
         ELSE
            STATUS = GNS__INWKID
            CALL EMS_REP( 'GNS_IWSG_INWKID',
     :                    'Invalid GKS workstation identifier', STATUS )
         END IF
      END IF
      END

