      SUBROUTINE GNS_IDNG ( IWKID, NAME, LNAME, STATUS )

*+
*  Name:
*     GNS_IDNG

*  Purpose:
*     Inquire device name of GKS workstation

*  Invocation:
*     CALL GNS_IDNG( IWKID, NAME, LNAME, STATUS )

*  Description:
*     The physical device name or file name of the specified GKS
*     workstation is returned. The name may be a logical name that
*     translates to the device or file name rather than the name itself.
*
*     If the name is longer than the supplied character variable the
*     name is truncated but the length returned is the actual length of
*     the name.

*  Arguments:
*     IWKID = INTEGER (Given)
*        GKS workstation identifier
*     NAME = CHARACTER*(GNS__SZDEV) (Returned)
*        Device or file name
*     LNAME = INTEGER (Returned)
*        Length of name
*     STATUS = INTEGER (Given and Returned)
*        The global status

*  Side Effects:
*     The GNS database may be opened.

*  Authors:
*     DLT: D.L. Terrett (STARLINK)
*     NE: Nick Eaton (Durham University)

*  History:
*      3-JUN-1988 (DLT):
*        Original version.
*      9-JUL-1990 (NE):
*      1-SEP-1992 (NE):
*        Updated prologue.
*-
      
*  Type Definitions:
      IMPLICIT NONE

*  Global Variables:
      INCLUDE 'GNS_ERR'

*  Arguments Given:
      INTEGER IWKID

*  Arguments Returned:
      CHARACTER*(*) NAME
      INTEGER LNAME

*  Status:
      INTEGER STATUS

*  Local Variables:
      INTEGER IERR, ITYPE, ICON
*.
      
      IF (STATUS.EQ.0) THEN

*     Get workstation type and connection id
         CALL GQWKC(IWKID, IERR, ICON, ITYPE)
         IF (IERR.NE.0) THEN
            STATUS = gns__inwkid
            CALL EMS_REP( 'GNS_IDNG_INWKID',
     :                    'Invalid GKS workstation identifier', STATUS )
         ELSE

*         if the connection id is 5 then the device is the terminal
            IF (ICON.EQ.5) THEN
               CALL gns_GTN( NAME, LNAME, STATUS)
            ELSE

*            Inquire the name of the file attahed to the logical unit
               NAME = ' '
               INQUIRE( UNIT=ICON, NAME=NAME)
               LNAME = INDEX(NAME,' ')
               IF (LNAME.EQ.0) LNAME = LEN(NAME)
            END IF
         END IF
      END IF
      END

