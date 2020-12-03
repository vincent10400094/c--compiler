#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "header.h"
#include "symbolTable.h"
// This file is for reference only, you are not required to follow the implementation. //
// You only need to check for errors stated in the hw4 document. //
int g_anyErrorOccur = 0;

DATA_TYPE getBiggerType(DATA_TYPE dataType1, DATA_TYPE dataType2);
void processProgramNode(AST_NODE* programNode);
void processDeclarationNode(AST_NODE* declarationNode);
void declareIdList(AST_NODE* typeNode, SymbolAttributeKind isVariableOrTypeAttribute, int ignoreArrayFirstDimSize);
void variableDeclareList(AST_NODE* declarationNode);
void declareFunction(AST_NODE* returnTypeNode);
void processDeclDimList(AST_NODE* variableDeclDimList, TypeDescriptor* typeDescriptor, int ignoreFirstDimSize);
void processTypeNode(AST_NODE* typeNode);
void processBlockNode(AST_NODE* blockNode);
void processStmtNode(AST_NODE* stmtNode);
void processGeneralNode(AST_NODE* node);
void checkAssignOrExpr(AST_NODE* assignOrExprRelatedNode);
void checkWhileStmt(AST_NODE* whileNode);
void checkForStmt(AST_NODE* forNode);
void checkAssignmentStmt(AST_NODE* assignmentNode);
void checkIfStmt(AST_NODE* ifNode);
void checkWriteFunction(AST_NODE* functionCallNode);
void checkFunctionCall(AST_NODE* functionCallNode);
void processExprRelatedNode(AST_NODE* exprRelatedNode);
void checkParameterPassing(Parameter* formalParameter, AST_NODE* actualParameter);
void checkReturnStmt(AST_NODE* returnNode);
void processExprNode(AST_NODE* exprNode);
void processVariableLValue(AST_NODE* idNode);
void processVariableRValue(AST_NODE* idNode);
void processConstValueNode(AST_NODE* constValueNode);
void getExprOrConstValue(AST_NODE* exprOrConstNode, int* iValue, float* fValue);
void evaluateExprValue(AST_NODE* exprNode);

typedef enum ErrorMsgKind {
  SYMBOL_IS_NOT_TYPE,
  SYMBOL_REDECLARE,
  SYMBOL_UNDECLARED,
  NOT_FUNCTION_NAME,
  TRY_TO_INIT_ARRAY,
  EXCESSIVE_ARRAY_DIM_DECLARATION,
  RETURN_ARRAY,
  VOID_VARIABLE,
  TYPEDEF_VOID_ARRAY,
  PARAMETER_TYPE_UNMATCH,
  TOO_FEW_ARGUMENTS,
  TOO_MANY_ARGUMENTS,
  RETURN_TYPE_UNMATCH,
  INCOMPATIBLE_ARRAY_DIMENSION,
  NOT_ASSIGNABLE,
  NOT_ARRAY,
  IS_TYPE_NOT_VARIABLE,
  IS_FUNCTION_NOT_VARIABLE,
  STRING_OPERATION,
  ARRAY_SIZE_NOT_INT,
  ARRAY_SIZE_NEGATIVE,
  ARRAY_SUBSCRIPT_NOT_INT,
  PASS_ARRAY_TO_SCALAR,
  PASS_SCALAR_TO_ARRAY
} ErrorMsgKind;

void printErrorMsgSpecial(AST_NODE* node1, char* name2, ErrorMsgKind errorMsgKind) {
  g_anyErrorOccur = 1;
  // printf("Error found in line %d\n", node1->linenumber);

  printf(ANSI_COLOR_RED "error: " ANSI_COLOR_RESET);

  switch (errorMsgKind) {
    case SYMBOL_REDECLARE: {
      printf("redefinition of '%s'\n", name2);
      break;
    }
    case SYMBOL_UNDECLARED: {
      printf("use of undeclared identifier '%s'\n", name2);
      break;
    }
    case TOO_FEW_ARGUMENTS: {
      printf("no matching function for call '%s'\n", name2);
      break;
    }
    case TOO_MANY_ARGUMENTS: {
      printf("no matching function for call '%s'\n", name2);
      break;
    }
    case PASS_ARRAY_TO_SCALAR: {
      printf("no matching function for call '%s'\n", name2);
      break;
    }
    case PASS_SCALAR_TO_ARRAY: {
      printf("no matching function for call '%s'\n", name2);
      break;
    }
    default: {
      printf("Unhandled case in void printErrorMsg(AST_NODE* node, ERROR_MSG_KIND* errorMsgKind)\n");
      break;
    }
  }
}

void printErrorMsg(AST_NODE* node, ErrorMsgKind errorMsgKind) {
  g_anyErrorOccur = 1;
  printf("Error found in line %d\n", node->linenumber);

  switch (errorMsgKind) {
    case ARRAY_SUBSCRIPT_NOT_INT: {
      printf("array subscript is not an integer\n");
      break;
    }
    default: {
      printf("Unhandled case in void printErrorMsg(AST_NODE* node, ERROR_MSG_KIND* errorMsgKind)\n");
      break;
    }
  }
}

void semanticAnalysis(AST_NODE* root) {
  processProgramNode(root);
}

DATA_TYPE getBiggerType(DATA_TYPE dataType1, DATA_TYPE dataType2) {
  if (dataType1 == FLOAT_TYPE || dataType2 == FLOAT_TYPE) {
    return FLOAT_TYPE;
  } else {
    return INT_TYPE;
  }
}

void processProgramNode(AST_NODE* programNode) {
  initializeSymbolTable();
  openScope();
  AST_NODE* node = programNode->child;
  while (node) {
    if (node->nodeType == VARIABLE_DECL_LIST_NODE) {
      variableDeclareList(node);
    } else if (node->nodeType == DECLARATION_NODE) {
    }
    node = node->rightSibling;
  }
  // printAllTable();
  closeScope();
}

void processDeclarationNode(AST_NODE* declarationNode) {
  AST_NODE* node = declarationNode;

  switch (declarationNode->semantic_value.declSemanticValue.kind) {
    case VARIABLE_DECL: {
      declareIdList(declarationNode, VARIABLE_ATTRIBUTE, 0);
      break;
    }
    case TYPE_DECL: {
      declareIdList(declarationNode, TYPE_ATTRIBUTE, 0);
      break;
    }
    case FUNCTION_DECL: {
      break;
    }
    case FUNCTION_PARAMETER_DECL: {
      break;
    }
    default: {
      break;
    }
  }
}

void processTypeNode(AST_NODE* idNodeAsType) {
}

void declareIdList(AST_NODE* declarationNode, SymbolAttributeKind isVariableOrTypeAttribute, int ignoreArrayFirstDimSize) {
  switch (isVariableOrTypeAttribute) {
    case VARIABLE_ATTRIBUTE: {
      AST_NODE* node = declarationNode->child;
      DATA_TYPE data_type = (strcmp("int", node->semantic_value.identifierSemanticValue.identifierName) == 0 ? INT_TYPE : FLOAT_TYPE);
      node = node->rightSibling;
      while (node) {
        if (node->semantic_value.identifierSemanticValue.kind == NORMAL_ID) {
          TypeDescriptor type_descriptor;
          type_descriptor.kind = SCALAR_TYPE_DESCRIPTOR;
          type_descriptor.properties.dataType = data_type;
          SymbolAttribute symbol_attr;
          symbol_attr.attributeKind = VARIABLE_ATTRIBUTE;
          symbol_attr.attr.typeDescriptor = &type_descriptor;
          node->semantic_value.identifierSemanticValue.symbolTableEntry = enterSymbol(node->semantic_value.identifierSemanticValue.identifierName, &symbol_attr);
        } else if (node->semantic_value.identifierSemanticValue.kind == ARRAY_ID) {
          TypeDescriptor type_descriptor;
          type_descriptor.kind = ARRAY_TYPE_DESCRIPTOR;
          int dimension = 0;
          AST_NODE* dimensionNode = node->child;
          while (dimensionNode) {
            processExprNode(dimensionNode);
            if (dimensionNode->semantic_value.const1->const_type == INTEGERC) {
              type_descriptor.properties.arrayProperties.sizeInEachDimension[dimension] = dimensionNode->semantic_value.const1->const_u.intval;
              dimension++;
            }
            else {
              //error : dimension must be int
            }
            dimensionNode = dimensionNode->rightSibling;
          }
          type_descriptor.properties.arrayProperties.dimension = dimension;
          type_descriptor.properties.arrayProperties.elementType = data_type;
          SymbolAttribute symbol_attr;
          symbol_attr.attributeKind = VARIABLE_ATTRIBUTE;
          symbol_attr.attr.typeDescriptor = &type_descriptor;
          node->semantic_value.identifierSemanticValue.symbolTableEntry = enterSymbol(node->semantic_value.identifierSemanticValue.identifierName, &symbol_attr);
        }
        node = node->rightSibling;
      }
      break;
    }
    case TYPE_ATTRIBUTE: {
      break;
    }
    default: {
      break;
    }
  }
}

void variableDeclareList(AST_NODE* declarationNode) {
  AST_NODE* node = declarationNode->child;
  while (node) {
    if (node->nodeType == DECLARATION_NODE) {
      processDeclarationNode(node);
    } else {
      fprintf(stderr, "it should not happen.");
    }
    node = node->rightSibling;
  }
}

void parameterList() {
}

void checkAssignOrExpr(AST_NODE* assignOrExprRelatedNode) {
}

void checkWhileStmt(AST_NODE* whileNode) {
}

void checkForStmt(AST_NODE* forNode) {
}

void checkAssignmentStmt(AST_NODE* assignmentNode) {
}

void checkIfStmt(AST_NODE* ifNode) {
}

void checkWriteFunction(AST_NODE* functionCallNode) {
}

void checkFunctionCall(AST_NODE* functionCallNode) {
}

void checkParameterPassing(Parameter* formalParameter, AST_NODE* actualParameter) {
}

void processExprRelatedNode(AST_NODE* exprRelatedNode) {
}

void getExprOrConstValue(AST_NODE* exprOrConstNode, int* iValue, float* fValue) {
}

void evaluateExprValue(AST_NODE* exprNode) {
}

void processExprNode(AST_NODE* exprNode) {
}

void processVariableLValue(AST_NODE* idNode) {
}

void processVariableRValue(AST_NODE* idNode) {
}

void processConstValueNode(AST_NODE* constValueNode) {
}

void checkReturnStmt(AST_NODE* returnNode) {
}

void processBlockNode(AST_NODE* blockNode) {
}

void processStmtNode(AST_NODE* stmtNode) {
}

void processGeneralNode(AST_NODE* node) {
}

void processDeclDimList(AST_NODE* idNode, TypeDescriptor* typeDescriptor, int ignoreFirstDimSize) {
}

void declareFunction(AST_NODE* declarationNode) {
}
