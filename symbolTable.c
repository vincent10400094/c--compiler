#ifndef __SYMBOLTABLE_H__
#define __SYMBOLTABLE_H__

#include "symbolTable.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// This file is for reference only, you are not required to follow the implementation. //

int HASH(char *str) {
  int idx = 0;
  while (*str) {
    idx = idx << 1;
    idx += *str;
    str++;
  }
  return (idx & (HASH_TABLE_SIZE - 1));
}

SymbolTable *topSymbolTable;
SymbolTable *firstSymbolTable, *currentSymbolTable;
int maxScope;

SymbolTableEntry *newSymbolTableEntry(int nestingLevel) {
  SymbolTableEntry *symbolTableEntry = (SymbolTableEntry *)malloc(sizeof(SymbolTableEntry));
  symbolTableEntry->nextInHashChain = NULL;
  symbolTableEntry->prevInHashChain = NULL;
  symbolTableEntry->attribute = NULL;
  symbolTableEntry->name = NULL;
  return symbolTableEntry;
}

void initializeSymbolTable() {
  maxScope = 0;
  topSymbolTable = (SymbolTable *)malloc(sizeof(SymbolTable));
  topSymbolTable->prevTable = NULL;
  topSymbolTable->scope = 0;
  firstSymbolTable = currentSymbolTable = topSymbolTable;
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

SymbolTableEntry *insertSymbolTable(SymbolTable *table, char *symbolName, SymbolAttribute *attribute) {
  assert(attribute != NULL && symbolName != NULL);
  int hashkey = HASH(symbolName);
  SymbolTableEntry *newEntry = (SymbolTableEntry *)malloc(sizeof(SymbolTableEntry));
  newEntry->name = strdup(symbolName);
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

SymbolTableEntry *retrieveSymbol(char *symbolName) {
  SymbolTable *currentSymbolTable = topSymbolTable;
  SymbolTableEntry *entry;
  while ((currentSymbolTable != NULL) && ((entry = lookupSymbolTable(currentSymbolTable, symbolName)) == NULL)) {
    currentSymbolTable = currentSymbolTable->prevTable;
  }
  return entry;
}

SymbolTableEntry *enterSymbol(char *symbolName, SymbolAttribute *attribute) {
  return insertSymbolTable(topSymbolTable, symbolName, attribute);
}

void printType(DATA_TYPE type) {
  switch (type) {
    case INT_TYPE: {
      printf("int");
      break;
    }
    case FLOAT_TYPE: {
      printf("float");
      break;
    }
    case VOID_TYPE: {
      printf("void");
      break;
    }
    default: {
      printf("unhandled type");
      break;
    }
  }
}

void printTypeDescriptor(TypeDescriptor *des) {
  if (des->kind == SCALAR_TYPE_DESCRIPTOR) {
    printType(des->properties.dataType);
    putchar(' ');
  } else if (des->kind == ARRAY_TYPE_DESCRIPTOR) {
    printType(des->properties.arrayProperties.elementType);
    putchar(' ');
    for (int i = 0; i < des->properties.arrayProperties.dimension; i++) {
      printf("[%d]", des->properties.arrayProperties.sizeInEachDimension[i]);
    }
  }
}

void printAttribute(SymbolAttribute *attribute) {
  switch (attribute->attributeKind) {
    case VARIABLE_ATTRIBUTE: {
      printf("%s: ", "var");
      TypeDescriptor *des = attribute->attr.typeDescriptor;
      printTypeDescriptor(des);
      break;
    }
    case TYPE_ATTRIBUTE: {
      printf("%s: ", "type");
      TypeDescriptor *des = attribute->attr.typeDescriptor;
      printTypeDescriptor(des);
      break;
    }
    case FUNCTION_SIGNATURE: {
      printf("%s: ", "func");
      printType(attribute->attr.functionSignature->returnType);
      putchar(' ');
      Parameter *ptr = attribute->attr.functionSignature->parameterList;
      putchar('(');
      while (ptr) {
        printTypeDescriptor(ptr->type);
        printf(", ");
        ptr = ptr->next;
      }
      putchar(')');
      break;
    }
  }
}

void printSymbolTable(SymbolTable *table) {
  printf("%-5d\n", table->scope);
  SymbolTableEntry *symptr;
  for (int i = 0; i < HASH_TABLE_SIZE; i++) {
    symptr = table->hashTable[i];
    while (symptr) {
      printf("     %-10s", symptr->name);
      printAttribute(symptr->attribute);
      putchar('\n');
      symptr = symptr->nextInHashChain;
    }
  }
}

void printAllTable() {
  SymbolTable *ptr = firstSymbolTable;
  while (ptr != NULL) {
    puts("---------------------");
    printSymbolTable(ptr);
    ptr = ptr->nxtTable;
  }
}

//remove the symbol from the current scope
void removeSymbol(char *symbolName) {
}

int declaredLocally(char *symbolName) {
  return (lookupSymbolTable(topSymbolTable, symbolName) != NULL);
}

void openScope() {
  SymbolTable *newSymbolTable = (SymbolTable *)malloc(sizeof(SymbolTable));
  newSymbolTable->prevTable = topSymbolTable;
  topSymbolTable = newSymbolTable;
  topSymbolTable->scope = ++maxScope;
  currentSymbolTable->nxtTable = topSymbolTable;
  currentSymbolTable = topSymbolTable;
}

void closeScope() {
  assert(topSymbolTable->prevTable != NULL);
  topSymbolTable = topSymbolTable->prevTable;
}

#endif
