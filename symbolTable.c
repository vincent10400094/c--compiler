#include "symbolTable.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
// This file is for reference only, you are not required to follow the implementation. //

int HASH(char* str) {
  int idx = 0;
  while (*str) {
    idx = idx << 1;
    idx += *str;
    str++;
  }
  return (idx & (HASH_TABLE_SIZE - 1));
}

SymbolTable *topSymbolTable;
int maxScope;

SymbolTableEntry* newSymbolTableEntry(int nestingLevel) {
  SymbolTableEntry* symbolTableEntry = (SymbolTableEntry*)malloc(sizeof(SymbolTableEntry));
  symbolTableEntry->nextInHashChain = NULL;
  symbolTableEntry->prevInHashChain = NULL;
  symbolTableEntry->attribute = NULL;
  symbolTableEntry->name = NULL;
  return symbolTableEntry;
}

void initializeSymbolTable() {
  topSymbolTable = NULL;
  maxScope = 0;
  topSymbolTable = (SymbolTable*)malloc(sizeof(SymbolTable));
  topSymbolTable->prevInStack = NULL;
  topSymbolTable->scope = 0;
}

void symbolTableEnd() {

}

SymbolTableEntry *lookupSymbolTable(SymbolTable *table, char *symbolname) {
  assert(symbolname != NULL);
  int hashkey = HASH(symbolname);
  SymbolTableEntry *symptr = table->hashTable[hashkey];
  while (symptr) {
    if (!strcmp(symbolname, symptr->name))
      return symptr;
    symptr = symptr->nextInHashChain;
  }
  return NULL;
}

SymbolTableEntry *insertSymbolTalbe(SymbolTable *table, char* symbolName, SymbolAttribute* attribute) {
  assert(attribute != NULL && symbolName != NULL);
  int hashkey = HASH(symbolName);
  SymbolTableEntry *newEntry = (SymbolTableEntry*)malloc(sizeof(SymbolTableEntry));
  strcpy(newEntry->name, symbolName);
  newEntry->attribute = attribute;
  newEntry->scope = table->scope;
  SymbolTableEntry *symptr = table->hashTable[hashkey];
  if (symptr == NULL) {
    table->hashTable[hashkey] = newEntry;
    newEntry->nextInHashChain = NULL;
  } else {
    newEntry->nextInHashChain = symptr;
    table->hashTable[hashkey] = newEntry;
  }
  return newEntry;
}

SymbolTableEntry* retrieveSymbol(char* symbolName) {
  SymbolTable *currentSymbolTable = topSymbolTable;
  SymbolTableEntry* entry;
  while ((currentSymbolTable != NULL) && ((entry = lookupSymbolTable(currentSymbolTable, symbolName)) == NULL)) {
    currentSymbolTable = currentSymbolTable->prevInStack;
  }
  return entry;
}

SymbolTableEntry* enterSymbol(char* symbolName, SymbolAttribute* attribute) {
  return insertSymbolTalbe(topSymbolTable, symbolName, attribute);
}

//remove the symbol from the current scope
void removeSymbol(char* symbolName) {

}

int declaredLocally(char* symbolName) {
  return (lookupSymbolTable(topSymbolTable, symbolName) != NULL);
}

void openScope() {
  SymbolTable *newSymbolTable = (SymbolTable*)malloc(sizeof(SymbolTable));
  newSymbolTable->prevInStack = topSymbolTable;
  topSymbolTable = newSymbolTable;
  topSymbolTable->scope = ++maxScope;
}

void closeScope() {
  assert(topSymbolTable->prevInStack != NULL);
  topSymbolTable = topSymbolTable->prevInStack;
}
