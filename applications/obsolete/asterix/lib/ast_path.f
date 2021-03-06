*+  AST_PATH - resolve path to file
      SUBROUTINE AST_PATH(DEF,ALT,REL,PATH,L,STATUS)
*    Description :
*  Creates a path name from an environment variable and relative path.
*  It allows two possibilities for the environment variable - a default
*  and an alternative.  The relative path may be blank.  There is also
*  an overall default of AST_ETC.  It allows for the possibility that
*  the default has a relative added, but the alternative points directly
*  to a file.  The special value of NULL for default suppresses use
*  of this environment variable, but allows the alternative.
*    History :
*     27/2/98 : Original (RJV)
*    Type definitions :
      IMPLICIT NONE
*    Status :
      INTEGER STATUS
*    Global variables :
      INCLUDE 'SAE_PAR'
      INCLUDE 'PAR_ERR'
*    Import :
      CHARACTER*(*) DEF,ALT,REL
*    Export :
      CHARACTER*(*) PATH
      INTEGER L
*    Functions :
      INTEGER CHR_LEN
*    Local Constants :
*    Local variables :
      CHARACTER*132 BUFF
      LOGICAL ISAFILE,ISADIR
*-
      IF (STATUS.EQ.SAI__OK) THEN

*  try alternative location first
        CALL PSX_GETENV(ALT,BUFF,STATUS)
        IF (STATUS.NE.SAI__OK) THEN
          CALL ERR_ANNUL(STATUS)
*  if not defined then try default location
          IF (DEF.EQ.'NULL'.OR.DEF.EQ.'null') THEN
            BUFF=' '
          ELSE
            CALL PSX_GETENV(DEF,BUFF,STATUS)
          ENDIF
          IF (STATUS.NE.SAI__OK) THEN
            CALL ERR_ANNUL(STATUS)
*  still not found then try AST_ETC as catch-all
            CALL PSX_GETENV('AST_ETC',BUFF,STATUS)
            IF (STATUS.NE.SAI__OK) THEN
              CALL MSG_PRNT('! Unable to resolve data path')
            ENDIF
          ENDIF
        ENDIF

        IF (STATUS.EQ.SAI__OK) THEN

          L=CHR_LEN(BUFF)

*  if relative path defined then append it
          IF (CHR_LEN(REL).GT.0) THEN
*  but first see if what we have already points to a file
            CALL AST_PATH_TRY(BUFF,ISAFILE,STATUS)
            IF (.NOT.ISAFILE) THEN
              CALL AST_PATH_TRY(BUFF(1:L)//'.sdf',ISAFILE,STATUS)
            ENDIF
*  could be case that default expects filename to be added
            IF (ISAFILE) THEN
*  but alternate doesn't
              PATH=BUFF

            ELSE
              IF (L.EQ.0) THEN
                PATH=REL
              ELSE
*  if we are appending then sort out slashes
                IF (BUFF(L:L).NE.'/'.AND.REL(1:1).NE.'/') THEN
                  BUFF=BUFF(1:L)//'/'
                  L=L+1
                ENDIF
                PATH=BUFF(1:L)//REL
              ENDIF

            ENDIF

          ELSE

            PATH=BUFF

          ENDIF

*  return length of final string
          L=CHR_LEN(PATH)

        ENDIF

        IF (STATUS.NE.SAI__OK) THEN
          CALL ERR_REP(' ','from AST_PATH',STATUS)
        ENDIF

      ENDIF

      END



      SUBROUTINE AST_PATH_TRY(NAME,ISAFILE,STATUS)

      IMPLICIT NONE

      INCLUDE 'SAE_PAR'

      INTEGER STATUS

      INTEGER CHR_LEN

      CHARACTER*(*) NAME
      LOGICAL ISAFILE

      CHARACTER*132 BUF
      INTEGER ISTAT,UNIT
      INTEGER L

      IF (STATUS.NE.SAI__OK) RETURN

*  cludge to get round DEC fortran bug
      L=CHR_LEN(NAME)
      IF (L.GE.4.AND.NAME(L-3:L).EQ.'/etc') THEN
        ISAFILE=.FALSE.

      ELSE

        CALL FIO_OPEN(NAME,'READ','LIST',80,UNIT,ISTAT)
        CALL FIO_READF(UNIT,BUF,ISTAT)
        IF (ISTAT.EQ.0) THEN
          CALL FIO_CLOSE(UNIT,ISTAT)
          ISAFILE=.TRUE.
        ELSE
          ISAFILE=.FALSE.
          CALL ERR_ANNUL(ISTAT)
        ENDIF

      ENDIF

      END

