#include "symtable.h"

static const size_t auBucketCounts[] = {509, 1021, 2039, 4093, 8191, 
16381, 32749, 65521};
static const size_t numBucketCounts = 
sizeof(auBucketCounts)/sizeof(auBucketCounts[0]);

struct Binding
{
    char *pcKey;
    void *pvValue;

    struct Binding *psNextBinding;
};

struct SymTable
{
   struct Binding **psHashTable;
   size_t bindings;
   size_t buckets;
};

SymTable_T SymTable_new(void) {
    SymTable_T oSymTable;

    oSymTable = (SymTable_T)malloc(sizeof(struct SymTable));
    if(oSymTable == NULL) {
        return NULL;
    }

    oSymTable->psHashTable = (struct Binding**)
    malloc(sizeof(struct Binding*) * auBucketCounts[0]);
    if(oSymTable->psHashTable == NULL) {
        free(oSymTable);
        return NULL;
    }

    oSymTable->bindings = 0;
    oSymTable->buckets = 0;
    return oSymTable;
}

/* Return a hash code for pcKey that is between 0 and uBucketCount-1,
inclusive. */
static size_t SymTable_hash(const char *pcKey, size_t uBucketCount){
    const size_t HASH_MULTIPLIER = 65599;
    size_t u;
    size_t uHash = 0;

    assert(pcKey != NULL);

    for (u = 0; pcKey[u] != '\0'; u++) {
        uHash = uHash * HASH_MULTIPLIER + (size_t)pcKey[u];
    }

    return uHash % uBucketCount;
}

void SymTable_free(SymTable_T oSymTable) {}

size_t SymTable_getLength(SymTable_T oSymTable) {
    assert(oSymTable != NULL);
    return oSymTable->bindings;
}

int SymTable_put(SymTable_T oSymTable, const char *pcKey, 
const void *pvValue) {
    struct Binding *psNewNode;
    size_t KeyHash;
    
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

    KeyHash = SymTable_hash(pcKey, auBucketCounts[oSymTable->buckets]);
    strcpy(psNewNode->pcKey, pcKey);
    psNewNode->pvValue = (char *) pvValue;
    psNewNode->psNextBinding = (oSymTable->psHashTable)[oSymTable->buckets];
    (oSymTable->psHashTable)[oSymTable->buckets] = psNewNode;
    (oSymTable->bindings)++;

    return 1;
}

void *SymTable_replace(SymTable_T oSymTable, const char *pcKey,
const void *pvValue) {}

int SymTable_contains(SymTable_T oSymTable, const char *pcKey) {
    struct Binding *psChecker;
    size_t KeyHash;

    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    KeyHash = SymTable_hash(pcKey, auBucketCounts[oSymTable->buckets]);

    if(oSymTable->psHashTable == NULL) {
        return 0;
    }

    psChecker = (oSymTable->psHashTable)[KeyHash];
    while(psChecker != NULL) {
        if(!strcmp(psChecker->pcKey, pcKey)) {
            return 1;
        }
        psChecker = psChecker->psNextBinding;
    }

    return 0;
}

void *SymTable_get(SymTable_T oSymTable, const char *pcKey) {}

void *SymTable_remove(SymTable_T oSymTable, const char *pcKey) {}

void SymTable_map(SymTable_T oSymTable,
void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra),
const void *pvExtra) {}