/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.3"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     ID = 258,
     CONST = 259,
     VOID = 260,
     INT = 261,
     FLOAT = 262,
     IF = 263,
     ELSE = 264,
     WHILE = 265,
     FOR = 266,
     TYPEDEF = 267,
     OP_ASSIGN = 268,
     OP_OR = 269,
     OP_AND = 270,
     OP_NOT = 271,
     OP_EQ = 272,
     OP_NE = 273,
     OP_GT = 274,
     OP_LT = 275,
     OP_GE = 276,
     OP_LE = 277,
     OP_PLUS = 278,
     OP_MINUS = 279,
     OP_TIMES = 280,
     OP_DIVIDE = 281,
     MK_LB = 282,
     MK_RB = 283,
     MK_LPAREN = 284,
     MK_RPAREN = 285,
     MK_LBRACE = 286,
     MK_RBRACE = 287,
     MK_COMMA = 288,
     MK_SEMICOLON = 289,
     MK_DOT = 290,
     ERROR = 291,
     RETURN = 292
   };
#endif
/* Tokens.  */
#define ID 258
#define CONST 259
#define VOID 260
#define INT 261
#define FLOAT 262
#define IF 263
#define ELSE 264
#define WHILE 265
#define FOR 266
#define TYPEDEF 267
#define OP_ASSIGN 268
#define OP_OR 269
#define OP_AND 270
#define OP_NOT 271
#define OP_EQ 272
#define OP_NE 273
#define OP_GT 274
#define OP_LT 275
#define OP_GE 276
#define OP_LE 277
#define OP_PLUS 278
#define OP_MINUS 279
#define OP_TIMES 280
#define OP_DIVIDE 281
#define MK_LB 282
#define MK_RB 283
#define MK_LPAREN 284
#define MK_RPAREN 285
#define MK_LBRACE 286
#define MK_RBRACE 287
#define MK_COMMA 288
#define MK_SEMICOLON 289
#define MK_DOT 290
#define ERROR 291
#define RETURN 292




/* Copy the first part of user declarations.  */
#line 3 "parser.y"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "header.h"
#include "symbolTable.h"
int linenumber = 1;
AST_NODE *prog;

extern int g_anyErrorOccur;

static inline AST_NODE* makeSibling(AST_NODE *a, AST_NODE *b)
{ 
    while (a->rightSibling) {
        a = a->rightSibling;
    }
    if (b == NULL) {
        return a;
    }
    b = b->leftmostSibling;
    a->rightSibling = b;
    
    b->leftmostSibling = a->leftmostSibling;
    b->parent = a->parent;
    while (b->rightSibling) {
        b = b->rightSibling;
        b->leftmostSibling = a->leftmostSibling;
        b->parent = a->parent;
    }
    return b;
}

static inline AST_NODE* makeChild(AST_NODE *parent, AST_NODE *child)
{
    if (child == NULL) {
        return parent;
    }
    if (parent->child) {
        makeSibling(parent->child, child);
    } else {
        child = child->leftmostSibling;
        parent->child = child;
        while (child) {
            child->parent = parent;
            child = child->rightSibling;
        }
    }
    return parent;
}

static AST_NODE* makeFamily(AST_NODE *parent, int childrenCount, ...)
{
    va_list childrenList;
    va_start(childrenList, childrenCount);
    AST_NODE* child = va_arg(childrenList, AST_NODE*);
    makeChild(parent, child);
    AST_NODE* tmp = child;
    int index = 1;
    for (index = 1; index < childrenCount; ++index) {
        child = va_arg(childrenList, AST_NODE*);
        tmp = makeSibling(tmp, child);
    }
    va_end(childrenList);
    return parent;
}

static inline AST_NODE* makeIDNode(char *lexeme, IDENTIFIER_KIND idKind)
{
    AST_NODE* identifier = Allocate(IDENTIFIER_NODE);
    identifier->semantic_value.identifierSemanticValue.identifierName = lexeme;
    identifier->semantic_value.identifierSemanticValue.kind = idKind;
    identifier->semantic_value.identifierSemanticValue.symbolTableEntry = NULL;
    return identifier;                        
}

static inline AST_NODE* makeStmtNode(STMT_KIND stmtKind)
{
    AST_NODE* stmtNode = Allocate(STMT_NODE);
    stmtNode->semantic_value.stmtSemanticValue.kind = stmtKind;
    return stmtNode;                        
}

static inline AST_NODE* makeDeclNode(DECL_KIND declKind)
{
    AST_NODE* declNode = Allocate(DECLARATION_NODE);
    declNode->semantic_value.declSemanticValue.kind = declKind;
    return declNode;                        
}

static inline AST_NODE* makeExprNode(EXPR_KIND exprKind, int operationEnumValue)
{
    AST_NODE* exprNode = Allocate(EXPR_NODE);
    exprNode->semantic_value.exprSemanticValue.isConstEval = 0;
    exprNode->semantic_value.exprSemanticValue.kind = exprKind;
    if (exprKind == BINARY_OPERATION) {
        exprNode->semantic_value.exprSemanticValue.op.binaryOp = operationEnumValue;
    } else if (exprKind == UNARY_OPERATION) {
        exprNode->semantic_value.exprSemanticValue.op.unaryOp = operationEnumValue;
    } else {
        printf("Error in static inline AST_NODE* makeExprNode(EXPR_KIND exprKind, int operationEnumValue)\n");
    }
    return exprNode;                        
}



/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 112 "parser.y"
{
	char *lexeme;
	CON_Type  *const1;
	AST_NODE  *node;
}
/* Line 193 of yacc.c.  */
#line 283 "parser.tab.c"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 296 "parser.tab.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int i)
#else
static int
YYID (i)
    int i;
#endif
{
  return i;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  20
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   282

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  38
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  41
/* YYNRULES -- Number of rules.  */
#define YYNRULES  115
/* YYNRULES -- Number of states.  */
#define YYNSTATES  229

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   292

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     6,     9,    11,    14,    16,    25,
      34,    43,    51,    59,    67,    71,    73,    76,    79,    83,
      87,    91,    96,    98,    99,   102,   104,   106,   107,   110,
     112,   114,   116,   121,   126,   130,   134,   136,   138,   140,
     144,   149,   152,   156,   161,   165,   169,   171,   175,   179,
     181,   183,   187,   189,   193,   195,   198,   202,   205,   207,
     211,   217,   227,   232,   238,   246,   252,   254,   257,   261,
     263,   264,   268,   270,   272,   276,   278,   280,   284,   286,
     290,   292,   296,   298,   300,   302,   304,   306,   308,   310,
     311,   315,   317,   321,   323,   325,   327,   331,   333,   335,
     337,   341,   346,   351,   353,   356,   359,   364,   370,   376,
     378,   381,   384,   386,   389,   394
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      39,     0,    -1,    40,    -1,    -1,    40,    41,    -1,    41,
      -1,    48,    42,    -1,    42,    -1,    52,     3,    29,    43,
      30,    31,    47,    32,    -1,     5,     3,    29,    43,    30,
      31,    47,    32,    -1,     3,     3,    29,    43,    30,    31,
      47,    32,    -1,    52,     3,    29,    30,    31,    47,    32,
      -1,     5,     3,    29,    30,    31,    47,    32,    -1,     3,
       3,    29,    30,    31,    47,    32,    -1,    43,    33,    44,
      -1,    44,    -1,    52,     3,    -1,     3,     3,    -1,    52,
       3,    45,    -1,     3,     3,    45,    -1,    27,    46,    28,
      -1,    45,    27,    72,    28,    -1,    72,    -1,    -1,    48,
      60,    -1,    60,    -1,    48,    -1,    -1,    48,    49,    -1,
      49,    -1,    50,    -1,    51,    -1,    12,    52,    53,    34,
      -1,    12,     5,    53,    34,    -1,    52,    58,    34,    -1,
       3,    53,    34,    -1,     6,    -1,     7,    -1,     3,    -1,
      53,    33,     3,    -1,    53,    33,     3,    54,    -1,     3,
      54,    -1,    27,    55,    28,    -1,    54,    27,    55,    28,
      -1,    55,    23,    56,    -1,    55,    24,    56,    -1,    56,
      -1,    56,    25,    57,    -1,    56,    26,    57,    -1,    57,
      -1,     4,    -1,    29,    55,    30,    -1,    59,    -1,    58,
      33,    59,    -1,     3,    -1,     3,    54,    -1,     3,    13,
      66,    -1,    60,    61,    -1,    61,    -1,    31,    47,    32,
      -1,    10,    29,    64,    30,    61,    -1,    11,    29,    62,
      34,    70,    34,    62,    30,    61,    -1,    77,    13,    66,
      34,    -1,     8,    29,    64,    30,    61,    -1,     8,    29,
      64,    30,    61,     9,    61,    -1,     3,    29,    70,    30,
      34,    -1,    34,    -1,    37,    34,    -1,    37,    66,    34,
      -1,    63,    -1,    -1,    63,    33,    65,    -1,    65,    -1,
      65,    -1,     3,    13,    66,    -1,    66,    -1,    67,    -1,
      66,    14,    67,    -1,    68,    -1,    67,    15,    68,    -1,
      72,    -1,    72,    69,    72,    -1,    17,    -1,    21,    -1,
      22,    -1,    18,    -1,    19,    -1,    20,    -1,    71,    -1,
      -1,    71,    33,    66,    -1,    66,    -1,    72,    73,    74,
      -1,    74,    -1,    23,    -1,    24,    -1,    74,    75,    76,
      -1,    76,    -1,    25,    -1,    26,    -1,    29,    66,    30,
      -1,    24,    29,    66,    30,    -1,    16,    29,    66,    30,
      -1,     4,    -1,    24,     4,    -1,    16,     4,    -1,     3,
      29,    70,    30,    -1,    24,     3,    29,    70,    30,    -1,
      16,     3,    29,    70,    30,    -1,    77,    -1,    24,    77,
      -1,    16,    77,    -1,     3,    -1,     3,    78,    -1,    78,
      27,    72,    28,    -1,    27,    72,    28,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   166,   166,   167,   170,   174,   180,   184,   190,   197,
     205,   213,   219,   226,   235,   239,   245,   250,   255,   260,
     266,   270,   276,   281,   286,   291,   296,   301,   306,   310,
     316,   320,   326,   331,   339,   344,   351,   355,   361,   365,
     369,   373,   378,   382,   387,   392,   397,   402,   407,   412,
     418,   423,   429,   433,   439,   443,   447,   453,   457,   465,
     469,   474,   479,   484,   489,   494,   499,   503,   508,   515,
     520,   525,   529,   535,   541,   547,   553,   557,   564,   568,
     575,   579,   585,   589,   593,   597,   601,   605,   612,   617,
     622,   626,   632,   636,   642,   646,   652,   656,   662,   666,
     672,   676,   681,   686,   691,   698,   705,   710,   717,   724,
     728,   733,   740,   744,   752,   756
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "ID", "CONST", "VOID", "INT", "FLOAT",
  "IF", "ELSE", "WHILE", "FOR", "TYPEDEF", "OP_ASSIGN", "OP_OR", "OP_AND",
  "OP_NOT", "OP_EQ", "OP_NE", "OP_GT", "OP_LT", "OP_GE", "OP_LE",
  "OP_PLUS", "OP_MINUS", "OP_TIMES", "OP_DIVIDE", "MK_LB", "MK_RB",
  "MK_LPAREN", "MK_RPAREN", "MK_LBRACE", "MK_RBRACE", "MK_COMMA",
  "MK_SEMICOLON", "MK_DOT", "ERROR", "RETURN", "$accept", "program",
  "global_decl_list", "global_decl", "function_decl", "param_list",
  "param", "dim_fn", "expr_null", "block", "decl_list", "decl",
  "type_decl", "var_decl", "type", "id_list", "dim_decl", "cexpr",
  "mcexpr", "cfactor", "init_id_list", "init_id", "stmt_list", "stmt",
  "assign_expr_list", "nonempty_assign_expr_list", "test", "assign_expr",
  "relop_expr", "relop_term", "relop_factor", "rel_op", "relop_expr_list",
  "nonempty_relop_expr_list", "expr", "add_op", "term", "mul_op", "factor",
  "var_ref", "dim_list", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    38,    39,    39,    40,    40,    41,    41,    42,    42,
      42,    42,    42,    42,    43,    43,    44,    44,    44,    44,
      45,    45,    46,    46,    47,    47,    47,    47,    48,    48,
      49,    49,    50,    50,    51,    51,    52,    52,    53,    53,
      53,    53,    54,    54,    55,    55,    55,    56,    56,    56,
      57,    57,    58,    58,    59,    59,    59,    60,    60,    61,
      61,    61,    61,    61,    61,    61,    61,    61,    61,    62,
      62,    63,    63,    64,    65,    65,    66,    66,    67,    67,
      68,    68,    69,    69,    69,    69,    69,    69,    70,    70,
      71,    71,    72,    72,    73,    73,    74,    74,    75,    75,
      76,    76,    76,    76,    76,    76,    76,    76,    76,    76,
      76,    76,    77,    77,    78,    78
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     0,     2,     1,     2,     1,     8,     8,
       8,     7,     7,     7,     3,     1,     2,     2,     3,     3,
       3,     4,     1,     0,     2,     1,     1,     0,     2,     1,
       1,     1,     4,     4,     3,     3,     1,     1,     1,     3,
       4,     2,     3,     4,     3,     3,     1,     3,     3,     1,
       1,     3,     1,     3,     1,     2,     3,     2,     1,     3,
       5,     9,     4,     5,     7,     5,     1,     2,     3,     1,
       0,     3,     1,     1,     3,     1,     1,     3,     1,     3,
       1,     3,     1,     1,     1,     1,     1,     1,     1,     0,
       3,     1,     3,     1,     1,     1,     3,     1,     1,     1,
       3,     4,     4,     1,     2,     2,     4,     5,     5,     1,
       2,     2,     1,     2,     4,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       3,     0,     0,    36,    37,     0,     0,     2,     5,     7,
       0,    29,    30,    31,     0,    38,     0,     0,     0,     0,
       1,     4,     6,    28,    54,     0,    52,     0,     0,    41,
       0,    35,     0,    38,     0,     0,     0,     0,    55,     0,
      34,    50,     0,     0,    46,    49,     0,     0,     0,    15,
       0,     0,    39,     0,     0,    33,    32,   112,   103,     0,
       0,     0,    56,    76,    78,    80,    93,    97,   109,     0,
       0,    54,    53,     0,     0,     0,    42,     0,     0,    17,
      27,     0,     0,    16,     0,    40,    27,     0,     0,    89,
     113,   112,   105,     0,   111,   112,   104,     0,   110,     0,
       0,     0,    82,    85,    86,    87,    83,    84,    94,    95,
       0,     0,    98,    99,     0,    27,     0,    51,    44,    45,
      47,    48,    23,    19,   112,     0,     0,     0,    27,    66,
       0,     0,    26,     0,    25,    58,     0,    27,    14,    18,
      43,     0,    27,     0,    91,     0,    88,     0,    89,     0,
      89,     0,   100,    77,    79,    81,    92,    96,     0,    27,
       0,    22,     0,    89,     0,     0,    70,     0,    67,     0,
      13,    24,   112,    57,     0,     0,    12,     0,   115,   106,
       0,     0,     0,   102,     0,   101,    11,     0,    20,     0,
       0,   112,     0,    73,    75,     0,     0,    69,    72,    59,
      68,     0,    10,     9,    90,   114,   108,   107,     8,    21,
       0,     0,     0,     0,    89,     0,    62,    65,    74,    63,
      60,     0,    71,     0,    70,    64,     0,     0,    61
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     6,     7,     8,     9,    48,    49,   123,   160,   131,
     132,    11,    12,    13,   133,    16,    29,    43,    44,    45,
      25,    26,   134,   135,   196,   197,   192,   193,   144,    63,
      64,   110,   145,   146,    65,   111,    66,   114,    67,    68,
      90
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -160
static const yytype_int16 yypact[] =
{
     155,    10,    25,  -160,  -160,   204,    32,   155,  -160,  -160,
     155,  -160,  -160,  -160,    52,    39,    45,    76,    71,    71,
    -160,  -160,  -160,  -160,   122,   187,  -160,    20,    24,    82,
     114,  -160,    40,   105,   191,   193,   117,    83,    82,   133,
    -160,  -160,    20,   149,   197,  -160,   145,   132,   112,  -160,
     165,    20,   105,   143,   173,  -160,  -160,   185,  -160,    16,
      31,   117,   164,   178,  -160,   163,   203,  -160,  -160,   170,
     175,    91,  -160,   141,    20,    20,  -160,    20,    20,   180,
     100,   208,   147,   180,   166,    82,   100,   209,   117,   117,
     211,   186,  -160,   117,  -160,   189,  -160,   117,  -160,    62,
     117,   117,  -160,  -160,  -160,  -160,  -160,  -160,  -160,  -160,
     117,   117,  -160,  -160,   117,   100,   210,  -160,   197,   197,
    -160,  -160,   117,   215,    23,   216,   217,   218,   100,  -160,
      35,   212,   100,   133,    88,  -160,   182,   100,  -160,   215,
    -160,   219,   100,   168,   164,   213,   220,   117,   117,    86,
     117,   110,  -160,   178,  -160,   207,   203,  -160,   222,   100,
     221,   207,   117,   117,   123,   123,   123,   223,  -160,    67,
    -160,    88,   190,  -160,   117,   224,  -160,   225,  -160,  -160,
     117,   174,   228,  -160,   229,  -160,  -160,   230,  -160,   176,
     231,   130,   233,  -160,   164,   234,   226,   232,  -160,  -160,
    -160,    81,  -160,  -160,   164,  -160,  -160,  -160,  -160,  -160,
     235,   117,    88,    88,   117,   123,  -160,  -160,   164,   239,
    -160,   236,  -160,    88,   123,  -160,   237,    88,  -160
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -160,  -160,  -160,   243,   242,   -15,   184,   188,  -160,   -75,
     169,    -9,  -160,  -160,     5,   214,    -8,   -33,   160,   159,
    -160,   238,   136,  -130,    48,  -160,   108,  -159,   -36,   179,
     177,  -160,  -142,  -160,   -74,  -160,   171,  -160,   161,   -57,
    -160
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
      62,    23,    94,    98,   173,    14,   182,   198,   184,    73,
      19,   141,    14,    15,   143,    14,    38,    54,    84,    91,
      92,   190,    70,   136,    41,    99,    33,    46,    17,   136,
       3,     4,    20,    50,    95,    96,   155,    50,    57,    58,
     158,   173,    50,    46,    85,    93,     3,     4,   161,    42,
      88,    59,   163,   167,    47,    24,   222,   149,   136,    60,
      97,   151,   175,    38,    61,   198,    27,   177,    28,   168,
      53,   136,   221,   181,    33,   136,   100,   136,    30,    31,
     136,   100,   219,   220,   187,   136,    46,    50,   189,     3,
       4,   172,   152,   225,   169,   100,   125,   228,   126,   127,
     100,   200,   136,   124,    36,    32,     3,     4,   125,    51,
     126,   127,     5,    69,   136,   216,   183,    52,    27,   128,
      57,    58,   129,    23,   100,   130,   191,    58,   194,   194,
     194,   128,    27,    59,   129,    36,    71,   130,   201,    59,
     185,    60,    81,   211,   204,    82,    61,    60,    79,    27,
      46,    37,    61,     3,     4,   136,   136,    88,     1,    89,
       2,     3,     4,    80,    74,    75,   136,     5,    83,    10,
     136,   117,    74,    75,    86,   218,    10,    76,   100,   194,
     102,   103,   104,   105,   106,   107,   108,   109,   194,    74,
      75,   108,   109,   101,   140,   174,   178,   108,   109,   108,
     109,   115,   205,    87,   209,   116,    82,   122,    82,    18,
       3,     4,    88,    88,    89,   148,    88,    88,   150,   163,
      39,    40,    77,    78,    30,    55,    30,    56,   112,   113,
     108,   109,    34,    35,   118,   119,   120,   121,   147,   137,
     142,   159,   162,   179,   170,   164,   165,   166,   223,   188,
      21,   176,    22,   180,   186,   199,   202,   203,   206,   207,
     214,   210,   208,   212,   213,   215,   138,   227,   171,   217,
     224,   139,   226,   195,     0,   157,     0,    72,   154,   153,
       0,     0,   156
};

static const yytype_int16 yycheck[] =
{
      36,    10,    59,    60,   134,     0,   148,   166,   150,    42,
       5,    86,     7,     3,    88,    10,    24,    32,    51,     3,
       4,   163,    37,    80,     4,    61,     3,     3,     3,    86,
       6,     7,     0,    28,     3,     4,   110,    32,     3,     4,
     115,   171,    37,     3,    52,    29,     6,     7,   122,    29,
      27,    16,    29,   128,    30,     3,   215,    93,   115,    24,
      29,    97,   137,    71,    29,   224,    27,   142,    29,    34,
      30,   128,   214,   147,     3,   132,    14,   134,    33,    34,
     137,    14,   212,   213,   159,   142,     3,    82,   162,     6,
       7,     3,    30,   223,   130,    14,     8,   227,    10,    11,
      14,    34,   159,     3,    13,    29,     6,     7,     8,    27,
      10,    11,    12,    30,   171,    34,    30,     3,    27,    31,
       3,     4,    34,   132,    14,    37,     3,     4,   164,   165,
     166,    31,    27,    16,    34,    13,     3,    37,   174,    16,
      30,    24,    30,    13,   180,    33,    29,    24,     3,    27,
       3,    29,    29,     6,     7,   212,   213,    27,     3,    29,
       5,     6,     7,    31,    23,    24,   223,    12,     3,     0,
     227,    30,    23,    24,    31,   211,     7,    28,    14,   215,
      17,    18,    19,    20,    21,    22,    23,    24,   224,    23,
      24,    23,    24,    15,    28,    13,    28,    23,    24,    23,
      24,    31,    28,    30,    28,    30,    33,    27,    33,     5,
       6,     7,    27,    27,    29,    29,    27,    27,    29,    29,
      33,    34,    25,    26,    33,    34,    33,    34,    25,    26,
      23,    24,    18,    19,    74,    75,    77,    78,    27,    31,
      31,    31,    27,    30,    32,    29,    29,    29,     9,    28,
       7,    32,    10,    33,    32,    32,    32,    32,    30,    30,
      34,    30,    32,    30,    30,    33,    82,    30,   132,    34,
      34,    83,   224,   165,    -1,   114,    -1,    39,   101,   100,
      -1,    -1,   111
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,     5,     6,     7,    12,    39,    40,    41,    42,
      48,    49,    50,    51,    52,     3,    53,     3,     5,    52,
       0,    41,    42,    49,     3,    58,    59,    27,    29,    54,
      33,    34,    29,     3,    53,    53,    13,    29,    54,    33,
      34,     4,    29,    55,    56,    57,     3,    30,    43,    44,
      52,    27,     3,    30,    43,    34,    34,     3,     4,    16,
      24,    29,    66,    67,    68,    72,    74,    76,    77,    30,
      43,     3,    59,    55,    23,    24,    28,    25,    26,     3,
      31,    30,    33,     3,    55,    54,    31,    30,    27,    29,
      78,     3,     4,    29,    77,     3,     4,    29,    77,    66,
      14,    15,    17,    18,    19,    20,    21,    22,    23,    24,
      69,    73,    25,    26,    75,    31,    30,    30,    56,    56,
      57,    57,    27,    45,     3,     8,    10,    11,    31,    34,
      37,    47,    48,    52,    60,    61,    77,    31,    44,    45,
      28,    47,    31,    72,    66,    70,    71,    27,    29,    66,
      29,    66,    30,    67,    68,    72,    74,    76,    47,    31,
      46,    72,    27,    29,    29,    29,    29,    47,    34,    66,
      32,    60,     3,    61,    13,    47,    32,    47,    28,    30,
      33,    72,    70,    30,    70,    30,    32,    47,    28,    72,
      70,     3,    64,    65,    66,    64,    62,    63,    65,    32,
      34,    66,    32,    32,    66,    28,    30,    30,    32,    28,
      30,    13,    30,    30,    34,    33,    34,    34,    66,    61,
      61,    70,    65,     9,    34,    61,    62,    30,    61
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *bottom, yytype_int16 *top)
#else
static void
yy_stack_print (bottom, top)
    yytype_int16 *bottom;
    yytype_int16 *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      fprintf (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      fprintf (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  int yystate;
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;
#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  yytype_int16 yyssa[YYINITDEPTH];
  yytype_int16 *yyss = yyssa;
  yytype_int16 *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     look-ahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to look-ahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 166 "parser.y"
    { (yyval.node)=Allocate(PROGRAM_NODE);  makeChild((yyval.node),(yyvsp[(1) - (1)].node)); prog=(yyval.node);;}
    break;

  case 3:
#line 167 "parser.y"
    { (yyval.node)=Allocate(PROGRAM_NODE); prog=(yyval.node);;}
    break;

  case 4:
#line 171 "parser.y"
    {
                        (yyval.node) = makeSibling((yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 5:
#line 175 "parser.y"
    {
                        (yyval.node) = (yyvsp[(1) - (1)].node);
                    ;}
    break;

  case 6:
#line 181 "parser.y"
    {
                    (yyval.node) = makeSibling(makeChild(Allocate(VARIABLE_DECL_LIST_NODE), (yyvsp[(1) - (2)].node)), (yyvsp[(2) - (2)].node));
                ;}
    break;

  case 7:
#line 185 "parser.y"
    {
                    (yyval.node) = (yyvsp[(1) - (1)].node);
                ;}
    break;

  case 8:
#line 191 "parser.y"
    {
                        (yyval.node) = makeDeclNode(FUNCTION_DECL);
                        AST_NODE* parameterList = Allocate(PARAM_LIST_NODE);
                        makeChild(parameterList, (yyvsp[(4) - (8)].node));
                        makeFamily((yyval.node), 4, (yyvsp[(1) - (8)].node), makeIDNode((yyvsp[(2) - (8)].lexeme), NORMAL_ID), parameterList, (yyvsp[(7) - (8)].node));
                    ;}
    break;

  case 9:
#line 198 "parser.y"
    {
                        (yyval.node) = makeDeclNode(FUNCTION_DECL);
                        AST_NODE* voidNode = makeIDNode("void", NORMAL_ID);
                        AST_NODE* parameterList = Allocate(PARAM_LIST_NODE);
                        makeChild(parameterList, (yyvsp[(4) - (8)].node));
                        makeFamily((yyval.node), 4, voidNode, makeIDNode((yyvsp[(2) - (8)].lexeme), NORMAL_ID), parameterList, (yyvsp[(7) - (8)].node));
                    ;}
    break;

  case 10:
#line 206 "parser.y"
    {
                        (yyval.node) = makeDeclNode(FUNCTION_DECL);
                        AST_NODE* idNode = makeIDNode((yyvsp[(1) - (8)].lexeme), NORMAL_ID);
                        AST_NODE* parameterList = Allocate(PARAM_LIST_NODE);
                        makeChild(parameterList, (yyvsp[(4) - (8)].node));
                        makeFamily((yyval.node), 4, idNode, makeIDNode((yyvsp[(2) - (8)].lexeme), NORMAL_ID), parameterList, (yyvsp[(7) - (8)].node));
                    ;}
    break;

  case 11:
#line 214 "parser.y"
    {
                        (yyval.node) = makeDeclNode(FUNCTION_DECL);
                        AST_NODE* emptyParameterList = Allocate(PARAM_LIST_NODE);
                        makeFamily((yyval.node), 4, (yyvsp[(1) - (7)].node), makeIDNode((yyvsp[(2) - (7)].lexeme), NORMAL_ID), emptyParameterList, (yyvsp[(6) - (7)].node));
                    ;}
    break;

  case 12:
#line 220 "parser.y"
    {
                        (yyval.node) = makeDeclNode(FUNCTION_DECL);
                        AST_NODE* voidNode = makeIDNode("void", NORMAL_ID);
                        AST_NODE* emptyParameterList = Allocate(PARAM_LIST_NODE);
                        makeFamily((yyval.node), 4, voidNode, makeIDNode((yyvsp[(2) - (7)].lexeme), NORMAL_ID), emptyParameterList, (yyvsp[(6) - (7)].node));
                    ;}
    break;

  case 13:
#line 227 "parser.y"
    {
                        (yyval.node) = makeDeclNode(FUNCTION_DECL);
                        AST_NODE* idNode = makeIDNode((yyvsp[(1) - (7)].lexeme), NORMAL_ID);
                        AST_NODE* emptyParameterList = Allocate(PARAM_LIST_NODE);
                        makeFamily((yyval.node), 4, idNode, makeIDNode((yyvsp[(2) - (7)].lexeme), NORMAL_ID), emptyParameterList, (yyvsp[(6) - (7)].node));
                    ;}
    break;

  case 14:
#line 236 "parser.y"
    {
                    (yyval.node) = makeSibling((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                ;}
    break;

  case 15:
#line 240 "parser.y"
    {
                    (yyval.node) = (yyvsp[(1) - (1)].node); 
                ;}
    break;

  case 16:
#line 246 "parser.y"
    {
                    (yyval.node) = makeDeclNode(FUNCTION_PARAMETER_DECL);
                    makeFamily((yyval.node), 2, (yyvsp[(1) - (2)].node), makeIDNode((yyvsp[(2) - (2)].lexeme), NORMAL_ID));
                ;}
    break;

  case 17:
#line 251 "parser.y"
    {
                    (yyval.node) = makeDeclNode(FUNCTION_PARAMETER_DECL);
                    makeFamily((yyval.node), 2, makeIDNode((yyvsp[(1) - (2)].lexeme), NORMAL_ID), makeIDNode((yyvsp[(2) - (2)].lexeme), NORMAL_ID));
                ;}
    break;

  case 18:
#line 256 "parser.y"
    {
                    (yyval.node) = makeDeclNode(FUNCTION_PARAMETER_DECL);
                    makeFamily((yyval.node), 2, (yyvsp[(1) - (3)].node), makeChild(makeIDNode((yyvsp[(2) - (3)].lexeme), ARRAY_ID), (yyvsp[(3) - (3)].node)));
                ;}
    break;

  case 19:
#line 261 "parser.y"
    {
                    (yyval.node) = makeDeclNode(FUNCTION_PARAMETER_DECL);
                    makeFamily((yyval.node), 2, makeIDNode((yyvsp[(1) - (3)].lexeme), NORMAL_ID), makeChild(makeIDNode((yyvsp[(2) - (3)].lexeme), ARRAY_ID), (yyvsp[(3) - (3)].node)));
                ;}
    break;

  case 20:
#line 267 "parser.y"
    {
                    (yyval.node) = (yyvsp[(2) - (3)].node);
                ;}
    break;

  case 21:
#line 271 "parser.y"
    {
                    (yyval.node) = makeSibling((yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].node));
                ;}
    break;

  case 22:
#line 277 "parser.y"
    {
                    (yyval.node) = (yyvsp[(1) - (1)].node);
                ;}
    break;

  case 23:
#line 281 "parser.y"
    {
                    (yyval.node) = Allocate(NUL_NODE); 
                ;}
    break;

  case 24:
#line 287 "parser.y"
    {
                        (yyval.node) = Allocate(BLOCK_NODE);
                        makeFamily((yyval.node), 2, makeChild(Allocate(VARIABLE_DECL_LIST_NODE), (yyvsp[(1) - (2)].node)), makeChild(Allocate(STMT_LIST_NODE), (yyvsp[(2) - (2)].node)));
                    ;}
    break;

  case 25:
#line 292 "parser.y"
    {
                        (yyval.node) = Allocate(BLOCK_NODE);
                        makeChild((yyval.node), makeChild(Allocate(STMT_LIST_NODE), (yyvsp[(1) - (1)].node)));
                    ;}
    break;

  case 26:
#line 297 "parser.y"
    {
                        (yyval.node) = Allocate(BLOCK_NODE);
                        makeChild((yyval.node), makeChild(Allocate(VARIABLE_DECL_LIST_NODE), (yyvsp[(1) - (1)].node)));
                    ;}
    break;

  case 27:
#line 301 "parser.y"
    {
                        (yyval.node) = Allocate(BLOCK_NODE);
                    ;}
    break;

  case 28:
#line 307 "parser.y"
    {
                    (yyval.node) = makeSibling((yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].node));
                ;}
    break;

  case 29:
#line 311 "parser.y"
    {
                    (yyval.node) = (yyvsp[(1) - (1)].node);
                ;}
    break;

  case 30:
#line 317 "parser.y"
    {
                    (yyval.node) = (yyvsp[(1) - (1)].node);
                ;}
    break;

  case 31:
#line 321 "parser.y"
    {
                    (yyval.node) = (yyvsp[(1) - (1)].node);
                ;}
    break;

  case 32:
#line 327 "parser.y"
    {
                    (yyval.node) = makeDeclNode(TYPE_DECL);
                    makeFamily((yyval.node), 2, (yyvsp[(2) - (4)].node), (yyvsp[(3) - (4)].node));
                ;}
    break;

  case 33:
#line 332 "parser.y"
    {
                    (yyval.node) = makeDeclNode(TYPE_DECL);
                    AST_NODE* voidNode = makeIDNode("void", NORMAL_ID);
                    makeFamily((yyval.node), 2, voidNode, (yyvsp[(3) - (4)].node));
                ;}
    break;

  case 34:
#line 340 "parser.y"
    {
                    (yyval.node) = makeDeclNode(VARIABLE_DECL);
                    makeFamily((yyval.node), 2, (yyvsp[(1) - (3)].node), (yyvsp[(2) - (3)].node));
                ;}
    break;

  case 35:
#line 345 "parser.y"
    {
                    (yyval.node) = makeDeclNode(VARIABLE_DECL);
                    makeFamily((yyval.node), 2, makeIDNode((yyvsp[(1) - (3)].lexeme), NORMAL_ID), (yyvsp[(2) - (3)].node));
                ;}
    break;

  case 36:
#line 352 "parser.y"
    {
                    (yyval.node) = makeIDNode("int", NORMAL_ID);  
                ;}
    break;

  case 37:
#line 356 "parser.y"
    {
                    (yyval.node) = makeIDNode("float", NORMAL_ID);
                ;}
    break;

  case 38:
#line 362 "parser.y"
    {
                    (yyval.node) = makeIDNode((yyvsp[(1) - (1)].lexeme), NORMAL_ID);
                ;}
    break;

  case 39:
#line 366 "parser.y"
    {
                    (yyval.node) = makeSibling((yyvsp[(1) - (3)].node), makeIDNode((yyvsp[(3) - (3)].lexeme), NORMAL_ID));
                ;}
    break;

  case 40:
#line 370 "parser.y"
    {
                    (yyval.node) = makeSibling((yyvsp[(1) - (4)].node), makeChild(makeIDNode((yyvsp[(3) - (4)].lexeme), ARRAY_ID), (yyvsp[(4) - (4)].node)));
                ;}
    break;

  case 41:
#line 374 "parser.y"
    {
                    (yyval.node) = makeChild(makeIDNode((yyvsp[(1) - (2)].lexeme), ARRAY_ID), (yyvsp[(2) - (2)].node));
                ;}
    break;

  case 42:
#line 379 "parser.y"
    {
                    (yyval.node) = (yyvsp[(2) - (3)].node);
                ;}
    break;

  case 43:
#line 383 "parser.y"
    {
                    (yyval.node) = makeSibling((yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].node));
                ;}
    break;

  case 44:
#line 388 "parser.y"
    {
                    (yyval.node) = makeExprNode(BINARY_OPERATION, BINARY_OP_ADD);
                    makeFamily((yyval.node), 2, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                ;}
    break;

  case 45:
#line 393 "parser.y"
    {
                    (yyval.node) = makeExprNode(BINARY_OPERATION, BINARY_OP_SUB);
                    makeFamily((yyval.node), 2, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                ;}
    break;

  case 46:
#line 398 "parser.y"
    {
                    (yyval.node) = (yyvsp[(1) - (1)].node);
                ;}
    break;

  case 47:
#line 403 "parser.y"
    {
                    (yyval.node) = makeExprNode(BINARY_OPERATION, BINARY_OP_MUL);
                    makeFamily((yyval.node), 2, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                ;}
    break;

  case 48:
#line 408 "parser.y"
    {
                    (yyval.node) = makeExprNode(BINARY_OPERATION, BINARY_OP_DIV);
                    makeFamily((yyval.node), 2, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                ;}
    break;

  case 49:
#line 413 "parser.y"
    {
                    (yyval.node) = (yyvsp[(1) - (1)].node);
                ;}
    break;

  case 50:
#line 419 "parser.y"
    {
                    (yyval.node) = Allocate(CONST_VALUE_NODE);
                    (yyval.node)->semantic_value.const1 = (yyvsp[(1) - (1)].const1);
                ;}
    break;

  case 51:
#line 424 "parser.y"
    {
                    (yyval.node) = (yyvsp[(2) - (3)].node);
                ;}
    break;

  case 52:
#line 430 "parser.y"
    {
                        (yyval.node) = (yyvsp[(1) - (1)].node); 
                    ;}
    break;

  case 53:
#line 434 "parser.y"
    {
                        (yyval.node) = makeSibling((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                    ;}
    break;

  case 54:
#line 440 "parser.y"
    {
                    (yyval.node) = makeIDNode((yyvsp[(1) - (1)].lexeme), NORMAL_ID);
                ;}
    break;

  case 55:
#line 444 "parser.y"
    {
                    (yyval.node) = makeChild(makeIDNode((yyvsp[(1) - (2)].lexeme), ARRAY_ID), (yyvsp[(2) - (2)].node));
                ;}
    break;

  case 56:
#line 448 "parser.y"
    {
                    (yyval.node) = makeChild(makeIDNode((yyvsp[(1) - (3)].lexeme), WITH_INIT_ID), (yyvsp[(3) - (3)].node));
                ;}
    break;

  case 57:
#line 454 "parser.y"
    {
                    (yyval.node) = makeSibling((yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].node));
                ;}
    break;

  case 58:
#line 458 "parser.y"
    {
                    (yyval.node) = (yyvsp[(1) - (1)].node);
                ;}
    break;

  case 59:
#line 466 "parser.y"
    {
                    (yyval.node) = (yyvsp[(2) - (3)].node);
                ;}
    break;

  case 60:
#line 470 "parser.y"
    {
                    (yyval.node) = makeStmtNode(WHILE_STMT);
                    makeFamily((yyval.node), 2, (yyvsp[(3) - (5)].node), (yyvsp[(5) - (5)].node));
                ;}
    break;

  case 61:
#line 475 "parser.y"
    {
                    (yyval.node) = makeStmtNode(FOR_STMT);
                    makeFamily((yyval.node), 4, (yyvsp[(3) - (9)].node), (yyvsp[(5) - (9)].node), (yyvsp[(7) - (9)].node), (yyvsp[(9) - (9)].node));
                ;}
    break;

  case 62:
#line 480 "parser.y"
    {
                    (yyval.node) = makeStmtNode(ASSIGN_STMT);
                    makeFamily((yyval.node), 2, (yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].node));
                ;}
    break;

  case 63:
#line 485 "parser.y"
    {
                    (yyval.node) = makeStmtNode(IF_STMT);
                    makeFamily((yyval.node), 3, (yyvsp[(3) - (5)].node), (yyvsp[(5) - (5)].node), Allocate(NUL_NODE));
                ;}
    break;

  case 64:
#line 490 "parser.y"
    {
                    (yyval.node) = makeStmtNode(IF_STMT);
                    makeFamily((yyval.node), 3, (yyvsp[(3) - (7)].node), (yyvsp[(5) - (7)].node), (yyvsp[(7) - (7)].node));
                ;}
    break;

  case 65:
#line 495 "parser.y"
    {
                    (yyval.node) = makeStmtNode(FUNCTION_CALL_STMT);
                    makeFamily((yyval.node), 2, makeIDNode((yyvsp[(1) - (5)].lexeme), NORMAL_ID), (yyvsp[(3) - (5)].node));
                ;}
    break;

  case 66:
#line 500 "parser.y"
    {
                    (yyval.node) = Allocate(NUL_NODE);
                ;}
    break;

  case 67:
#line 504 "parser.y"
    {
                    (yyval.node) = makeStmtNode(RETURN_STMT);
                    makeChild((yyval.node), Allocate(NUL_NODE));
                ;}
    break;

  case 68:
#line 509 "parser.y"
    {
                    (yyval.node) = makeStmtNode(RETURN_STMT);
                    makeChild((yyval.node), (yyvsp[(2) - (3)].node));
                ;}
    break;

  case 69:
#line 516 "parser.y"
    {
                        (yyval.node) = makeChild(Allocate(NONEMPTY_ASSIGN_EXPR_LIST_NODE), (yyvsp[(1) - (1)].node));
                     ;}
    break;

  case 70:
#line 520 "parser.y"
    {
                         (yyval.node) = Allocate(NUL_NODE); 
                     ;}
    break;

  case 71:
#line 526 "parser.y"
    {
                                        (yyval.node) = makeSibling((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                                    ;}
    break;

  case 72:
#line 530 "parser.y"
    {
                                        (yyval.node) = (yyvsp[(1) - (1)].node);
                                    ;}
    break;

  case 73:
#line 536 "parser.y"
    {
                    (yyval.node) = (yyvsp[(1) - (1)].node);
                ;}
    break;

  case 74:
#line 542 "parser.y"
    {
                        //TODO: for simpler implementation, use ASSIGN_STMT for now
                        (yyval.node) = makeStmtNode(ASSIGN_STMT);
                        makeFamily((yyval.node), 2, makeIDNode((yyvsp[(1) - (3)].lexeme), NORMAL_ID), (yyvsp[(3) - (3)].node));
                    ;}
    break;

  case 75:
#line 548 "parser.y"
    {
                        (yyval.node) = (yyvsp[(1) - (1)].node);
                    ;}
    break;

  case 76:
#line 554 "parser.y"
    {
                    (yyval.node) = (yyvsp[(1) - (1)].node);
                ;}
    break;

  case 77:
#line 558 "parser.y"
    {
                    (yyval.node) = makeExprNode(BINARY_OPERATION, BINARY_OP_OR);
                    makeFamily((yyval.node), 2, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                ;}
    break;

  case 78:
#line 565 "parser.y"
    {
                    (yyval.node) = (yyvsp[(1) - (1)].node);
                ;}
    break;

  case 79:
#line 569 "parser.y"
    {
                    (yyval.node) = makeExprNode(BINARY_OPERATION, BINARY_OP_AND);
                    makeFamily((yyval.node), 2, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                ;}
    break;

  case 80:
#line 576 "parser.y"
    {
                        (yyval.node) = (yyvsp[(1) - (1)].node);
                    ;}
    break;

  case 81:
#line 580 "parser.y"
    {
                        (yyval.node) = makeFamily((yyvsp[(2) - (3)].node), 2, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                    ;}
    break;

  case 82:
#line 586 "parser.y"
    {
                    (yyval.node) = makeExprNode(BINARY_OPERATION, BINARY_OP_EQ);
                ;}
    break;

  case 83:
#line 590 "parser.y"
    {
                    (yyval.node) = makeExprNode(BINARY_OPERATION, BINARY_OP_GE);
                ;}
    break;

  case 84:
#line 594 "parser.y"
    {
                    (yyval.node) = makeExprNode(BINARY_OPERATION, BINARY_OP_LE);
                ;}
    break;

  case 85:
#line 598 "parser.y"
    {
                    (yyval.node) = makeExprNode(BINARY_OPERATION, BINARY_OP_NE);
                ;}
    break;

  case 86:
#line 602 "parser.y"
    {
                    (yyval.node) = makeExprNode(BINARY_OPERATION, BINARY_OP_GT);
                ;}
    break;

  case 87:
#line 606 "parser.y"
    {
                    (yyval.node) = makeExprNode(BINARY_OPERATION, BINARY_OP_LT);
                ;}
    break;

  case 88:
#line 613 "parser.y"
    {
                        (yyval.node) = makeChild(Allocate(NONEMPTY_RELOP_EXPR_LIST_NODE), (yyvsp[(1) - (1)].node));
                    ;}
    break;

  case 89:
#line 617 "parser.y"
    {
                        (yyval.node) = Allocate(NUL_NODE);
                    ;}
    break;

  case 90:
#line 623 "parser.y"
    {
                                    (yyval.node) = makeSibling((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                                ;}
    break;

  case 91:
#line 627 "parser.y"
    {
                                    (yyval.node) = (yyvsp[(1) - (1)].node);
                                ;}
    break;

  case 92:
#line 633 "parser.y"
    {
                    (yyval.node) = makeFamily((yyvsp[(2) - (3)].node), 2, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                ;}
    break;

  case 93:
#line 637 "parser.y"
    {
                    (yyval.node) = (yyvsp[(1) - (1)].node);
                ;}
    break;

  case 94:
#line 643 "parser.y"
    {
                    (yyval.node) = makeExprNode(BINARY_OPERATION, BINARY_OP_ADD);
                ;}
    break;

  case 95:
#line 647 "parser.y"
    {
                    (yyval.node) = makeExprNode(BINARY_OPERATION, BINARY_OP_SUB);
                ;}
    break;

  case 96:
#line 653 "parser.y"
    {
                    (yyval.node) = makeFamily((yyvsp[(2) - (3)].node), 2, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                ;}
    break;

  case 97:
#line 657 "parser.y"
    {
                    (yyval.node) = (yyvsp[(1) - (1)].node);
                ;}
    break;

  case 98:
#line 663 "parser.y"
    {
                    (yyval.node) = makeExprNode(BINARY_OPERATION, BINARY_OP_MUL);
                ;}
    break;

  case 99:
#line 667 "parser.y"
    {
                    (yyval.node) = makeExprNode(BINARY_OPERATION, BINARY_OP_DIV);
                ;}
    break;

  case 100:
#line 673 "parser.y"
    {
                    (yyval.node) = (yyvsp[(2) - (3)].node);
                ;}
    break;

  case 101:
#line 677 "parser.y"
    {
                    (yyval.node) = makeExprNode(UNARY_OPERATION, UNARY_OP_NEGATIVE);
                    makeChild((yyval.node), (yyvsp[(3) - (4)].node));
                ;}
    break;

  case 102:
#line 682 "parser.y"
    {   
                    (yyval.node) = makeExprNode(UNARY_OPERATION, UNARY_OP_LOGICAL_NEGATION);
                    makeChild((yyval.node), (yyvsp[(3) - (4)].node));
                ;}
    break;

  case 103:
#line 687 "parser.y"
    {
                    (yyval.node) = Allocate(CONST_VALUE_NODE);
                    (yyval.node)->semantic_value.const1=(yyvsp[(1) - (1)].const1);
                ;}
    break;

  case 104:
#line 692 "parser.y"
    {
                    (yyval.node) = makeExprNode(UNARY_OPERATION, UNARY_OP_NEGATIVE);
                    AST_NODE *constNode = Allocate(CONST_VALUE_NODE);
                    constNode->semantic_value.const1 = (yyvsp[(2) - (2)].const1);
                    makeChild((yyval.node), constNode);
                ;}
    break;

  case 105:
#line 699 "parser.y"
    {
                    (yyval.node) = makeExprNode(UNARY_OPERATION, UNARY_OP_LOGICAL_NEGATION);
                    AST_NODE *constNode = Allocate(CONST_VALUE_NODE);
                    constNode->semantic_value.const1 = (yyvsp[(2) - (2)].const1);
                    makeChild((yyval.node), constNode);
                ;}
    break;

  case 106:
#line 706 "parser.y"
    {
                    (yyval.node) = makeStmtNode(FUNCTION_CALL_STMT);
                    makeFamily((yyval.node), 2, makeIDNode((yyvsp[(1) - (4)].lexeme), NORMAL_ID), (yyvsp[(3) - (4)].node));
                ;}
    break;

  case 107:
#line 711 "parser.y"
    {
                    (yyval.node) = makeExprNode(UNARY_OPERATION, UNARY_OP_NEGATIVE);
                    AST_NODE* functionCallNode = makeStmtNode(FUNCTION_CALL_STMT);
                    makeFamily(functionCallNode, 2, makeIDNode((yyvsp[(2) - (5)].lexeme), NORMAL_ID), (yyvsp[(4) - (5)].node));
                    makeChild((yyval.node), functionCallNode);
                ;}
    break;

  case 108:
#line 718 "parser.y"
    {
                    (yyval.node) = makeExprNode(UNARY_OPERATION, UNARY_OP_LOGICAL_NEGATION);
                    AST_NODE* functionCallNode = makeStmtNode(FUNCTION_CALL_STMT);
                    makeFamily(functionCallNode, 2, makeIDNode((yyvsp[(2) - (5)].lexeme), NORMAL_ID), (yyvsp[(4) - (5)].node));
                    makeChild((yyval.node), functionCallNode);
                ;}
    break;

  case 109:
#line 725 "parser.y"
    {
                    (yyval.node) = (yyvsp[(1) - (1)].node);
                ;}
    break;

  case 110:
#line 729 "parser.y"
    {
                    (yyval.node) = makeExprNode(UNARY_OPERATION, UNARY_OP_NEGATIVE);
                    makeChild((yyval.node), (yyvsp[(2) - (2)].node));
                ;}
    break;

  case 111:
#line 734 "parser.y"
    {
                    (yyval.node) = makeExprNode(UNARY_OPERATION, UNARY_OP_LOGICAL_NEGATION);
                    makeChild((yyval.node), (yyvsp[(2) - (2)].node));
                ;}
    break;

  case 112:
#line 741 "parser.y"
    {
                    (yyval.node) = makeIDNode((yyvsp[(1) - (1)].lexeme), NORMAL_ID);
                ;}
    break;

  case 113:
#line 745 "parser.y"
    {
                    (yyval.node) = makeIDNode((yyvsp[(1) - (2)].lexeme), ARRAY_ID);
                    makeChild((yyval.node),(yyvsp[(2) - (2)].node));
                ;}
    break;

  case 114:
#line 753 "parser.y"
    {
                    (yyval.node) = makeSibling((yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].node));
                ;}
    break;

  case 115:
#line 757 "parser.y"
    {
                    (yyval.node) = (yyvsp[(2) - (3)].node);
                ;}
    break;


/* Line 1267 of yacc.c.  */
#line 2568 "parser.tab.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse look-ahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}


#line 763 "parser.y"


#include "lex.yy.c"
main (argc, argv)
int argc;
char *argv[];
  {
     yyin = fopen(argv[1],"r");
     yyparse();
     // printGV(prog, NULL);
     
     initializeSymbolTable();
     
     semanticAnalysis(prog);
     
     symbolTableEnd();
     if (!g_anyErrorOccur) {
        printf("Parsing completed. No errors found.\n");
     }
  } /* main */


int yyerror (mesg)
char *mesg;
  {
  printf("%s\t%d\t%s\t%s\n", "Error found in Line ", linenumber, "next token: ", yytext );
  exit(1);
  }

