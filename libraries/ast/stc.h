#if !defined( STC_INCLUDED ) /* Include this file only once */
#define STC_INCLUDED
/*
*+
*  Name:
*     stc.h

*  Type:
*     C include file.

*  Purpose:
*     Define the interface to the Stc class.

*  Invocation:
*     #include "stc.h"

*  Description:
*     This include file defines the interface to the Stc class and
*     provides the type definitions, function prototypes and macros,
*     etc.  needed to use this class.
*
*     The Stc class is an implementation of the IVOA STC class which forms 
*     part of the IVOA Space-Time Coordinate Metadata system. See:
*
*     http://hea-www.harvard.edu/~arots/nvometa/STC.html

*  Inheritance:
*     The Stc class inherits from the Region class.

*  Feature Test Macros:
*     astCLASS
*        If the astCLASS macro is undefined, only public symbols are
*        made available, otherwise protected symbols (for use in other
*        class implementations) are defined. This macro also affects
*        the reporting of error context information, which is only
*        provided for external calls to the AST library.

*  Copyright:
*     <COPYRIGHT_STATEMENT>

*  Authors:
*     DSB: David S. Berry (Starlink)

*  History:
*     23-NOV-2004 (DSB):
*        Original version.
*-
*/

/* Include files. */
/* ============== */
/* Interface definitions. */
/* ---------------------- */
#include "region.h"              /* Coordinate regions (parent class) */
#include "keymap.h"              /* Lists of value/key pairs */

#if defined(astCLASS)            /* Protected */
#include "channel.h"             /* I/O channels */
#endif

/* C header files. */
/* --------------- */
#if defined(astCLASS)            /* Protected */
#include <stddef.h>
#endif

/* Macros. */
/* ======= */

#define AST__STCNAME  "Name"
#define AST__STCVALUE "Value"
#define AST__STCERROR "Error"
#define AST__STCRES   "Resolution"
#define AST__STCSIZE  "Size"
#define AST__STCPIXSZ "PixSize"

/* Type Definitions. */
/* ================= */
/* Stc structure. */
/* ------------------ */

/* This structure contains all information that is unique to each object in
   the class (e.g. its instance variables). */
typedef struct AstStc {

/* Attributes inherited from the parent class. */
   AstRegion parent_region;      /* Parent class structure */

/* Attributes specific to objects in this class. */
   AstRegion *region;            /* Encapsulated Region */
   AstKeyMap **coord;            /* STC AstroCoords info */
   int ncoord;                   /* Number of AstroCoords in "coords" */
} AstStc;

/* Virtual function table. */
/* ----------------------- */
/* This table contains all information that is the same for all
   objects in the class (e.g. pointers to its virtual functions). */
#if defined(astCLASS)            /* Protected */
typedef struct AstStcVtab {

/* Properties (e.g. methods) inherited from the parent class. */
   AstRegionVtab region_vtab;    /* Parent class virtual function table */

/* Unique flag value to determine class membership. */
   int *check;                   /* Check value */

/* Properties (e.g. methods) specific to this class. */
   const char *(* GetRegionClass)( AstStc * );
   AstRegion *(* GetStcRegion)( AstStc * );
   AstKeyMap *(* GetStcCoord)( AstStc *, int );
   int (* GetStcNCoord)( AstStc * );

} AstStcVtab;
#endif

/* Function prototypes. */
/* ==================== */
/* Prototypes for standard class functions. */
/* ---------------------------------------- */
astPROTO_CHECK(Stc)          /* Check class membership */
astPROTO_ISA(Stc)            /* Test class membership */

#if defined(astCLASS)            /* Protected */

/* Initialiser. */
AstStc *astInitStc_( void *, size_t, int, AstStcVtab *, const char *, 
                     AstRegion *, int, AstKeyMap ** );

/* Vtab initialiser. */
void astInitStcVtab_( AstStcVtab *, const char * );

/* Loader. */
AstStc *astLoadStc_( void *, size_t, AstStcVtab *,
                     const char *, AstChannel * );

#endif

/* Prototypes for member functions. */
/* -------------------------------- */
AstRegion *astGetStcRegion_( AstStc * );
AstKeyMap *astGetStcCoord_( AstStc *, int );
int astGetStcNCoord_( AstStc * );

# if defined(astCLASS)           /* Protected */
const char *astGetRegionClass_( AstStc * );
#endif

/* Function interfaces. */
/* ==================== */
/* These macros are wrap-ups for the functions defined by this class
   to make them easier to invoke (e.g. to avoid type mis-matches when
   passing pointers to objects from derived classes). */

/* Interfaces to standard class functions. */
/* --------------------------------------- */
/* Some of these functions provide validation, so we cannot use them
   to validate their own arguments. We must use a cast when passing
   object pointers (so that they can accept objects from derived
   classes). */

/* Check class membership. */
#define astCheckStc(this) astINVOKE_CHECK(Stc,this)

/* Test class membership. */
#define astIsAStc(this) astINVOKE_ISA(Stc,this)

#if defined(astCLASS)            /* Protected */

/* Initialiser. */
#define astInitStc(mem,size,init,vtab,name,reg,ncoords,coords) \
astINVOKE(O,astInitStc_(mem,size,init,vtab,name,reg,ncoords,coords))

/* Vtab Initialiser. */
#define astInitStcVtab(vtab,name) astINVOKE(V,astInitStcVtab_(vtab,name))
/* Loader. */
#define astLoadStc(mem,size,vtab,name,channel) \
astINVOKE(O,astLoadStc_(mem,size,vtab,name,astCheckChannel(channel)))
#endif

/* Interfaces to public member functions. */
/* -------------------------------------- */
/* Here we make use of astCheckStc to validate Stc pointers
   before use.  This provides a contextual error report if a pointer
   to the wrong sort of Object is supplied. */

#define astGetStcRegion(this) astINVOKE(O,astGetStcRegion_(astCheckStc(this)))
#define astGetStcCoord(this,icoord) astINVOKE(O,astGetStcCoord_(astCheckStc(this),icoord))
#define astGetStcNCoord(this) astINVOKE(V,astGetStcNCoord_(astCheckStc(this)))
#if defined(astCLASS)            /* Protected */
#define astGetRegionClass(this) astINVOKE(V,astGetRegionClass_(astCheckStc(this)))
#endif
#endif
