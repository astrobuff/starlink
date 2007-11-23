/* Module Macros. */
/* -------------- */
/* HDS names for components used in the PROVENANCE extension of an NDF */
#define EXT_NAME "PROVENANCE"
#define DATE_NAME "DATE"
#define CREATOR_NAME "CREATOR"
#define ANCESTORS_NAME "ANCESTORS"
#define PARENTS_NAME "PARENTS"
#define PATH_NAME "PATH"
#define MORE_NAME "MORE"

/* HDS types for components used in the PROVENANCE extension of an NDF */
#define EXT_TYPE "PROVENANCE"
#define ANCESTORS_TYPE "PROV"
#define MORE_TYPE "MORE"

/* Max length allowed for each text component in the PROVENANCE extension
   of an NDF */
#define DATE_LEN 27
#define CREATOR_LEN 50
#define PATH_LEN 256

/* Include files. */
/* -------------- */
/* Starlink packages. */
#include "star/hds.h"
#include "star/hds_fortran.h"
#include "mers.h"
#include "ndf.h"
#include "ast.h"
#include "sae_par.h"
#include "f77.h"
#include "cnf.h"
#include "ndg.h"

/* C header files. */
#include <stdio.h>
#include <string.h>
#include <time.h> 

/* Module Variables. */
/* ----------------- */


/* Type Definitions. */
/* ----------------- */
/* A structure that stores the provenance information for a single
   ancestor NDF. */
typedef struct Prov {
   char *path;                 /* String holding the NDF path as returned
                                  by ndfMsg */
   char *date;                 /* String holding the formated UTC date & time 
                                  at which the NDF's provenance was recorded */
   char *creator;              /* String describing the software that created 
                                  the NDF */
   HDSLoc *more;               /* HDS object holding extra information about 
                                  the NDF */
   struct Prov **parents;      /* Array of pointers to the Prov structures for 
                                  the direct parents of the NDF */
   struct Prov **children;     /* Array of pointers to the Prov structures for 
                                  the direct children of the NDF */
   int nparent;                /* The length of the parents array */
   int nchild;                 /* The length of the children array */
} Prov;


/* A structure that stores an ordered list of all Prov structures that
   are derived from a given NDF PROVENANCE extension. */
typedef struct Provenance {
   Prov *main;                 /* Pointer to the Prov structure for the main 
                                  NDF */
   Prov **provs;               /* Array of pointers to Prov structures for the 
                                  main NDF and all ancestors NDFs */
   int nprov;                  /* The length of the "provs" array */
} Provenance;




/* Prototypes for Private Functions. */
/* --------------------------------- */
static void ndg1PurgeProvenance( Provenance *, int * );
static Prov *ndg1FreeProv( Prov *, int * );
static Prov *ndg1MakeProv( const char *, const char *, const char *, HDSLoc *, Provenance *, int * );
static Provenance *ndg1FreeProvenance( Provenance *, int, int * );
static Provenance *ndg1MakeProvenance( Prov *, int * );
static Provenance *ndg1ReadProvenanceExtension( int, const char *, int, int * );
static char *ndg1GetTextComp( HDSLoc *, const char *, char *, size_t, int * );
static int ndg1TheSame( Prov *, Prov *, int * );
static void ndg1Disown( Prov *, Prov *, int * );
static void ndg1ParentChild( Prov *, Prov *, int * );
static void ndg1ParentChildIndex( Provenance *, int, int, int * );
static void ndg1WriteProvenanceExtension( Provenance *, int, int * );


/* Public functions. */
/* ================= */

F77_SUBROUTINE(ndg_ptprv)( INTEGER(indf1), INTEGER(indf2), CHARACTER(fmore),
                           LOGICAL(isroot), CHARACTER(creatr), INTEGER(status)
                           TRAIL(fmore) TRAIL(creatr) ){
/*
*+
*  Name:
*     NDG_PTPRV

*  Purpose:
*     Add provenance information to an NDF.

*  Language:
*     Starlink ANSI C (callable from Fortran)

*  Invocation:
*     CALL NDG_PTPRV( INDF1, INDF2, MORE, ISROOT, CREATR, STATUS )

*  Description:
*     This routine stores information in the "PROVENANCE" extension of
*     INDF1 indicating that INDF2 was used in the creation of INDF1. 
*     The provenance information is stored in an NDF extension call 
*     "PROVENANCE".

*  Arguments:
*     INDF1 = INTEGER (Given)
*        An identifier for a newly created NDF.
*     INDF2 = INTEGER (Given)
*        An identifier for an NDF that was used in the creation of INDF1.
*     MORE = CHARACTER * (DAT__SZLOC) (Given)
*        A locator for an HDS structure containing arbitrary additional 
*        information about INDF2, and how INDF2 was used in the creation 
*        of INDF1. This information is stored in the provenance extension 
*        of INDF1.
*     ISROOT = LOGICAL (Given)
*        If TRUE, then INDF2 will be treated as a root NDF. That is,
*        any provenance information in INDF2 is ignored. If FALSE, then
*        any provenance information in INDF2 is copied into INDF1. INDF2
*        is then only a root NDF if it contains no provenance information.
*     CREATR = CHARACTER * ( * ) (Given)
*        A text identifier for the software that created INDF1 (usually the
*        name of the calling application). The format of the identifier
*        is arbitrary, but the form "PACKAGE:COMMAND" is recommended.
*     STATUS = INTEGER (Given and Returned)
*        The global status.

*  Notes:
*     - The PROVENANCE extension in an NDF contains four components:
*     "PARENTS", "ANCESTORS", "CREATOR" and "DATE". The DATE component is 
*     a character string holding the date and time at which the information 
*     in the provenance extension was last modified. The date is GMT 
*     formatted by PSX_ASCTIME. The ANCESTORS component is a 1D array
*     of "PROV" structures (described below). Each element describes a
*     single NDF that was used in the creation of the main NDF, either
*     directly or indirectly. The PARENTS component is a 1D integer 
*     array holding the indices within the ANCESTORS array of the NDFs
*     that are the direct parents of the main NDF. The CREATOR component
*     holds an arbitrary identifier for the software that created the
*     main NDF.
*     - Each PROV structure describes a single NDF that was used in the
*     creation of the main NDF, and contains up to four components; "PARENTS", 
*     "DATE", "PATH", "CREATOR" and "MORE". If present, the PARENTS component 
*     is a 1D integer array holding the the indices within the ANCESTORS array 
*     of the direct parents of the ancestor NDF. If PARENTS is not present, 
*     the ancestor NDF is a "root" NDF (that is, it has no known parents).
*     If present, the DATE component is a string holding the formatted GMT 
*     date at which the provenance information for the ancestor NDF was 
*     determined. If this date is not known, the DATE component will not
*     be present (this will be the case, for instance, for all root NDFs).
*     The PATH component will always be present, and is a string holding
*     the full path to the ancestor NDF. This includes any HDS path
*     within the container file, but will not include any NDF or HDS section 
*     specifier. Neither will it include the trailing ".sdf" suffix. If
*     present, the MORE component is an arbitrary HDS structure in which 
*     any extra information about the ancestor NDF can be stored. The
*     CREATOR component holds an arbitrary identifier for the software that 
*     created the ancestor NDF.

*  Copyright:
*     Copyright (C) 2007 Science & Technology Facilities Council.
*     All Rights Reserved.

*  Licence:
*     This programme is free software; you can redistribute it and/or
*     modify it under the terms of the GNU General Public License as
*     published by the Free Software Foundation; either Version 2 of
*     the License, or (at your option) any later version.
*     
*     This programme is distributed in the hope that it will be
*     useful, but WITHOUT ANY WARRANTY; without even the implied
*     warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
*     PURPOSE.  See the GNU General Public License for more details.
*     
*     You should have received a copy of the GNU General Public License
*     along with this programme; if not, write to the Free Software
*     Foundation, Inc., 59, Temple Place, Suite 330, Boston, MA
*     02111-1307, USA.

*  Authors:
*     DSB: David Berry (STARLINK)
*     {enter_new_authors_here}

*  History:
*     12-NOV-2007 (DSB):
*        Original version.
*     {enter_further_changes_here}

*  Bugs:
*     {note_any_bugs_here}

*-
*/
   GENPTR_INTEGER(indf1)
   GENPTR_INTEGER(indf2)
   GENPTR_CHARACTER(fmore) 
   GENPTR_LOGICAL(isroot)
   GENPTR_CHARACTER(creatr) 
   GENPTR_INTEGER(status)

/* Local variables: */
   char *creator = NULL;
   HDSLoc *more = NULL;

/* Check the inherited status. */
   if( *status != SAI__OK ) return;

/* Import the creator string */
   creator = cnfCreim( creatr, creatr_length );

/* Import the locator string. */
   if( strncmp( DAT__NOLOC, fmore, fmore_length) ){
      datImportFloc( fmore, fmore_length, &more, status );
   }

/* Call the C function to do the work. */
   ndgPtprv( *indf1, *indf2, more, F77_ISTRUE( *isroot ), creator, status );

/* Free the memory used to hold local copies of the supplied strings */
   cnfFree( creator );
}


void ndgPtprv( int indf1, int indf2, HDSLoc *more, int isroot, 
               const char *creator, int *status ){
/*
*  Name:
*     ndgPtPrv

*  Purpose:
*     Add provenance information to an NDF.

*  Synopsis:
*     void ndgPtprv( int indf1, int indf2, HDSLoc *more, int isroot, 
*                    const char *creator, int *status )

*  Description:
*     This routine stores information in the "PROVENANCE" extension of
*     INDF1 indicating that INDF2 was used in the creation of INDF1. 
*     The provenance information is stored in an NDF extension call 
*     "PROVENANCE".

*  Parameters:
*     indf1 
*        An identifier for a newly created NDF.
*     indf2 
*        An identifier for an NDF that was used in the creation of INDF1.
*     more 
*        A locator for an HDS structure containing arbitrary additional 
*        information about INDF2, and how INDF2 was used in the creation 
*        of INDF1. This information is stored in the provenance extension 
*        of INDF1.
*     isroot 
*        If non-zero, then INDF2 will be treated as a root NDF. That is,
*        any provenance information in INDF2 is ignored. If zero, then
*        any provenance information in INDF2 is copied into INDF1. INDF2
*        is then only a root NDF if it contains no provenance information.
*     creator
*        A text identifier for the software that created INDF1 (usually the
*        name of the calling application). The format of the identifier
*        is arbitrary, but the form "PACKAGE:COMMAND" is recommended.
*     status 
*        The global status.
*/

/* Local variables: */
   Provenance *prov1 = NULL;
   Provenance *prov2 = NULL;
   int free_provs;
   int i;
   int *old_status;

/* Check the inherited status. */
   if( *status != SAI__OK ) return;

/* Ensure AST uses the supplied status variable. */
   old_status = astWatch( status );

/* Get the existing provenance information from the two NDFs. */
   prov1 = ndg1ReadProvenanceExtension( indf1, creator, 0, status );
   prov2 = ndg1ReadProvenanceExtension( indf2, NULL, isroot, status );

/* Indicate that the "Prov" structures referred to by prov2 should be
   freed when ndgFreeProvenance is called. */
   free_provs = 1;

/* Extend the "provs" list in "prov1" so that we can add pointers to all the 
   Prov structures in "prov2. */
   if( prov1 && prov2 ) {
      prov1->provs = astGrow( prov1->provs, prov1->nprov + prov2->nprov,
                              sizeof( Prov *) );
   }
   if( astOK ) {

/* Copy the Prov pointers from "prov2" to "prov1". */
      for( i = 0; i < prov2->nprov; i++ ) {
         prov1->provs[ i + prov1->nprov ] = prov2->provs[ i ];
      }   

/* Update the length of the "provs" array in "prov1". */
      prov1->nprov += prov2->nprov;   

/* Indicate that the "Prov" structures referred to by "prov2" should not be
   freed when ndgFreeProvenance is called. This is because they are now
   the responsibility of "prov1", having been copied into the
   prov1->provs list above. */
      free_provs = 0;

/* Record INDF2 as a parent of INDF1. */
      ndg1ParentChild( prov2->main, prov1->main, status );

/* Purge any duplicate entries in the extended provenance information. */
      ndg1PurgeProvenance( prov1, status );

/* Write the extended provenance information out to INDF1. */
      ndg1WriteProvenanceExtension( prov1, indf1, status );
   }

/* Free the provenance structures. */
   ndg1FreeProvenance( prov1, 1, status );
   ndg1FreeProvenance( prov2, free_provs, status );

/* Re-instate the original AST status variable. */
   astWatch( old_status );
}


/* Private functions. */
/* ================= */
static void ndg1Disown( Prov *parent, Prov *child, int *status ){
/*
*  Name:
*     ndg1Disown

*  Purpose:
*     Break a parent-child link between two supplied Prov structures.

*  Synopsis:
*     void ndg1Disown( Prov *parent, Prov *child, int *status )

*  Description:
*     This function first checks to see that the supplied "parent" Prov 
*     is genuinely a parent of the supplied "child" Prov. If so, it breaks 
*     the parent-child relationship between them.
*
*     Each Prov structure describes a single NDF. The parent NDF is used
*     in the construction of ("gives birth to") the child NDF.

*  Parameters:
*     parent
*        The parent Prov structure.
*     child
*        The child Prov structure.
*     status
*        Pointer to the inherited status variable.

*/

/* Local Variables: */
   int j;
   int i;

/* Check the inherited status value, and the two pointers. */
   if( *status != SAI__OK || !parent || !child ) return;

/* NULLify all occurrences of the supplied parent within the list of 
   parents stored in the supplied child. */
   for( i = 0; i < child->nparent; i++ ) {
      if( child->parents[ i ] == parent ) child->parents[ i ] = NULL;
   }

/* Shuffle the remaining parent pointers down to over-write the NULL
   pointers introduced above. */
   j = 0;
   for( i = 0; i < child->nparent; i++ ) {
      if( child->parents[ i ] ) {
         child->parents[ j++ ] = child->parents[ i ];
      }
   }

/* Reduce the number of parents. */
   child->nparent = j;

/* NULLify all occurrences of the supplied child within the list of 
   children stored in the supplied parent. */
   for( i = 0; i < parent->nchild; i++ ) {
      if( parent->children[ i ] == child ) parent->children[ i ] = NULL;
   }

/* Shuffle the remaining child pointers down to over-write the NULL
   pointers introduced above. */
   j = 0;
   for( i = 0; i < parent->nchild; i++ ) {
      if( parent->children[ i ] ) {
         parent->children[ j++ ] = parent->children[ i ];
      }
   }

/* Reduce the number of children. */
   parent->nchild = j;

}

static Prov *ndg1FreeProv( Prov *prov, int *status ){
/*
*  Name:
*     ndg1FreeProv

*  Purpose:
*     Release the resources used by a Prov structure.

*  Synopsis:
*     Prov *ndg1FreeProv( Prov *prov, int *status )

*  Description:
*     This function releases all the resoures used by a Prov structure
*     and returns a NULL pointer.

*  Parameters:
*     prov
*        Pointer to the Prov to be released.
*     status
*        Pointer to the inherited status variable.

*  Returned Value:
*     A NULL pointer.

*/

/* Check that something has been supplied, but ignore the inherited
   status since we want this function to execute even if an error has 
   occurred. */
   if( !prov ) return NULL;

/* Release the memory used to store information within the Prov
   structure. */
   prov->path = astFree( prov->path );
   prov->date = astFree( prov->date );
   prov->creator = astFree( prov->creator );
   prov->parents = astFree( prov->parents );
   prov->children = astFree( prov->children );
   prov->nparent = 0;
   prov->nchild = 0;

/* Annul the HDS locator for the MORE structure. */
   if( prov->more ) datAnnul( &(prov->more), status );

/* Free the memory used to hold the Prov itself, and return a NULL pointer. */
   return astFree( prov );
}

static Provenance *ndg1FreeProvenance( Provenance *provenance, 
                                       int free_provs, int *status ){
/*
*  Name:
*     ndg1FreeProvenance

*  Purpose:
*     Release the resources used by a Provenance structure.

*  Synopsis:
*     Provenance *ndg1FreeProvenance( Provenance *provenance, 
*                                     int free_provs, int *status )

*  Description:
*     This function releases all the resoures used by a Provenance
*     structure, including (if required) all the enclosed Prov structures, 
*     and returns a NULL pointer.

*  Parameters:
*     provenance
*        Pointer to the Provenance to be released.
*     free_provs
*        Should the enclosed Prov structures also be freed?
*     status
*        Pointer to the inherited status variable.

*  Returned Value:
*     A NULL pointer.

*/

/* Local Variables; */
   int i;

/* Check that something has been supplied, but ignore the inherited
   status since we want this function to execute even if an error has 
   occurred. */
   if( !provenance ) return NULL;

/* If required, free each Prov structure listed in the Provenance structure. */
   if( free_provs ) {
      for( i = 0; i < provenance->nprov; i++ ) {
         provenance->provs[ i ] = ndg1FreeProv( provenance->provs[ i ],
                                                    status );
      }

/* Otherwise, just nullify each Prov pointer listed in the Provenance 
   structure. */
   } else {
      for( i = 0; i < provenance->nprov; i++ ) provenance->provs[ i ] = NULL;
   }

/* For safety, nullify other items in the structure. */
   provenance->main = NULL;
   provenance->nprov = 0;

/* Release the memory used to store information within the Prov
   structure. */
   provenance->provs = astFree( provenance->provs );

/* Free the memory used to hold the Provenance itself, and return a NULL 
   pointer. */
   return astFree( provenance );
}

static char *ndg1GetTextComp( HDSLoc *loc, const char *comp, char *buf, 
                              size_t buflen, int *status ){
/*
*  Name:
*     ndg1GetTextComp

*  Purpose:
*     Return a pointer to a string holding the value of an HDS _CHAR component

*  Synopsis:
*     char *ndg1GetTextComp( HDSLoc *loc, const char *comp, char *buf, 
*                            size_t buflen, int *status )

*  Description:
*     This function reads the value of a named character component in a
*     supplied HDS structure. If the component exists a pointer to its
*     value is returned. Otherwise, a NULL pointer is returned (without 
*     error).

*  Parameters:
*     loc
*        Locator for the HDS structure.
*     comp 
*        The name of the component to read.
*     buf
*        A pointer to a buff in which to store the value, if it exists.
*        If NULL is supplied, dynamic memory is allocated to store the
*        value.
*     buflen
*        The length of the supplied buffer (excluding the trailing null).
*        Not used if "buf" is NULL.
*     status
*        Pointer to the inherited status variable.

*  Returned Value:
*     Pointer to a string holding the component value, or a NULL pointer
*     if the component does not exist in the structure. If "buf" is NULL,
*     this pointer will point to newly allocated dynamic memory that should 
*     be freed using astFree when no longer needed. This buffer will be
*     long enough to hold the whole component value. If "buf" is not NULL,
*     then the returned pointer will equal "buf" (if the component was
*     found).

*/

/* Local Variables: */
   HDSLoc *cloc = NULL;
   int there;
   char *result;

/* Initialise */
   result = NULL;

/* Check the inherited status value. */
   if( *status != SAI__OK ) return result;

/* Does the component exist? If not return with the NULL result. */
   datThere( loc, comp, &there, status );
   if( there ) {

/* If it is there, get a locator to it. */
      datFind( loc, comp, &cloc, status );

/* If no buffer was supplied, create one of the correct length for the
   component (including the trailing null). */
      result = buf;
      if( !result ) {
         datLen( cloc, &buflen, status );
         result = astMalloc( ( buflen + 1 )*sizeof( char ) );
      }

/* Read the value of the component into the results buffer. */
      if( result ) datGet0C( cloc, result, buflen, status );

/* Annul the component locator. */
      datAnnul( &cloc, status ); 
   }

/* Return the result. */
   return result;
}

static Prov *ndg1MakeProv( const char *path, const char *date, 
                           const char *creator, HDSLoc *more, 
                           Provenance *provenance, int *status ){
/*
*  Name:
*     ndg1MakeProv

*  Purpose:
*     Make a new Prov structure and add it into an existing Provenance.

*  Synopsis:
*     Prov *ndg1MakeProv( const char *path, const char *date, 
*                         const char *creator, HDSLoc *more, 
*                         Provenance *provenance, int *status )

*  Description:
*     This function allocates dynamic memory to hold a new Prov structure,
*     and stores the supplied values in it. The new structure is appended
*     to the end of the list of Prov structures held in the supplied
*     Provenance structure. No parents or children are stored in the
*     new Prov structure.

*  Parameters:
*     path
*        Pointer to a string holding the path to the NDF described by the
*        new Prov structure.
*     date
*        Pointer to a string holding the formatted UTC date and time at
*        which provenance information was recorded for the NDF.
*     creator
*        Pointer to a string holding an arbitrary identifier for the
*        software that created the NDF.
*     more
*        Pointer to a locator for an HDS object holding extra information
*        about the NDF. A deep copy is taken of the supplied structure.
*     provenance
*        Pointer to an existing Provenance structure to which the new
*        Prov structure will be added, or NULL.
*     status
*        Pointer to the inherited status variable.

*  Returned Value:
*     Pointer to the new Prov structure. It should be freed using
*     ndg1FreeProv when no longer needed.

*/

/* Local Variables: */
   Prov *result;
   size_t len;

/* Initialise */
   result = NULL;

/* Check the inherited status value. */
   if( *status != SAI__OK ) return result;

/* Allocate the memory for the new Prov structure. */
   result = astMalloc( sizeof( Prov ) );
   if( result ) {

/* Store copies of the supplied strings. Store NULL pointers for any
   unspecified strings. */
      len = path ? strlen( path ) + 1 : 0;
      result->path = astStore( NULL, path, len*sizeof( char ) );

      len = date ? strlen( date ) + 1 : 0;
      result->date = astStore( NULL, date, len*sizeof( char ) );

      len = creator ? strlen( creator ) + 1 : 0;
      result->creator = astStore( NULL, creator, len*sizeof( char ) );

/* Store a deep copy of the "more" structure in a temporary HDS object. */
      result->more = NULL;
      if( more ) {
         datTemp( "STARLINK_PROV", 0, NULL, &( result->more ), status );
         datCopy( more, result->more, MORE_NAME, status );
      }

/* Initialise the lists of direct parents and children. */
      result->parents = NULL;
      result->nparent = 0;
      result->children = NULL;
      result->nchild = 0;

/* Extend the provs array in the supplied Provenance structure. */
      if( provenance ) {
         provenance->provs = astGrow( provenance->provs, 
                                      provenance->nprov + 1,
                                      sizeof( Prov *) );

/* Store a pointer to the new Prov structure and increment the number of
   provs. */
         if( astOK ) provenance->provs[ provenance->nprov++ ] = result;
      }
   }

/* If anything went wrong attempt to free the Prov structure. */
   if( !astOK ) result = ndg1FreeProv( result, status );

/* Return the result */
   return result;
}

static Provenance *ndg1MakeProvenance( Prov *main, int * status ){
/*
*  Name:
*     ndg1MakeProvenance

*  Purpose:
*     Make a new Provenance structure for a main NDF.

*  Synopsis:
*     Provenance *ndg1MakeProvenance( Prov *main, int * status )

*  Description:
*     This function allocates dynamic memory to hold a new Provenance 
*     structure, and stores the supplied main NDF Prov structure in it.
*
*     A Provenance structure represents the entire NDF PROVENANCE extension
*     of a particular NDF (the "main" NDF, as opposed to the ancestor
*     NDFs that were used to create the main NDF).

*  Parameters:
*     main
*        Pointer to a Prov structure describing the main NDF. The
*        returned Provenance structure represents the PROVENANCE extension 
*        in the main NDF.
*     status
*        Pointer to the inherited status variable.

*  Returned Value:
*     Pointer to the new Provenance structure. It should be freed using
*     ndg1FreeProvenance when no longer needed.

*/

/* Local Variables: */
   Provenance *result;

/* Initialise */
   result = NULL;

/* Check the inherited status value. */
   if( *status != SAI__OK ) return result;

/* Allocate the memory for the new Provenance structure. */
   result = astMalloc( sizeof( Provenance ) );
   if( result ) {

/* Store a poitner ot the mina NDF Prov structure. */
      result->main = main;

/* Initialise the provs array to be one element long and put a
   pointer to the supplied main NDF Prov in it. */
      result->provs = astMalloc( sizeof( Prov * ) );
      if( astOK ) {
         result->provs[ 0 ] = main;
         result->nprov = 1;
      } else {  
         result->nprov = 0;
      }
   }

/* If anything went wrong attempt to free the Provenance structure. */
   if( !astOK ) result = ndg1FreeProvenance( result, 1, status );

/* Return the result */
   return result;
}

static void ndg1ParentChild( Prov *parent, Prov *child, int *status ){
/*
*  Name:
*     ndg1ParentChild

*  Purpose:
*     Create a parent-child link between two supplied Prov structures.

*  Synopsis:
*     void ndg1ParentChild( Prov *parent, Prov *child, int *status )

*  Description:
*     This function joins the two supplied Prov structures together into 
*     a parent-child relationsip.
*
*     Each Prov structure describes a single NDF. The parent NDF is used
*     in the construction of ("gives birth to") the child NDF.

*  Parameters:
*     parent
*        The parent Prov structure.
*     child
*        The child Prov structure.
*     status
*        Pointer to the inherited status variable.

*/

/* Local Variables: */
   int found;
   int i;

/* Check the inherited status value. */
   if( *status != SAI__OK ) return;

/* Check that the parent is not already a parent of the child. */
   found = 0;
   for( i = 0; i < child->nparent; i++ ) {
      if( ndg1TheSame( child->parents[ i ], parent, status ) ) {
         found = 1;
         break;
      }
   }

/* Jump to the end if the parent-child link already exists. */
   if( !found ) {

/* Extend the array of parent pointers within the child, and add the new
   parent to the end of the array, incrementing the count of parents in the
   child. */
      child->parents = astGrow( child->parents, child->nparent + 1, 
                                sizeof( Prov * ) );
      if( astOK ) child->parents[ child->nparent++ ] = parent;

/* Extend the array of child pointers within the parent, and add the new
   child to the end of the array, incrementing the count of children in the
   parent. */
      parent->children = astGrow( parent->children, parent->nchild + 1, 
                                  sizeof( Prov * ) );
      if( astOK ) parent->children[ parent->nchild++ ] = child;
   }
}

static void ndg1ParentChildIndex( Provenance *provenance, int iparent, 
                                  int ichild, int *status ){
/*
*  Name:
*     ndg1ParentChildIndex

*  Purpose:
*     Create a parent-child link between two Prov structures with given
*     indices.

*  Synopsis:
*     void ndg1ParentChildIndex( Provenance *provenance, int iparent, 
*                                int ichild, int *status )

*  Description:
*     This function looks up the Prov structures with the specified
*     indices within the supplied Provenance structure, and joins them
*     together into a parent-child relationsip.
*
*     Each Prov structure describes a single NDF. The parent NDF is used
*     in the construction of ("gives birth to") the child NDF.

*  Parameters:
*     provenance
*        The Provenance structure holding the Prov structures that are to
*        be joined together.
*     iparent
*        The index within the "provenance->provs" array of the parent
*        Prov structure.
*     ichild
*        The index within the "provenance->provs" array of the child
*        Prov structure.
*     status
*        Pointer to the inherited status variable.

*/

/* Local Variables: */
   Prov *parent;
   Prov *child;

/* Check the inherited status value. */
   if( *status != SAI__OK ) return;

/* Get pointers to the parent and child Prov structures. */
   parent = provenance->provs[ iparent ];
   child = provenance->provs[ ichild ];

/* set up the connection. */
   ndg1ParentChild( parent, child, status );
}

static void ndg1PurgeProvenance( Provenance *provenance, 
                                 int *status ){
/*
*  Name:
*     ndg1PurgeProvenance

*  Purpose:
*     Purge duplicate ancestors from a Provenance structure.

*  Synopsis:
*     void ndg1PurgeProvenance( Provenance *provenance, 
*                               int *status )

*  Description:
*     This function removes any duplicated ancestors in the supplied
*     Provenancen structure.

*  Parameters:
*     provenance
*        Pointer to the Provenance to be checked.
*     status
*        Pointer to the inherited status variable.

*/

/* Local Variables; */
   Prov *child;
   Prov *prov1;
   Prov *prov2;
   int done;
   int i;
   int ichild;
   int j;

/* Check the inherited status value. */
   if( *status != SAI__OK ) return;

/* Loop through every Prov structure in the supplied Provenance. */
   for( i = 0; i < provenance->nprov; i++ ) {
      prov1 = provenance->provs[ i ];
      if( prov1 ) {

/* Check all the remaining Prov structures to see if any of them refer to
   the same NDF. */
         for( j = i + 1; j < provenance->nprov; j++ ) {
            prov2 = provenance->provs[ j ];
            if( prov2 && ndg1TheSame( prov1, prov2, status ) ) {

/* If the two provenance structures refer to the same NDF, break the
   parent-child link for the j'th Prov and register the i'th Prov with
   the parent in place of the j'th Prov. */
               for( ichild = 0; ichild < prov2->nchild; ichild++ ) {
                  child = prov2->children[ ichild ];
                  ndg1Disown( prov2, child, status );
                  ndg1ParentChild( prov1, child, status );
               }
            }
         }
      }
   }

/* Now look for Prov structures that have no children, and remove them. Do
   this repeatedly to go down the chain of parent-child links to the
   roots. */
   done = 0;
   while( !done) {

/* Assume there are no childless entries in the list of Prov structures. */
      done = 1;

/* Loop round all non-NULL entries in the list of Prov structures. */
      for( i = 0; i < provenance->nprov; i++ ) {
         prov1 = provenance->provs[ i ];
         if( prov1 ) {

/* Is this Prov structure without any children? If so, it is not used in
   the creation of any of the other NDFs, so we can remove it, so long as
   it is not the main NDF. */
            if( prov1 != provenance->main && prov1->nchild == 0 ) {

/* First, check all remaining Provs to see if the Prov that we are about
   to remove is a child. If so, break the parent child relationship. */
               for( j = 0; j < provenance->nprov; j++ ) {
                  ndg1Disown( provenance->provs[ j ], prov1, status );
               }

/* Now free the resources used by the Prov. */
               provenance->provs[ i ] = ndg1FreeProv( prov1, status );

/* The above call to ndg1Disown may have produced a new childless Prov
   structure, so indicate we need to check again. */
               done = 0;
            }
         }
      }
   }

/* Shuffle all the remaining Provs down to the beginning of the Prov
   list, and adjust the length of the list. Read from index "i", write to
   index "j". */
   j = 0;
   for( i = 0; i < provenance->nprov; i++ ) {
      if( provenance->provs[ i ] ) {
         provenance->provs[ j++ ] = provenance->provs[ i ];
      }
   }
   provenance->nprov = j;

}

static Provenance *ndg1ReadProvenanceExtension( int indf, const char *creator,
                                                int isroot, int *status ){
/*
*  Name:
*     ndg1ReadProvenanceExtension

*  Purpose:
*     Create a new Provenance structure from an NDF PROVENANCE extension.

*  Synopsis:
*     Provenance *ndg1ReadProvenanceExtension( int indf, const char *creator,
*                                              int isroot, int *status )

*  Description:
*     This function allocates dynamic memory to hold a new Provenance
*     structure, and copies provenance information from the PROVENANCE
*     extension of the supplied NDF into the new Provenance structure.
*     If the NDF does not have a PROVENANCE extension, then the returned
*     Provenance structure contains a only single Prov structure, for the 
*     supplied NDF itself. The only item stored in this Prov structure is
*     the NDF path.

*  Parameters:
*     indf
*        The NDF identifier.
*     creator
*        An optional text string to be stored as the "creator" string in
*        the returned Prov structure. This is only used if the supplied
*        NDF has no creator string in its provenance.
*     isroot
*        If non-zero, then the NDF is treated like a root NDF (that is,
*        information about any parents in the provenance information for
*        the NDF is ignored, and it is assumed the NDF has no parents).
*     status
*        Pointer to the inherited status variable.

*  Returned Value:
*     Pointer to the new Provenance structure. It should be freed using
*     ndg1FreeProvenance when no longer needed.

*/

/* Local Variables: */
   HDSLoc *aloc = NULL;
   HDSLoc *cloc = NULL;
   HDSLoc *mloc = NULL;
   HDSLoc *ploc = NULL;
   HDSLoc *xloc = NULL;
   Prov *anc_prov = NULL;
   Prov *main_prov = NULL;
   Provenance *result = NULL;
   char *date = NULL;
   char *path = NULL;
   char creator_buf[ CREATOR_LEN + 1 ];     
   char date_buf[ DATE_LEN + 1 ];     
   char path_buf[ PATH_LEN + 1 ];     
   hdsdim  dim[1];
   int path_len;
   int there;
   size_t nanc;
   size_t npar;
   int i;
   int j;
   int *parents = NULL;

/* Check the inherited status value. */
   if( *status != SAI__OK ) return result;

/* See if the supplied NDF has a provenance extension. If it has, get a 
   HDS locator to it. */
   ndfXstat( indf, EXT_NAME, &there, status ); 
   if( there && !isroot ) {
      ndfXloc( indf, EXT_NAME, "Read", &xloc, status ); 

/* Get the values of the DATE and CREATOR components, if they exist. */
      date = ndg1GetTextComp( xloc, DATE_NAME, date_buf, 
                              DATE_LEN, status );
      creator = ndg1GetTextComp( xloc, CREATOR_NAME, creator_buf,
                                 CREATOR_LEN, status );

/* Get a locator for the ANCESTORS array, and get its length. */
      datThere( xloc, ANCESTORS_NAME, &there, status );
      if( there ) {
         datFind( xloc, ANCESTORS_NAME, &aloc, status );
         datSize( aloc, &nanc, status );
      }
   }

/* Get the path to the supplied NDF. */
   ndfMsg( "NDF", indf ); 
   msgLoad( "", "^NDF", path_buf, PATH_LEN, &path_len, status );
   path = path_buf;

/* Create a Prov structure to describe the main NDF. */
   main_prov = ndg1MakeProv( path, date, creator, NULL, NULL, status );

/* Create the basic Provenance structure. As yet it only contains
   information about the main NDF. */
   result = ndg1MakeProvenance( main_prov, status );

/* If the PROVENANCE extension specifies any ancestors for the main NDF,
   then create a Prov structure for each and add it into the Provenance
   struture. Links to parents and children are left blank for the moment. */
   if( aloc ) {

/* Loop round the elements of the ANCESTORS array. */
      for( i = 1; i <= nanc; i++ ) {
         dim[ 0 ] = i;
         datCell( aloc, 1, dim, &cloc, status );

/* Get the PATH, DATE and CREATOR components. */
         date = ndg1GetTextComp( cloc, DATE_NAME, date_buf, 
                                 DATE_LEN, status );
         path = ndg1GetTextComp( cloc, PATH_NAME, path_buf,
                                 PATH_LEN, status );
         creator = ndg1GetTextComp( cloc, CREATOR_NAME, creator_buf,
                                 CREATOR_LEN, status );
/* If the ancestor has a MORE structure, get a locator for it. */
         datThere( cloc, MORE_NAME, &there, status );
         if( there ) {
            datFind( cloc, MORE_NAME, &mloc, status );
         } else {
            mloc = NULL;
         }

/* Create a Prov structure to describe the current ancestor, and add it
   into the returned Provenance structure. */
         anc_prov = ndg1MakeProv( path, date, creator, mloc, 
                                  result, status );
/* Free locators. */
         if( mloc ) datAnnul( &mloc, status );
         datAnnul( &cloc, status );
      }

/* Get the list of parents from the main NDF. */
      datThere( xloc, PARENTS_NAME, &there, status );
      if( there ) {
         datFind( xloc, PARENTS_NAME, &ploc, status );
         datMapV( ploc, "_INTEGER", "READ", (void **) &parents, &npar, 
                  status );

/* For each, establish a parent-child link between the main NDF Prov
   structure and the parent Prov structure. The values in the PARENTS
   array are one-based, and the "provs" array is zero-based. However,
   "provs[0]" is the main NDF and so the first ANCESTORS value will be
   stored at "prov1[1]". So this means we can use the PARENT values
   directly without needing to cvhange from one-based to zero-based. */
         for( i = 0; i < npar; i++ ) {
            ndg1ParentChildIndex( result, parents[ i ], 0, status );
         }

/* Free the main NDF parents list. */
         datUnmap( ploc, status );
         datAnnul( &ploc, status );
      }

/* Now loop through the ANCESTORS array again. */
      for( i = 1; i <= nanc; i++ ) {
         dim[ 0 ] = i;
         datCell( aloc, 1, dim, &cloc, status );

/* Get the list of parents from the ancestor NDF. */
         datThere( cloc, PARENTS_NAME, &there, status );
         if( there ) {
            datFind( cloc, PARENTS_NAME, &ploc, status );
            datMapV( ploc, "_INTEGER", "READ", (void **) &parents, &npar, 
                     status );

/* For each, establish a parent-child link between the ancestor NDF Prov
   structure and the parent Prov structure. */
            for( j = 0; j < npar; j++ ) {
               ndg1ParentChildIndex( result, parents[ j ], i, status );
            }

/* Free the main NDF parents list. */
            datUnmap( ploc, status );
            datAnnul( &ploc, status );
         }

/* Free locators. */
         datAnnul( &cloc, status );
      }

   }

/* Free resources */
   if( aloc ) datAnnul( &aloc, status );
   if( xloc ) datAnnul( &xloc, status );

/* If an error occurred, free the result. */
   if( !astOK ) result = ndg1FreeProvenance( result, 1, status );

/* Return the result */
   return result;
}

static int ndg1TheSame( Prov *prov1, Prov *prov2, int *status ) {
/*
*  Name:
*     ndg1TheSame

*  Purpose:
*     Checks if two Prov structures are for the same NDF.

*  Synopsis:
*     int ndg1TheSame( Prov *prov1, Prov *prov2, int *status ) 

*  Description:
*     This function returns non-zero if the two supplied Prov structures
*     describe the same NDF.

*  Parameters:
*     prov1
*        The first Prov structure.
*     prov2
*        The second Prov structure.
*     status
*        Pointer to the inherited status variable.

* Returned Value:
*   One if the twio Prov structures describe the same NDF, and zero
*   otherwise.

*/

/* Local Variables: */
   int result;

/* Initialise. */
   result = 0;

/* Check the inherited status value. */
   if( *status != SAI__OK ) return result;

/* If the pointer are the same, they describe the same ND. */
   if( prov1 == prov2 ) {
      result = 1;

/* Otherwise, we assume a match if the NDF path and the date of the
   provenance information are the same. */
   } else {
      result = 1;
      if( prov1->date ) {
         if( prov2->date ) {
            if( strcmp( prov1->date, prov2->date ) ) result = 0;
         } else {
            result = 0;
         }
      } else if( prov2->date ) {
         result = 0;
      }

      if( prov1->path ) {
         if( prov2->path ) {
            if( strcmp( prov1->path, prov2->path ) ) result = 0;
         } else {
            result = 0;
         }
      } else if( prov2->path ) {
         result = 0;
      }

   }

/* Return the result. */
   return result;
}


static void ndg1WriteProvenanceExtension( Provenance *provenance, int indf, 
                                          int *status ){
/*
*  Name:
*     ndg1WriteProvenanceExtension

*  Purpose:
*     Create a new NDF PROVENANCE extension from a Provenance structure.

*  Synopsis:
*     void ndg1WriteProvenanceExtension( Provenance *provenance, int indf, 
*                                        int *status )

*  Description:
*     This function erases any existing PROVENANCE extension within the
*     supplied NDF, and creates a new one holding the information in the
*     supplied Provenance structure.

*  Parameters:
*     provenance
*        The Provenance structure.
*     indf
*        The NDF identifier.
*     status
*        Pointer to the inherited status variable.

*/

/* Local Variables: */
   HDSLoc *aloc = NULL;
   HDSLoc *cloc = NULL;
   HDSLoc *loc = NULL;
   HDSLoc *more = NULL;
   HDSLoc *xloc = NULL;
   Prov *parent = NULL;
   Prov *prov = NULL;
   char *date = NULL;
   char *path = NULL;
   hdsdim  dim[1];
   int *parents = NULL;
   int i;
   int imain;
   int iparent;
   int k;
   int len;
   int there;
   time_t t;

/* Check the inherited status value. */
   if( *status != SAI__OK ) return;

/* See if the supplied NDF has a provenance extension. If it has, delete
   it. */
   ndfXstat( indf, EXT_NAME, &there, status ); 
   if( there ) ndfXdel( indf, EXT_NAME, status ); 

/* If a Provenance structure was supplied, create a new PROVENANCE
   extension. */
   if( provenance ) {
      ndfXnew( indf, EXT_NAME, EXT_TYPE, 0, NULL, &xloc, status ); 

/* Find the index of the main NDF in the provs array. */
      for( imain = 0; imain < provenance->nprov; imain++ ) {
         if( provenance->provs[ imain ] == provenance->main ) break;
      }

/* Create an ANCESTORS array of the correct length (exclude the main NDF
   Prov structure), and get a locator for it. */
      if( provenance->nprov > 1 ) {
         dim[ 0 ] = provenance->nprov - 1;
         datNew( xloc, ANCESTORS_NAME, ANCESTORS_TYPE,  1, dim, status );
         datFind( xloc, ANCESTORS_NAME, &aloc, status );
      }

/* Loop round all the Prov structures. */
      for( i = 0; i < provenance->nprov; i++ ) {
         prov = provenance->provs[ i ];
      
/* Get a locator to the place where this Prov structure should be
   written. The Prov structure for the main NDF is written to the 
   PROVENANCE extension itself. Other Prov structures are written
   to an element of the ANCESTORS array. */
         if( prov == provenance->main ) {
            datClone( xloc, &cloc, status );
         } else {
            dim[ 0 ] = i;
            datCell( aloc, 1, dim, &cloc, status );
         }

/* If this is not the main NDF provenance, use the PATH and DATE components
   from the Prov structure. */
         if( prov != provenance->main ) {
            path = prov->path;
            date = prov->date;
            more = prov->more;

/* If it is the main NDF provenance, do not write out the PATH (since the
   NDF may subsequently be moved), and force the DATE to the current
   date. Also no "more" information is stored for the main NDF since the
   information that may be stored in "more" will usually be available at 
   other places in the NDF. It is the responsibilty of each application
   to extract the information it thinks is relevant from each input NDF and
   store it in the "more" structure with the input NDF's provenance
   information. */
         } else {
            path = NULL;
            time( &t );
            date = asctime( gmtime( &t ) );
            more = NULL;
         }

/* Store the path. */
         if( path ) {
            len = astChrLen( path );
            path[ len ] = 0;
            datNew0C( cloc, PATH_NAME, len, status );
            datFind( cloc, PATH_NAME, &loc, status );
            datPut0C( loc, path, status );
            datAnnul( &loc, status );
         }

/* Store the date. */
         if( date ) {
            len = astChrLen( date );
            date[ len ] = 0;
            datNew0C( cloc, DATE_NAME, len, status );
            datFind( cloc, DATE_NAME, &loc, status );
            datPut0C( loc, date, status );
            datAnnul( &loc, status );
         }

/* Store the creator (the same code for both main and ancestor NDFs). */
         if( prov->creator ) {
            len = astChrLen( prov->creator );
            prov->creator[ len ] = 0;
            datNew0C( cloc, CREATOR_NAME, len, status );
            datFind( cloc, CREATOR_NAME, &loc, status );
            datPut0C( loc, prov->creator, status );
            datAnnul( &loc, status );
         }

/* Store "more" */
         if( more ) {
            dim[ 0 ] = 1;
            datNew( cloc, MORE_NAME, MORE_TYPE, 1, dim, status );
            datCopy( more, cloc, MORE_NAME, status );
         }

/* Create a PARENTS array of the correct length, and map it. */
         if( prov->nparent > 0 ) {
            datNew1I( cloc, PARENTS_NAME, prov->nparent, status );
            datFind( cloc, PARENTS_NAME, &loc, status );
            dim[ 0 ] = prov->nparent;
            datMapI( loc, "WRITE", 1, dim, &parents, status );

/* Loop round each cell of this array. */
            for( k = 0; k < prov->nparent && *status == SAI__OK; k++ ) {
               parent = prov->parents[ k ];

/* Find the index of this parent in the provs array. */
               for( iparent = 0; iparent < provenance->nprov; iparent++ ) {
                  if( provenance->provs[ iparent ] == parent ) break;
               }

/* If it is greater than the index of the main NDF in "provs" then reduce
   it by one since the main NDF is not stored in the ANCESTORS array. */
               if( iparent > imain ) iparent--;

/* Add 1 to convert from zero-based to one-based, and store this index in 
   the ANCESTORS array. */
               parents[ k ] = iparent + 1;
            }

/* Free the parents array. */
            datUnmap( loc, status );
            datAnnul( &loc, status );
         }

/* Free the locator to the Prov structure. */
         datAnnul( &cloc, status );
      } 

/* Free resources */
      if( aloc ) datAnnul( &aloc, status );
      datAnnul( &xloc, status );
   }

}




