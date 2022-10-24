#include <assert.h>
#include <stdlib.h>
#include "symtable.h"

struct SymTableBinding
{
    char *pcKey;
    void *pvValue;

    struct SymTableBinding *psNextBinding;
};

struct SymTable
{
   struct SymTableBinding *psFirstBinding;
   size_t bindings;
};

SymTable_T SymTable_new(void) {
    SymTable_T oSymTable;

    oSymTable = (SymTable_T)malloc(sizeof(struct SymTable));
    if (oSymTable == NULL) {
        return NULL;
    }

    oSymTable->psFirstBinding = NULL;
    oSymTable->bindings = 0;
    return oSymTable;
}

void SymTable_free(SymTable_T oSymTable) {
    struct SymTableBinding *psCurrentBinding;
    struct SymTableBinding *psNextBinding;

    assert(oSymTable != NULL);

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
    struct SymTableBinding *psNewNode;
    struct SymTableBinding *psChecker;
    
    assert(oSymTable != NULL);
    assert(pcKey != NULL);
    assert(pvValue != NULL);

    psChecker = oSymTable->psFirstBinding;
    while(psChecker != NULL) {
        if(SymTable_contains(oSymTable, pcKey)) {
            return 0;
        }
        psChecker = psChecker->psNextBinding;
    }

    psNewNode = (struct SymTableBinding*)
        malloc(sizeof(struct SymTableBinding));
    if (psNewNode == NULL) {
        return 0;
    }

    psNewNode->pcKey = (char*)malloc(strlen(pcKey));
    
    if(psNewNode->pcKey == NULL || psNewNode->pvValue == NULL) {
        return 0;
    }

    *(psNewNode->pcKey) = strcpy(pcKey);
    psNewNode->pvValue = pvValue;
    psNewNode->psNextBinding = oSymTable->psFirstBinding;
    oSymTable->bindings += 1;

    return 1;
}

void *SymTable_replace(SymTable_T oSymTable, const char *pcKey,
const void *pvValue) {
    struct SymTableBinding *psChecker;
    void *pvTempValue;
    
    assert(oSymTable != NULL);
    assert(pcKey != NULL);
    assert(pvValue != NULL);

    psChecker = oSymTable->psFirstBinding;
    while(psChecker != NULL) {
        if(SymTable_contains(oSymTable, pcKey)) {
            pvTempValue = psChecker->pvValue;
            psChecker->pvValue = pvValue;
            return pvTempValue;
        }
        psChecker = psChecker->psNextBinding;
    }

    return NULL;
}

int SymTable_contains(SymTable_T oSymTable, const char *pcKey) {
    struct SymTableBinding *psChecker;

    assert(oSymTable != NULL);
    assert(pcKey != NULL);

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
    struct SymTableBinding *psChecker;

    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    psChecker = oSymTable->psFirstBinding;
    while(psChecker != NULL) {
        if(SymTable_contains(oSymTable, pcKey)) {
            return psChecker->pvValue;
        }
        psChecker = psChecker->psNextBinding;
    }

    return NULL;
}

void *SymTable_remove(SymTable_T oSymTable, const char *pcKey) {
    struct SymTableBinding *psCurrent;
    struct SymTableBinding *psPrevious;
    void *pvTempValue;
    
    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    psCurrent = oSymTable->psFirstBinding;
    
    if(SymTable_contains(oSymTable, pcKey)) {
        oSymTable->psFirstBinding = psCurrent->psNextBinding;
        free(psCurrent->pcKey);
        pvTempValue = psCurrent->pvValue;
        free(psCurrent);
        oSymTable->bindings -= 1;
        return pvTempValue;
    }

    psPrevious = psCurrent;
    psCurrent = psCurrent->psNextBinding;
    while(psChecker != NULL) {
        if(SymTable_contains(oSymTable, pcKey)) {
            psPrevious->psNextBinding = psCurrent->psNextBinding;
            free(psCurrent->pcKey);
            pvTempValue = psCurrent->pvValue;
            free(psCurrent);
            return pvTempValue;
        }
        psPrevious = psCurrent;
        psCurrent = psCurrent->psNextBinding;
    }

    return NULL;
}

/* void SymTable_map(SymTable_T oSymTable,
void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra),
const void *pvExtra) */