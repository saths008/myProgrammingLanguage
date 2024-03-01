#ifndef c_table_h
#define c_table_h

#include "common.h"
#include "value.h"

typedef struct {
  ObjString *key;
  Value value;
} Entry;

typedef struct {
  int count;
  int capacity;
  Entry *entries;
} HashTable;

void initHashTable(HashTable *hashTable);
void freeHashTable(HashTable *hashTable);
bool tableSet(HashTable *hashTable, ObjString *key, Value value);
void tableAddAll(HashTable *from, HashTable *to);
bool tableGet(HashTable *table, ObjString *key, Value *value);
bool tableDelete(HashTable *table, ObjString *key);
ObjString *tableFindString(HashTable *table, const char *chars, int length,
                           uint32_t hash);
#endif
