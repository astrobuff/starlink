      SUBROUTINE TXT2AST( STATUS )
*+
*  Name:
*     TXT2AST

*  Purpose:
*     Reads in a formatted text file and constructs a binned dataset

*  Language:
*     Starlink Fortran

*  Type of Module:
*     ASTERIX task

*  Invocation:
*     CALL TXT2AST( STATUS )

*  Arguments:
*     STATUS = INTEGER (Given and Returned)
*        The global status.

*  Description:
*     TXT2AST reads a plain ascii text file composed of record descriptors
*     and data lines. The former describe the structure of the latter. This
*     application is a means of getting numeric data into a form with which
*     ASTERIX can work.

*  Usage:
*     import {parameter_usage}

*  Environment Parameters:
*     INP = CHAR (read)
*        The input file name
*     SORT = LOGICAL (read)
*        Sort data on each axis?. Default N
*     OUT = CHAR (read)
*        The output dataset

*  Examples:
*     {routine_example_text}
*        {routine_example_description}

*  Pitfalls:
*     {pitfall_description}...

*  Notes:
*     {routine_notes}...

*  Prior Requirements:
*     {routine_prior_requirements}...

*  Side Effects:
*     {routine_side_effects}...

*  Algorithm:
*     TXT2AST picks up the record descriptors and validates the data lines
*     based on those descriptors. After being read in, the data is processed
*     and put into standard HDS form.
*
*     TXT2AST allocates space for its data in a very efficient manner.
*     Initially only a table of pointers exist. When the need arises a new
*     block of memory is fetched and its address assigned to a table entry.
*     Once all the data has been read one new large table of pointers to
*     the individual data table rows is constructed - this is then used to
*     sort the data. With 40 table entries, and each block being 10000 rows
*     deep, TXT2AST therefore places a limit of 400,000 on the number of
*     points being entered.

*  Accuracy:
*     {routine_accuracy}

*  Timing:
*     {routine_timing}

*  Implementation Status:
*     {routine_implementation_status}

*  External Routines Used:
*     {name_of_facility_or_package}:
*        {routine_used}...

*  Implementation Deficiencies:
*     {routine_deficiencies}...

*  References:
*     {task_references}...

*  Keywords:
*     import, usage:public

*  Copyright:
*     Copyright (C) University of Birmingham, 1995

*  Authors:
*     DJA: David J. Allan (Jet-X, University of Birmingham)
*     ISH: Ian S. Hobbs (University of Birmingham)
*     RB: Richard Beard (ROSAT, University of Birmingham)
*     {enter_new_authors_here}

*  History:
*     10 Nov 1988 V1.0-0 (DJA):
*        Original version.
*     11 May 1989 V1.0-1 (DJA):
*        Added axis widths creation
*      6 Jun 1989 V1.0-2 (DJA):
*        Fixed bug whereby abscence of TOPLEVEL record resulted in USI_ASSOCO
*        failure due to missing TYPE : type now set to UNKNOWN in this case
*     29 Aug 1989 V1.0-3 (DJA):
*        Added QUALITY handling. If a QUALITY descriptor is specified, and
*        DATA_ARRAY and/or VARIANCE values are unspecified, then QUALITY is
*        set MISSING
*     19 Dec 1989 V1.0-4 (DJA):
*        Added continuation characters ~ and +
*      3 Jan 1990 V1.0-5 (DJA):
*        Now correctly handles dataset with one point
*     12 Jan 1990 V1.0-6 (DJA):
*        'Extra ignored' bug fixed
*     22 Jan 1990 V1.0-7 (DJA):
*        Data UNITS and LABEL moved from TOPLEVEL. Writes input filename into
*        history. Accepts ERROR instead of VARIANCE in input.
*     12 Mar 1990 V1.2-0 (DJA):
*        Added SORT option (DJA)
*     29 Apr 1990 V1.2-1 (DJA):
*        Erroneous messages due to incomplete initialisation
*        of the OBJ variable fixed
*     16 May 1990 V1.2-2 (DJA):
*        Bug in reported line number fixed
*     28 Jun 1990 V1.2-3 (DJA):
*        Bug when more than BLOCK_SIZE records fixed
*     26 Jul 1990 V1.2-4 (DJA):
*        Warns if records being ignored in SORT mode
*     15 Aug 1990 V1.2-5 (DJA):
*        Output array values explicitly zeroed. Symbolic quality values used
*      6 Dec 1990 V1.3-0 (DJA):
*        Replaces all control characters by spaces
*     28 May 1991 V1.4-0 (DJA):
*        TOPLEVEL no longer needed. Quality masks redone
*     29 Oct 1991 V1.6-0 (DJA):
*        Asymmetric axis widths and data errors
*      3 Mar 1992 V1.6-1 (DJA):
*        Added MASK and DECREASING keywords. No longer capitalises input
*        text not in quotes. Automatic axis regularisation.
*        Allows primitive output
*      6 Mar 1992 V1.6-2 (DJA):
*        More efficient use of memory, maxmium number of
*        points raised to 4x10^5
*      4 Jun 1992 V1.6-3 (DJA):
*        Adds comments to history
*     24 Jun 1992 V1.6-4 (DJA):
*        FIO_CANCL input to prevent eventual ICL crash
*     11 Feb 1994 V1.7-0 (DJA):
*        No longer uses Fortran structures
*     21 Feb 1994 V1.7-1 (DJA):
*        Fixed bug uncovered by last change in handling non-scalar widths
*     31 Jul 1994 V1.7-2 (ISH):
*        Added LOLIM/UPLIM option for DATA values
*     24 Nov 1994 V1.8-0 (DJA):
*        Now use USI for user interface
*     12 Feb 1995 V1.8-1 (DJA):
*        Use new BDI, don't use FIO ADAM calls
*     20 Nov 1995 V2.0-0 (DJA):
*        Full ADI port. No longer coerces regular axes.
*      3 Apr 1996 V2.0-1 (DJA):
*        Added ability import grouping
*      2 Mar 1998 V2.2-1 (RB):
*        Convert from IMPORT
*     {enter_changes_here}

*  Bugs:
*     {note_any_bugs_here}

*-

*  Type Definitions:
      IMPLICIT NONE              ! No implicit typing

*  Global Constants:
      INCLUDE 'SAE_PAR'          ! Standard SAE constants
      INCLUDE 'ADI_PAR'
      INCLUDE 'QUAL_PAR'

*  Global Variables:
      INCLUDE 'TXT2AST_CMN'

*  Status:
      INTEGER			STATUS             	! Global status

*  External References:
      EXTERNAL			CHR_LEN
        INTEGER             	CHR_LEN
      EXTERNAL			TXT2AST_LOCSYM
        INTEGER             	TXT2AST_LOCSYM

*  Local Constants:
      INTEGER             DEFAULT           ! Tells FIO to use default
         PARAMETER        (DEFAULT = 0)

      INTEGER             MAX_DESCS         ! Maximum no. of record descriptors
         PARAMETER        (MAX_DESCS = 30)

      INTEGER             PTR_TABLES        ! Number of pointer tables
         PARAMETER        (PTR_TABLES = 1000)

      INTEGER             PTR_TABLEN        ! Length of a pointer table
         PARAMETER        (PTR_TABLEN = 10000)

      INTEGER             NODEFAULT         ! No data line default flag value
         PARAMETER        (NODEFAULT = -9999)

      INTEGER             	NMASKS            	! # recognised quality
        PARAMETER        	( NMASKS = 6 )		! mask names

      CHARACTER*30		VERSION
        PARAMETER		( VERSION = 'TXT2AST Version 2.2-0' )

*  Local Variables:
      CHARACTER*80		AX_LABEL(ADI__MXDIM) 	! Axis labels
      CHARACTER*80		AX_UNITS(ADI__MXDIM) 	! Axis labels
      CHARACTER           	CHA
      CHARACTER*132		FNAME			! User supplied file
      CHARACTER           	FILE_NAME*200   	! The full-file name
      CHARACTER           	HTEXT*80        	! History text
      CHARACTER*80		LABEL_DATA		! Output object label
      CHARACTER*80		QMASK			! Output quality mask
      CHARACTER*80		TITLE_DATA		! Output object title
      CHARACTER           	TSTR*80         	! Temporary storage
      CHARACTER*80		TYPE_DATA		! Output object type
      CHARACTER*80		UNITS_DATA		! Output object units

      REAL                DATAROW(MAX_DESCS)! The values read from a data line
      REAL                LASTROW(MAX_DESCS)! Last values read from a data line
      REAL			AX_SCADAT(ADI__MXDIM)	! Scalar axis widths

      INTEGER             BLOCK(PTR_TABLES) ! Pointers to data blocks
      INTEGER             DEF_VALS(MAX_DESCS)! The default values for each column

      INTEGER             	CLINE           	! Current line in file
      INTEGER             	CURBLOCK          	! Current block in use
      INTEGER             CURROW            ! Current row in current block
      INTEGER             DATA_LEN          ! Number of char in a real string
      INTEGER			DIMS(ADI__MXDIM)	! Output dimensions
      INTEGER             HTLEN             ! Length of history text
      INTEGER             IFD               ! Input file descriptor
      INTEGER             	I                 	! Loop variable
      INTEGER             MASTER_PTR        ! Pointer to master table
      INTEGER             	N                 	! Space saving variable
      INTEGER			NAXES			! Number of axes
      INTEGER			NDIM			! Output dimensionality
      INTEGER			NELM			! Number of data recs
      INTEGER             NUM_DESCS         ! Number of record descriptors
      INTEGER			OFID			! Output file identifier
      INTEGER             ORIG_PTR          ! Original order of MASTER_PTR table
      INTEGER             	QVAL              	! Quality value
      INTEGER             ROWADDR           ! Address of a start of a data row
      INTEGER             WPTR,WPTR2,WPTR3,WPTRW  ! Workspace arrays

      LOGICAL			AX_DECR(ADI__MXDIM)	! Axis decreasing?
      LOGICAL			AX_LABEL_OK(ADI__MXDIM)	! Axis labels ok?
      LOGICAL			AX_NORM(ADI__MXDIM)	! Axis normalised?
      LOGICAL                   AX_SCALAR(ADI__MXDIM)
      LOGICAL			AX_SYMWID(ADI__MXDIM)	! Axis widths symmetric?
      LOGICAL			AX_UNITS_OK(ADI__MXDIM)	! Axis labels ok?
      LOGICAL			AX_WOK(ADI__MXDIM)	! Axis widths ok?
      LOGICAL			ERROR_TO_VAR		! Error to variance?
      LOGICAL             	EXTRA             	! Any garbage on a line?
      LOGICAL             	GOOD_LINE         	! Good line read in?
      LOGICAL             	IGNORE(MAX_DESCS) 	! Ignore this column?
      LOGICAL			LABEL_OK		! Output label ok?
      LOGICAL             	MORE_DATA         	! More lines to be read?
      LOGICAL             	MORE_DESCS        	! More descriptors to do?
      LOGICAL			PRIM			! Output primitive?
      LOGICAL			QMASK_OK		! Quality mask ok?
      LOGICAL             SET_DEFAULT       ! Set a default using a data item
      LOGICAL             SPACE_LEFT        ! Space left in pointer lists
      LOGICAL             	TOP_LEVEL         	! TOPLEVEL record found
      LOGICAL			TITLE_OK		! Output title ok?
      LOGICAL			TYPE_OK			! Output type ok?
      LOGICAL			UNITS_OK		! Output units ok?
      LOGICAL                   LOLIM                   ! LOLIM instead of LOERROR ?
      LOGICAL                   UPLIM                   ! UPLIM instead of UPERROR ?

*  Local Data:
      CHARACTER           QMASK_NAMES(NMASKS)*8
      INTEGER             QMASK_VALS(NMASKS)
         DATA             QMASK_NAMES/'OK','MISSING','ARITH','BAD',
     :                            'IGNORE', 'PATCHED'/
         DATA             QMASK_VALS/QUAL__GOOD,QUAL__MISSING,
     :                               QUAL__ARITH,QUAL__BAD,
     :                             QUAL__IGNORE, QUAL__PATCHED/
*.

*  Check inherited global status.
      IF ( STATUS .NE. SAI__OK ) RETURN

*  Version id
      CALL MSG_PRNT( VERSION )

*  Initialise ASTERIX
      CALL AST_INIT()

      CLINE = 0
      MORE_DESCS = .TRUE.
      NUM_DESCS = 0

*  Initialise object structure
      LABEL_OK = .FALSE.
      TITLE_OK = .FALSE.
      TYPE_OK = .FALSE.
      UNITS_OK = .FALSE.

      DO I = 1, ADI__MXDIM
        AX_LABEL_OK(I) = .FALSE.
        AX_UNITS_OK(I) = .FALSE.
        AX_WOK(I) = .FALSE.
        AX_SYMWID(I) = .FALSE.
        AX_SCALAR(I) = .FALSE.
        USE_AX_DATA(I) = .FALSE.
        USE_AX_WID(I) = .FALSE.
        USE_AX_AWIDLO(I) = .FALSE.
        USE_AX_AWIDHI(I) = .FALSE.
        AX_NORM(I) = .FALSE.
      END DO
      QMASK_OK = .FALSE.
      USE_DATA = .FALSE.
      USE_VAR = .FALSE.
      USE_QUAL = .FALSE.
      USE_LOERR = .FALSE.
      USE_UPERR = .FALSE.
      LOLIM = .FALSE.
      UPLIM = .FALSE.
      NAXES = 0
      TYPE_DATA = 'BinDS'

*  Enable comment buffering
      CMT_BUF = .TRUE.
      CMT_N = 0

*  Initialise descriptors
      CALL ARR_INIT1L( .FALSE., MAX_DESCS, IGNORE, STATUS )

*  Get the name of and open the input text file
      CALL USI_GET0C( 'INP', FNAME, STATUS )
      CALL FIO_OPEN( FNAME, 'READ', 'LIST', DEFAULT, IFD, STATUS )
      IF ( STATUS .NE. SAI__OK ) GOTO 99

*  Create the output file
      CALL USI_CREAT( 'OUT', ADI__NULLID, OFID, STATUS )

*  Get the first input line
      CALL TXT2AST_NEXTLINE( OFID, IFD, CLINE, STATUS )
      IF ( STATUS .NE. SAI__OK ) THEN
        CALL MSG_PRNT( 'TOPLEVEL or data descriptor expected.' )
        STATUS = SAI__ERROR
      END IF

*  Does the input file have a TOPLEVEL record
      IF ( TXT2AST_LOCSYM( 'TOPLEVEL' ) .EQ. 1 ) THEN

*    It does so process the options
        IF ( WD_NUM .GT. 1 ) THEN

*      See if the TOPLEVEL line contains a TYPE specifier
          CALL TXT2AST_EXTQSYM( 'TYPE', TYPE_DATA, TYPE_OK, STATUS )

          IF ( .NOT. TYPE_OK ) THEN
            TYPE_DATA = 'BinDS'
            TYPE_OK = .TRUE.
          END IF

*      See if the TOPLEVEL line contains a TITLE specifier
          CALL TXT2AST_EXTQSYM( 'TITLE', TITLE_DATA, TITLE_OK, STATUS )

*      Tell user if any garbage on command line
          EXTRA = .FALSE.
          DO I = 1, WD_NUM
            IF ( .NOT. WD_USED(I) ) EXTRA = .TRUE.
          END DO
          IF ( EXTRA ) CALL TXT2AST_ERROR( 'WARNING : Extra ignored',
     :                                                     CLINE )

        ELSE
          CALL TXT2AST_ERROR( 'TITLE or TYPE expected after TOPLEVEL',
     :                                                      CLINE )
        END IF
        TOP_LEVEL = .TRUE.
      ELSE
        TITLE_OK = .FALSE.
        TYPE_OK = .FALSE.
        TOP_LEVEL = .FALSE.

      END IF

*  Create history
      CALL FIO_FNAME( IFD, FILE_NAME, STATUS )
      IF ( .NOT. PRIM ) THEN

*    Create HISTORY structure
        CALL HSI_ADD( OFID, VERSION, STATUS )
        CALL MSG_SETC( 'FILE', FILE_NAME )
        CALL MSG_MAKE( 'Created from text file ^FILE', HTEXT, HTLEN )
        CALL HSI_PTXT( OFID, 1, HTEXT(:HTLEN), STATUS )

*    Output buffered comments
        IF ( CMT_N .GT. 0 ) THEN
          CALL HSI_PTXT( OFID, CMT_N, CMT, STATUS )
        END IF
        CMT_BUF = .FALSE.

      END IF

*  Right. Thats the top level components out the way. Try to get the
*  record desciptors.
      DO WHILE ( MORE_DESCS .AND. (STATUS .EQ. SAI__OK) )

        IF ( TOP_LEVEL ) THEN
          CALL TXT2AST_NEXTLINE( OFID, IFD, CLINE, STATUS )
        END IF
        TOP_LEVEL= .TRUE.

        IF ( STATUS .EQ. SAI__OK ) THEN

*      Check word against valid descriptors
          IF ( TXT2AST_LOCSYM( 'AXIS' ) .EQ. 1 ) THEN

*        Check that last axis, if any, did not leave any dangling asymmetric
*        widths.
            IF ( (NAXES.GT.0) .AND. .NOT. AX_WOK(NAXES) ) THEN
              IF ( USE_AX_AWIDLO(NAXES) .OR.
     :             USE_AX_AWIDHI(NAXES) ) THEN
                IF ( USE_AX_AWIDLO(NAXES) ) THEN
                  CALL MSG_SETC( 'DESC', 'UPWIDTH' )
                ELSE
                  CALL MSG_SETC( 'DESC', 'LOWIDTH' )
                END IF
                CALL TXT2AST_ERROR( 'Missing ^DESC for previous axis',
     :                                                      CLINE )
              END IF
            END IF

            IF ( NAXES .EQ. ADI__MXDIM ) THEN
              CALL TXT2AST_ERROR( 'Maximum number of axes exceeded',
     :                                                    CLINE )
            ELSE
              NAXES = NAXES + 1
              NUM_DESCS = NUM_DESCS + 1
              N = NAXES
            END IF

*        Set table position to current record descriptor number
            POS_AX_DATA(NAXES) = NUM_DESCS

*        Skip all this next stuff if primitive output
            IF ( PRIM ) GOTO 17

*        Test for keywords after AXIS. First LABEL
            CALL TXT2AST_EXTQSYM( 'LABEL', AX_LABEL(N),
     :                           AX_LABEL_OK(N), STATUS )

*        ...then UNITS
            CALL TXT2AST_EXTQSYM( 'UNITS', AX_UNITS(N),
     :                           AX_UNITS_OK(N), STATUS )

*        ... then NORM
            AX_NORM(N) = TXT2AST_LOCSYM('NORM') .NE. 0

*        ...then DECREASING
            AX_DECR(N) = TXT2AST_LOCSYM('DECREASING') .NE. 0

*        ...then SCALAR_WIDTH
            CALL TXT2AST_EXTQSYM( 'SCALAR_WIDTH', TSTR,
     :                           AX_SCALAR(N), STATUS )
            IF ( AX_SCALAR(N) ) THEN
              CALL CHR_CTOR( TSTR(:CHR_LEN(TSTR)),AX_SCADAT(N), STATUS )
              IF ( STATUS .NE. SAI__OK ) GOTO 99
              AX_WOK(N) = .TRUE.
            END IF

*        Come here from primitive skip
 17         CONTINUE

*      Axis width descriptor
          ELSE IF ( TXT2AST_LOCSYM( 'WIDTH' ) .EQ. 1 ) THEN

*        Assume WIDTH descriptor corresponds to most recent
*        AXIS specifier. If NAXES is zero, then error...
            IF ( NAXES .EQ. 0 ) THEN
              CALL TXT2AST_ERROR( 'No axis for this WIDTH '/
     :                           /'descriptor', CLINE )
            ELSE IF ( AX_WOK(NAXES) ) THEN
              IF ( AX_SYMWID(NAXES) ) THEN
                CALL MSG_SETC( 'FORM', 'Symmetric' )
              ELSE
                CALL MSG_SETC( 'FORM', 'Asymmetric' )
              END IF
              CALL TXT2AST_ERROR( '^FORM axis widths already specified'/
     :                                     /' for this axis', CLINE )
            ELSE IF ( AX_SCALAR(NAXES) ) THEN
              CALL TXT2AST_ERROR( 'Scalar width already specified'/
     :                           /' for this axis', CLINE )
            ELSE
              NUM_DESCS = NUM_DESCS + 1
              AX_WOK(NAXES) = .TRUE.
              AX_SYMWID(NAXES) = .TRUE.
              POS_AX_WID(NAXES) = NUM_DESCS
            END IF

*      Lower or upper axis widths descriptors
          ELSE IF ( ( TXT2AST_LOCSYM('LOWIDTH') .EQ. 1 ) .OR.
     :              ( TXT2AST_LOCSYM('UPWIDTH') .EQ. 1 ) ) THEN

*        Assume LO/HI WIDTH descriptor corresponds to most recent
*        AXIS specifier. If NAXES is zero, then error...
            IF ( NAXES .EQ. 0 ) THEN
              CALL MSG_SETC( 'DESC', WD_DATA(1) )
              CALL TXT2AST_ERROR( 'No axis for this ^DESC '/
     :                           /'descriptor', CLINE )
            ELSE IF ( AX_WOK(NAXES) ) THEN
              IF ( AX_SYMWID(NAXES) ) THEN
                CALL MSG_SETC( 'FORM', 'Symmetric' )
              ELSE
                CALL MSG_SETC( 'FORM', 'Asymmetric' )
              END IF
              CALL TXT2AST_ERROR( '^FORM axis widths already specified'/
     :                                     /' for this axis', CLINE )
            ELSE

*          Set flags for lower/upper axis widths
              NUM_DESCS = NUM_DESCS + 1
              IF ( TXT2AST_LOCSYM('LOWIDTH') .EQ. 1 ) THEN
                IF ( USE_AX_AWIDLO(NAXES) ) THEN
                  CALL TXT2AST_ERROR( 'Lower axis widths already'/
     :                     /' specified for this axis', CLINE )
                ELSE
                  USE_AX_AWIDLO(NAXES) = .TRUE.
                  POS_AX_AWIDLO(NAXES) = NUM_DESCS
                END IF
              ELSE
                IF ( USE_AX_AWIDHI(NAXES) ) THEN
                  CALL TXT2AST_ERROR( 'Upper axis widths already'/
     :                     /' specified for this axis', CLINE )
                ELSE
                  USE_AX_AWIDHI(NAXES) = .TRUE.
                  POS_AX_AWIDHI(NAXES) = NUM_DESCS
                END IF
              END IF

*          Both widths specified?
              IF ( USE_AX_AWIDLO(NAXES) .AND.
     :             USE_AX_AWIDHI(NAXES) ) THEN
                AX_SYMWID(NAXES) = .FALSE.
                AX_WOK(NAXES) = .TRUE.
              END IF

            END IF

*      Lower error descriptor
          ELSE IF ( TXT2AST_LOCSYM('LOERROR') .EQ. 1 ) THEN

            IF ( USE_LOERR ) THEN
              CALL TXT2AST_ERROR( 'Lower data widths already'/
     :                               /' specified', CLINE )
            ELSE
              NUM_DESCS = NUM_DESCS + 1
              USE_LOERR = .TRUE.
              POS_LOERR = NUM_DESCS
            END IF

*      Upper error descriptor
          ELSE IF ( TXT2AST_LOCSYM('UPERROR') .EQ. 1 ) THEN

            IF ( USE_UPERR ) THEN
              CALL TXT2AST_ERROR( 'Upper data widths already'/
     :                               /' specified', CLINE )
            ELSE
              NUM_DESCS = NUM_DESCS + 1
              USE_UPERR = .TRUE.
              POS_UPERR = NUM_DESCS
            END IF

*      Lower error descriptor, but with LOLIM
          ELSE IF ( TXT2AST_LOCSYM('LOLIM') .EQ. 1 ) THEN

            IF ( USE_LOERR ) THEN
              CALL TXT2AST_ERROR( 'Lower data widths already'/
     :                               /' specified', CLINE )
            ELSE
              NUM_DESCS = NUM_DESCS + 1
              USE_LOERR = .TRUE.
              POS_LOERR = NUM_DESCS
              LOLIM = .TRUE.
            END IF

*      Upper error descriptor, but with UPLIM
          ELSE IF ( TXT2AST_LOCSYM('UPLIM') .EQ. 1 ) THEN

            IF ( USE_UPERR ) THEN
              CALL TXT2AST_ERROR( 'Upper data widths already'/
     :                               /' specified', CLINE )
            ELSE
              NUM_DESCS = NUM_DESCS + 1
              USE_UPERR = .TRUE.
              POS_UPERR = NUM_DESCS
              UPLIM = .TRUE.
            END IF

*      Ignore descriptor
          ELSE IF ( TXT2AST_LOCSYM('IGNORE') .EQ. 1 ) THEN

*        If IGNORE descriptor we're ignoring a column
            NUM_DESCS = NUM_DESCS + 1
            IGNORE(NUM_DESCS) = .TRUE.

*      Data descriptor
          ELSE IF ( TXT2AST_LOCSYM('DATA') .EQ. 1 ) THEN

*        Make sure DATA hasn't already been specified
            IF ( USE_DATA ) THEN
              CALL TXT2AST_ERROR( 'DATA multiply specified', CLINE )
            ELSE
              NUM_DESCS = NUM_DESCS + 1
              USE_DATA = .TRUE.
              POS_DATA = NUM_DESCS

*          Data units present?
              CALL TXT2AST_EXTQSYM( 'UNITS', UNITS_DATA, UNITS_OK,
     :                             STATUS )

*          Data label present?
              CALL TXT2AST_EXTQSYM( 'LABEL', LABEL_DATA, LABEL_OK,
     :                             STATUS )

            END IF

*      Quality descriptor
          ELSE IF ( TXT2AST_LOCSYM('QUALITY') .EQ. 1 ) THEN

*        Make sure QUALITY hasn't already been specified
            IF ( USE_QUAL ) THEN
              CALL TXT2AST_ERROR( 'QUALITY multiply specified', CLINE )
            ELSE
              NUM_DESCS = NUM_DESCS + 1
              USE_QUAL = .TRUE.
              POS_QUAL = NUM_DESCS

*          QUALITY mask present?
              CALL TXT2AST_EXTQSYM( 'MASK', QMASK, QMASK_OK, STATUS )

            END IF

*      Variance descriptor
          ELSE IF ( TXT2AST_LOCSYM('VARIANCE') .EQ. 1 ) THEN

*        Make sure VARIANCE hasn't already been specified
            IF ( USE_VAR ) THEN
              CALL TXT2AST_ERROR( 'VARIANCE multiply specified', CLINE )
            ELSE
              NUM_DESCS = NUM_DESCS + 1
              USE_VAR = .TRUE.
              POS_VAR = NUM_DESCS
              ERROR_TO_VAR = .FALSE.
            END IF

*      Grouping descriptor
          ELSE IF ( TXT2AST_LOCSYM('GROUP') .EQ. 1 ) THEN

*        Make sure GROUP hasn't already been specified
            IF ( USE_GRP ) THEN
              CALL TXT2AST_ERROR( 'GROUP multiply specified', CLINE )
            ELSE
              NUM_DESCS = NUM_DESCS + 1
              USE_GRP = .TRUE.
              POS_GRP = NUM_DESCS
            END IF

*      Error descriptor
          ELSE IF ( TXT2AST_LOCSYM('ERROR') .EQ. 1 ) THEN

*        Make sure VARIANCE hasn't already been specified
            IF ( USE_VAR ) THEN
              CALL TXT2AST_ERROR( 'Data error/variance multiply'/
     :                                  /' specified', CLINE )
            ELSE
              NUM_DESCS = NUM_DESCS + 1
              USE_VAR = .TRUE.
              POS_VAR = NUM_DESCS
              ERROR_TO_VAR = .TRUE.
            END IF

          ELSE
            IF ( NUM_DESCS .EQ. 0 ) THEN
              STATUS = SAI__ERROR
            ELSE
              MORE_DESCS = .FALSE.
            END IF

          END IF

        END IF

        IF ( STATUS .NE. SAI__OK ) THEN
          IF ( NUM_DESCS .EQ. 0 ) THEN
            CALL TXT2AST_ERROR( 'Record descriptor expected', CLINE )
          ELSE
            CALL TXT2AST_ERROR( 'Record descriptor or numeric'/
     :                           /' data expected', CLINE )
          END IF
          GOTO 99

        ELSE IF ( MORE_DESCS ) THEN

*      Tell user if any garbage on text line
          EXTRA = .FALSE.
          DO I = 1, WD_NUM
            IF ( .NOT. WD_USED(I) ) EXTRA = .TRUE.
          END DO
          IF ( EXTRA ) CALL TXT2AST_ERROR( 'WARNING : Extra ignored',
     :                                                      CLINE )
        END IF

      END DO
      IF ( STATUS .NE. SAI__OK ) GOTO 99

*  Check that last axis, if any, did not leave any dangling asymmetric widths
      IF ( ( NAXES.GT.0) .AND. .NOT. AX_WOK(NAXES) ) THEN
        IF ( USE_AX_AWIDLO(NAXES) .OR. USE_AX_AWIDHI(NAXES) ) THEN
          IF ( USE_AX_AWIDLO(NAXES) ) THEN
            CALL MSG_SETC( 'DESC', 'UPWIDTH' )
          ELSE
            CALL MSG_SETC( 'DESC', 'LOWIDTH' )
          END IF
          CALL TXT2AST_ERROR( 'Missing ^DESC for previous axis',CLINE )
          GOTO 99
        END IF
      END IF

*  Must have both lower and upper data errors, or neither
      IF ( (USE_LOERR.OR.USE_UPERR) .AND.
     :     .NOT. (USE_LOERR.AND.USE_UPERR) ) THEN
        IF ( USE_LOERR ) THEN
          CALL MSG_SETC( 'DESC', 'UPERROR/UPLIM' )
        ELSE
          CALL MSG_SETC( 'DESC', 'LOERROR/LOLIM' )
        END IF
        CALL TXT2AST_ERROR( 'Missing ^DESC descriptor', CLINE )
        GOTO 99
      END IF

*  The data lines are now read in. We do not know the size of any of the
*  axes until we've finished reading in all the data and sorted it. Only
*  in the case where no axis structures were requested can we output the
*  data in a simple manner - into a one dimensional array.
      MORE_DATA = .TRUE.
      SPACE_LEFT = .TRUE.
      CURBLOCK = 1
      CURROW = 1
      NELM = 0

*  Set up the initial defaults - there are none for axis fields
      CALL ARR_INIT1R( 0.0, NUM_DESCS, DEF_VALS, STATUS )
      IF ( NAXES .GT. 0 ) THEN
        DO I = 1, NAXES
          DEF_VALS(POS_AX_DATA(I)) = NODEFAULT
        END DO
      END IF

*  Allocate memory for the first block
      CALL DYN_MAPR( 1, PTR_TABLEN*NUM_DESCS, BLOCK(CURBLOCK), STATUS )

*  While more lines
      DO WHILE ( MORE_DATA .AND. SPACE_LEFT )

*    Good line
        GOOD_LINE = .TRUE.

*    NB : At this point we've already got the text of the current data line.
        NELM = NELM + 1

*    Find out where in memory to put this data row.
        ROWADDR = BLOCK(CURBLOCK) + (CURROW - 1)*4*NUM_DESCS

*    Analyse the strings and put into DATAROW
        IF ( WD_NUM .GE. NUM_DESCS ) THEN

*      For each item on the data row...
          DO I = 1, NUM_DESCS

            IF ( IGNORE(I) ) THEN
              DATAROW(I) = 0.0

            ELSE IF ( WD_DATA(I)(1:1) .EQ. '''' ) THEN

*          The data item is a single quote - try to repeat last value
              IF ( NELM .GT. 1 ) THEN
                DATAROW(I) = LASTROW(I)

              ELSE
                CALL TXT2AST_ERROR( 'Cannot ditto on first data'/
     :                                       /' line', CLINE )
                STATUS = SAI__ERROR
                GOTO 99

              END IF

            ELSE IF ( WD_DATA(I) .EQ. '-' ) THEN

*          The item is a hyphen - try and use default
              IF ( DEF_VALS(I) .EQ. NODEFAULT ) THEN
                CALL TXT2AST_ERROR( 'No default for this field',CLINE )
                STATUS = SAI__ERROR
                GOTO 99

              ELSE
                DATAROW(I) = DEF_VALS(I)

              END IF

*        Is this the quality column
            ELSE IF ( (I.EQ.POS_QUAL) .AND. USE_QUAL ) THEN

*          Otherwise it must be real data
              SET_DEFAULT = ( INDEX( WD_DATA(I),'^' ) .EQ. WD_LEN(I) )
              DATA_LEN = WD_LEN(I)

              IF ( SET_DEFAULT ) DATA_LEN = DATA_LEN - 1

              CHA = WD_DATA(I)(1:1)
              IF ( (CHA.NE.'-') .AND. ((CHA.LT.'0')
     :                            .OR. (CHA.GT.'9')) ) THEN

*            Analyse quality string for mask name(s)
                CALL PRS_BITPAT( WD_DATA(I)(:DATA_LEN), NMASKS,
     :                     QMASK_NAMES, QMASK_VALS, QVAL, STATUS )
                IF ( STATUS .NE. SAI__OK ) THEN
                  CALL MSG_SETC( 'QSTR', WD_DATA(I)(:DATA_LEN) )
                  CALL TXT2AST_ERROR( 'Invalid quality byte '/
     :                          /'specifier ^QSTR', CLINE )
                  STATUS = SAI__ERROR
                  GOTO 99
                ELSE
                  DATAROW(I) = REAL(QVAL)
                END IF

              ELSE

                READ ( WD_DATA(I)(:DATA_LEN), *, IOSTAT=STATUS )
     :                                                DATAROW(I)
                GOOD_LINE = ( STATUS .EQ. SAI__OK )
                IF ( STATUS .NE. SAI__OK ) THEN
                  IF ( NELM .EQ. 1 ) THEN
                    CALL MSG_SETC( 'TXT', 'unrecognised header'/
     :                             /' keyword' )
                  ELSE
                    CALL MSG_SETC( 'TXT', 'data = ' )
                  END IF
                  CALL MSG_SETC( 'DAT', WD_DATA(I)(:DATA_LEN) )
                  CALL TXT2AST_ERROR( 'Read error, ^TXT "^DAT"', CLINE )
                  STATUS = SAI__ERROR
                  GOTO 99
                END IF

              END IF

*          Set default if requested
              IF ( SET_DEFAULT ) DEF_VALS(I) = DATAROW(I)

            ELSE

*          Otherwise it must be real data
              SET_DEFAULT = ( INDEX( WD_DATA(I),'^' ) .EQ. WD_LEN(I) )
              DATA_LEN = WD_LEN(I)

              IF ( SET_DEFAULT ) DATA_LEN = DATA_LEN - 1

*          Attempt to read value
              READ ( WD_DATA(I)(:DATA_LEN), *, IOSTAT=STATUS )
     :                                              DATAROW(I)
              GOOD_LINE = ( STATUS .EQ. SAI__OK )
              IF ( STATUS .NE. SAI__OK ) THEN
                IF ( NELM .EQ. 1 ) THEN
                  CALL MSG_SETC( 'TXT', 'unrecognised header'/
     :                           /' keyword or bad data' )
                ELSE
                  CALL MSG_SETC( 'TXT', 'data =' )
                END IF
                CALL MSG_SETC( 'DAT', WD_DATA(I)(:DATA_LEN) )
                CALL TXT2AST_ERROR( 'Read error, ^TXT "^DAT"', CLINE )
                STATUS = SAI__ERROR
                GOTO 99
              END IF

              IF ( SET_DEFAULT ) DEF_VALS(I) = DATAROW(I)

            END IF
          END DO

        ELSE
          CALL TXT2AST_ERROR( 'Data item(s) missing', CLINE )
          STATUS = SAI__ERROR
          GOTO 99

        END IF

*    Put the data values into the row in memory
        IF ( GOOD_LINE ) THEN

*      If data errors were entered with LO/UPLIM option,
*      set to LO/UPERROR values.
          IF ( LOLIM ) DATAROW(POS_LOERR) = DATAROW(POS_DATA) -
     :                                      DATAROW(POS_LOERR)
          IF ( UPLIM ) DATAROW(POS_UPERR) = DATAROW(POS_UPERR) -
     :                                      DATAROW(POS_DATA)

          CALL ARR_COP1R( NUM_DESCS, DATAROW, %VAL(ROWADDR), STATUS )

*    Reset status for bad line
        ELSE

          STATUS = SAI__OK

        END IF

*    Get the next data row
        CALL TXT2AST_NEXTLINE( OFID, IFD, CLINE, STATUS )

        IF ( STATUS .NE. SAI__OK ) THEN
          MORE_DATA = .FALSE.
          CALL ERR_ANNUL( STATUS )

        ELSE

*      Work out where we're going to put this next row. If no room in
*      current block, get another. If that isn't possible then we've
*      run out of room ( an unlikely event ).
          IF ( CURROW .LT. PTR_TABLEN ) THEN
            CURROW = CURROW + 1

          ELSE
            CURROW = 1
            IF ( CURBLOCK .LT. PTR_TABLES ) THEN
              CURBLOCK = CURBLOCK + 1
              CALL DYN_MAPR( 1, PTR_TABLEN*NUM_DESCS,
     :                      BLOCK(CURBLOCK), STATUS )
              IF ( STATUS .NE. SAI__OK ) GOTO 99

            ELSE
              SPACE_LEFT = .FALSE.
              CALL TXT2AST_ERROR( 'TXT2AST has run out of space',
     :                            CLINE )
            END IF

          END IF

        END IF

      END DO
      IF ( STATUS .NE. SAI__OK ) GOTO 99

*  Allocate the memory for the master table
      IF ( NELM .GT. 0 ) THEN
        CALL DYN_MAPI( 1, NELM, MASTER_PTR, STATUS )

*    Fill the master data table with the address of all the data
*    rows entered.
        CALL TXT2AST_FILLTAB( NELM, %VAL(MASTER_PTR), PTR_TABLES,
     :                    BLOCK, PTR_TABLEN, NUM_DESCS, STATUS )

      END IF
      IF ( STATUS .NE. SAI__OK ) GOTO 99

*  Map in everything we need, except for axis stuff.
      IF ( USE_DATA ) THEN
        CALL DYN_MAPR( 1, NELM, PTR_DATA, STATUS )
      END IF
      IF ( USE_VAR ) THEN
        CALL DYN_MAPR( 1, NELM, PTR_VAR, STATUS )
      END IF
      IF ( USE_QUAL ) THEN
        CALL DYN_MAPB( 1, NELM, PTR_QUAL, STATUS )
      END IF
      IF ( USE_LOERR ) THEN
        CALL DYN_MAPR( 1, NELM, PTR_LOERR, STATUS )
        CALL DYN_MAPR( 1, NELM, PTR_UPERR, STATUS )
      END IF
      IF ( STATUS .NE. SAI__OK ) GOTO 99

*  Now, did the input specify axes...
      IF ( NAXES .NE. 0 ) THEN

        NDIM = NAXES

*    Grab some workspace.
        CALL DYN_MAPR( 1, NELM, WPTR, STATUS )
        CALL DYN_MAPR( 1, NELM*2, WPTRW, STATUS )
        CALL DYN_MAPR( 1, MAX(1,NAXES)*NELM, WPTR2, STATUS )
        CALL DYN_MAPR( 1, NELM, WPTR3, STATUS )
        CALL DYN_MAPR( 1, NELM, ORIG_PTR, STATUS )
        CALL ARR_COP1R( NELM, %VAL(MASTER_PTR), %VAL(ORIG_PTR), STATUS )

      ELSE
        NDIM = 1
        DIMS(1) = NELM
        ORIG_PTR = MASTER_PTR

      END IF

*  Tell user number of records
      CALL MSG_SETI( 'NREC', NELM )
      CALL MSG_SETC( 'FILE', FILE_NAME )
      CALL MSG_PRNT( '^NREC data records read in from ^FILE' )

*  Process the data
      CALL TXT2AST_CRUNCH( OFID, TYPE_DATA, PRIM, NAXES, NELM, NDIM,
     :                    DIMS, AX_WOK, AX_DECR, AX_SYMWID, AX_SCALAR,
     :                    QMASK_OK, QMASK, ERROR_TO_VAR,
     :                    %VAL(MASTER_PTR),
     :                    %VAL(ORIG_PTR), NUM_DESCS, %VAL(WPTR),
     :                    %VAL(WPTRW), MAX(1,NAXES),
     :                    %VAL(WPTR2),%VAL(WPTR3), STATUS )
      IF ( STATUS .NE. SAI__OK ) GOTO 99

*  Output not primitive?
      IF ( .NOT. PRIM ) THEN

*    If we've got a data label then create it
        IF ( LABEL_OK ) THEN
          CALL BDI_PUT0C( OFID, 'Label', LABEL_DATA, STATUS )
        END IF

*    If we've got data units then create it
        IF ( UNITS_OK ) THEN
          CALL BDI_PUT0C( OFID, 'Units', UNITS_DATA, STATUS )
        END IF
        IF ( STATUS .NE. SAI__OK ) GOTO 99

*    Set 'easy' axis components
        DO N = 1, NAXES
          IF ( AX_LABEL_OK(N) ) THEN
            CALL BDI_AXPUT0C( OFID, N, 'Label', AX_LABEL(N), STATUS )
          END IF
          IF ( AX_UNITS_OK(N) ) THEN
            CALL BDI_AXPUT0C( OFID, N, 'Units', AX_UNITS(N), STATUS )
          END IF
          IF ( AX_NORM(N) ) THEN
            CALL BDI_AXPUT0L( OFID, N, 'Normalised', .TRUE., STATUS )
          END IF
          IF ( AX_SCALAR(N) ) THEN
            CALL BDI_AXPUT0R( OFID, N, 'ScalarWidth', AX_SCADAT(N),
     :                        STATUS )
          END IF
        END DO

      END IF

*  If we've got a data title then create it
      IF ( TITLE_OK ) THEN
        IF ( PRIM ) THEN
          CALL MSG_PRNT( 'Cannot write TITLE to primitive output' )
        ELSE
          CALL BDI_PUT0C( OFID, 'Title', TITLE_DATA, STATUS )
        END IF
      END IF
      IF ( STATUS .NE. SAI__OK ) GOTO 99

*  Close input file
      CALL FIO_CLOSE( IFD, STATUS )

*  Close down ASTERIX
 99   CALL AST_CLOSE
      CALL AST_ERR( STATUS )

      END


*+  TXT2AST_NEXTLINE - Returns next useful line from input
      SUBROUTINE TXT2AST_NEXTLINE( OFID, INFILE, CURLINE, STATUS )
*
*    Description :
*
*     Reads the next significant line from the text file supplied. Significant
*     means blank lines are skipped, as are lines which are wholly comments.
*     Any comment appending a 'significant line' is removed. The text is
*     broken up into its constituent words.
*
*    Author :
*
*     David J. Allan (BHVAD::DJA)
*
*    History :
*
*     07 Nov 88 : Original (DJA)
*
*    Type definitions :
*
      IMPLICIT NONE
*
*    Global constants :
*
      INCLUDE 'SAE_PAR'
      INCLUDE 'ADI_PAR'
      INCLUDE 'FIO_PAR'
      INCLUDE 'FIO_ERR'
*
*    Status :
*
      INTEGER             STATUS
*
*    Global variables :
*
      INCLUDE 'TXT2AST_CMN'
*
*    Import :
*
      INTEGER			OFID			! Output file object
      INTEGER             	INFILE            	! Input file descriptor
*
*    Import-Export :
*
      INTEGER             	CURLINE           	! Current line in file
*
*    Functions :
*
      INTEGER             	CHR_LEN           	! Logical length of a string
*
*    Local constants :
*
      INTEGER             EOF               ! End of file
         PARAMETER        ( EOF = -1 )
*
*    Local variables :
*
      CHARACTER           TEXT*1000         ! The text of the line
      CHARACTER           BTEXT*200         ! Individual records from input file
      CHARACTER           CH                ! Character of TEXT

      INTEGER             CPOS              ! Character pointer
      INTEGER             ENDWORD           ! Character pointer
      INTEGER             I
      INTEGER             N                 ! Current word number
      INTEGER             NCHAR             ! Characters in text read
      INTEGER             TLEN              ! Total length so far

      LOGICAL             INQUOTES          ! TRUE if word in quotes
      LOGICAL             GOT_LINE          ! FALSE while not got line
*-

*    Check status
      IF ( STATUS .NE. SAI__OK ) RETURN

      GOT_LINE = .FALSE.
      TEXT = ' '
      TLEN = 0
      DO WHILE ( ( STATUS .EQ. SAI__OK ) .AND. .NOT. GOT_LINE )

*      Get line from input file
        CALL FIO_READF( INFILE, BTEXT, STATUS )
        IF ( STATUS .EQ. FIO__EOF ) GOTO 20

*      Bump up line counter
        CURLINE = CURLINE + 1

*      Remove leading comments and spaces
        NCHAR = CHR_LEN(BTEXT)
        IF ( NCHAR .EQ. 0 ) THEN

          GOTO 20

        ELSE

*        Convert control characters
          DO I = 1, NCHAR
            IF ( ICHAR(BTEXT(I:I)) .LT. 32 ) BTEXT(I:I) = ' '
          END DO

*        Skip along to first comment character
          CPOS = 1
          INQUOTES = .FALSE.
          DO WHILE ( CPOS .LE. NCHAR )
            IF (BTEXT(CPOS:CPOS) .EQ. '"' ) THEN
              INQUOTES = ( .NOT. INQUOTES )
            ELSE IF ( .NOT. INQUOTES ) THEN

*            Write comments to output file
              IF ( (BTEXT(CPOS:CPOS).EQ.';') .OR.
     :             (BTEXT(CPOS:CPOS).EQ.'!') .OR.
     :                (BTEXT(CPOS:CPOS).EQ.'*') ) THEN
                IF ( BTEXT(CPOS+1:) .GT. ' ' ) THEN
                  IF ( CMT_BUF ) THEN
                    IF ( CMT_N .LT. MAXCMT ) THEN
                      CMT_N = CMT_N + 1
                      CMT(CMT_N) = BTEXT(CPOS+1:)
                    END IF
                  ELSE
                    CALL HSI_PTXT( OFID, 1, BTEXT(CPOS+1:), STATUS )
                  END IF
                END IF
                GOTO 23
              END IF

            END IF
            CPOS = CPOS + 1
          END DO
 23       CONTINUE

*        Extract the segment
          IF ( ( CPOS.EQ.0 ) .OR. (BTEXT(1:(CPOS-1)).GT.' ')) THEN
            BTEXT = BTEXT(1:MIN(CPOS-1,NCHAR))//' '
          ELSE
            GOTO 20
          END IF

        END IF

*      Append new bit of text to buffer
        NCHAR = CHR_LEN(BTEXT)
        IF ( TLEN .EQ. 0 ) THEN
          TEXT = BTEXT
          TLEN = NCHAR
        ELSE
          TEXT = TEXT(:TLEN)//BTEXT(:NCHAR)
          TLEN = TLEN + NCHAR
        END IF

*      If no continuation character, then we have a good line
        IF ( ( TEXT(TLEN:TLEN) .EQ. '+' ) .OR.
     :           ( TEXT(TLEN:TLEN) .EQ. '~' ) ) THEN
          TLEN = TLEN - 1
        ELSE
          GOT_LINE = .TRUE.
        END IF

 20   END DO

*    Escape if end of file and no line read in
      IF ( ( STATUS .NE. SAI__OK ) .AND. ( TLEN .EQ. 0  ) ) GOTO 99

*    Reset status - we may have a continued line not finished
      STATUS = SAI__OK

*    Now we break up the string into its component words. Words are
*    delimited by spaces, but anything inside double quotes is one word
      N = 0
      CPOS = 1
      NCHAR = TLEN
      DO WHILE ( CPOS .LE. NCHAR )
        CH = TEXT(CPOS:CPOS)
        IF ( CH .NE. ' ' ) THEN
          INQUOTES = ( CH .EQ. '"' )
          N = N + 1
          IF ( INQUOTES ) THEN
            ENDWORD = INDEX(TEXT((CPOS+1):),'"')
            WD_DATA(N)=TEXT(CPOS:(CPOS+ENDWORD))
            CPOS = CPOS + ENDWORD + 1

          ELSE
            ENDWORD = INDEX(TEXT(CPOS:),' ')
            WD_DATA(N) = TEXT(CPOS:(CPOS+ENDWORD-2))
            CPOS = CPOS + ENDWORD

          END IF
          WD_USED(N) = .FALSE.
          WD_LEN(N) = CHR_LEN(WD_DATA(N))

        ELSE
          CPOS = CPOS + 1

        END IF

      END DO
      WD_NUM = N

 99   CONTINUE

      END



*+  TXT2AST_ERROR - Signal an error in TXT2AST
      SUBROUTINE TXT2AST_ERROR( MESSAGE, LINENUMBER )
*
*    Description :
*
*     Outputs a text message describing an error
*
*    Author :
*
*     David J. Allan  (BHVAD::DJA)
*
*    History :
*
*     08 Nov 88 : Original (DJA)
*
*    Type definitions :
*
      IMPLICIT NONE
*
*    Global constants :
*
      INCLUDE 'SAE_PAR'
*
*    Import :
*
      CHARACTER*(*)        MESSAGE              ! Start of message text
      INTEGER              LINENUMBER           ! Text line where error occ'd
*-
      CALL MSG_SETI( 'LINE', LINENUMBER  )
      CALL MSG_SETC( 'MSG', MESSAGE )
      CALL MSG_PRNT( '^MSG at line ^LINE' )
      END



*+  TXT2AST_EXTQSYM - Gets a keyword-data pair from a word list
      SUBROUTINE TXT2AST_EXTQSYM( KEYWORD, KDAT, KDAT_OK, STATUS )
*
*    Description :
*
*     Looks to see if a given keyword is in a word list. If it is then
*     that keyword's data is returned. The word data is nulled afterwards.
*
*    Author :
*
*     David J. Allan  (BHVAD::DJA)
*
*    History :
*
*     08 Nov 88 : Original (DJA)
*
*    Type definitions :
*
      IMPLICIT NONE
*
*    Global constants :
*
      INCLUDE 'SAE_PAR'
      INCLUDE 'ADI_PAR'
*
*    Status :
*
      INTEGER             STATUS
*
*    Global variables :
*
      INCLUDE 'TXT2AST_CMN'
*
*    Import :
*
      CHARACTER*(*)       KEYWORD                ! The keyword of interest
*
*    Export :
*
      CHARACTER*(*)       KDAT               ! The data for the keyword

      LOGICAL             KDAT_OK            ! Keyword data found OK
*
*    Function declarations :
*
      INTEGER             CHR_LEN                ! Logical length of a string
      INTEGER             TXT2AST_LOCSYM          ! Find a word in list
*
*    Local variables :
*
      INTEGER             I                      ! Loop counter
      INTEGER             I2                     ! Length

      LOGICAL             KEY_OK                 ! Keyword found OK
*-

      IF ( STATUS .NE. SAI__OK ) RETURN

*    Search for the keyword
      KDAT_OK = .FALSE.
      KDAT = ' '

      I = TXT2AST_LOCSYM(KEYWORD)
      KEY_OK = ( I .NE. 0 )

      IF ( KEY_OK ) THEN
        IF ( I .LT. WD_NUM ) THEN

          I2 = WD_LEN(I+1)

          IF ( WD_DATA(I+1)(1:1) .EQ. '"' ) THEN
            KDAT = WD_DATA(I+1)(2:(I2-1))
          ELSE
            KDAT = WD_DATA(I+1)(1:I2)
          END IF
          KDAT_OK = ( CHR_LEN( KDAT ) .GT. 0 )
          WD_USED(I+1) = .TRUE.
        END IF
      END IF

      END



*+  TXT2AST_LOCSYM - Locate a word in word table
      INTEGER FUNCTION TXT2AST_LOCSYM( KEYWORD )
*
*    Description :
*
*     Looks to see if a given keyword is in a word list. If it is then
*     that keyword's position is returned, otherwise zero is returned.
*     The word position is nulled if the keyword is matched.
*
*    Author :
*
*     David J. Allan  (BHVAD::DJA)
*
*    History :
*
*     08 Nov 88 : Original (DJA)
*
*    Type definitions :
*
      IMPLICIT NONE
*
*    Local constants :
*
      INCLUDE 'SAE_PAR'
      INCLUDE 'ADI_PAR'
*
*    Global variables :
*
      INCLUDE 'TXT2AST_CMN'
*
*    Import :
*
      CHARACTER*(*)       KEYWORD                ! The keyword of interest
*
*    Function declarations :
*
      LOGICAL             CHR_SIMLR
*
*    Local variables :
*
      INTEGER             I                      ! Loop counter
      INTEGER             KEY_POS                ! Temporary position
      INTEGER             I2                     ! Length
*-

      KEY_POS = 0
      I = 1
      DO WHILE ( ( I .LE. WD_NUM ) .AND. ( KEY_POS .EQ. 0) )
        I2 = WD_LEN(I)
        IF ( CHR_SIMLR(WD_DATA(I)(:I2),KEYWORD) ) THEN
          KEY_POS = I
          WD_USED(I) = .TRUE.
        ELSE
          I = I + 1
        END IF
      END DO

      TXT2AST_LOCSYM = KEY_POS

      END



*+  TXT2AST_FILLTAB - Creates master table of row addresses
      SUBROUTINE TXT2AST_FILLTAB( N, MASTER, NBLOCKS, BLOCK_STARTS,
     :                               BLOCK_LEN, ITEMSIZE, STATUS )
*
*    Description :
*
*     Creates master pointer table. BLOCK_STARTS contains the start addresses
*     of each of the allocated blocks.
*
*    Author :
*
*     David J. Allan  (BHVAD::DJA)
*
*    History :
*
*     09 Nov 88 : Original (DJA)
*
*    Type definitions :
*
      IMPLICIT NONE
*
*    Global constants :
*
      INCLUDE 'SAE_PAR'
      INCLUDE 'PRM_PAR'
*
*    Status :
*
      INTEGER             STATUS
*
*    Import :
*
      INTEGER             N                      ! Number of ptr's in master
      INTEGER             NBLOCKS                ! Size of BLOCK_STARTS
      INTEGER             BLOCK_LEN              ! Length of dynamic block
      INTEGER             BLOCK_STARTS(NBLOCKS)  ! Starts of dynamic blocks
      INTEGER             ITEMSIZE               ! Size of basic record
*
*    Export :
*
      INTEGER             MASTER(N)              ! The master table
*
*    Local variables :
*
      INTEGER             I                      ! Loop counter
      INTEGER             J                      ! Block number of element
      INTEGER             OFFSET                 ! Offset in a block of element
*-

      IF ( STATUS .NE. SAI__OK ) RETURN

      DO I = 1, N
        J = MAX(0,(I-1) / BLOCK_LEN) + 1
        OFFSET = I - (J-1)*BLOCK_LEN
        MASTER(I) = BLOCK_STARTS(J) + (OFFSET - 1)*VAL__NBR*ITEMSIZE
      END DO

      END


*+  TXT2AST_CRUNCH - Do main data manipulation for TXT2AST
      SUBROUTINE TXT2AST_CRUNCH( OFID, TYPE, PRIM, NAXES, N, NDIM, DIMS,
     :                AX_WOK, AX_DECR, AX_SYMWID, AX_SCALAR,
     :                QMASK_OK, CQMASK, ERROR_TO_VAR, MASTER, ORIG,
     :                NCOLS, WORK,
     :                WWORK, NAX, INDICES, TABLE, STATUS )
*
*    Description :
*
*     TXT2AST_CRUNCH inserts data,variance and quality values into output
*     arrays in positions dictated by the axis values.
*
*    Method :
*
*     IF number of axes = 0 THEN
*       Dims(1) = ( N )
*       Ndims = 1
*     ELSE
*       FOR each axis(i) DO
*         Sort all data using axis(i) values as key.
*         Count number of distinct values for axis(i). This value is the
*           size of axis(i), ie. Dims(i)
*         Set the axis(i) value for (data,variance,quality) record to the
*           corresponding distinct value.
*       Transform Dims() into 7D system for AR7 routines.
*     END IF
*     Create (data,variance,quality) components in output dataset using Dims()
*     Insert data into output arrays depending on axis values in INDICES()()
*
*    Author :
*
*     David J. Allan  (BHVAD::DJA)
*
*    History :
*
*     10 Nov 88 : Original (DJA)
*
*    Type definitions :
*
      IMPLICIT NONE
*
*    Global constants :
*
      INCLUDE 'SAE_PAR'
      INCLUDE 'ADI_PAR'
      INCLUDE 'PRM_PAR'
      INCLUDE 'QUAL_PAR'
*
*    Status :
*
      INTEGER             STATUS
*
*    Global variables :
*
      INCLUDE 'TXT2AST_CMN'
*
*    Import :
*
      INTEGER			OFID			! Output file object
      CHARACTER*(*)		TYPE			! Output object type
      LOGICAL			PRIM			! Output primitive?
      INTEGER             	NCOLS                   ! Number of items in a
							! data row
      INTEGER			NAXES			! Number of axes
      INTEGER             	N                      	! Number of ptr's in
							! master table
      INTEGER			NDIM			! Output dimensionality
      INTEGER                   DIMS(*)			! Output dimensions
      LOGICAL                   AX_WOK(*)		! Axis widths ok?
      LOGICAL                   AX_DECR(*)		! Axis decreasing?
      LOGICAL                   AX_SYMWID(*)		! Axis widths symmetric?
      LOGICAL                   AX_SCALAR(*)		! Axis width scalar
      LOGICAL			QMASK_OK		! Quality mask defined?
      CHARACTER*(*)		CQMASK			! Quality mask
      LOGICAL			ERROR_TO_VAR		! Convert error to var?
*
*    Import/Export :
*
      INTEGER             MASTER(N),ORIG(N)      ! The master table and copy
      REAL                WORK(N)                ! Axis work array
      REAL                WWORK(N,2)             ! Axis width work array
      INTEGER             NAX                    ! MAX(1,Number of dimensions)
      INTEGER             INDICES(NAX,N)         ! Indices array
      INTEGER             TABLE(N)               ! Positions array
*
*    Local variables :
*
      REAL                DVAL                   ! Two data values
      REAL                RQUAL                  ! Quality value

      INTEGER			AX_DPTR(ADI__MXDIM)	! Axis data
      INTEGER			AX_HIWPTR(ADI__MXDIM)	! Axis high widths
      INTEGER			AX_LOWPTR(ADI__MXDIM)	! Axis low widths
      INTEGER			AX_WPTR(ADI__MXDIM)	! Axis widths
      INTEGER             AX_SIZE(ADI__MXDIM)		! Values per axis
      INTEGER             I, J, K                ! Loop counter
      INTEGER             IQMASK                 ! Integer quality mask
      INTEGER             IQUAL                  ! Integer quality value
      INTEGER             INDS(ADI__MXDIM)       !
      INTEGER             NELM

      BYTE                BQUAL                  ! Quality value
      BYTE                QMASK                  ! Quality mask

      LOGICAL             SORT                   ! Sort on each axis
      LOGICAL             VECTOR_WIDTHS
*-

*  Check status
      IF ( STATUS .NE. SAI__OK ) RETURN

*  Get SORT keyword from user
      CALL USI_GET0L( 'SORT', SORT, STATUS )
      IF ( STATUS .NE. SAI__OK ) GOTO 99

      IF ( NAXES .EQ. 0 ) THEN
        NDIM = 1
        DIMS(1) = NELM
        GOTO 50
      END IF

*  Set up array of original record numbers
      CALL ARR_REG1I( 1, 1, N, TABLE, STATUS )

*  Initialise INDICES
      CALL ARR_INIT1R( 1, NAX*N, INDICES, STATUS )

*  For each axis, sort the data and count the number of distinct values.
      DO I = 1, NAXES

*    Vector widths on this axis?
        VECTOR_WIDTHS = AX_WOK(I) .AND. .NOT. AX_SCALAR(I)
        IF ( SORT ) THEN
          CALL TXT2AST_SORT( N, MASTER, TABLE, AX_DECR(I),
     :                           POS_AX_DATA(I), STATUS )
        END IF

        K = 1

        CALL ARR_COP1R( 1, %VAL(MASTER(1)+VAL__NBR*(POS_AX_DATA(I)-1)),
     :                                                WORK(1), STATUS )
        IF ( VECTOR_WIDTHS ) THEN
          IF ( AX_SYMWID(I) ) THEN
            CALL ARR_COP1R( 1, %VAL(MASTER(1)+VAL__NBR*(POS_AX_WID(I)
     :                                     -1)), WWORK(1,1), STATUS )
          ELSE
            CALL ARR_COP1R( 1, %VAL(MASTER(1)+VAL__NBR*(POS_AX_AWIDLO(I)
     :                                        -1)), WWORK(1,1), STATUS )
            CALL ARR_COP1R( 1, %VAL(MASTER(1)+VAL__NBR*(POS_AX_AWIDHI(I)
     :                                        -1)), WWORK(1,2), STATUS )
          END IF
        END IF
        INDICES( I, TABLE(1) ) = 1

        DO J = 2, N

*      Get next data value on axis
          CALL ARR_COP1R( 1, %VAL(MASTER(J)+(POS_AX_DATA(I)-1)*
     :                                VAL__NBR), DVAL, STATUS )

          IF ( ( DVAL .NE. WORK(K) ) .OR. .NOT. SORT ) THEN
            K = K + 1
            WORK(K) = DVAL
            IF ( VECTOR_WIDTHS ) THEN
              IF ( AX_SYMWID(I) ) THEN
                CALL ARR_COP1R( 1, %VAL(MASTER(J)+VAL__NBR*(
     :               POS_AX_WID(I)-1)), WWORK(K,1), STATUS )
              ELSE
                CALL ARR_COP1R( 1, %VAL(MASTER(J)+VAL__NBR*(
     :                     POS_AX_AWIDLO(I)-1)), WWORK(K,1), STATUS )
                CALL ARR_COP1R( 1, %VAL(MASTER(J)+VAL__NBR*(
     :                     POS_AX_AWIDHI(I)-1)), WWORK(K,2), STATUS )
              END IF
            END IF
          END IF

*      Set the index for the data element
          INDICES( I, TABLE(J) ) = K

        END DO

*    At this point, K is the size of axis I, and WORK(1:K) contains
*    the axis values.
        AX_SIZE(I) = K

*    Is output structured?
        IF ( .NOT. PRIM ) THEN

*      Copy axis data
          CALL DYN_MAPR( 1, AX_SIZE(I), AX_DPTR(I), STATUS )
          CALL ARR_COP1R(  AX_SIZE(I), WORK, %VAL(AX_DPTR(I)), STATUS )

*      Are vector widths wanted
          IF ( VECTOR_WIDTHS ) THEN
            IF ( AX_SYMWID(I) ) THEN
              CALL DYN_MAPR( 1, AX_SIZE(I), AX_WPTR(I), STATUS )
              CALL ARR_COP1R(  AX_SIZE(I), WWORK(1,1),
     :                             %VAL(AX_WPTR(I)), STATUS )
            ELSE
              CALL DYN_MAPR( 1, AX_SIZE(I), AX_LOWPTR(I), STATUS )
              CALL ARR_COP1R(  AX_SIZE(I), WWORK(1,1),
     :                             %VAL(AX_LOWPTR(I)), STATUS )
              CALL DYN_MAPR( 1, AX_SIZE(I), AX_HIWPTR(I), STATUS )
              CALL ARR_COP1R(  AX_SIZE(I), WWORK(1,2),
     :                             %VAL(AX_HIWPTR(I)), STATUS )
            END IF

          END IF
          IF ( STATUS .NE. SAI__OK ) GOTO 99

        END IF
        IF ( STATUS .NE. SAI__OK ) GOTO 99

      END DO

*  Create the dimensions array
      DO I = 1, NAXES
        DIMS(I) = AX_SIZE(I)
      END DO
      CALL AR7_PAD( NAXES, DIMS, STATUS )

*  Link to output object
      CALL BDI_LINK( TYPE, NAXES, DIMS, 'REAL', OFID, STATUS )
      IF ( STATUS .NE. SAI__OK ) THEN
        CALL ERR_ANNUL( STATUS )
        TYPE = 'BinDS'
        CALL BDI_LINK( TYPE, NAXES, DIMS, 'REAL', OFID, STATUS )
      END IF

*  Create axis data
      IF ( .NOT. PRIM ) THEN

        DO I = 1, NAXES

*      Create the axis array, map it and copy in the
          CALL BDI_AXPUT1R( OFID, I, 'Data', AX_SIZE(I),
     :                        %VAL(AX_DPTR(I)), STATUS )

*      Are vector widths wanted
          IF ( VECTOR_WIDTHS ) THEN
            IF ( AX_SYMWID(I) ) THEN
              CALL BDI_AXPUT1R( OFID, I, 'Width', AX_SIZE(I),
     :                          %VAL(AX_WPTR(I)), STATUS )
            ELSE
              CALL BDI_AXPUT1R( OFID, I, 'LoWidth', AX_SIZE(I),
     :                          %VAL(AX_LOWPTR(I)), STATUS )
              CALL BDI_AXPUT1R( OFID, I, 'HiWidth', AX_SIZE(I),
     :                          %VAL(AX_HIWPTR(I)), STATUS )
            END IF

          END IF

        END DO

      END IF

*  How many elements in output data array
      CALL ARR_SUMDIM( NAXES, DIMS, NELM )

*  Warn user that records will be lost in SORT mode
      IF ( SORT .AND. ( N .GT. NELM ) ) THEN
        CALL MSG_PRNT( 'WARNING : Records lost in sorting process' )
      END IF

*     At this point we now have,
*
*      o Data, variance and quality arrays created where appropriate.
*        There data has not yet been set.
*      o An array INDICES which specifies where each data row should
*        go in the aforementioned data arrays. The values in INDICES
*        are stored in the order the data was passed - this data is
*        accessed using the copy of MASTER in ORIG.

 50   CONTINUE

*  Create components with dimensions calculated
      IF ( USE_DATA ) THEN
        CALL BDI_MAPR( OFID, 'Data', 'WRITE/ZERO', PTR_DATA, STATUS )
      END IF
      IF ( USE_VAR ) THEN
        IF ( ERROR_TO_VAR ) THEN
          CALL BDI_MAPR( OFID, 'Error', 'WRITE/ZERO', PTR_VAR, STATUS )
        ELSE
          CALL BDI_MAPR( OFID, 'Variance', 'WRITE/ZERO', PTR_VAR,
     :                   STATUS )
        END IF
      END IF
      IF ( USE_GRP ) THEN
        CALL BDI_MAPR( OFID, 'Grouping', 'WRITE/ZERO', PTR_GRP,
     :                 STATUS )
      END IF

*  QUALITY descriptor present?
      IF ( USE_QUAL ) THEN

*    Write the quality byte mask
        IF ( QMASK_OK ) THEN
          CALL CHR_CTOI( CQMASK, IQMASK, STATUS )
          QMASK = IQMASK
        ELSE
          QMASK = QUAL__MASK
        END IF
        CALL BDI_PUT0UB( OFID, 'QualityMask', QMASK, STATUS )

*    Map it and initialise to mssing
        CALL BDI_MAPUB( OFID, 'Quality', 'WRITE/QMISSING', PTR_QUAL,
     :                  STATUS )

      END IF

      IF ( USE_LOERR ) THEN
        CALL BDI_MAPR( OFID, 'LoError', 'WRITE/ZERO', PTR_LOERR,
     :                 STATUS )
        CALL BDI_MAPR( OFID, 'HiError', 'WRITE/ZERO', PTR_UPERR,
     :                 STATUS )
      END IF

*  Check case where no axis data
      IF ( NAXES .EQ. 0 ) THEN

*    Loop over data
        DO I = 1, N
          IF ( USE_DATA ) THEN
            CALL ARR_COP1R( 1, %VAL(MASTER(I)+VAL__NBR*(POS_DATA-1)),
     :                        %VAL(PTR_DATA+(I-1)*VAL__NBR), STATUS )
          END IF
          IF ( USE_VAR ) THEN
            CALL ARR_COP1R( 1, %VAL(MASTER(I)+VAL__NBR*(POS_VAR-1)),
     :                        %VAL(PTR_VAR+(I-1)*VAL__NBR), STATUS )
          END IF
          IF ( USE_GRP ) THEN
            CALL ARR_COP1R( 1, %VAL(MASTER(I)+VAL__NBR*(POS_GRP-1)),
     :                        %VAL(PTR_GRP+(I-1)*VAL__NBR), STATUS )
          END IF
          IF ( USE_QUAL ) THEN

*          Convert quality to byte value
            CALL ARR_COP1R( 1, %VAL(MASTER(I)+VAL__NBR*(POS_QUAL-1)),
     :                      RQUAL, STATUS )
            IQUAL = NINT(RQUAL)
            BQUAL = IQUAL
            CALL ARR_COP1B( 1, BQUAL, %VAL(PTR_QUAL+(I-1)*VAL__NBB),
     :                      STATUS )

          END IF
          IF ( USE_LOERR ) THEN
            CALL AR7_MOV0R( %VAL(ORIG(I)+VAL__NBR*(POS_LOERR-1)),
     :            INDS, DIMS, %VAL(PTR_LOERR), STATUS )
            CALL AR7_MOV0R( %VAL(ORIG(I)+VAL__NBR*(POS_UPERR-1)),
     :            INDS, DIMS, %VAL(PTR_UPERR), STATUS )
          END IF


        END DO

      ELSE

*    Initialise unused bits of INDS
        CALL AR7_PAD( NDIM, INDS, STATUS )

*    Go through all the data inserting it into the appropriate arrays.
        DO I = 1, N

*      Set up data element indices
          DO J = 1, NDIM
            INDS(J) = INDICES(J,I)
          END DO

*      DATA if present
          IF ( USE_DATA ) THEN
            CALL AR7_MOV0R( %VAL(ORIG(I)+VAL__NBR*(POS_DATA-1)),
     :            INDS, DIMS, %VAL(PTR_DATA), STATUS )
          END IF

*      GROUP if present
          IF ( USE_GRP ) THEN
            CALL AR7_MOV0R( %VAL(ORIG(I)+VAL__NBR*(POS_GRP-1)),
     :            INDS, DIMS, %VAL(PTR_GRP), STATUS )
          END IF

*      VARIANCE if present
          IF ( USE_VAR ) THEN
            CALL AR7_MOV0R( %VAL(ORIG(I)+VAL__NBR*(POS_VAR-1)),
     :            INDS, DIMS, %VAL(PTR_VAR), STATUS )
          END IF

*      QUALITY if present
          IF ( USE_QUAL ) THEN

*        Convert quality to byte value
            CALL ARR_COP1R( 1, %VAL(ORIG(I)+VAL__NBR*(POS_QUAL-1)),
     :                      RQUAL, STATUS )
            BQUAL = NINT(RQUAL)
            CALL AR7_MOV0B( BQUAL, INDS, DIMS, %VAL(PTR_QUAL), STATUS )

          END IF

*      Asymmetric data errors
          IF ( USE_LOERR ) THEN
            CALL AR7_MOV0R( %VAL(ORIG(I)+VAL__NBR*(POS_LOERR-1)),
     :            INDS, DIMS, %VAL(PTR_LOERR), STATUS )
            CALL AR7_MOV0R( %VAL(ORIG(I)+VAL__NBR*(POS_UPERR-1)),
     :            INDS, DIMS, %VAL(PTR_UPERR), STATUS )
          END IF

        END DO

      END IF

 99   IF ( STATUS .NE. SAI__OK ) THEN
        CALL AST_REXIT( 'TXT2AST_CRUNCH', STATUS )
      END IF

      END


*+  TXT2AST_SORT - Sort a 1D list of pointers to data rows by a given column
      SUBROUTINE TXT2AST_SORT( N, MASTER, AUXIL, DECREASING,
     :                                     COLUMN, STATUS )
*
*    Description :
*
*     Sorts a table of pointers ( usually the calling program passes a subset
*     of the whole master table ) using the data in the COLUMN'th column of
*     the data row pointed to by the pointers. The algorithm used is a shell
*     sort. In addition, the values of the AUXIL array are swapped.
*
*     NB : No data is moved - only pointers are moved about.
*
*    Method :
*
*     A standard shell sort extended to cope with indirection.
*
*    Author :
*
*     David J. Allan  (BHVAD::DJA)
*
*    History :
*
*     10 Nov 88 : Original (DJA)
*      3 Mar 92 : Added DECREASING argument (DJA)
*
*    Type definitions :
*
      IMPLICIT NONE
*
*    Global constants :
*
      INCLUDE 'SAE_PAR'
      INCLUDE 'PRM_PAR'
*
*    Status :
*
      INTEGER             STATUS
*
*    Import :
*
      INTEGER             N                      ! Number of ptr's in master
      INTEGER             COLUMN                 ! Column number to sort on
      LOGICAL             DECREASING             ! Sort into decreasing order
*
*    Import/Export :
*
      INTEGER             MASTER(N)              ! The master table
      INTEGER             AUXIL(N)               ! Other array to be sorted
*
*    Local variables :
*
      REAL                KEY                    ! Value to compare
      REAL                DVAL                   ! Other value to compare

      INTEGER             H                      ! Sort granularity
      INTEGER             I,S,J                  ! Loop counters
      INTEGER             OFFSET                 ! Offset in bytes of column
      INTEGER             T                      ! Number of iterations of sort
      INTEGER             KEYPTR                 ! Pointer to KEY's record
      INTEGER             KEYAUX                 ! AUXIL value for KEY

      LOGICAL             LOOP
*-

      IF ( STATUS .NE. SAI__OK ) RETURN

*    Calculate offset in bytes into each data row
      OFFSET = ( COLUMN - 1 ) * VAL__NBR

*    Starting value of T is largest power of 2 less than the number of
*    elements.
      T = NINT(LOG(REAL(N))/LOG(2.0))
      IF ((2**T).GT.N) T=T-1
      H = 2**(T+1)-1

*    Sort the data. We compare KEY but we sort the pointers
      DO S = T, 1, -1
        H = ( H + 1 )/2-1
        DO J = H+1, N
          CALL ARR_COP1R( 1, %VAL(MASTER(J)+OFFSET), KEY, STATUS )
          KEYPTR = MASTER(J)
          KEYAUX = AUXIL(J)
          I = J - H

          LOOP = .TRUE.
          DO WHILE ( (I.GT.0) .AND. LOOP )
            CALL ARR_COP1R( 1, %VAL(MASTER(I)+OFFSET), DVAL, STATUS )
            IF ( DECREASING ) THEN
              IF ( DVAL .LT. KEY ) THEN
                MASTER(I+H) = MASTER(I)
                AUXIL(I+H) = AUXIL(I)
                I = I - H
              ELSE
                LOOP = .FALSE.
              END IF
            ELSE
              IF ( DVAL .GT. KEY ) THEN
                MASTER(I+H) = MASTER(I)
                AUXIL(I+H) = AUXIL(I)
                I = I - H
              ELSE
                LOOP = .FALSE.
              END IF
            END IF

          END DO

          MASTER(I+H) = KEYPTR
          AUXIL(I+H) = KEYAUX

        END DO

      END DO

      END
