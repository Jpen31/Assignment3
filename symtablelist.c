/*--------------------------------------------------------------------*/
/* symtablelist.c                                                     */
/* Author: Jacob Penstein                                             */
/*--------------------------------------------------------------------*/

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "symtable.h"

/* Each key/value pair is stored in a Binding. Bindings are linked to
form a linked list symbol table. */
struct Binding
{
    /* Key */
    char *pcKey; 
    /* Value */
    void *pvValue; 

    /* Address of next binding */
    struct Binding *psNextBinding; 
};

/* SymTable is a structure that points to the first Binding and tracks
total number of bindings. */
struct SymTable
{
   /* address of first binding */
   struct Binding *psFirstBinding; 
   
   /* number of bindings in the linked list. */
   size_t bindings; 
};

SymTable_T SymTable_new(void) {
    SymTable_T oSymTable;

    /* Allocates memory for oSymTable. */
    oSymTable = (SymTable_T)malloc(sizeof(struct SymTable));
    if (oSymTable == NULL) {
        return NULL;
    }

    /* Initilizes oSymTable parameters. */
    oSymTable->psFirstBinding = NULL;
    oSymTable->bindings = 0;
    return oSymTable;
}

void SymTable_free(SymTable_T oSymTable) {
    struct Binding *psCurrentBinding;
    struct Binding *psNextBinding;

    assert(oSymTable != NULL);

    /* frees each binding in oSymTable */
    for (psCurrentBinding = oSymTable->psFirstBinding;
        psCurrentBinding != NULL;
        psCurrentBinding = psNextBinding)
    {
        psNextBinding = psCurrentBinding->psNextBinding;
        free(psCurrentBinding->pcKey);
        free(psCurrentBinding);
    }

    free(oSymTable);
}

size_t SymTable_getLength(SymTable_T oSymTable) {
    assert(oSymTable != NULL);
    return oSymTable->bindings;
}

int SymTable_put(SymTable_T oSymTable, const char *pcKey, 
const void *pvValue) {
    struct Binding *psNewBinding;
    
    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    if(SymTable_contains(oSymTable, pcKey)) {
        return 0;
    }
    
    /* allocates memory for new binding and for copy of key */
    psNewBinding = (struct Binding*)malloc(sizeof(struct Binding));
    if (psNewBinding == NULL) {
        return 0;
    }
    psNewBinding->pcKey = (char*)calloc(strlen(pcKey) + 1, sizeof(*pcKey));
    if(psNewBinding->pcKey == NULL) {
        free(psNewBinding);  
        return 0;
    }

    /* initializes values for paramters in the binding */
    strcpy(psNewBinding->pcKey, pcKey);
    psNewBinding->pvValue = (void *) pvValue;
    psNewBinding->psNextBinding = oSymTable->psFirstBinding;
    
    /* inserts binding into lists and updates binding total */
    oSymTable->psFirstBinding = psNewBinding;
    (oSymTable->bindings)++;

    return 1;
}

void *SymTable_replace(SymTable_T oSymTable, const char *pcKey,
const void *pvValue) {
    struct Binding *psChecker;
    void *pvTempValue;
    
    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    /* Searches for binding with pcKey. Replaces it if in the table*/
    psChecker = oSymTable->psFirstBinding;
    while(psChecker != NULL) {
        if(!strcmp(psChecker->pcKey, pcKey)) {
            pvTempValue = psChecker->pvValue;
            psChecker->pvValue = (char *) pvValue;
            return pvTempValue;
        }
        psChecker = psChecker->psNextBinding;
    }

    return NULL;
}

int SymTable_contains(SymTable_T oSymTable, const char *pcKey) {
    struct Binding *psChecker;

    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    /* Checks if oSymTable is empty. */
    if(oSymTable->psFirstBinding == NULL) {
        return 0;
    }
    
    /* checks for pcKey in oSYmTable */
    psChecker = oSymTable->psFirstBinding;
    while(psChecker != NULL) {
        if(!strcmp(psChecker->pcKey, pcKey)) {
            return 1;
        }
        psChecker = psChecker->psNextBinding;
    }

    return 0;
}

void *SymTable_get(SymTable_T oSymTable, const char *pcKey) {
    struct Binding *psChecker;

    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    /* checks for empty oSymTable */
    if(oSymTable->psFirstBinding == NULL) {
        return NULL;
    }

    /* checks for binding with pcKey in oSymTable */
    psChecker = oSymTable->psFirstBinding;
    while(psChecker != NULL) {
        if(!strcmp(psChecker->pcKey, pcKey)) {
            return psChecker->pvValue;
        }
        psChecker = psChecker->psNextBinding;
    }

    return NULL;
}

void *SymTable_remove(SymTable_T oSymTable, const char *pcKey) {
    struct Binding *psCurrent;
    struct Binding *psPrevious;
    void *pvTempValue;
    
    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    /* checks for empty oSymTable */
    if(oSymTable->psFirstBinding == NULL) {
        return NULL;
    }
    
    /* checks if first binding contains pcKey. Removes it if it does */
    psCurrent = oSymTable->psFirstBinding;
    if(!strcmp(psCurrent->pcKey, pcKey)) {
        oSymTable->psFirstBinding = psCurrent->psNextBinding;
        free(psCurrent->pcKey);
        pvTempValue = psCurrent->pvValue;
        free(psCurrent);
        (oSymTable->bindings)--;
        return pvTempValue;
    }

    /* Checks all subsequent bindings. Removes it if it contains
    pcKey. */
    psPrevious = psCurrent;
    psCurrent = psCurrent->psNextBinding;
    while(psCurrent != NULL) {
        if(!strcmp(psCurrent->pcKey, pcKey)) {
            psPrevious->psNextBinding = psCurrent->psNextBinding;
            free(psCurrent->pcKey);
            pvTempValue = psCurrent->pvValue;
            free(psCurrent);
            (oSymTable->bindings)--;
            return pvTempValue;
        }
        psPrevious = psCurrent;
        psCurrent = psCurrent->psNextBinding;
    } 

    return NULL;
}

void SymTable_map(SymTable_T oSymTable,
void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra),
const void *pvExtra) {
    struct Binding *psCurrentBinding;

    assert(oSymTable != NULL);
    assert(pfApply != NULL);

    /* applies pfApply to every binding in oSymTable */
    psCurrentBinding = oSymTable->psFirstBinding;
    while(psCurrentBinding != NULL) {
        pfApply(psCurrentBinding->pcKey, psCurrentBinding->pvValue, 
        (char*) pvExtra);
        psCurrentBinding = psCurrentBinding->psNextBinding;
    }

    return;
}