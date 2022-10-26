/*--------------------------------------------------------------------*/
/* symtable.h                                                         */
/* Author: Jacob Penstein                                             */
/*--------------------------------------------------------------------*/

#ifndef SYMTABLE_INCLUDED
#define SYMTABLE_INCLUDED

#include <stddef.h>

/* A SymTable_T object stores a collection key/value pairs. */
typedef struct SymTable *SymTable_T;

/* Returns a new SymTable_T object, or NULL if insufficeint memory is 
avalible */
SymTable_T SymTable_new(void);

/* Frees oSymTable. oSymTable cannot be NULL */
void SymTable_free(SymTable_T oSymTable);

/* Returns total number of key/value pairs in oSymTable. oSymTable 
cannot be NULL */
size_t SymTable_getLength(SymTable_T oSymTable);

/* Puts pcKey/pvValue pair into oSymTable. Returns 1 (TRUE) if
successful. Returns 0 (FALSE) if pcKey is already in oSymTable, leaving 
oSymTable unchanged. Also returns 0 if there is insuffient memory. 
oSymTable and pcKey cannot be NULL. Creates a copy of pcKey but not of 
pvValue. */
int SymTable_put(SymTable_T oSymTable, const char *pcKey, 
const void *pvValue);

/* If oSymTable contains a pair with pcKey, replaces its value with
pvValue and returns the old value. If oStymTable does not contain
pcKey, returns NULL. oSymTable and pcKey cannot be NULL. */
void *SymTable_replace(SymTable_T oSymTable, const char *pcKey,
const void *pvValue);

/* Returns 1 (TURE) if o SymTable contains pcKey and 0 (FALSE) if it
does not. oSymTable and pcKey cannot be NULL. */
int SymTable_contains(SymTable_T oSymTable, const char *pcKey);

/* Returns value associated with pcKey in oSymTable. If pcKey is not
in oSymtable, returns NULL. pcKey and oSymTable cannot be NULL. */
void *SymTable_get(SymTable_T oSymTable, const char *pcKey);

/* If oSymTable contains pcKey, removes the key/value pair and returns
the value. If not, does not change oSymTable and returns NULL. 
oSymTable and pcKey cannot be NULL. */
void *SymTable_remove(SymTable_T oSymTable, const char *pcKey);

/* Applies function *pfApply to each key/value pair in oSymTable and 
passes pvExtra as an extra parameter. oSymTable and pfApply cannot be
NULL. */
void SymTable_map(SymTable_T oSymTable,
void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra),
const void *pvExtra);

#endif