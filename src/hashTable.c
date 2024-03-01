#include <stdlib.h>
#include <string.h>

#include "hashTable.h"
#include "memory.h"
#include "object.h"
#include "value.h"
#define TABLE_MAX_LOAD 0.75
static Entry *findEntry(Entry *entries, int capacity, ObjString *key) {
  uint32_t index = key->hash % capacity;
  Entry *tombstone = NULL;
  for (;;) {
    Entry *entry = &entries[index];

    if (entry->key == NULL) {
      if (IS_NIL(entry->value)) {
        return tombstone != NULL ? tombstone : entry;
      } else {
        if (tombstone == NULL)
          tombstone = entry;
      }
    } else if (entry->key == key) {
      return entry;
    }

    index = (index + 1) % capacity;
  }
}
static void adjustCapacity(HashTable *table, int capacity) {
  Entry *entries = ALLOCATE(Entry, capacity);
  for (int i = 0; i < capacity; i++) {
    entries[i].key = NULL;
    entries[i].value = NIL_VAL;
  }
  table->count = 0;
  for (int i = 0; i < table->capacity; i++) {
    Entry *entry = &table->entries[i];
    if (entry->key == NULL)
      continue;

    Entry *dest = findEntry(entries, capacity, entry->key);
    dest->key = entry->key;
    dest->value = entry->value;
    table->count++;
  }
  FREE_ARRAY(Entry, table->entries, table->capacity);
  table->entries = entries;
  table->capacity = capacity;
}
ObjString *tableFindString(HashTable *table, const char *chars, int length,
                           uint32_t hash) {
  if (table->count == 0)
    return NULL;

  uint32_t index = hash % table->capacity;
  for (;;) {
    Entry *entry = &table->entries[index];
    if (entry->key == NULL) {
      if (IS_NIL(entry->value))
        return NULL;
    } else if (entry->key->length == length && entry->key->hash == hash &&
               memcmp(entry->key->chars, chars, length) == 0) {
      return entry->key;
    }

    index = (index + 1) % table->capacity;
  }
}
void tableAddAll(HashTable *from, HashTable *to) {
  for (int i = 0; i < from->capacity; i++) {
    Entry *entry = &from->entries[i];
    if (entry->key != NULL) {
      tableSet(to, entry->key, entry->value);
    }
  }
}
bool tableSet(HashTable *hashTable, ObjString *key, Value value) {
  if (hashTable->count + 1 > hashTable->capacity * TABLE_MAX_LOAD) {
    int capacity = CALCULATE_NEW_CAPACITY(hashTable->capacity);
    adjustCapacity(hashTable, capacity);
  }

  Entry *entry = findEntry(hashTable->entries, hashTable->capacity, key);
  bool isNewKey = entry->key == NULL;
  if (isNewKey && IS_NIL(entry->value))
    hashTable->count++;

  entry->key = key;
  entry->value = value;
  return isNewKey;
}
bool tableGet(HashTable *table, ObjString *key, Value *value) {
  if (table->count == 0)
    return false;

  Entry *entry = findEntry(table->entries, table->capacity, key);
  if (entry->key == NULL)
    return false;

  *value = entry->value;
  return true;
}
bool tableDelete(HashTable *table, ObjString *key) {
  if (table->count == 0)
    return false;

  Entry *entry = findEntry(table->entries, table->capacity, key);
  if (entry->key == NULL)
    return false;

  entry->key = NULL;
  entry->value = BOOL_VAL(true);
  return true;
}
void initHashTable(HashTable *hashTable) {
  hashTable->count = 0;
  hashTable->capacity = 0;
  hashTable->entries = NULL;
}
void freeHashTable(HashTable *hashTable) {
  FREE_ARRAY(Entry, hashTable->entries, hashTable->capacity);
  initHashTable(hashTable);
}
