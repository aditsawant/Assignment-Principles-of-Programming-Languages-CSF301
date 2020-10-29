/*
Team Members:
ADIT SAWANT 2018A7PS0234P
AMEY PATE 2018A7PS0393P
DIPANSHI BANSAL 2018A7PS0510P
MAITHIL MEHTA 2018A7PS0345P
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <limits.h>
#define num_rules 66
int numvars = 0, ind = 0, tentativeTableSize = 0;
char* arr[3] = {"primitive","rectangularArray","jaggedArray"};
char* brr[3] = {"static","dynamic","not_applicable"};
typedef enum {Primitive, Rect_Array, Jagged_Array} dtype;

typedef struct symbol{
    char nt[30];
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
	dtype dtype;
	enum { Static, Dynamic, not_applicable } nature;
	typeExp tex;
	bool isError;
} typeElement;

//////////////////////////////////////////////////////////////////////

typedef struct parseTree {
    symbol* sym;
    llnode* ptr;
    struct parseTree *child;   // point to children of this node
    struct parseTree *sibling;    // point to next node at same level
	tokenStream tok;			// only for line num and lexeme
	int depth;
	dtype dtype;
	typeExp* tex;
} parseTree;

typedef struct Stack { 
    int top; 
    unsigned capacity; 
    symbol* array; 
}Stack; 
  
Stack* createStack(unsigned capacity) 
{ 
    Stack* stack = (struct Stack*)malloc(sizeof(Stack)); 
    stack->capacity = capacity; 
    stack->top = -1; 
    stack->array = (symbol*)malloc(stack->capacity * sizeof(symbol)); 
    return stack; 
} 

int isFull(Stack* stack) 
{ 
    return stack->top == stack->capacity - 1; 
} 
  
int isEmpty(Stack* stack) 
{ 
    return stack->top == -1; 
}

void push(Stack* stack,symbol item) 
{ 
    if (isFull(stack)) 
        return; 
    stack->array[++stack->top] = item;
}

 
symbol* pop(Stack* stack) 
{ 
    if (isEmpty(stack)) 
        return NULL; 
    return &(stack->array[stack->top--]); 
}

////////////////////////////////////////////////////////////
typedef struct special{
	tokenStream* endhead;
	parseTree* pt;
	bool verdict;
}special;

///////////////////////////////////////////////////////////////////
llnode* readGrammar(char* fname, llnode* grammar){
	grammar = (llnode*) malloc(sizeof(llnode)*num_rules);
    FILE* fptr = fopen(fname, "r");
	int rule_num = 0;
	char buffer[200];

	while (fgets(buffer, sizeof(buffer), fptr) != NULL) {
        
		char *sym_read;
		int i;
		sym_read = strtok(buffer, " \n");
        
		for (i = 0; sym_read != NULL; i++) {
			//printf("8");
            
			if(i == 0){
				//printf(" %s ", sym_read);
				llnode* temp = (llnode*) malloc(sizeof(llnode));
                if(temp == NULL){
                    printf("temp is null\n");
                }
				temp->sym.is_terminal = false;
				strcpy(temp->sym.nt, sym_read);
				//printf(" %s ", temp->sym.nt);
				temp->next = NULL;
				grammar[rule_num] = *temp;
			}
			else {
				//printf(" %s ", sym_read);
				llnode* temp = (llnode*) malloc(sizeof(llnode));
				if(temp == NULL) printf("malloc unsuccessful");
				if(!strcmp(sym_read, "R1")) {
					temp->sym.is_terminal = true; 
					strcpy(temp->sym.nt, sym_read);
                    //printf(" %s ", temp->sym.t);
					temp->next = NULL;
				}
				else {
					// Check if upper or lower case
					if(isupper(sym_read[0])){
						temp->sym.is_terminal = false; 
						strcpy(temp->sym.nt, sym_read);
                        //printf(" %s ", temp->sym.nt);
						temp->next = NULL;
					}
					else {
						temp->sym.is_terminal = true; 
						strcpy(temp->sym.nt, sym_read);
                        //printf(" %s ", temp->sym.t);
						temp->next = NULL;
					}
				}
				llnode* temphead = &grammar[rule_num];
				while(temphead->next != NULL){
                    //printf(" AMEY ");
					temphead = temphead->next;
				}
				temphead->next = temp;
			} 
			//printf(" %s ", sym_read);
			sym_read = strtok(NULL, " \n");
		} 
		//printf("\n");
		rule_num++; 
	} 
    //printf("%d",rule_num);
	fclose(fptr);
    return grammar;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
char *strstrip(char *input)
{
        size_t sz;
        char *end;

        sz = strlen(input);

        if (!sz)
                return input;

        end = input + sz - 1;
        while (end >= input && isspace(*end))
                end--;
        *(end + 1) = '\0';

        while (*input && isspace(*input))
                input++;

        return input;
}

char* identifyToken(char* c){
	char * s = (char*)malloc(sizeof(char)*30);
	if(s == NULL) printf("malloc unsuccessful\n");
	if(strcmp(c,"program") == 0) return s = "program";
    else if(strcmp(c,"(") == 0) return s = "open_p";
	else if(strcmp(c,")") == 0) return s = "close_p";
	else if(strcmp(c,"{") == 0) return s = "open_curl";
	else if(strcmp(c,"}") == 0) return s = "close_curl";
	else if(strcmp(c,"declare") == 0) return s = "declare";
	else if(strcmp(c,"list") == 0) return s = "list";
	else if(strcmp(c,"of") == 0) return s = "of";
	else if(strcmp(c,"variables") == 0) return s = "variables";
	else if(strcmp(c,":") == 0) return s = "colon";
	else if(strcmp(c,";") == 0) return s = "semicol";
	else if(strcmp(c,"integer") == 0) return s = "integer";
	else if(strcmp(c,"real") == 0) return s = "real";
	else if(strcmp(c,"boolean") == 0) return s = "boolean";
	else if(strcmp(c,"array") == 0) return s = "array";
	else if(strcmp(c,"jagged") == 0) return s = "jagged";
	else if(strcmp(c,"[") == 0) return s = "open_sq";
	else if(strcmp(c,"]") == 0) return s = "close_sq";
	else if(strcmp(c,"..") == 0) return s = "rangeop";
	else if(strcmp(c,"R1") == 0) return s = "R1";
	else if(strcmp(c,"size") == 0) return s = "size";
	else if(strcmp(c,"values") == 0) return s = "values";
	else if(strcmp(c,"epsilon") == 0) return s = "epsilon";
	else if(strcmp(c,"+") == 0) return s = "plus";
	else if(strcmp(c,"-") == 0) return s = "minus";
	else if(strcmp(c,"=") == 0) return s = "assignop";
	else if(strcmp(c,"*") == 0) return s = "mul";
	else if(strcmp(c,"/") == 0) return s = "div";
	else if(strcmp(c,"|||") == 0) return s = "or";
	else if(strcmp(c,"&&&") == 0) return s = "and";
	else if(c[0] > 47 && c[0] <= 57) return s = "num";
	else {
		return s = "id";     
	}                         // TODO: Can implement a set for getting the total number of variables.
}

tokenStream* tokeniseSourcecode(char* fname, tokenStream *head){
	FILE* fptr = fopen(fname, "r");
	int line_num = 1;
	char buffer[400];
	head = (tokenStream*)malloc(sizeof(tokenStream));
	tokenStream* s;
	s = head;
	while (fgets(buffer, sizeof(buffer), fptr) != NULL) {
		char *sym_read;
		int i;
		sym_read = strtok(buffer, " \n");
		for (i = 0; sym_read != NULL; i++){
			sym_read = strstrip(sym_read);
			if(strcmp(sym_read,"") != 0){
				tokenStream* temp = (tokenStream*)malloc(sizeof(tokenStream));
				temp->line_num = line_num;
				strcpy(temp->lexeme,sym_read);
				strcpy(temp->token_name, identifyToken(sym_read));
				temp->next = NULL;
				temp->before = s;
				s->next = temp;
				s = temp;
			}
			sym_read = strtok(NULL, " \n");
		}
		line_num++; 
	} 
	fclose(fptr);

	tokenStream* temp = (tokenStream*)malloc(sizeof(tokenStream));
	strcpy(temp->lexeme,"EOF");
	strcpy(temp->token_name, "EOF");
	temp->next = NULL;
	temp->before = s;
	s->next = temp;

	head = head->next;
	head->before = NULL;
	return head;
}
//////////////////////////////////////////////////////////////////

bool fill_aux(Stack* aux, symbol lhs_sym, llnode* G, int counter, parseTree* t){
	for(int i=0; i< num_rules; i++){ //define it
		//if(G[i].sym == lhs_sym){
		//printf("%d\n",strcmp(G[i].sym.nt, lhs_sym.nt));
		if(strcmp(G[i].sym.nt, lhs_sym.nt) == 0){
			if(counter==0){
				llnode* temp = G + i;
                t->ptr = temp;
				temp = temp->next;
				while(temp != NULL){
					push(aux, temp->sym);
					temp = temp->next;
				}
				return false;
			}
			else counter--;
		} 
	} 
	return true;
}

void copy_stack(Stack* aux, Stack* stack){
	while(!isEmpty(aux)){
		push(stack, *pop(aux));
	}
}

special createSubTree(symbol* lhs_sym, tokenStream *head, llnode* G, int counter){
	special st, ret_st;
	ret_st.pt = NULL;
	if(head == NULL) return ret_st;

	parseTree* t = (parseTree*)malloc(sizeof(parseTree));
	t->sym = lhs_sym;
	t->sibling = NULL;
	t->child = NULL;
    //t->tok = NULL;

	Stack* stack = createStack(100);
	Stack* aux = createStack(100);

	ret_st.verdict = fill_aux(aux,*lhs_sym, G, counter, t);
	if(ret_st.verdict == true) return ret_st;
	copy_stack(aux,stack);

//	printf("Start of subtree %s\n", lhs_sym->nt);
	parseTree* temp;
	while(!isEmpty(stack))
	{	
		int nextcounter = 0;
		
		if(head == NULL) break;
	
		if(strcmp(stack->array[stack->top].nt,"epsilon") == 0){
//					printf("EPSILON found\n");
					temp = (parseTree*)malloc(sizeof(parseTree));
					t->child = temp;
					temp->sym = &(stack->array[stack->top]);
					temp->sibling = NULL;
					temp->child = NULL;
					//temp->tok = NULL;
					pop(stack);
		}
		else if(stack->array[stack->top].is_terminal)
		{
//			printf("Entered While of subtree %s and stack top is T %s at line %d\n", lhs_sym->nt, stack->array[stack->top].t,head->line_num);

			if(strcmp(stack->array[stack->top].nt,head->token_name) == 0){
				if(t->child == NULL){
					temp = (parseTree*)malloc(sizeof(parseTree));
					t->child = temp;
				}
				else{
					temp->sibling = (parseTree*)malloc(sizeof(parseTree));
					temp = temp->sibling;
				}
				temp->sym = &(stack->array[stack->top]);
				temp->sibling = NULL;
				temp->child = NULL;
				temp->tok = *head;
				/*
				if(strcmp(head->lexeme,"array") == 0){
					printf("YES\n");
					printf("%s\n", temp->tok->lexeme);
				} 
				*/
				head = head->next;
				pop(stack);
			}
			else{
				return ret_st; // if NULL is returned, something is wrong
			}
		}
		else{
//			printf("Entered while of subtree %s and stack top is NT %s\n", lhs_sym->nt, stack->array[stack->top].nt);
			//calls createsubtree, but verifies before attaching and also backtrack code 
			if(t->child == NULL){
				do{
					st = createSubTree(&(stack->array[stack->top]), head, G, nextcounter);
					if(st.verdict == true) return ret_st;
					t->child = st.pt;
					nextcounter++;
				} while(t->child == NULL);
				
//				printf("Back into while of subtree %s and stack top is NT %s\n", lhs_sym->nt, stack->array[stack->top].nt);
//				printf("Head before %s\n", head->lexeme);
				if(t->child != NULL){
					head = st.endhead;
					temp = t->child;
					//temp->tok = NULL;
				}
			}
			else{
				do{
					st = createSubTree(&(stack->array[stack->top]), head, G, nextcounter);
					if(st.verdict == true) return ret_st;
					temp->sibling = st.pt;
					nextcounter++;
				} while(temp->sibling == NULL);
//				printf("Back into while of subtree %s and stack top is NT %s\n", lhs_sym->nt, stack->array[stack->top].nt);
//				printf("Head before %s\n", head->lexeme);
				if(temp->sibling != NULL){
					head = st.endhead;
					//temp->tok = NULL;
					temp = temp->sibling;
				}
			}
			pop(stack);
		}
	}
	ret_st.pt = t;
	ret_st.endhead = head;
	return ret_st;
}

parseTree* createParseTree(parseTree* t, tokenStream *head, llnode* G)
{
	special st;
	st = createSubTree(&(G[0].sym) , head, G, 0);
	if(st.pt != NULL) printf("Parse Tree Created Successfully\n");
	return t = st.pt;
}

void printParseTree(parseTree* tree){
	if(tree == NULL) return;
    //print for this node
    if(tree->sym->is_terminal){
        printf("\n%s terminal %s %d %d", tree->sym->nt, tree->tok.lexeme, tree->tok.line_num, tree->depth);
    }
    else{
        printf("\n%s nonterminal", tree->sym->nt);
        if(tree->tex != NULL){
            printf("%s ", arr[tree->dtype]);
        }
        llnode * ptr = tree->ptr;
        while(ptr != NULL){
            printf("%s ", ptr->sym.nt);
            ptr = ptr->next;
        }
        printf("%d\n",tree->depth);
    }
    //recurse in preorder
	if(!tree->sym->is_terminal){
		printParseTree(tree->child);
	}
	printParseTree(tree->sibling);
}

void calculateDepth(parseTree* tree, int depth){
	if(tree == NULL) return;
	tree->depth = depth;
	if(!tree->sym->is_terminal){
		calculateDepth(tree->child, depth+1);
	}
	calculateDepth(tree->sibling, depth);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void printTypeExpressionTable(typeElement* table){
    printf("FIELD1 \t\t FIELD2 \t\t FIELD3 \t\t FIELD4 \n");
	for(int i = 0; i < numvars; i++){
		printf("%s \t%s \t%s \t",table[i].varname, arr[table[i].dtype], brr[table[i].nature]);
        if(table[i].dtype==0){
            printf("<type=%s>\n", table[i].tex.prim_type);
        }
        else if(table[i].dtype==1){
            printf("<type=rectangularArray,dimensions=%d,",table[i].tex.ra.dimensions);
            rangePair* ptr = table[i].tex.ra.rangeListHead;
            for(int j=1; j<=table[i].tex.ra.dimensions; j++){
                printf("range_R%d=(%s,%s),",j,ptr->lower,ptr->upper);
                ptr = ptr->next;
            }
            printf("basicElementType=integer>\n");
        }
        else{
            printf("<type=jaggedArray,dimensions=%d,",table[i].tex.ja.dimensions);
            if(table[i].tex.ja.dimensions==2){
                printf("range_R1=(%d,%d),",table[i].tex.ja.lower,table[i].tex.ja.upper);
                printf("range_R2=(");
                row* ptr = table[i].tex.ja.rowListHead;
                while(ptr != NULL){
                    printf("%d",ptr->size);
                    ptr = ptr->next;
                    if(ptr!=NULL) printf(",");
                }
                printf("),");
            }
            else{
                printf("range_R1=(%d,%d),",table[i].tex.ja.lower,table[i].tex.ja.upper);
                printf("range_R2=(");
                row* ptr = table[i].tex.ja.rowListHead;
                while(ptr != NULL){
                    innerSize* ptr2 = ptr->innerSizeHead;
                    printf("%d [",ptr->size);
                    while(ptr2 != NULL){
                        printf("%d",ptr2->innerdim);
                        ptr2 = ptr2->next;
                        if(ptr2!=NULL) printf(",");
                    }
                    printf("]");
                    ptr = ptr->next;
                    if(ptr!=NULL) printf(",");
                }
                printf("),");
            }
            printf("basicElementType=integer>\n");
        }
	}
}

void printTypeError(typeElement t1, typeElement t2, parseTree* tree, int line, char* lex){ //incomplete
	char type1[20], type2[20];
    if(t1.dtype==0){
        strcpy(type1,t1.tex.prim_type);
    }
    else if(t1.dtype==1){
        strcpy(type1,"rect_array");
    }
    else{
        strcpy(type1,"jagged_array");
    }
    if(t2.dtype==0){
        strcpy(type2,t2.tex.prim_type);
    }
    else if(t2.dtype==1){
        strcpy(type2,"rect_array");
    }
    else{
        strcpy(type2,"jagged_array");
    }
    if(strcmp(type1,type2)!=0){
        printf("\nLine Number %d\n", line);
        printf("Statement type : Assignment\n");
        printf("Operator %s\n", lex); 
        printf("First operand lexeme %s and type %s\n", t2.varname, type2);
        printf("Second operand lexeme %s and type %s\n", t1.varname, type1);
		printf("Depth of node in parse tree %d\n", tree->depth);
        printf("Type Error\n");
    }
	else if(t1.dtype == t2.dtype == 1){
		if(t1.tex.ra.dimensions != t2.tex.ra.dimensions){
		printf("\nLine Number %d\n", line);
        printf("Statement type : Assignment\n");
        printf("Operator %s\n", lex); 
        printf("First operand lexeme %s and %d-D type %s\n", t2.varname, t2.tex.ra.dimensions, type2);
        printf("Second operand lexeme %s and %d-D type %s\n", t1.varname, t1.tex.ra.dimensions, type1);
		printf("Depth of node in parse tree %d\n", tree->depth);
        printf("Type Error, different dimensions of rect array\n");
		}	
	}
	else if(t1.dtype == t2.dtype == 2){
		if(t1.tex.ja.dimensions != t2.tex.ja.dimensions){
		printf("\nLine Number %d\n", line);
        printf("Statement type : Assignment\n");
        printf("Operator %s\n", lex); 
        printf("First operand lexeme %s and %d-D type %s\n", t2.varname, t2.tex.ja.dimensions, type2);
        printf("Second operand lexeme %s and %d-D type %s\n", t1.varname, t1.tex.ja.dimensions, type1);
		printf("Depth of node in parse tree %d\n", tree->depth);
        printf("Type Error, different dimensions of jagged array\n");
		}
	}
    return;
}

void printTypeDefError(typeExp* tex, int line, int depth){

    printf("\nLine Number %d\n", line);
    printf("Statement type : Declaration\n");
    printf("Operator ***\n");
    printf("First operand lexeme *** and type ***\n");
    printf("Second operand lexeme *** and type ***\n");
    printf("Depth of node in parse tree %d\n", depth);
    printf("%d-D Jagged Array Type Definition Error\n", tex->ja.dimensions);
}

void printSizeMismatchError(typeExp* tex, int sz, int line, int depth){
    printf("\nLine Number %d\n", line);
    printf("Statement type : Declaration\n");
    printf("Operator ***\n");
    printf("First operand lexeme *** and type ***\n");
    printf("Second operand lexeme *** and type ***\n");
    printf("Depth of node in parse tree %d\n", depth);
    printf("%d-D Jagged Array Size Mismatch Error, size is %d\n", tex->ja.dimensions, sz);
}

void printRangeError(int decide, int line, int depth, int num, int range, int dim, char* varname){
        printf("\nLine Number %d\n", line);
        printf("Statement type : Assignment\n");
        printf("Operator ***\n"); 
        printf("First operand lexeme *** and type ***\n");
        printf("Second operand lexeme *** and type ***\n");
		printf("Depth of node in parse tree %d\n", depth);
        if(decide == 1) printf("Type Error for %s in %d dimension as %d is greater than upper range %d\n",varname,dim,num,range);
        else if(decide == 0) printf("Type Error for %s in %d dimension as %d is lesser than lower range %d\n",varname,dim,num,range);
}

void validateJA(typeExp* tex, int line, int depth){
    line++;
    row* ptr = tex->ja.rowListHead;
    while(ptr != NULL){
        int sizecounter = 0;
        //printf("%d\n",ptr->size);
        innerSize* itr = ptr->innerSizeHead;
        while(itr != NULL){
            //printf("  %d  ", itr->innerdim);
            if((tex->ja.dimensions == 2 && itr->innerdim != 1)||(tex->ja.dimensions == 3 && itr->innerdim == 0)){
                printTypeDefError(tex,line, depth);
            } 
            itr = itr->next;
            sizecounter++;
        }
        if(sizecounter != ptr->size) printSizeMismatchError(tex,ptr->size,line, depth);
        ptr = ptr->next;
        line++;
    }
}

typeExp* populateTex(parseTree* dec, parseTree* tree){
	symbol sym = *(tree->sym);
	typeExp* tex = (typeExp*) malloc(sizeof(typeExp));
	if(sym.is_terminal){
		dec->dtype = 0;
		if(strcmp(sym.nt, "integer") == 0){
			strcpy(tex->prim_type, "integer");
		}
		else if(strcmp(sym.nt, "real") == 0){
			strcpy(tex->prim_type, "real");	
		}
		else if(strcmp(sym.nt, "boolean") == 0){
			strcpy(tex->prim_type, "boolean");
		}
		else {
			tex = NULL;
		}
	}
	else{
		//sym is a non-terminal, then what?
		if(strcmp(sym.nt, "REC_ARRAY") == 0){
			dec->dtype = 1;
			parseTree* temp = tree;
            tokenStream *token = &tree->child->tok;
            //printf("%s\n", tree->child->sym->nt);
            //printf("%s", token->lexeme);
            int counter = 0;
			rangePair* rtemp, * orig_rtemp = NULL;
			//printf("%s\n", tree->child->tok.token_name);
			while(strcmp(token->lexeme, "of") != 0){
				counter++;
				if(strcmp(token->lexeme, "[") == 0){
                    rtemp = (rangePair*) malloc(sizeof(rangePair));
                    if(tex->ra.rangeListHead == NULL){
                        tex->ra.rangeListHead = rtemp;
                    }
                    else{
                        orig_rtemp->next = rtemp;
                    }
					strcpy(rtemp->lower, token->next->lexeme);
				}
				if(strcmp(token->lexeme, "..") == 0){
					strcpy(rtemp->upper, token->next->lexeme);
                    orig_rtemp = rtemp;
				}
				token = token->next;
			}
			tex->ra.dimensions = counter/5;
			//test for rect array
			/*
			printf("%d\n", tex->ra.dimensions);
			rangePair* ptr = tex->ra.rangeListHead;
			while(ptr != NULL){
				printf("%s %s\n", ptr->lower, ptr->upper);
				ptr = ptr->next;
			}
			*/
		}
		else if(strcmp(sym.nt, "JAGGED_ARRAY") == 0){
			dec->dtype = 2;
			tree = tree->child;
			if(strcmp(tree->sym->nt, "TWO_JA") == 0){
				tex->ja.dimensions = 2;	
			}
			else{
				tex->ja.dimensions = 3;	
			}
			tree = tree->child->sibling; //now at array (of tree)
			tokenStream *token = &tree->tok;
			
			tex->ja.lower = atoi(token->next->next->lexeme);
			tex->ja.upper = atoi(token->next->next->next->next->lexeme);
			int counter = tex->ja.upper - tex->ja.lower + 1;
			row* temp = NULL, * oldtemp = NULL;
			while(counter--){
				while(strcmp(token->lexeme,"size") != 0) token = token->next;
				temp = (row*)malloc(sizeof(row));
				if(oldtemp == NULL) tex->ja.rowListHead = temp;
				else oldtemp->next = temp;
				temp->size = atoi(token->next->lexeme);
				while(strcmp(token->lexeme,"{") != 0) token = token->next;
				token = token->next; //first number
				innerSize* inptr = NULL, *oldptr = NULL;
				while(strcmp(token->lexeme,"}") != 0){
					inptr = (innerSize*)malloc(sizeof(innerSize));
					if(oldptr == NULL) temp->innerSizeHead = inptr;
					else oldptr->next = inptr;
					while(strcmp(token->lexeme,";") != 0 && strcmp(token->lexeme,"}") != 0){
						token = token->next;
						inptr->innerdim++;
					} 
					if(strcmp(token->lexeme,"}") != 0) token = token->next;
					oldptr = inptr;
				}
				oldtemp = temp;
			}
            validateJA(tex,dec->tok.line_num,tree->depth);
			//test for jagged array
            /*
			printf("%d %d\n", tex->ja.lower, tex->ja.upper);
			row* ptr = tex->ja.rowListHead;
			while(ptr != NULL){
				printf("%d\n",ptr->size);
				innerSize* itr = ptr->innerSizeHead;
				while(itr != NULL){
					printf("  %d  ", itr->innerdim);
					itr = itr->next;
				}
				printf("\n");
				ptr = ptr->next;
			}
			*/
		}
        else{
            tex = NULL;
        }
	}
	return tex;
}

void traverseParseTreeA(parseTree* tree){
	if(tree == NULL || strcmp(tree->sym->nt,"ASSIGNMENT_STATEMENTS") == 0) return;
	if(strcmp(tree->sym->nt,"declare") == 0){
        //to find tablesize
        parseTree* ptr = tree->sibling;
        if(strcmp(ptr->tok.lexeme,"list") == 0) ptr = ptr->sibling->sibling->sibling;
        while(strcmp(ptr->tok.lexeme,":")!=0){
            ptr = ptr->sibling;
            tentativeTableSize++;
            //printf("%d\n",tentativeTableSize);
        }
        //to populate tex
		parseTree* temp = tree;
		while(temp->sibling != NULL) temp = temp->sibling;
		temp = temp->child->child;
		tree->tex = populateTex(tree,temp);
		//printf("\n\ndeclare %d\n", tree->tex->ra.dimensions);
	}
	if(!tree->sym->is_terminal){
		traverseParseTreeA(tree->child);
	}
	traverseParseTreeA(tree->sibling); 
}

typeElement fetchTypeElement(typeElement* table, char* lexeme){
	int i;
	// printf("numvars: %d\n", numvars);
	for(i = 0; i < numvars; i++){
		// printf("table[%d] : %s\n", i, table[i].varname);
		if(strcmp(table[i].varname, lexeme) == 0){
			break;
		}	
	}
	// printf("Lexeme: %s\n", lexeme);
	// printf("##%s\n", table[i].varname);
	return table[i];
}

typeElement populateTypeElement(parseTree* tree, char* varname){
		// TODO: Errors need to be handled here itself
		typeElement temp; 
		temp.varname = varname;
		temp.dtype = tree->dtype;
		//printf("%s dtype: %d, %d\n", tree->sym->nt, temp.dtype, tree->dtype);
        if(temp.dtype == 1){
			// write for nature;
			//trvaerse the ranges and check for any identifiers. 
			rangePair* listHead = temp.tex.ra.rangeListHead;
			while(listHead != NULL) {
				char* l = listHead->lower;
				char* u = listHead->upper;
				if(isalpha(l[0]) == 0 || isalpha(u[0]) == 0){
					temp.nature = 1;
                    break;
				} 
				else temp.nature = 0;
				listHead = listHead->next;
			}
		} else temp.nature = 2;
		temp.tex = *tree->tex;
		return temp;
}

typeElement recursiveTraverse(typeElement* table, parseTree* tree, int line){
	if(tree->child == NULL && tree->sibling == NULL){
		//printf("Both NULL reached for %s.\n", tree->tok.lexeme);
		return fetchTypeElement(table, tree->tok.lexeme); //Correct

	}
	else if(tree->child == NULL && tree->sibling != NULL){
        
        if(strcmp(tree->sibling->sym->nt, "open_sq")==0){
            //bound checking 
            typeElement tempo = fetchTypeElement(table,tree->tok.lexeme);
            tokenStream* ptr = tree->tok.next->next;
            if(tempo.dtype == 1){
                rangePair* bound = tempo.tex.ra.rangeListHead;
                int localdim = 1;
                while(strcmp(ptr->lexeme,"]")){
                    int num = atoi(ptr->lexeme);
                    int low = atoi(bound->lower);
                    int up = atoi(bound->upper);
                    if(num > up) printRangeError(1, ptr->line_num,tree->depth,num,up,localdim, tempo.varname);
                    if(num < low) printRangeError(0,ptr->line_num,tree->depth,num,low,localdim, tempo.varname);
                    ptr = ptr->next;
                    bound = bound->next;
                    localdim++;
                }
            } 
            return fetchTypeElement(table,tree->tok.lexeme);
        }
		else return recursiveTraverse(table, tree->sibling, line); // Correct
	} 
	else if(tree->child != NULL && tree->sibling == NULL){
		return recursiveTraverse(table, tree->child, line); // Correct?
	}
	else if(tree->child != NULL && tree->sibling != NULL){
		typeElement tel = recursiveTraverse(table, tree->sibling, line);
		typeElement ptr = recursiveTraverse(table, tree->child, line);
		if(strcmp(tree->sibling->sym->nt,"div")==0){
                //printf("div found\n");
                strcpy(tel.tex.prim_type,"real");
            }
        else if(tel.isError || ptr.isError || memcmp(&tel.tex, &ptr.tex, sizeof(tel.tex)) != 0){
			//printf("%s %s\n", tel.varname, ptr.varname);
			//printf("telptr wala error\n");
			printTypeError(tel,ptr,tree,line,tree->sibling->tok.lexeme);
            tel.isError = true;
            ptr.isError = true;
            return ptr;
		}
        return tel;
	}
}

void traverseParseTreeB(typeElement* table, parseTree* tree){
	// For Assignment Statements
	if(tree == NULL) return;
	if(strcmp(tree->sym->nt,"ASSIGNMENT_STATEMENT") == 0){
		//printf("In the Assign section\n");
		// printTypeExpTable(table);
        int line = tree->child->tok.line_num;
		typeElement exptel;
		if(tree->child->sibling->sym->is_terminal){
			exptel = recursiveTraverse(table, tree->child->sibling->sibling->child,line);  // For BooleanExp ka child
		}
		else {
			exptel = recursiveTraverse(table, tree->child->sibling->sibling->sibling->child,line); // For Arith ka child	
		}
		typeElement idtel = fetchTypeElement(table, tree->child->tok.lexeme);
        
        if(idtel.isError==false && (tree->child->tok.next->token_name, "open_sq")==0){
        //bound checking 
        tokenStream* ptr = tree->child->tok.next->next;
        rangePair* bound = idtel.tex.ra.rangeListHead;
        int localdim = 1;
        while(strcmp(ptr->lexeme,"]")){
            int num = atoi(ptr->lexeme);
            int low = atoi(bound->lower);
            int up = atoi(bound->upper);
            if(num > up) printRangeError(1, ptr->line_num,tree->depth,num,up,localdim, idtel.varname);
            if(num < low) printRangeError(0,ptr->line_num,tree->depth,num,low,localdim, idtel.varname);
            ptr = ptr->next;
            bound = bound->next;
            localdim++;
        }
        }
        //bound checking for LHS id
		//printf("%s %s\n", exptel.varname, idtel.varname);

		if(exptel.isError == true || memcmp(&exptel.tex, &idtel.tex, sizeof(idtel.tex)) != 0){
			//printf("exptel idtel wala error\n");
			printTypeError(exptel,idtel,tree,line,"=");
		}
		// if(tel != recursiveTraverse(table, tree->child)) {/* ERROR*/}
	}
	else{
		// For Declaration statements:
		if(strcmp(tree->sym->nt, "declare") == 0){
			//printf("In the Declare section\n");
			parseTree* declareptr = tree;
			if(strcmp(tree->sibling->sym->nt, "id") == 0){
				//printf("In the single var section\n");
				table[ind] = populateTypeElement(tree, tree->sibling->tok.lexeme);
				numvars++;
				//printf("NUMVARS: %d\n\n", numvars);
				//table = (typeElement*) realloc(table, sizeof(table) + sizeof(typeElement));
				ind++;
			} else{
				//printf("In the List of vars section\n");
				parseTree* idlistptr = tree->sibling->sibling->sibling->sibling; // now pointing to idlist
				while(idlistptr->child->sibling != NULL){
					//printf("In the while loop\n");
					typeElement temptel = populateTypeElement(tree, idlistptr->child->tok.lexeme);
					// printf("%d\n", ind);
					
					table[ind] = temptel;
					// printf("Out\n");
					idlistptr = idlistptr->child->sibling;
					numvars++;
					//printf("NUMVARS: %d\n\n", numvars);
					// printf("Before\n");
					// table = (typeElement*) realloc(table, sizeof(typeElement) * (numvars+5));
					// printf("After\n");
					ind++;
				}
				// To handle the last variable in the list:
				typeElement temptel = populateTypeElement(tree, idlistptr->child->tok.lexeme);
					// printf("%d\n", ind);
					table[ind] = temptel;
					idlistptr = idlistptr->child->sibling;
					numvars++;
					//printf("NUMVARS: %d\n\n", numvars);
					// table = (typeElement*) realloc(table, sizeof(typeElement) * (numvars+5));
					ind++;
			}
		}
		if(!tree->sym->is_terminal){
			// printf("Inside smol if\n");
			traverseParseTreeB(table, tree->child);
		}
	}
	traverseParseTreeB(table, tree->sibling);
}

typeElement* traverseParseTree(typeElement* table, parseTree* tree){
	calculateDepth(tree,0);
	traverseParseTreeA(tree);
    //printf("%d\n",tentativeTableSize);
    table = (typeElement*) malloc(sizeof(typeElement)*tentativeTableSize);
	traverseParseTreeB(table, tree);
	printf("\nTraversal Completed Successfully\n");
    return table;
}


// int main(){
// 	// bool val; // default false;
// 	// printf("%s", val ? "true" : "false");
	
// 	llnode* G;
// 	G = readGrammar("newgrammar.txt", G);
// 	//test read grammar function
//     /*
// 	for(int i=0; i<67; i++){
// 		llnode* temp = G+i;
// 		printf("\n");
// 		while(temp != NULL){
// 			if(temp->sym.is_terminal) printf("%s ", temp->sym.t);
// 			else printf("%s ", temp->sym.nt);
// 			temp = temp->next;
// 		}		
// 	}
// 	*/
// 	tokenStream* head;
// 	head = tokeniseSourcecode("sourcecode.txt", head);
// 	//test tokenise function
//     /*
// 	while(head->next != NULL){
// 		printf("%s\n", head->token_name);
// 		head = head->next;
// 	}
// 	while(head != NULL){
// 		printf("%s\n", head->token_name);
// 		head = head->before;
// 	}
// 	*/
// 	parseTree* tree;
// 	tree = createParseTree(tree,head,G);
// 	//printf("\n\n");
	
// 	typeElement* table; 
// 	table = traverseParseTree(table, tree);
//     printf("\n");

//     //printParseTree(tree);
// 	//printTypeExpressionTable(table);
// 	return 0;
// }

int main(int argc, char* argv){
    int option;
    while(1){
    numvars = 0, ind = 0, tentativeTableSize = 0;
    scanf("%d",&option);
    if(option == 0) break;
    llnode* G;
    G = readGrammar("newgrammar.txt", G);
    tokenStream* head;
    head = tokeniseSourcecode("t6.txt", head);
    parseTree* tree;
    tree = createParseTree(tree,head,G);
    typeElement* table; 
    table = traverseParseTree(table, tree);
    printParseTree(tree);
    printTypeExpressionTable(table);
    }
    return 0;
}
    
/*
        llnode* Ga;
        Ga = readGrammar("newgrammar.txt", Ga);
        tokenStream* heada;
        heada = tokeniseSourcecode("sourcecode2.txt", heada);
    while(1){
        numvars = 0, ind = 0, tentativeTableSize = 0;
        scanf("%d",&option);
        if(option == 0 ){
            printf("Exiting...\n");
            return 0;
        }
        else if(option == 1){
                    llnode *G = Ga;
        tokenStream* head = heada;
            parseTree* tree;
            tree = createParseTree(tree,head,G);
        }
        else if(option == 2){
                    llnode *G = Ga;
        tokenStream* head = heada;
            parseTree* tree;
            tree = createParseTree(tree,head,G);
            typeElement* table; 
            table = traverseParseTree(table,tree);
 
        }
        else if(option == 3){
                    llnode *G = Ga;
        tokenStream* head = heada;
            parseTree* tree, *otree;
            tree = createParseTree(tree,head,G);
            otree = tree;
            typeElement* table; 
            table = traverseParseTree(table,tree);
            printParseTree(otree);

        }
        else if(option == 4){
                    llnode *G = Ga;
        tokenStream* head = heada;
            parseTree* tree;
            tree = createParseTree(tree,head,G);
            typeElement* table; 
            table = traverseParseTree(table, tree);
            printTypeExpressionTable(table);
        }
        else
        {
            printf("Enter number between 1 and 4\n");
        }
    }

    return 0;
} */
