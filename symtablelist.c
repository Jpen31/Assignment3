#include "symtable.h"

struct Binding
{
    char *pcKey;
    void *pvValue;

    struct Binding *psNextBinding;
};

struct SymTable
{
   struct Binding *psFirstBinding;
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
    struct Binding *psCurrentBinding;
    struct Binding *psNextBinding;

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
    struct Binding *psNewNode;
    
    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    if(SymTable_contains(oSymTable, pcKey)) {
        return 0;
    }
        
    psNewNode = (struct Binding*)malloc(sizeof(struct Binding));
    if (psNewNode == NULL) {
        return 0;
    }

    psNewNode->pcKey = (char*)calloc(strlen(pcKey) + 1, sizeof(*pcKey));
    if(psNewNode->pcKey == NULL) {
        free(psNewNode);  
        return 0;
    }

    strcpy(psNewNode->pcKey, pcKey);
    psNewNode->pvValue = (char *) pvValue;
    psNewNode->psNextBinding = oSymTable->psFirstBinding;
    oSymTable->psFirstBinding = psNewNode;
    (oSymTable->bindings)++;

    return 1;
}

void *SymTable_replace(SymTable_T oSymTable, const char *pcKey,
const void *pvValue) {
    struct Binding *psChecker;
    void *pvTempValue;
    
    assert(oSymTable != NULL);
    assert(pcKey != NULL);

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

    if(oSymTable->psFirstBinding == NULL) {
        return 0;
    }

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

    if(oSymTable->psFirstBinding == NULL) {
        return NULL;
    }

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

    if(oSymTable->psFirstBinding == NULL) {
        return NULL;
    }
    
    psCurrent = oSymTable->psFirstBinding;
    
    if(!strcmp(psCurrent->pcKey, pcKey)) {
        oSymTable->psFirstBinding = psCurrent->psNextBinding;
        free(psCurrent->pcKey);
        pvTempValue = psCurrent->pvValue;
        free(psCurrent);
        (oSymTable->bindings)--;
        return pvTempValue;
    }

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

    psCurrentBinding = oSymTable->psFirstBinding;
    while(psCurrentBinding != NULL) {
        pfApply(psCurrentBinding->pcKey, psCurrentBinding->pvValue, 
        (char*) pvExtra);
        psCurrentBinding = psCurrentBinding->psNextBinding;
    }

    return;
}