#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <limits.h>
typedef struct symbol{
    union
    {
        char t[30];
        char nt[30];
    };
    bool is_terminal;
} symbol;

typedef struct llnode{
	symbol sym;
	struct llnode* next;
} llnode;


typedef struct tokenStream{
	char token_name[30];
	char lexeme[30];
	int line_num;
	struct tokenStream* next;
	struct tokenStream* before;
} tokenStream;

//tokenStream** endhead;

////////////////////////////////////////////////////////////////////

typedef struct rangePair {
    char lower[20];
    char upper[20];
	struct rangePair* next;
} rangePair;

typedef struct innerSize {
	int innerdim;
	struct innerSize* next;
} innerSize;

typedef struct row {
	int size;
	innerSize* innerSizeHead;
	struct row* next;
} row;

typedef struct jaggedArrayType {
	enum {two = 2, three} dimensions;
	int upper, lower;
	row* rowListHead;
} jaggedArrayType;

typedef struct rectArrayType {
       // char* basicElementType; remember while printing
        int dimensions;
        rangePair* rangeListHead;
} rectArrayType;

typedef union typeExp {
    jaggedArrayType ja;
	rectArrayType ra;
    char prim_type[10];
} typeExp;

typedef struct typeElement{
	char* varname;
	enum {Primitive, Rect_Array, Jagged_Array} dtype;
	enum { Static, Dynamic, not_applicable } nature;
	typeExp tex;
} typeElement;

//////////////////////////////////////////////////////////////////////

typedef struct parseTree {
    symbol* sym;
    struct parseTree *child;   // point to children of this node
    struct parseTree *sibling;    // point to next node at same level
	tokenStream tok;			// for line num and lexeme, add in create parse tree
	enum {Primitive, Rect_Array, Jagged_Array} dtype;
	typeExp* tex;
} parseTree;

typedef struct Stack { 
    int top; 
    unsigned capacity; 
    symbol* array; 
}Stack; 

typedef struct special{
	tokenStream* endhead;
	parseTree* pt;
	bool verdict;
}special;
