*+  REBIN - ND data rebinning
      SUBROUTINE REBIN(STATUS)
*    Description :
*     Rebins an ND dataset into regularly or irregularly spaced bins
*    Parameters :
*     INP=UNIV(R)	input dataset
*     OUT=UNIV(W)	output dataset
*    Method :
*    Deficiencies :
*
*     When VARIANCEs are rebinned they are no longer independent of the VARIANCE
*     in adjacent bins. If a rebinned dataset is rebinned again the effect of
*     this is artificially to lower the VARIANCEs.
*    Bugs :
*    Authors :
*     Jim Peden (BHVAD::JCMP)
*     Alan McFadzean (BHVAD::ADM)
*     Bob Vallance (BHVAD::RJV)
*    History :
*      3 Sep 84 : Original (BHVAD::JCMP)
*     12 Dec 84 : Version announcement; normalisation added (BHVAD::JCMP)
*     18 Dec 84 : Strange bugs encountered - REB_MAVAL code (BHVAD::JCMP)
*     16 Jan 85 : Bug fix to 18 Dec 84 code (BHVAD::JCMP)
*     26 Jan 85 : Minor change to history (BHVAD::JCMP)
*     28 Mar 85 : V0.3-1 Normalisation selection added (BHVAD::JCMP)
*     27 Jan 86 : V0.4-1 ADAM version (BHVAD::JCMP)
*      3 Feb 86 : V0.4-2 Allow i/p obj's with only DATA_ARRAY,AXIS1_DATA (JCMP)
*     14 Aug 86 : V0.5-1 Dummy durations set up; also REB_SQUAL added (JCMP)
*     24 Sep 86 : V0.5-2 Changes to REB_GEN1, etc. (JKD)
*      2 Oct 86 : V0.5-3 Changes in receptor boundaries etc in subroutines (jkd)
*                 V0.5-4 2D version (BHVAD::JKD)
*     23 Jun 87 : V0.6-1 Changes to permit full rebinning even if no
*                        AXIS<n>_VARIANCE data (BHVAD:ADM)
*     29 Jul 87 : V0.6-2 Option of not fully normalising receptor bins
*                        (BHVAD::ADM)
*     26 Aug 87 : V0.6-3 Allows irregular output bins in 2D case (BHVAD::ADM)
*     15 Jun 88 : V0.6-4 ASTERIX88 ND rebinning
*     16 Aug 88 : V0.1-1 Projections
*     15 Jun 89 : V1.0-2 Bug with regular axes - was setting up bin boundaries
*                        unnecessarily. (BHVAD::DJA)
*        Nov 89 : V1.1-0 Major overhaul (BHVAD::RJV)
*      8 Feb 90 : V1.1-1 Change to QUALITY handling (RJV)
*     18 Jun 90 : V1.2-0 Bug locating high bin bound on irregular axes. Cludge
*                        to fix rounding problem inserted (DJA)
*     10 Jun 91 : V1.5-0 Bug in receptor bin output fixed (DJA)
*        Mar 92 : V1.5-1 new faster mode - projection removed (RJV)
*        Dec 92 : V1.5-2 new option to use axes from another file (RJV)
*      1 jun 93 : V1.5-3 selection of axes to rebin (RJV)
*     30 Jun 94 : V1.5-4 quality problem fixed (RJV)
*      9 Sep 94 : V1.5-5 another quality problem fixed (RJV)
*     24 Nov 94 : V1.8-0 Now use USI for user interface (DJA)
*     12 Dec 94 : V1.8-1 Uses axis bounds rather than widths (RJV)
*
*    Type Definitions :
      IMPLICIT NONE
*    Global constants :
      INCLUDE 'SAE_PAR'
      INCLUDE 'DAT_PAR'
      INCLUDE 'PAR_PAR'
      INCLUDE 'PAR_ERR'
*    Status :
      INTEGER STATUS
*    Global variables :
*    Functions :
      INTEGER UTIL_PLOC
*    Local Constants :
      INTEGER MXBIN           	! maximum # of irreg. spaced bins
         PARAMETER (MXBIN=500)

*    Local variables :
      CHARACTER*(DAT__SZLOC) ILOC       ! input dataset locator
      CHARACTER*(DAT__SZLOC) OLOC       ! output dataset locator
      CHARACTER*(DAT__SZLOC) CLOC       ! clone dataset locator
      CHARACTER*(DAT__SZTYP) TYPE       ! input dataset type
      CHARACTER*80           ITEXT(4)   ! name of input file
      CHARACTER*80           OTEXT(4)   ! name of output file
      CHARACTER*80           AXLABEL(7) ! AXIS labels

      LOGICAL OK			! data valid
      LOGICAL VOK			! Data VARIANCE present
      LOGICAL QOK                     ! data quality present
      LOGICAL AOK(DAT__MXDIM)         ! axes data present
      LOGICAL AXOK(DAT__MXDIM)        ! axis(n) ok?
      LOGICAL WDOK(DAT__MXDIM)        ! axis(n).width ok?
      LOGICAL NORM(DAT__MXDIM)        ! data normalised rel. to axis(n)?
      LOGICAL REG(DAT__MXDIM)	      ! regularly spaced input data
      LOGICAL REGO(DAT__MXDIM)        ! regularly spaced output data
      LOGICAL UNIF(DAT__MXDIM)        ! uniform input axis(n).width
      LOGICAL UNIFO(DAT__MXDIM)       ! uniform output axis(n).width
      LOGICAL INPRIM                  ! Primitive input?
      LOGICAL CPRIM
      LOGICAL EXCLUDE
      LOGICAL SEL(DAT__MXDIM)

      INTEGER OPT
      INTEGER NAXVDIM(DAT__MXDIM)     ! # axis values (should=dims)
      INTEGER NAXWDIM(DAT__MXDIM)     ! # axis widths (should=dims)
      INTEGER DIMS(DAT__MXDIM)	! data_array dimensions
      INTEGER NDATVAR(DAT__MXDIM)	! data_VARIANCE   ..    (should equal dims)
      INTEGER NDATQUAL(DAT__MXDIM)    ! data_quality ..       ..    ..    ..
      INTEGER NDIM			! # dimensions
      INTEGER NDIMQ			! # dimensions QUALITY
      INTEGER NDIMV			! # dimensions data VARIANCE
      INTEGER ODIM(DAT__MXDIM)      ! # output axis values (7-D)
      INTEGER RAT(DAT__MXDIM)           ! rebinning ratio
*   Pointers...input
      INTEGER DPTR	              ! data
      INTEGER QPTR	              ! quality
      INTEGER VPTR	              ! data VARIANCE
      INTEGER AXVP(DAT__MXDIM)        ! axes
      INTEGER AXWP(DAT__MXDIM)        ! axes widths
      INTEGER AXBP(DAT__MXDIM)	      ! axes bounds

*   ...and output
      INTEGER DPTRO	              ! data
      INTEGER QPTRO	              ! quality
      INTEGER VPTRO	              ! data VARIANCE
      INTEGER AXVPO(DAT__MXDIM)       ! axes
      INTEGER AXWPO(DAT__MXDIM)       ! axes widths
      INTEGER AXES(DAT__MXDIM)
      INTEGER ISEL,NSEL

      INTEGER I

      BYTE MASK                      	! QUALITY mask

      INTEGER INLINES                	! # lines input name
      INTEGER ONLINES                	! # line output name
      INTEGER NAX
      REAL WID				! scalar bin width
      REAL WIDLOW,WIDUPP		! upper and lower bin widths
      REAL RLOW				! donor bin lower range
      REAL RUPP				! donor bin upper range
      REAL DIR(DAT__MXDIM)		! axis direction indicator (input)
      REAL BASEO,SCALEO
      REAL BNDS(2,MXBIN,DAT__MXDIM)	! required boundaries (irregular)
      REAL DUMAXVAL(DAT__MXDIM)		! dummy axis values for coerced dims
      REAL DUMAXWID(DAT__MXDIM)		! dummy axis widths
      REAL DUMAXBND(2,DAT__MXDIM)	! dummy axis bounds
*
*    Version id :
*
      CHARACTER*20 VERSION
         PARAMETER (VERSION='REBIN Version 1.8-1')
*-
      CALL MSG_PRNT (VERSION)

      CALL AST_INIT()

* Associate input dataset
      CALL USI_ASSOCI ('INP','READ',ILOC,INPRIM,STATUS)
      CALL USI_NAMEI(INLINES,ITEXT,STATUS)

* Check input dataset
      CALL BDA_CHKDATA(ILOC,OK,NDIM,DIMS,STATUS)
      IF ( STATUS .NE. SAI__OK ) GOTO 9000

      IF ( .NOT. OK ) THEN
        CALL MSG_PRNT('AST_ERR: invalid input data')
        STATUS=SAI__ERROR
      ENDIF

* Obtain type of dataset
      CALL DAT_TYPE(ILOC,TYPE,STATUS)

* create output  dataset
      CALL USI_ASSOCO('OUT',TYPE,OLOC,STATUS)

      IF (STATUS .NE. SAI__OK) GOTO 9000

* get option number
      CALL USI_GET0I('OPT',OPT,STATUS)

* get axes to rebin
      IF (NDIM.GT.1) THEN
        CALL PRS_GETLIST('AXES',NDIM,AXES,NSEL,STATUS)
      ELSE
        NSEL=1
        AXES(1)=1
      ENDIF

      IF (OPT.EQ.5) THEN
        CALL USI_ASSOCI('CLONE','READ',CLOC,CPRIM,STATUS)
        CALL BDA_CHKAXES(CLOC,NAX,STATUS)
        IF (NAX.EQ.NDIM.OR.(NAX.EQ.1.AND.NSEL.EQ.1)) THEN

        ELSE
          CALL MSG_PRNT('AST_ERR: clone has wrong - '//
     :                          'dimensionality ')
          STATUS=SAI__ERROR
        ENDIF
      ENDIF

* Map bin attributes
      DO I=1,NDIM

        CALL BDA_CHKAXIS(ILOC,I,AOK(I),STATUS)

* axis values
        CALL BDA_CHKAXVAL(ILOC,I,AXOK(I),REG(I),NAXVDIM(I),STATUS)
        CALL BDA_GETAXLABEL(ILOC,I,AXLABEL(I),STATUS)
        CALL BDA_MAPAXVAL(ILOC,'READ',I,AXVP(I),STATUS)

* axis bounds
        CALL BDA_MAPAXBNDS(ILOC,'READ',I,AXBP(I),STATUS)

* axis widths
        CALL BDA_CHKAXWID(ILOC,I,WDOK(I),UNIF(I),NAXWDIM(I),STATUS)
        CALL BDA_MAPAXWID(ILOC,'READ',I,AXWP(I),STATUS)

* check if donor bins normalised
        CALL BDA_GETAXNORM(ILOC,I,NORM(I),STATUS)

      ENDDO

* set up dummy axes for coerced data (coerced to 7-D)
      DO I=NDIM+1,7
        AXVP(I)=UTIL_PLOC(DUMAXVAL(I))
        AXWP(I)=UTIL_PLOC(DUMAXWID(I))
        AXWPO(I)=UTIL_PLOC(DUMAXWID(I))
        AXBP(I)=UTIL_PLOC(DUMAXBND(1,I))
        DUMAXVAL(I)=0.5
        DUMAXWID(I)=1.0
        DUMAXBND(1,I)=0.0
        DUMAXBND(2,I)=1.0
        DIMS(I)=1
        NORM(I)=.FALSE.
        DIR(I)=1.0
      ENDDO

* set option code for each axis
      DO I=1,NDIM
        EXCLUDE=.TRUE.
        DO ISEL=1,NSEL
          IF (AXES(ISEL).EQ.I) THEN
            EXCLUDE=.FALSE.
          ENDIF
        ENDDO
        IF (EXCLUDE) THEN
          SEL(I)=.FALSE.
        ELSE
          SEL(I)=.TRUE.
        ENDIF
      ENDDO
      DO I=NDIM+1,7
        SEL(I)=.TRUE.
      ENDDO

* map QUALITY if present
      CALL BDA_CHKQUAL(ILOC,QOK,NDIMQ,NDATQUAL,STATUS)
      IF (QOK) THEN
        CALL BDA_MAPQUAL(ILOC,'READ',QPTR,STATUS)
      ENDIF

* map VARIANCE if present
      CALL BDA_CHKVAR(ILOC,VOK,NDIMV,NDATVAR,STATUS)
      IF(VOK) THEN
        CALL BDA_MAPVAR(ILOC,'READ',VPTR,STATUS)
      ENDIF

* map DATA
      CALL BDA_MAPDATA(ILOC,'READ',DPTR,STATUS)

* initialise bounds etc for output
      DO I=1,7
        RAT(I)=1
        BNDS(1,1,I)=0.0
        BNDS(2,1,I)=1.0
        REGO(I)=.TRUE.
        UNIFO(I)=.TRUE.
        ODIM(I)=1
      ENDDO

* Find axes ranges
      DO I=1,NDIM

        CALL ARR_ELEM1R(AXVP(I),DIMS(I),1,RLOW,STATUS)
        CALL ARR_ELEM1R(AXVP(I),DIMS(I),DIMS(I),RUPP,STATUS)

* set axis direction indicator
        IF (RUPP.GE.RLOW) THEN
          DIR(I)=1.0
        ELSE
          DIR(I)=-1.0
        ENDIF

* adjust range taking bin-width into account
        IF (REG(I)) THEN
          CALL ARR_ELEM1R(AXWP(I),DIMS(I),1,WID,STATUS)
          RLOW=RLOW-DIR(I)*WID/2.0
          RUPP=RUPP+DIR(I)*WID/2.0

* Trap case of RLOW and RUPP near integral values
          IF ( ABS(ABS(RLOW)-NINT(ABS(RLOW))) .LT. 1.0E-6 ) THEN
             RLOW = SIGN(REAL(NINT(RLOW)),RLOW)
          END IF
          IF ( ABS(ABS(RUPP)-NINT(ABS(RUPP))) .LT. 1.0E-6 ) THEN
             RUPP = SIGN(REAL(NINT(RUPP)),RUPP)
          END IF

        ELSE
* get lower and upper bin-widths
          CALL ARR_ELEM1R(AXWP(I),DIMS(I),1,WIDLOW,STATUS)
          CALL ARR_ELEM1R(AXWP(I),DIMS(I),DIMS(I),WIDUPP,STATUS)
          WID=0.0
          RLOW=RLOW-DIR(I)*WIDLOW/2.0
          RUPP=RUPP+DIR(I)*WIDUPP/2.0
        ENDIF

* Obtain binning specs.
        CALL REBIN_GSPEC(SEL(I),AXLABEL(I),I,DIMS(I),MXBIN,RLOW,RUPP,
     :                    %VAL(AXWP(I)),REG(I),DIR(I),OPT,CLOC,RAT(I),
     :                     REGO(I),NORM(I),BNDS(1,1,I),ODIM(I),STATUS)
        IF ( STATUS .NE. SAI__OK ) GOTO 9000
      ENDDO

*  set up coerced dimensions in output
      DO I = NDIM+1, 7
        ODIM(I)=DIMS(I)
      ENDDO


* create output data components
      IF (STATUS.EQ.SAI__OK) THEN
        CALL MSG_PRNT('Creating output...')
      ENDIF
* textual lables
      CALL BDA_COPTEXT(ILOC,OLOC,STATUS)

* DATA
      CALL BDA_CREDATA(OLOC,NDIM,ODIM,STATUS)
      CALL BDA_MAPDATA(OLOC,'WRITE',DPTRO,STATUS)
* QUALITY
      CALL BDA_CREQUAL(OLOC,NDIM,ODIM,STATUS)
      CALL BDA_MAPQUAL(OLOC,'WRITE',QPTRO,STATUS)
      IF (QOK) THEN
        CALL BDA_GETMASK(ILOC,MASK,STATUS)
        CALL BDA_PUTMASK(OLOC,MASK,STATUS)
      ENDIF
* VARIANCE if required
      IF(VOK) THEN
        CALL BDA_CREVAR(OLOC,NDIM,ODIM,STATUS)
        CALL BDA_MAPVAR(OLOC,'WRITE',VPTRO,STATUS)
      ENDIF

      CALL BDA_CREAXES(OLOC,NDIM,STATUS)
      DO I=1,NDIM
        IF (SEL(I)) THEN
* copy textual lables from equivalent input axis
          CALL BDA_COPAXTEXT(ILOC,OLOC,I,I,STATUS)

* write normalisation flag
          CALL BDA_PUTAXNORM(OLOC,I,NORM(I),STATUS)

* axis values and width
          CALL BDA_CREAXVAL(OLOC,I,REGO(I),ODIM(I),STATUS)
          CALL BDA_CREAXWID(OLOC,I,REGO(I),ODIM(I),STATUS)
* put in values
          IF (REGO(I)) THEN
            BASEO=(BNDS(1,1,I)+BNDS(2,1,I))/2.0
            SCALEO=BNDS(2,1,I)-BNDS(1,1,I)
            CALL BDA_PUTAXVAL(OLOC,I,BASEO,SCALEO,ODIM(I),STATUS)
            CALL BDA_PUTAXWID(OLOC,I,ABS(SCALEO),STATUS)

          ELSE
            CALL BDA_MAPAXVAL(OLOC,'WRITE',I,AXVPO(I),STATUS)
            CALL BDA_MAPAXWID(OLOC,'WRITE',I,AXWPO(I),STATUS)
            CALL REBIN_WRTAXIS(OPT,MXBIN,BNDS(1,1,I),ODIM(I),
     :                      %VAL(AXVP(I)),%VAL(AXWP(I)),DIR(I),RAT(I),
     :                           %VAL(AXVPO(I)),%VAL(AXWPO(I)),STATUS)

          ENDIF

*  not rebinning so copy input axis
        ELSE
          CALL BDA_COPAXIS(ILOC,OLOC,I,I,STATUS)

        ENDIF

      ENDDO



      IF (STATUS.EQ.SAI__OK) THEN
        CALL MSG_PRNT('Rebinning...')
      ENDIF

* Perform rebinning
      IF (OPT.EQ.1) THEN
        CALL REBIN_DOIT_BYRATIO(
     :    DIMS(1),DIMS(2),DIMS(3),DIMS(4),DIMS(5),DIMS(6),DIMS(7),
     :    ODIM(1),ODIM(2),ODIM(3),ODIM(4),ODIM(5),ODIM(6),ODIM(7),
     :    %VAL(AXWP(1)),%VAL(AXWP(2)),%VAL(AXWP(3)),%VAL(AXWP(4)),
     :    %VAL(AXWP(5)),%VAL(AXWP(6)),%VAL(AXWP(7)),RAT,NORM,VOK,QOK,
     :    %VAL(DPTR),%VAL(VPTR),%VAL(QPTR),MASK,%VAL(DPTRO),
     :               %VAL(VPTRO),%VAL(QPTRO),STATUS)
      ELSE
        CALL REBIN_DOIT_BYBOUNDS(NDIM,DIMS,
     :    %VAL(DPTR),VOK,%VAL(VPTR),QOK,MASK,%VAL(QPTR),
     :    %VAL(AXBP(1)),%VAL(AXBP(2)),%VAL(AXBP(3)),%VAL(AXBP(4)),
     :    %VAL(AXBP(5)),%VAL(AXBP(6)),%VAL(AXBP(7)),DIR,NORM,
     :    ODIM,BNDS,REGO,SEL,%VAL(DPTRO),%VAL(VPTRO),%VAL(QPTRO),
     :                                                     STATUS)

      ENDIF
      IF (STATUS.EQ.SAI__OK) THEN
        CALL MSG_PRNT('Done!')
      ENDIF

* Copy and update history
      CALL HIST_COPY(ILOC,OLOC,STATUS)
      CALL HIST_ADD(OLOC,VERSION,STATUS)
      CALL HIST_PTXT(OLOC,INLINES,ITEXT,STATUS)
      CALL USI_NAMEO(ONLINES,OTEXT,STATUS)
      CALL HIST_PTXT(OLOC,ONLINES,OTEXT,STATUS)

* Copy over ancillary components
      CALL BDA_COPMORE(ILOC,OLOC,STATUS)

* Tidy up
9000  CONTINUE

      CALL BDA_RELEASE(OLOC,STATUS)
      CALL USI_ANNUL(OLOC,STATUS)
      CALL BDA_RELEASE(ILOC,STATUS)
      CALL USI_ANNUL(ILOC,STATUS)


      CALL AST_CLOSE()
      CALL AST_ERR(STATUS)

      END




      SUBROUTINE REBIN_WRTAXIS(OPT,MXBINS,BOUNDS,N,IVAL,IWID,DIR,RAT,
     :                                             OVAL,OWID,STATUS)
*    Description :
*         Initialise
*    Bugs :
*    Authors :
*    History :
*    Type Definitions :
      IMPLICIT NONE
*    Global constants :
      INCLUDE 'SAE_PAR'
      INCLUDE 'DAT_PAR'
*    Import :
      INTEGER OPT
      INTEGER MXBINS
      REAL BOUNDS(2,MXBINS)
      INTEGER N
      INTEGER RAT
      REAL IVAL(*)
      REAL IWID(*)
      REAL DIR
*    Import-Export :
*    Export :
      REAL OVAL(*)
      REAL OWID(*)
*    Status :
      INTEGER STATUS
*    Local Constants :
*    Local variables :
      REAL LOW,UPP
      INTEGER I,J
*-

      IF (STATUS.EQ.SAI__OK) THEN
        IF (OPT.EQ.1) THEN
*  group bins together according to rebinning ratio
          J=0
          DO I=1,N
            J=J+1
            LOW=IVAL(J)-IWID(J)*DIR/2.0
            J=J+RAT-1
            UPP=IVAL(J)+IWID(J)*DIR/2.0
            OVAL(I)=(LOW+UPP)/2.0
            OWID(I)=ABS(UPP-LOW)
          ENDDO

        ELSE
          DO I=1,N
*  calculate axis centre value and width from bounds
            OVAL(I)=(BOUNDS(1,I)+BOUNDS(2,I))/2.0
            OWID(I)=ABS(BOUNDS(2,I)-BOUNDS(1,I))
          ENDDO
        ENDIF


      ENDIF

      END



      SUBROUTINE REBIN_DOIT_BYRATIO(ID1,ID2,ID3,ID4,ID5,ID6,ID7,
     :                              OD1,OD2,OD3,OD4,OD5,OD6,OD7,
     :                              IWID1,IWID2,IWID3,IWID4,IWID5,
     :                              IWID6,IWID7,
     :                              RAT,NORM,VOK,QOK,DATI,VARI,QI,
     :                                    MASK,DATO,VARO,QO,STATUS)
*    Description :
*    Authors :
*        BHAVD::RJV
*    Type definitions :
      IMPLICIT NONE
*    Global constants :
      INCLUDE 'SAE_PAR'
      INCLUDE 'DAT_PAR'
      INCLUDE 'QUAL_PAR'
*    Import :
      INTEGER ID1,ID2,ID3,ID4,ID5,ID6,ID7
      INTEGER OD1,OD2,OD3,OD4,OD5,OD6,OD7
      REAL IWID1(*),IWID2(*),IWID3(*),IWID4(*),IWID5(*),IWID6(*),
     :     IWID7(*)
      INTEGER RAT(7)
      LOGICAL NORM(7)
      LOGICAL VOK,QOK
      REAL DATI(*),VARI(*)
      BYTE QI(*),MASK

*    Import-Export :
*    Export :
      REAL DATO(*),VARO(*)
      BYTE QO(*)
*    Status :
      INTEGER STATUS
*    Function declarations :
      BYTE BIT_ANDUB,BIT_ORUB
*    Local constants :
*    Local variables :
      REAL OWID1,OWID2,OWID3,OWID4,OWID5,OWID6,OWID7
      REAL VAL,VAR
      REAL OVAL,OVAR
      REAL WT,RENORM
      BYTE Q
      BYTE OQUAL
      INTEGER DIMS(7)
      INTEGER I1,I2,I3,I4,I5,I6,I7
      INTEGER I(7)
      EQUIVALENCE (I1,I(1)),(I2,I(2)),(I3,I(3)),(I4,I(4)),
     :                      (I5,I(5)),(I6,I(6)),(I7,I(7))
      INTEGER J1,J2,J3,J4,J5,J6,J7
      INTEGER J(7),JJ(7)
      EQUIVALENCE (J1,J(1)),(J2,J(2)),(J3,J(3)),(J4,J(4)),
     :                      (J5,J(5)),(J6,J(6)),(J7,J(7))
      LOGICAL SOMEGOOD,NORMALISED
*-

      IF (STATUS.EQ.SAI__OK) THEN

*  set flag for any normalisation
        NORMALISED=(NORM(1).OR.NORM(2).OR.NORM(3).OR.NORM(4)
     :                     .OR.NORM(5).OR.NORM(6).OR.NORM(7))

*  initialise output arrays
        DIMS(1)=OD1
        DIMS(2)=OD2
        DIMS(3)=OD3
        DIMS(4)=OD4
        DIMS(5)=OD5
        DIMS(6)=OD6
        DIMS(7)=OD7
        CALL AR7_INITR(0.0,DIMS,DATO,STATUS)
        CALL AR7_INITB(QUAL__GOOD,DIMS,QO,STATUS)
        IF (VOK) THEN
          CALL AR7_INITR(0.0,DIMS,VARO,STATUS)
        ENDIF

*  loop through output bins
        DO I7=1,OD7
          DO I6=1,OD6
            DO I5=1,OD5
              DO I4=1,OD4
                DO I3=1,OD3
                  DO I2=1,OD2
                    DO I1=1,OD1

                      OVAL=0.0
                      OVAR=0.0
                      OQUAL=QUAL__GOOD
                      SOMEGOOD=.FALSE.
                      OWID1=0.0
                      OWID2=0.0
                      OWID3=0.0
                      OWID4=0.0
                      OWID5=0.0
                      OWID6=0.0
                      OWID7=0.0
                      IF (NORMALISED) THEN
                        RENORM=0.0
                      ELSE
                        RENORM=1.0
                      ENDIF

*  loop through input bins donating to each output bin
                      DO J7=1,RAT(7)
                        JJ(7)=(I7-1)*RAT(7)+J7
                        OWID7=OWID7+IWID7(JJ(7))
                        DO J6=1,RAT(6)
                          JJ(6)=(I6-1)*RAT(6)+J6
                          OWID6=OWID6+IWID6(JJ(6))
                          DO J5=1,RAT(5)
                            JJ(5)=(I5-1)*RAT(5)+J5
                            OWID5=OWID5+IWID5(JJ(5))
                            DO J4=1,RAT(4)
                              JJ(4)=(I4-1)*RAT(4)+J4
                              OWID4=OWID4+IWID4(JJ(4))
                              DO J3=1,RAT(3)
                                JJ(3)=(I3-1)*RAT(3)+J3
                                OWID3=OWID3+IWID3(JJ(3))
                                DO J2=1,RAT(2)
                                  JJ(2)=(I2-1)*RAT(2)+J2
                                  OWID2=OWID2+IWID2(JJ(2))
                                  DO J1=1,RAT(1)
                                    JJ(1)=(I1-1)*RAT(1)+J1
                                    OWID1=OWID1+IWID1(JJ(1))

*  get weighting factor for normalisation
                                    WT=1.0
                                    IF (NORM(1)) THEN
                                      WT=WT*IWID1(JJ(1))
                                    ENDIF
                                    IF (NORM(2)) THEN
                                      WT=WT*IWID2(JJ(2))
                                    ENDIF
                                    IF (NORM(3)) THEN
                                      WT=WT*IWID3(JJ(3))
                                    ENDIF
                                    IF (NORM(4)) THEN
                                      WT=WT*IWID4(JJ(4))
                                    ENDIF
                                    IF (NORM(5)) THEN
                                      WT=WT*IWID5(JJ(5))
                                    ENDIF
                                    IF (NORM(6)) THEN
                                      WT=WT*IWID6(JJ(6))
                                    ENDIF
                                    IF (NORM(7)) THEN
                                      WT=WT*IWID7(JJ(7))
                                    ENDIF

                                    IF (VOK.AND.QOK) THEN

                                     CALL REBIN_DOIT_GETDVQ(ID1,ID2,ID3,
     :                                                  ID4,ID5,ID6,ID7,
     :                                                  JJ,DATI,VARI,QI,
     :                                                        VAL,VAR,Q,
     :                                                           STATUS)

                                     IF (BIT_ANDUB(Q,MASK)
     :                                        .EQ.QUAL__GOOD) THEN
                                       SOMEGOOD=.TRUE.
                                       OVAL=OVAL+VAL*WT
                                       OVAR=OVAR+VAR*WT**2
                                       IF (NORMALISED) THEN
                                         RENORM=RENORM+WT
                                       ENDIF
                                     ELSE
                                       OQUAL=BIT_ORUB(OQUAL,Q)
                                     ENDIF

                                    ELSEIF (VOK) THEN

                                     CALL REBIN_DOIT_GETDV(ID1,ID2,ID3,
     :                                                 ID4,ID5,ID6,ID7,
     :                                                    JJ,DATI,VARI,
     :                                                         VAL,VAR,
     :                                                          STATUS)

                                     OVAL=OVAL+VAL*WT
                                     OVAR=OVAR+VAR*WT**2
                                     IF (NORMALISED) THEN
                                       RENORM=RENORM+WT
                                     ENDIF


                                    ELSEIF (QOK) THEN

                                     CALL REBIN_DOIT_GETDQ(ID1,ID2,ID3,
     :                                                  ID4,ID5,ID6,ID7,
     :                                                       JJ,DATI,QI,
     :                                                            VAL,Q,
     :                                                           STATUS)

                                     IF (BIT_ANDUB(Q,MASK)
     :                                         .EQ.QUAL__GOOD) THEN
                                       OVAL=OVAL+VAL*WT
                                       SOMEGOOD=.TRUE.
                                       IF (NORMALISED) THEN
                                         RENORM=RENORM+WT
                                       ENDIF
                                     ELSE
                                       OQUAL=BIT_ORUB(OQUAL,Q)
                                     ENDIF


                                    ELSE

                                      CALL REBIN_DOIT_GETD(ID1,ID2,ID3,
     :                                                  ID4,ID5,ID6,ID7,
     :                                                      JJ,DATI,VAL,
     :                                                           STATUS)

                                      OVAL=OVAL+VAL*WT
                                      IF (NORMALISED) THEN
                                        RENORM=RENORM+WT
                                      ENDIF

                                    ENDIF

                                  ENDDO
                                ENDDO
                              ENDDO
                            ENDDO
                          ENDDO
                        ENDDO
                      ENDDO

*  renormalise and output
                      IF (RENORM.EQ.0.0) THEN
                        RENORM=1.0
                      ENDIF

                      IF (VOK.AND.QOK) THEN

                        OVAL=OVAL/RENORM
                        OVAR=OVAR/RENORM**2
                        IF (SOMEGOOD) THEN
                          OQUAL=QUAL__GOOD
                        ENDIF
                        CALL REBIN_DOIT_PUTVQ(
     :                                   OD1,OD2,OD3,OD4,OD5,OD6,OD7,
     :                                   I,OVAL,OVAR,OQUAL,DATO,VARO,QO,
     :                                                          STATUS)
                      ELSEIF (VOK) THEN

                        OVAL=OVAL/RENORM
                        OVAR=OVAR/RENORM**2
                        CALL REBIN_DOIT_PUTV(
     :                                    OD1,OD2,OD3,OD4,OD5,OD6,OD7,
     :                                    I,OVAL,OVAR,DATO,VARO,STATUS)

                      ELSEIF (QOK) THEN

                        OVAL=OVAL/RENORM
                        IF (SOMEGOOD) THEN
                          OQUAL=QUAL__GOOD
                        ENDIF
                        CALL REBIN_DOIT_PUTQ(
     :                                    OD1,OD2,OD3,OD4,OD5,OD6,OD7,
     :                                    I,OVAL,OQUAL,DATO,QO,STATUS)

                      ELSE

                        OVAL=OVAL/RENORM
                        CALL REBIN_DOIT_PUT(
     :                                   OD1,OD2,OD3,OD4,OD5,OD6,OD7,
     :                                             I,OVAL,DATO,STATUS)

                      ENDIF

                    ENDDO
                  ENDDO
                ENDDO
              ENDDO
            ENDDO
          ENDDO
        ENDDO

      ENDIF

      END



      SUBROUTINE REBIN_DOIT_BYBOUNDS(NDIM,DIMS,FRVAL,VOK,FRVAR,QOK,
     :   MASK,FRQAL,FRBND1,FRBND2,FRBND3,FRBND4,FRBND5,FRBND6,FRBND7,
     :          DIR,NORM,NBIN,BNDS,REGO,SEL,TOVAL,TOVAR,TOQAL,STATUS)

*    Description :
*
*    Authors :
*     Alan McFadzean (BHVAD::ADM)
*     Bob Vallance  (BHVAD::RJV)
*    History :
*     4 Aug 1988: original, based on reb_gen2eq (BHVAD::ADM)
*       Nov 1989: major overhaul (BHVAD::RJV)
*       Jun 1992: DQUAL changed to BYTE type (LTVAD::RDS)
*       Dec 1994: change from using widths to bounds (RJV)
*    Type Definitions :
      IMPLICIT NONE
*    Global constants :
      INCLUDE 'SAE_PAR'
      INCLUDE 'DAT_PAR'
      INCLUDE 'QUAL_PAR'
*    Import :
        INTEGER NDIM			! number of actual dimensions
	INTEGER DIMS(7)		        ! numbers of donor bins
	REAL FRVAL(*)	                ! values of donor bins
        LOGICAL VOK			! whether input variance present
	REAL FRVAR(*)			! values of donor VARIANCE
        LOGICAL QOK			! whether input QUALITY present
        BYTE MASK			! QUALITY mask
	BYTE FRQAL(*)			! values of donor qualities
        REAL FRBND1(2,*) 		! donor bin widths
        REAL FRBND2(2,*) 		! donor bin widths
        REAL FRBND3(2,*) 		! donor bin widths
        REAL FRBND4(2,*) 		! donor bin widths
        REAL FRBND5(2,*) 		! donor bin widths
        REAL FRBND6(2,*) 		! donor bin widths
        REAL FRBND7(2,*)		! donor bin widths
        REAL DIR(7)			! axis direction indicator
	LOGICAL NORM(7)	        	! whether axes normalised
	INTEGER NBIN(7)	        	! number of receptor bins
	REAL BNDS(2,500,7) 	        ! boundaries of receptor bins
        LOGICAL REGO(7)                 ! regular output bins?
        LOGICAL SEL(7)
*    Import-Export :
*    Export :
	REAL TOVAL(*)			! values of receptor bins
	REAL TOVAR(*)		        ! values of receptor VARIANCE
	BYTE TOQAL(*)		 	! values of receptor qualities
*    Status :
	INTEGER STATUS
*    Global variables :
*    Functions :
        BYTE BIT_ANDUB
*    Local Constants :
        REAL ZERO
        PARAMETER (ZERO=0.0)
*    Local variables :
	REAL DVAL		! donor bin value
        REAL DVAR               ! donor bin VAR
        BYTE DQUAL		! donor bin QUALITY
        REAL DWID(7)            ! donor bin width
        REAL DLBND(7)           ! donor lower bound
        REAL DUBND(7)		! donor upper bound
	REAL DBOT(7)		! donor bin lower boundary
	REAL DTOP(7)		! donor bin upper boundary
	REAL RBOT(7)		! receptor lower boundary
	REAL RTOP(7)		! receptor upper boundary
        REAL BASEO(7)		! regular axis base
        REAL SCALEO(7)		! regular axis scale
	INTEGER RIL(7)		! lower receptor bin index
	INTEGER RIH(7)		! upper receptor bin index
        INTEGER FPTR		! pointer to flag array
        REAL OV1,OV2		! overlaps of donor and receptor bins
	REAL FRAC(7)		! fractional part of donor bin per axis
        REAL FRACT              ! total fraction
        INTEGER IAX
        INTEGER IBIN
	INTEGER I(7),I1,I2,I3,I4,I5,I6,I7
        EQUIVALENCE (I(1),I1),(I(2),I2),(I(3),I3),(I(4),I4),
     :              (I(5),I5),(I(6),I6),(I(7),I7)
	INTEGER II(7),II1,II2,II3,II4,II5,II6,II7
        EQUIVALENCE (II(1),II1),(II(2),II2),(II(3),II3),(II(4),II4),
     :              (II(5),II5),(II(6),II6),(II(7),II7)
        LOGICAL LOFOUND,HIFOUND
        LOGICAL INCLUDE		! whether donor bin to be included
*-


*      Status check
      IF (STATUS.NE.SAI__OK) RETURN

*  set up array of flags to record whether output bins have been filled
      CALL DYN_MAPB(7,NBIN,FPTR,STATUS)

*  initialise output arrays
      CALL AR7_INITR(ZERO,NBIN,TOVAL,STATUS)
      CALL AR7_INITB(QUAL__GOOD,NBIN,TOQAL,STATUS)
      IF (VOK) THEN
        CALL AR7_INITR(ZERO,NBIN,TOVAR,STATUS)
      ENDIF
      CALL AR7_INITB(QUAL__GOOD,NBIN,%VAL(FPTR),STATUS)

*  get BASE, SCALE for regular output axes
      DO IAX=1,NDIM
        BASEO(IAX)=BNDS(1,1,IAX)
        SCALEO(IAX)=BNDS(2,1,IAX)-BASEO(IAX)
      ENDDO

*   Loop through donor bins
      DO I7=1,DIMS(7)
        DO I6=1,DIMS(6)
          DO I5=1,DIMS(5)
            DO I4=1,DIMS(4)
              DO I3=1,DIMS(3)
                DO I2=1,DIMS(2)
                  DO I1=1,DIMS(1)

*  Upper and lower extent of bin in each axis
                    DLBND(1)=FRBND1(1,I(1))
                    DUBND(1)=FRBND1(2,I(1))
                    DLBND(2)=FRBND2(1,I(2))
                    DUBND(2)=FRBND2(2,I(2))
                    DLBND(3)=FRBND3(1,I(3))
                    DUBND(3)=FRBND3(2,I(3))
                    DLBND(4)=FRBND4(1,I(4))
                    DUBND(4)=FRBND4(2,I(4))
                    DLBND(5)=FRBND5(1,I(5))
                    DUBND(5)=FRBND5(2,I(5))
                    DLBND(6)=FRBND6(1,I(6))
                    DUBND(6)=FRBND6(2,I(6))
                    DLBND(7)=FRBND7(1,I(7))
                    DUBND(7)=FRBND7(2,I(7))

*  boundaries and width of donor bin in each dimension
                    DO IAX=1,NDIM
	              DBOT(IAX)=DLBND(IAX)
	              DTOP(IAX)=DUBND(IAX)
	              DWID(IAX)=ABS(DUBND(IAX)-DLBND(IAX))
                    ENDDO
	print *,dbot(1),dtop(1),dwid(1)

*  get donor bin values
                    IF (VOK.AND.QOK) THEN
                      CALL REBIN_DOIT_GETDVQ(DIMS(1),DIMS(2),DIMS(3),
     :                                DIMS(4),DIMS(5),DIMS(6),DIMS(7),
     :                                  I,FRVAL,FRVAR,FRQAL,
     :                                    DVAL,DVAR,DQUAL,STATUS)
                    ELSEIF (VOK) THEN
                      CALL REBIN_DOIT_GETDV(DIMS(1),DIMS(2),DIMS(3),
     :                                DIMS(4),DIMS(5),DIMS(6),DIMS(7),
     :                                  I,FRVAL,FRVAR,
     :                                    DVAL,DVAR,STATUS)
                    ELSEIF (QOK) THEN
                      CALL REBIN_DOIT_GETDQ(DIMS(1),DIMS(2),DIMS(3),
     :                                DIMS(4),DIMS(5),DIMS(6),DIMS(7),
     :                                  I,FRVAL,FRQAL,
     :                                    DVAL,DQUAL,STATUS)
                    ELSE
                      CALL REBIN_DOIT_GETD(DIMS(1),DIMS(2),DIMS(3),
     :                                DIMS(4),DIMS(5),DIMS(6),DIMS(7),
     :                                  I,FRVAL,DVAL,STATUS)
                    ENDIF

*  find range of receptor bins for this donor
                    IAX=1
                    INCLUDE=.TRUE.
                    DO WHILE (IAX.LE.NDIM.AND.INCLUDE)

                      IF (.NOT.(SEL(IAX))) THEN		! not rebinning
                        RIL(IAX)=I(IAX)
                        RIH(IAX)=I(IAX)

                      ELSEIF(REGO(IAX)) THEN		! regular bins

                        RIL(IAX)=INT((DBOT(IAX)-BASEO(IAX))
     :                                              /SCALEO(IAX))+1
                        RIH(IAX)=INT((DTOP(IAX)-BASEO(IAX))
     :                                              /SCALEO(IAX))+1
*  case where upper boundaries coincide
                        IF (MOD((DTOP(IAX)-BASEO(IAX)),SCALEO(IAX))
     :                                                   .EQ.0.0) THEN
                          RIH(IAX)=RIH(IAX)-1
                        ENDIF

*  check donor falls completely within output range
                        INCLUDE=(RIL(IAX).GT.0.AND.RIL(IAX).LE.NBIN(IAX)
     :                     .AND.RIH(IAX).GT.0.AND.RIH(IAX).LE.NBIN(IAX))

                      ELSE			! irregular bins

                        LOFOUND=.FALSE.
                        IBIN=1
*  lower receptor bin
                        DO WHILE (IBIN.LE.NBIN(IAX).AND..NOT.LOFOUND)
                          RBOT(IAX)=BNDS(1,IBIN,IAX)
                          RTOP(IAX)=BNDS(2,IBIN,IAX)
                          IF((DBOT(IAX)-RBOT(IAX))*DIR(IAX).GE.0.0
     :                                                           .AND.
     :                       (RTOP(IAX)-DBOT(IAX))*DIR(IAX).GT.0.0)
     :                                                             THEN
                            RIL(IAX)=IBIN
                            LOFOUND=.TRUE.
                          ELSE
                            IBIN=IBIN+1
                          ENDIF
                        ENDDO
                        HIFOUND=.FALSE.
*  upper receptor bin
                        DO WHILE (IBIN.LE.NBIN(IAX).AND..NOT.HIFOUND
     :                                                  .AND.LOFOUND)
                          RBOT(IAX)=BNDS(1,IBIN,IAX)
                          RTOP(IAX)=BNDS(2,IBIN,IAX)
                          IF((DTOP(IAX)-RBOT(IAX))*DIR(IAX).GE.0.0
     :                                                            .AND.
     :                       (RTOP(IAX)-DTOP(IAX))*DIR(IAX).GT.0.0)
     :                                                            THEN
                            RIH(IAX)=IBIN
                            HIFOUND=.TRUE.
                          ELSE
                            IBIN=IBIN+1
                          ENDIF
                        ENDDO

                        INCLUDE=(LOFOUND.AND.HIFOUND)

                      ENDIF

                      IAX=IAX+1

                    ENDDO

*  set dummy values for redundent dimensions
                    DO IAX=NDIM+1,7
                      RIL(IAX)=1
                      RIH(IAX)=1
                    ENDDO


*  if donor is to be included then loop through receptor bins containing it
                    IF (INCLUDE) THEN

                      DO II7=RIL(7),RIH(7)
                        DO II6=RIL(6),RIH(6)
                          DO II5=RIL(5),RIH(5)
                            DO II4=RIL(4),RIH(4)
                              DO II3=RIL(3),RIH(3)
                                DO II2=RIL(2),RIH(2)
                                  DO II1=RIL(1),RIH(1)

                                    IF (QOK) THEN
                                      IF (BIT_ANDUB(DQUAL,MASK)
     :                                             .EQ.QUAL__GOOD) THEN
                                        FRACT=1.0
                                      ELSE
                                        FRACT=0.0
                                      ENDIF
                                    ELSE
                                      FRACT=1.0
                                    ENDIF

                                    DO IAX=1,NDIM
*  set receptor bin boundaries
                                      IF (SEL(IAX)) THEN
                                        IF(REGO(IAX)) THEN
                                          RBOT(IAX)=BASEO(IAX)
     :                                   +SCALEO(IAX)*REAL(II(IAX)-1)
		                          RTOP(IAX)=RBOT(IAX)+
     :                                              SCALEO(IAX)
                                        ELSE
                                          RBOT(IAX)=BNDS(1,II(IAX),IAX)
                                          RTOP(IAX)=BNDS(2,II(IAX),IAX)
                                        ENDIF

*  calculate various overlaps between donor and receptor
                                        OV1=(RTOP(IAX)-DTOP(IAX))
     :                                                 *DIR(IAX)
                                        OV2=(RBOT(IAX)-DBOT(IAX))
     :                                                 *DIR(IAX)
*  now get fraction of donor to be donated in each axis direction
                                        FRAC(IAX)=0.0

*        ----         donor
*     ----------      receptor
                                        IF (OV2.LE.0.0.AND.OV1.GE.0.0)
     :                                                             THEN
                                          FRAC(IAX)=1.0
*     ----------      donor
*        ----         receptor
                                        ELSEIF
     :                                     (OV1.LE.0.0.AND.OV2.GE.0.0)
     :                                                             THEN
                                          FRAC(IAX)=
     :                                         ABS(RTOP(IAX)-RBOT(IAX))
     :                                                        /DWID(IAX)
*     ---------       donor
*         ---------   receptor
                                        ELSEIF
     :                                     (OV1.GE.0.0.AND.OV2.GE.0.0)
     :                                                             THEN
                                          FRAC(IAX)=
     :                                         ABS(DTOP(IAX)-RBOT(IAX))
     :                                                        /DWID(IAX)
*         ---------   donor
*     --------        receptor
                                        ELSEIF
     :                                    (OV1.LE.0.0.AND.OV2.LE.0.0)
     :                                                             THEN
                                          FRAC(IAX)=
     :                                         ABS(RTOP(IAX)-DBOT(IAX))
     :                                                        /DWID(IAX)

	                                ENDIF
*  include factor for renormalisation of normalised axes
                                        IF (NORM(IAX)) THEN
                                          FRAC(IAX)=FRAC(IAX)*DWID(IAX)/
     :                                          ABS(RTOP(IAX)-RBOT(IAX))
                                        ENDIF
*  update overall fraction
                                        FRACT=FRACT*FRAC(IAX)

                                      ENDIF

                                    ENDDO

*  add relevant bits to receptor bin
                                    IF (VOK.AND.QOK) THEN
                                      CALL REBIN_DOIT_ADDVQ(FRACT,
     :                                  NBIN(1),NBIN(2),NBIN(3),NBIN(4),
     :                                      NBIN(5),NBIN(6),NBIN(7),II,
     :                                         DVAL,DVAR,DQUAL,
     :                                           TOVAL,TOVAR,TOQAL,
     :                                              %VAL(FPTR),STATUS)

                                    ELSEIF (VOK) THEN
                                      CALL REBIN_DOIT_ADDV(FRACT,
     :                                  NBIN(1),NBIN(2),NBIN(3),NBIN(4),
     :                                      NBIN(5),NBIN(6),NBIN(7),II,
     :                                         DVAL,DVAR,
     :                                           TOVAL,TOVAR,
     :                                              %VAL(FPTR),STATUS)

                                    ELSEIF (QOK) THEN
                                      CALL REBIN_DOIT_ADDQ(FRACT,
     :                                  NBIN(1),NBIN(2),NBIN(3),NBIN(4),
     :                                      NBIN(5),NBIN(6),NBIN(7),II,
     :                                         DVAL,DQUAL,
     :                                           TOVAL,TOQAL,
     :                                              %VAL(FPTR),STATUS)
                                    ELSE
                                      CALL REBIN_DOIT_ADD(FRACT,
     :                                  NBIN(1),NBIN(2),NBIN(3),NBIN(4),
     :                                      NBIN(5),NBIN(6),NBIN(7),II,
     :                                         DVAL,TOVAL,
     :                                              %VAL(FPTR),STATUS)

                                    ENDIF

*  end loop through receptor bins for this donor
                                  ENDDO
                                ENDDO
                              ENDDO
                            ENDDO
                          ENDDO
                        ENDDO
                      ENDDO

                    ENDIF

*  end loop through donor bins
                  ENDDO
                ENDDO
              ENDDO
            ENDDO
          ENDDO
        ENDDO
      ENDDO

* set QUALITY to 'missing' for receptor bins which haven't been filled
      CALL REBIN_DOIT_MISSING(NBIN(1),NBIN(2),NBIN(3),NBIN(4),NBIN(5),
     :                        NBIN(6),NBIN(7),%VAL(FPTR),TOQAL,STATUS)

* release flag array
      CALL DYN_UNMAP(FPTR,STATUS)

      END


      SUBROUTINE REBIN_DOIT_PUTVQ(N1,N2,N3,N4,N5,N6,N7,I,
     :                             VAL,VAR,QUAL,TOVAL,TOVAR,TOQUAL,
     :                                                       STATUS)
*    Description :
*         Initialise
*    Bugs :
*    Authors :
*    History :
*    Type Definitions :
      IMPLICIT NONE
*    Global constants :
      INCLUDE 'SAE_PAR'
      INCLUDE 'DAT_PAR'
*    Import :
      INTEGER N1,N2,N3,N4,N5,N6,N7
      INTEGER I(7)
      REAL VAL,VAR
      BYTE QUAL
*    Import-Export :
*    Export :
      REAL TOVAL(N1,N2,N3,N4,N5,N6,N7)
      REAL TOVAR(N1,N2,N3,N4,N5,N6,N7)
      BYTE TOQUAL(N1,N2,N3,N4,N5,N6,N7)
*    Status :
	INTEGER STATUS
*    Local Constants :
*    Local variables :
*-
      IF (STATUS.EQ.SAI__OK) THEN

        TOVAL(I(1),I(2),I(3),I(4),I(5),I(6),I(7))=VAL

        TOVAR(I(1),I(2),I(3),I(4),I(5),I(6),I(7))=VAR

        TOQUAL(I(1),I(2),I(3),I(4),I(5),I(6),I(7))=QUAL


      ENDIF

      END


      SUBROUTINE REBIN_DOIT_PUTV(N1,N2,N3,N4,N5,N6,N7,I,
     :                             VAL,VAR,TOVAL,TOVAR,STATUS)
*    Description :
*         Initialise
*    Bugs :
*    Authors :
*    History :
*    Type Definitions :
      IMPLICIT NONE
*    Global constants :
      INCLUDE 'SAE_PAR'
      INCLUDE 'DAT_PAR'
*    Import :
      INTEGER N1,N2,N3,N4,N5,N6,N7
      INTEGER I(7)
      REAL VAL,VAR
*    Import-Export :
*    Export :
      REAL TOVAL(N1,N2,N3,N4,N5,N6,N7)
      REAL TOVAR(N1,N2,N3,N4,N5,N6,N7)
*    Status :
	INTEGER STATUS
*    Local Constants :
*    Local variables :
*-

      IF (STATUS.EQ.SAI__OK) THEN

        TOVAL(I(1),I(2),I(3),I(4),I(5),I(6),I(7))=VAL

        TOVAR(I(1),I(2),I(3),I(4),I(5),I(6),I(7))=VAR


      ENDIF

      END


      SUBROUTINE REBIN_DOIT_PUTQ(N1,N2,N3,N4,N5,N6,N7,I,
     :                             VAL,QUAL,TOVAL,TOQUAL,STATUS)
*    Description :
*         Initialise
*    Bugs :
*    Authors :
*    History :
*    Type Definitions :
      IMPLICIT NONE
*    Global constants :
      INCLUDE 'SAE_PAR'
      INCLUDE 'DAT_PAR'
*    Import :
      INTEGER N1,N2,N3,N4,N5,N6,N7
      INTEGER I(7)
      REAL VAL
      BYTE QUAL
*    Import-Export :
*    Export :
      REAL TOVAL(N1,N2,N3,N4,N5,N6,N7)
      BYTE TOQUAL(N1,N2,N3,N4,N5,N6,N7)
*    Status :
	INTEGER STATUS
*    Local Constants :
*    Local variables :
*-

      IF (STATUS.EQ.SAI__OK) THEN

        TOVAL(I(1),I(2),I(3),I(4),I(5),I(6),I(7))=VAL

        TOQUAL(I(1),I(2),I(3),I(4),I(5),I(6),I(7))=QUAL


      ENDIF

      END


      SUBROUTINE REBIN_DOIT_PUT(N1,N2,N3,N4,N5,N6,N7,I,
     :                                   VAL,TOVAL,STATUS)
*    Description :
*         Initialise
*    Bugs :
*    Authors :
*    History :
*    Type Definitions :
      IMPLICIT NONE
*    Global constants :
      INCLUDE 'SAE_PAR'
      INCLUDE 'DAT_PAR'
*    Import :
      INTEGER N1,N2,N3,N4,N5,N6,N7
      INTEGER I(7)
      REAL VAL
*    Import-Export :
*    Export :
      REAL TOVAL(N1,N2,N3,N4,N5,N6,N7)
*    Status :
	INTEGER STATUS
*    Local Constants :
*    Local variables :
*-

      IF (STATUS.EQ.SAI__OK) THEN

        TOVAL(I(1),I(2),I(3),I(4),I(5),I(6),I(7))=VAL


      ENDIF

      END




      SUBROUTINE REBIN_DOIT_GETDVQ(N1,N2,N3,N4,N5,N6,N7,I,
     :                             FRVAL,FRVAR,FRQUAL,VAL,VAR,QUAL,
     :                                                         STATUS)
*    Description :
*         Initialise
*    Bugs :
*    Authors :
*    History :
*    Type Definitions :
      IMPLICIT NONE
*    Global constants :
      INCLUDE 'SAE_PAR'
      INCLUDE 'DAT_PAR'
*    Import :
      INTEGER N1,N2,N3,N4,N5,N6,N7
      INTEGER I(7)
      REAL FRVAL(N1,N2,N3,N4,N5,N6,N7)
      REAL FRVAR(N1,N2,N3,N4,N5,N6,N7)
      BYTE FRQUAL(N1,N2,N3,N4,N5,N6,N7)
*    Import-Export :
*    Export :
      REAL VAL,VAR
      BYTE QUAL
*    Status :
	INTEGER STATUS
*    Local Constants :
*    Local variables :
*-

      IF (STATUS.EQ.SAI__OK) THEN

        VAL=FRVAL(I(1),I(2),I(3),I(4),I(5),I(6),I(7))

        VAR=FRVAR(I(1),I(2),I(3),I(4),I(5),I(6),I(7))

        QUAL=FRQUAL(I(1),I(2),I(3),I(4),I(5),I(6),I(7))

      ENDIF

      END


      SUBROUTINE REBIN_DOIT_GETDV(N1,N2,N3,N4,N5,N6,N7,I,
     :                             FRVAL,FRVAR,VAL,VAR,STATUS)
*    Description :
*         Initialise
*    Bugs :
*    Authors :
*    History :
*    Type Definitions :
      IMPLICIT NONE
*    Global constants :
      INCLUDE 'SAE_PAR'
      INCLUDE 'DAT_PAR'
*    Import :
      INTEGER N1,N2,N3,N4,N5,N6,N7
      INTEGER I(7)
      REAL FRVAL(N1,N2,N3,N4,N5,N6,N7)
      REAL FRVAR(N1,N2,N3,N4,N5,N6,N7)
*    Import-Export :
*    Export :
      REAL VAL,VAR
*    Status :
	INTEGER STATUS
*    Local Constants :
*    Local variables :
*-

      IF (STATUS.EQ.SAI__OK) THEN

        VAL=FRVAL(I(1),I(2),I(3),I(4),I(5),I(6),I(7))

        VAR=FRVAR(I(1),I(2),I(3),I(4),I(5),I(6),I(7))

      ENDIF

      END


      SUBROUTINE REBIN_DOIT_GETDQ(N1,N2,N3,N4,N5,N6,N7,I,
     :                             FRVAL,FRQUAL,VAL,QUAL,STATUS)
*    Description :
*         Initialise
*    Bugs :
*    Authors :
*    History :
*    Type Definitions :
      IMPLICIT NONE
*    Global constants :
      INCLUDE 'SAE_PAR'
      INCLUDE 'DAT_PAR'
*    Import :
      INTEGER N1,N2,N3,N4,N5,N6,N7
      INTEGER I(7)
      REAL FRVAL(N1,N2,N3,N4,N5,N6,N7)
      BYTE FRQUAL(N1,N2,N3,N4,N5,N6,N7)
*    Import-Export :
*    Export :
      REAL VAL
      BYTE QUAL
*    Status :
	INTEGER STATUS
*    Local Constants :
*    Local variables :
*-

      IF (STATUS.EQ.SAI__OK) THEN

        VAL=FRVAL(I(1),I(2),I(3),I(4),I(5),I(6),I(7))

        QUAL=FRQUAL(I(1),I(2),I(3),I(4),I(5),I(6),I(7))

      ENDIF

      END


      SUBROUTINE REBIN_DOIT_GETD(N1,N2,N3,N4,N5,N6,N7,I,
     :                                   FRVAL,VAL,STATUS)
*    Description :
*         Initialise
*    Bugs :
*    Authors :
*    History :
*    Type Definitions :
      IMPLICIT NONE
*    Global constants :
      INCLUDE 'SAE_PAR'
      INCLUDE 'DAT_PAR'
*    Import :
      INTEGER N1,N2,N3,N4,N5,N6,N7
      INTEGER I(7)
      REAL FRVAL(N1,N2,N3,N4,N5,N6,N7)
*    Import-Export :
*    Export :
      REAL VAL
*    Status :
	INTEGER STATUS
*    Local Constants :
*    Local variables :
*-

      IF (STATUS.EQ.SAI__OK) THEN

        VAL=FRVAL(I(1),I(2),I(3),I(4),I(5),I(6),I(7))

      ENDIF

      END


      SUBROUTINE REBIN_DOIT_ADDVQ(FRAC,N1,N2,N3,N4,N5,N6,N7,I,
     :                             VAL,VAR,QUAL,TOVAL,TOVAR,TOQUAL,
     :                                                    FLAG,STATUS)
*    Description :
*         Initialise
*    Bugs :
*    Authors :
*    History :
*    Type Definitions :
      IMPLICIT NONE
*    Global constants :
      INCLUDE 'SAE_PAR'
      INCLUDE 'DAT_PAR'
*    Import :
      REAL FRAC
      INTEGER N1,N2,N3,N4,N5,N6,N7
      INTEGER I(7)
      REAL VAL,VAR
      BYTE QUAL
*    Import-Export :
*    Export :
      REAL TOVAL(N1,N2,N3,N4,N5,N6,N7)
      REAL TOVAR(N1,N2,N3,N4,N5,N6,N7)
      BYTE TOQUAL(N1,N2,N3,N4,N5,N6,N7)
      BYTE FLAG(N1,N2,N3,N4,N5,N6,N7)
*    Status :
	INTEGER STATUS
*    Functions :
      BYTE  BIT_ORUB
*    Local Constants :
      BYTE FILLED
      PARAMETER (FILLED=1)
*    Local variables :
*-
      IF (STATUS.EQ.SAI__OK) THEN

        IF (FRAC.GT.0.0) THEN
          TOVAL(I(1),I(2),I(3),I(4),I(5),I(6),I(7))=
     :      TOVAL(I(1),I(2),I(3),I(4),I(5),I(6),I(7))+VAL*FRAC

          TOVAR(I(1),I(2),I(3),I(4),I(5),I(6),I(7))=
     :     TOVAR(I(1),I(2),I(3),I(4),I(5),I(6),I(7))+VAR*FRAC**2

          FLAG(I(1),I(2),I(3),I(4),I(5),I(6),I(7))=FILLED

        ENDIF

        TOQUAL(I(1),I(2),I(3),I(4),I(5),I(6),I(7))=
     :     BIT_ORUB(QUAL,TOQUAL(I(1),I(2),I(3),I(4),I(5),I(6),I(7)))


      ENDIF

      END


      SUBROUTINE REBIN_DOIT_ADDV(FRAC,N1,N2,N3,N4,N5,N6,N7,I,
     :                             VAL,VAR,TOVAL,TOVAR,FLAG,STATUS)
*    Description :
*         Initialise
*    Bugs :
*    Authors :
*    History :
*    Type Definitions :
      IMPLICIT NONE
*    Global constants :
      INCLUDE 'SAE_PAR'
      INCLUDE 'DAT_PAR'
*    Import :
      REAL FRAC
      INTEGER N1,N2,N3,N4,N5,N6,N7
      INTEGER I(7)
      REAL VAL,VAR
*    Import-Export :
*    Export :
      REAL TOVAL(N1,N2,N3,N4,N5,N6,N7)
      REAL TOVAR(N1,N2,N3,N4,N5,N6,N7)
      BYTE FLAG(N1,N2,N3,N4,N5,N6,N7)
*    Status :
	INTEGER STATUS
*    Local Constants :
      BYTE FILLED
      PARAMETER (FILLED=1)
*    Local variables :
*-

      IF (STATUS.EQ.SAI__OK) THEN

        IF (FRAC.GT.0.0) THEN
          TOVAL(I(1),I(2),I(3),I(4),I(5),I(6),I(7))=
     :      TOVAL(I(1),I(2),I(3),I(4),I(5),I(6),I(7))+VAL*FRAC

          TOVAR(I(1),I(2),I(3),I(4),I(5),I(6),I(7))=
     :     TOVAR(I(1),I(2),I(3),I(4),I(5),I(6),I(7))+VAR*FRAC**2


          FLAG(I(1),I(2),I(3),I(4),I(5),I(6),I(7))=FILLED

        ENDIF

      ENDIF

      END


      SUBROUTINE REBIN_DOIT_ADDQ(FRAC,N1,N2,N3,N4,N5,N6,N7,I,
     :                             VAL,QUAL,TOVAL,TOQUAL,FLAG,STATUS)
*    Description :
*         Initialise
*    Bugs :
*    Authors :
*    History :
*    Type Definitions :
      IMPLICIT NONE
*    Global constants :
      INCLUDE 'SAE_PAR'
      INCLUDE 'DAT_PAR'
*    Import :
      REAL FRAC
      INTEGER N1,N2,N3,N4,N5,N6,N7
      INTEGER I(7)
      REAL VAL
      BYTE QUAL
*    Import-Export :
*    Export :
      REAL TOVAL(N1,N2,N3,N4,N5,N6,N7)
      BYTE TOQUAL(N1,N2,N3,N4,N5,N6,N7)
      BYTE FLAG(N1,N2,N3,N4,N5,N6,N7)
*    Status :
	INTEGER STATUS
*    Functions :
      BYTE BIT_ORUB
*    Local Constants :
      BYTE FILLED
      PARAMETER (FILLED=1)
*    Local variables :
*-

      IF (STATUS.EQ.SAI__OK) THEN

        IF (FRAC.GT.0.0) THEN
          TOVAL(I(1),I(2),I(3),I(4),I(5),I(6),I(7))=
     :      TOVAL(I(1),I(2),I(3),I(4),I(5),I(6),I(7))+VAL*FRAC

          FLAG(I(1),I(2),I(3),I(4),I(5),I(6),I(7))=FILLED

        ENDIF

        TOQUAL(I(1),I(2),I(3),I(4),I(5),I(6),I(7))=
     :      BIT_ORUB(QUAL,TOQUAL(I(1),I(2),I(3),I(4),I(5),I(6),I(7)))



      ENDIF

      END


      SUBROUTINE REBIN_DOIT_ADD(FRAC,N1,N2,N3,N4,N5,N6,N7,I,
     :                                   VAL,TOVAL,FLAG,STATUS)
*    Description :
*         Initialise
*    Bugs :
*    Authors :
*    History :
*    Type Definitions :
      IMPLICIT NONE
*    Global constants :
      INCLUDE 'SAE_PAR'
      INCLUDE 'DAT_PAR'
*    Import :
      REAL FRAC
      INTEGER N1,N2,N3,N4,N5,N6,N7
      INTEGER I(7)
      REAL VAL
*    Import-Export :
*    Export :
      REAL TOVAL(N1,N2,N3,N4,N5,N6,N7)
      BYTE FLAG(N1,N2,N3,N4,N5,N6,N7)
*    Status :
	INTEGER STATUS
*    Local Constants :
      BYTE FILLED
      PARAMETER (FILLED=1)
*    Local variables :
*-

      IF (STATUS.EQ.SAI__OK) THEN

        IF (FRAC.GT.0.0) THEN

          TOVAL(I(1),I(2),I(3),I(4),I(5),I(6),I(7))=
     :      TOVAL(I(1),I(2),I(3),I(4),I(5),I(6),I(7))+VAL*FRAC


          FLAG(I(1),I(2),I(3),I(4),I(5),I(6),I(7))=FILLED

        ENDIF

      ENDIF

      END




      SUBROUTINE REBIN_DOIT_MISSING(N1,N2,N3,N4,N5,N6,N7,FLAG,QUAL,
     :                                                        STATUS)
*    Description :
*    Bugs :
*    Authors :
*    History :
*    Type Definitions :
      IMPLICIT NONE
*    Global constants :
      INCLUDE 'SAE_PAR'
      INCLUDE 'DAT_PAR'
      INCLUDE 'QUAL_PAR'
*    Import :
      INTEGER N1,N2,N3,N4,N5,N6,N7
      BYTE FLAG(N1,N2,N3,N4,N5,N6,N7)
*    Import-Export :
*    Export :
      BYTE QUAL(N1,N2,N3,N4,N5,N6,N7)
*    Status :
	INTEGER STATUS
*    Local Constants :
      BYTE EMPTY
      PARAMETER (EMPTY=0)
*    Local variables :
      INTEGER I,J,K,L,M,N,O
*-

      IF (STATUS.EQ.SAI__OK) THEN

        DO O=1,N7
          DO N=1,N6
            DO M=1,N5
              DO L=1,N4
                DO K=1,N3
                  DO J=1,N2
                    DO I=1,N1

                      IF (FLAG(I,J,K,L,M,N,O).EQ.EMPTY.AND.
     :                    QUAL(I,J,K,L,M,N,O).EQ.QUAL__GOOD) THEN
                        QUAL(I,J,K,L,M,N,O)=QUAL__MISSING
                      ELSEIF (FLAG(I,J,K,L,M,N,O).NE.EMPTY) THEN
                        QUAL(I,J,K,L,M,N,O)=QUAL__GOOD
                      ENDIF

                    ENDDO
                  ENDDO
                ENDDO
              ENDDO
            ENDDO
          ENDDO
        ENDDO

      ENDIF

      END




*+  REBIN_GSPEC - Get bin description for rebinning
      SUBROUTINE REBIN_GSPEC(SEL,AXLBL,AXN,INBIN,MXBIN,LOW,UPP,IWID,
     :           REGI,DIR,OPT,CLOC,RAT,REGO,NORM,BOUNDS,ONBIN,STATUS)
*    Description :
*    Method :
*    Deficiencies :
*    Bugs :
*    Authors :
*    History :
*    Type Definitions :
        IMPLICIT NONE
*    Global constants :
        INCLUDE 'SAE_PAR'
      INCLUDE 'DAT_PAR'
*    Import :
      CHARACTER*(DAT__SZLOC) CLOC
        CHARACTER*(*) AXLBL  ! axis label
        INTEGER AXN          ! axis #
        INTEGER INBIN	     ! old number of bins
        INTEGER MXBIN        ! Max. no. irregular bins
	REAL LOW 	     ! data min. range
        REAL UPP             ! data max. range
        REAL DIR	     ! axis direction
        REAL IWID	     ! width of first input bin
        INTEGER OPT	     ! specification option
        LOGICAL REGI
        LOGICAL NORM
        LOGICAL SEL
*    Import-Export :
*    Export :
	LOGICAL REGO         ! regular binning
	REAL BOUNDS(2,MXBIN) ! bin spacing/bin boundaries in x axis
	INTEGER ONBIN        ! number of output bins
        INTEGER RAT	     ! rebinning ratio
*    Status :
	INTEGER STATUS
*    External references :
*    Global variables :
*    Local Constants :
*    Local variables :
      CHARACTER*7 C/'1234567'/
      CHARACTER*20 PARNAM       ! Input parameter
      REAL OWID
      REAL BASE,SCALE
      INTEGER XPTR,WPTR
      INTEGER NAX,CAX
      LOGICAL OK
*-

      IF (STATUS .NE. SAI__OK) RETURN

      IF (SEL) THEN
* tell user what axis range is
        CALL MSG_PRNT (' ')
        CALL MSG_SETC ('AXLB',AXLBL)
        CALL MSG_SETR ('LOW',LOW)
        CALL MSG_SETR ('UPP',UPP)
        CALL MSG_SETI ('NBIN',INBIN)
        CALL MSG_SETR ('WID',IWID)
        IF (REGI) THEN
          CALL MSG_PRNT
     :    ('^AXLB axis has ^NBIN regular bins, a range of ^LOW to ^UPP '
     :     //'and bin width ^WID')
        ELSE
          CALL MSG_PRNT
     :    ('^AXLB axis has ^NBIN irregular bins with a range of'//
     :     ' ^LOW to ^UPP ')
        ENDIF
        IF (NORM) THEN
          CALL MSG_PRNT(' - data are normalised to this axis')
        ELSE
          CALL MSG_PRNT(' - data are not normalised to this axis')
        ENDIF

      ELSE
        CALL MSG_PRNT (' ')
        CALL MSG_SETC ('AXLB',AXLBL)
        CALL MSG_PRNT('^AXLB axis will not be rebinned')
      ENDIF

      IF (OPT.EQ.1) THEN
* get rebinning factor
        IF (SEL) THEN
          PARNAM='RAT'//C(AXN:AXN)
          CALL USI_GET0I(PARNAM,RAT,STATUS)
        ELSE
          RAT=1
        ENDIF
        IF ( STATUS .NE. SAI__OK ) GOTO 99
        ONBIN=INBIN/RAT
        IF(ONBIN.LT.1) THEN
          ONBIN=1
          RAT=INBIN
        ENDIF
        REGO=REGI
        IF (REGO) THEN
          BOUNDS(1,1)=LOW
          BOUNDS(2,1)=LOW+REAL(RAT)*IWID*DIR
        ENDIF
        IF (SEL) THEN
          CALL MSG_SETI('NBIN',ONBIN)
          CALL MSG_PRNT ('Axis will have ^NBIN bins')
        ENDIF

      ELSEIF (OPT.EQ.2) THEN
* or get total number of bins
        IF (SEL) THEN
          PARNAM='NBIN'//C(AXN:AXN)
          CALL USI_GET0I(PARNAM,ONBIN,STATUS)
        ELSE
          ONBIN=INBIN
        ENDIF
        IF ( STATUS .NE. SAI__OK ) GOTO 99
        IF(ONBIN.LT.1) THEN
          ONBIN=1
        ENDIF
        REGO=.TRUE.
        OWID=(UPP-LOW)/REAL(ONBIN)*DIR
        IF (SEL) THEN
          CALL MSG_SETR('BINW',OWID)
          CALL MSG_PRNT ('Axis will have bin width of ^BINW')
        ENDIF

      ELSEIF (OPT.EQ.3.AND.SEL) THEN
*  get bin width
        PARNAM='WID'//C(AXN:AXN)
        CALL USI_GET0R(PARNAM,OWID,STATUS)
        IF ( STATUS .NE. SAI__OK ) GOTO 99
        ONBIN=INT((UPP-LOW)*DIR/OWID)
        IF(ONBIN.LT.1) THEN
          ONBIN=1
        ENDIF
        REGO=.TRUE.
        CALL MSG_SETI('NBIN',ONBIN)
        CALL MSG_PRNT ('Axis will have ^NBIN bins')

      ELSEIF (OPT.EQ.4.AND.SEL) THEN
* get individual bounds
        PARNAM='BOUNDS'//C(AXN:AXN)
        CALL PRS_GETRANGES (PARNAM, MXBIN, 1, LOW, UPP,
     :                         BOUNDS, ONBIN, STATUS)
        IF ( STATUS .NE. SAI__OK ) GOTO 99
        REGO=.FALSE.
        CALL MSG_SETI('NBIN',ONBIN)
        CALL MSG_SETI('AXN',AXN)
        CALL MSG_PRNT( 'This will give ^NBIN AXIS ^AXN bins' )

      ELSEIF (OPT.EQ.5.AND.SEL) THEN
*  clone axis from another dataset
        CALL BDA_CHKAXES(CLOC,NAX,STATUS)
        IF (AXN.GT.NAX) THEN
          CAX=1
        ELSE
          CAX=AXN
        ENDIF
        CALL BDA_CHKAXVAL(CLOC,CAX,OK,REGO,ONBIN,STATUS)
        IF (OK) THEN
          IF (REGO) THEN
            CALL BDA_GETAXVAL(CLOC,CAX,BASE,SCALE,ONBIN,STATUS)
            BOUNDS(1,1)=BASE-SCALE/2.0
            BOUNDS(2,1)=BOUNDS(1,1)+SCALE
          ELSE
            IF (ONBIN.LE.MXBIN) THEN
              CALL BDA_MAPAXVAL(CLOC,'R',CAX,XPTR,STATUS)
              CALL BDA_MAPAXWID(CLOC,'R',CAX,WPTR,STATUS)
              CALL REBIN_GSPEC_BOUNDS(ONBIN,%VAL(XPTR),%VAL(WPTR),DIR,
     :                                                  BOUNDS,STATUS)
              CALL BDA_UNMAPAXVAL(CLOC,CAX,STATUS)
              CALL BDA_UNMAPAXWID(CLOC,CAX,STATUS)
            ELSE
              CALL MSG_SETI('AXN',AXN)
              CALL MSG_PRNT('AST_ERR: maximum number of output bins'//
     :                                       ' exceeded for axis ^AXN')
              STATUS=SAI__ERROR
            ENDIF
          ENDIF
          CALL MSG_SETI('NBIN',ONBIN)
          CALL MSG_PRNT ('Axis will be cloned'//
     :                        ' - there will be ^NBIN bins')
        ELSE
          CALL MSG_SETI('AXN',CAX)
          CALL MSG_PRNT('AST_ERR: bad data in clone axis ^AXN')
          STATUS=SAI__ERROR
        ENDIF

      ENDIF

      IF (OPT.EQ.2.OR.OPT.EQ.3) THEN
* set up bounds of 1st bin for options 2 & 3
        BOUNDS(1,1)=LOW
        BOUNDS(2,1)=LOW+OWID*DIR
* if this would be bigger than whole input dataset then shrink it
        IF((UPP-BOUNDS(2,1))*DIR.LT.0.0) THEN
          BOUNDS(2,1)=UPP
        ENDIF
      ENDIF

      IF (.NOT.SEL) THEN
        REGO=REGI
        ONBIN=INBIN
      ENDIF


 99   CONTINUE

      END



      SUBROUTINE REBIN_GSPEC_BOUNDS(N,X,W,DIR,BOUNDS,STATUS)
*    Description :
*    Method :
*    Authors :
*    History :
*    Type definitions :
      IMPLICIT NONE
*    Global constants :
      INCLUDE 'SAE_PAR'
      INCLUDE 'DAT_PAR'
      INCLUDE 'PAR_ERR'
*    Import :
      INTEGER N
      REAL X(*),W(*)
      REAL DIR
*    Import-Export :
*    Export :
      REAL BOUNDS(2,*)
*    Status :
      INTEGER STATUS
*    Function declarations :
*    Local constants :
*    Local variables :
      INTEGER I
      REAL HWID
*-

      IF (STATUS.EQ.SAI__OK) THEN


        DO I=1,N
          HWID=W(I)*DIR/2.0
          BOUNDS(1,I)=X(I)-HWID
          BOUNDS(2,I)=X(I)+HWID

        ENDDO

      ENDIF

      END
