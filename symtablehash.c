/*--------------------------------------------------------------------*/
/* symtablehash.c                                                     */
/* Author: Jacob Penstein                                             */
/*--------------------------------------------------------------------*/

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "symtable.h"
#include <stdio.h>

/* array of bucket count sizes for hash expansion */
static const size_t auBucketCounts[] = {509, 1021, 2039, 4093, 8191, 
16381, 32749, 65521};

/* total number of buckets counts */
static const size_t numBucketCounts = 
sizeof(auBucketCounts)/sizeof(auBucketCounts[0]);

/* Each key/value pair is stored in a Binding. Bindings are each found
in a linked list beginning at a bucket in the hush table. */
struct Binding
{
    /* key */
    char *pcKey;
    /* value */
    void *pvValue;

    /* address of next binding */
    struct Binding *psNextBinding;
};

/* SymTable is a structure that points to the first Binding and tracks
total number of bindings. */
struct SymTable
{
   /* pointer to hash table, an array of pointers to bindings  */
   struct Binding **psHashTable;
   
   /* number of bindings */
   size_t bindings;
   
   /* index to keep track of number of buckets */
   size_t buckets;
};

SymTable_T SymTable_new(void) {
    SymTable_T oSymTable;

    /* Allocates memory for oSymTable and the hash table */
    oSymTable = (SymTable_T)malloc(sizeof(struct SymTable));
    if(oSymTable == NULL) {
        return NULL;
    }
    oSymTable->psHashTable = (struct Binding**)
    calloc(sizeof(struct Binding*), auBucketCounts[0]);
    if(oSymTable->psHashTable == NULL) {
        free(oSymTable);
        return NULL;
    }

    /* initializes parameters of oSymTable */
    oSymTable->bindings = 0;
    oSymTable->buckets = 0;
    
    return oSymTable;
}

/* Return a hash code for pcKey that is between 0 and uBucketCount-1,
inclusive. */
static size_t SymTable_hash(const char *pcKey, size_t uBucketCount) {
    const size_t HASH_MULTIPLIER = 65599;
    size_t u;
    size_t uHash = 0;

    assert(pcKey != NULL);

    for (u = 0; pcKey[u] != '\0'; u++) {
        uHash = uHash * HASH_MULTIPLIER + (size_t)pcKey[u];
    }

    return uHash % uBucketCount;
}

void SymTable_free(SymTable_T oSymTable) {
    struct Binding *psCurrentBinding;
    struct Binding *psNextBinding;
    size_t bucket = 0;

    assert(oSymTable != NULL);

    /* frees each binding in each of the buckets of the hash table */
    while(bucket < auBucketCounts[oSymTable->buckets]) {
        psCurrentBinding = oSymTable->psHashTable[bucket];
        while(psCurrentBinding != NULL) {
            psNextBinding = psCurrentBinding->psNextBinding;
            free(psCurrentBinding->pcKey);
            free(psCurrentBinding);
            psCurrentBinding = psNextBinding;
        }
        bucket++;
    }
    
    /* frees hash table array and symbol table */
    free(oSymTable->psHashTable);
    free(oSymTable);
}

size_t SymTable_getLength(SymTable_T oSymTable) {
    assert(oSymTable != NULL);
    return oSymTable->bindings;
}

static SymTable_T SymTable_ExpandNew(size_t buckets) {
    SymTable_T oSymTable;

    /* Allocates memory for oSymTable and the hash table */
    oSymTable = (SymTable_T)malloc(sizeof(struct SymTable));
    if(oSymTable == NULL) {
        return NULL;
    }
    oSymTable->psHashTable = (struct Binding**)
    calloc(sizeof(struct Binding*), auBucketCounts[buckets]);
    if(oSymTable->psHashTable == NULL) {
        free(oSymTable);
        return NULL;
    }

    /* initializes parameters of oSymTable */
    oSymTable->bindings = 0;
    oSymTable->buckets = buckets;
    
    return oSymTable;
}

static void SymTable_expand(SymTable_T oSymTable) {
    SymTable_T oNewSymTable;
    struct Binding **psNewHashTable;
    struct Binding *psCurrentBinding;
    size_t bucket = 0; 
    int success;
    
    if(oSymTable->buckets == numBucketCounts) {
        return oSymTable;
    }
    
    oNewSymTable = SymTable_ExpandNew((oSymTable->buckets) + 1);
    if(oNewSymTable == NULL) {
        return oSymTable;
    }

    while(bucket < auBucketCounts[oSymTable->buckets]) {
        psCurrentBinding = oSymTable->psHashTable[bucket];
        while(psCurrentBinding != NULL) {
            success = SymTable_put(oNewSymTable, 
            psCurrentBinding->pcKey, psCurrentBinding->pvValue);
            
            if(success == 0) {
                free(oNewSymTable);
                return oSymTable;
            }
        }
        bucket++;
    }
    psNewHashTable = oNewSymTable->psHashTable;
    oNewSymTable->psHashTable = oSymTable->psHashTable;
    oSymTable->psHashTable = psNewHashTable;

    free(oNewSymTable);

}

int SymTable_put(SymTable_T oSymTable, const char *pcKey, 
const void *pvValue) {
    struct Binding *psNewBinding;
    size_t KeyHash;
    
    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    /* checks if the symbol table aready contains the key */
    if(SymTable_contains(oSymTable, pcKey)) {
        return 0;
    }

    if(oSymTable->bindings == auBucketCounts[oSymTable->buckets]) {
        SymTable_expand(oSymTable);
    }

    /* allocates memory for new binding and copy of pcKey */    
    psNewBinding = (struct Binding*)malloc(sizeof(struct Binding));
    if (psNewBinding == NULL) {
        return 0;
    }
    psNewBinding->pcKey = 
    (char*)malloc((strlen(pcKey) + 1));
    if(psNewBinding->pcKey == NULL) {
        free(psNewBinding);  
        return 0;
    }

    /* initializes values of psNewBinding */
    KeyHash = SymTable_hash(pcKey, auBucketCounts[oSymTable->buckets]);
    strcpy(psNewBinding->pcKey, pcKey);
    psNewBinding->pvValue = (void *) pvValue;
    psNewBinding->psNextBinding = 
    (oSymTable->psHashTable)[KeyHash];
    
    /* updates oSymTable parameters */
    (oSymTable->psHashTable)[KeyHash] = psNewBinding;
    (oSymTable->bindings)++;

    return 1;
}

void *SymTable_replace(SymTable_T oSymTable, const char *pcKey,
const void *pvValue) {
    struct Binding *psChecker;
    void *pvTempValue;
    size_t KeyHash;
    
    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    KeyHash = SymTable_hash(pcKey, auBucketCounts[oSymTable->buckets]);
    
    /* checks the appropriate hash bucket for pcKey and replaces the
    value if found*/
    psChecker = oSymTable->psHashTable[KeyHash];
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
    size_t KeyHash;

    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    KeyHash = SymTable_hash(pcKey, auBucketCounts[oSymTable->buckets]);

    /* checks each binding of the appropriate hash bucket for pcKey */
    psChecker = (oSymTable->psHashTable)[KeyHash];
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
    size_t KeyHash;

    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    KeyHash = SymTable_hash(pcKey, auBucketCounts[oSymTable->buckets]);
   
    /* checks the appropriate hash bucket for pcKey and returns value
    if found */
    psChecker = (oSymTable->psHashTable)[KeyHash];
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
    size_t KeyHash;
    
    assert(oSymTable != NULL);
    assert(pcKey != NULL);
    
    KeyHash = SymTable_hash(pcKey, auBucketCounts[oSymTable->buckets]);
    
     /* checks for empty bucket */
    if((oSymTable->psHashTable)[KeyHash] == NULL) {
        return NULL;
    }
    
    /* checks if first binding in appropriate hash bucket contains 
    pcKey. Removes it if it does */
    psCurrent = (oSymTable->psHashTable)[KeyHash];
    if(!strcmp(psCurrent->pcKey, pcKey)) {
        (oSymTable->psHashTable)[KeyHash] = psCurrent->psNextBinding;
        free(psCurrent->pcKey);
        pvTempValue = psCurrent->pvValue;
        free(psCurrent);
        (oSymTable->bindings)--;
        return pvTempValue;
    }

    /* Checks all subsequent bindings in bucket. Removes it if 
    if it contain pcKey. */
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
    size_t bucket = 0;

    assert(oSymTable != NULL);
    assert(pfApply != NULL);

    /* applies pfApply to each binding in every bucket */
    while(bucket < auBucketCounts[oSymTable->buckets]) {
        psCurrentBinding = oSymTable->psHashTable[bucket];
        while(psCurrentBinding != NULL) {
            pfApply(psCurrentBinding->pcKey, psCurrentBinding->pvValue, 
            (char*) pvExtra);
            psCurrentBinding = psCurrentBinding->psNextBinding;
        }
        bucket++;
    }
    
    return;
}