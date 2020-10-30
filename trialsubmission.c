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
#define num_rules 70
int numvars = 0, ind = 0;
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

typedef struct special{
	tokenStream* endhead;
	parseTree* pt;
	bool verdict;
}special;

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

// readGrammar function
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

char *strstrip(char *input){
	size_t sz;
	char *end;
	sz = strlen(input);
	if (!sz)
			return input;
	end = input + sz - 1;
	while (end >= input && isspace(*end)) end--;
	*(end + 1) = '\0';
	while (*input && isspace(*input)) input++;
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
	else return s = "id";
}

// tokeniseSourcecode function
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

 
bool fill_aux(Stack* aux, symbol lhs_sym, llnode* G, int counter, parseTree* t){
	for(int i=0; i< num_rules; i++){ 
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
	if(head == NULL){
		//printf("Head indeed is null\n");
		return ret_st;
	} 

	parseTree* t = (parseTree*)malloc(sizeof(parseTree));
	t->sym = lhs_sym;
	t->sibling = NULL;
	t->child = NULL;
    //t->tok = NULL;

	Stack* stack = createStack(20);
	Stack* aux = createStack(20);

	ret_st.verdict = fill_aux(aux,*lhs_sym, G, counter, t);
	if(ret_st.verdict == true) return ret_st;
	copy_stack(aux,stack);

	//printf("Start of subtree %s\n", lhs_sym->nt);
	parseTree* temp;
	while(!isEmpty(stack))
	{	
		int nextcounter = 0;
		
		if(head == NULL) break;
	
		if(strcmp(stack->array[stack->top].nt,"epsilon") == 0){
					//printf("EPSILON found\n");
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
			//printf("Entered While of subtree %s and stack top is T %s at line %d\n", lhs_sym->nt, stack->array[stack->top].nt,head->line_num);
			//printf("%s,%s\n", stack->array[stack->top].nt, head->token_name);
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
				//printf("Error found\n");
				return ret_st; // if NULL is returned, something is wrong
			}
		}
		else{
			//printf("Entered while of subtree %s and stack top is NT %s\n", lhs_sym->nt, stack->array[stack->top].nt);
			//calls createsubtree, but verifies before attaching and also backtrack code 
			if(t->child == NULL){
				do{
					st = createSubTree(&(stack->array[stack->top]), head, G, nextcounter);
					if(st.verdict == true) return ret_st;
					t->child = st.pt;
					nextcounter++;
				} while(t->child == NULL);
				
				//printf("Back into while of subtree %s and stack top is NT %s\n", lhs_sym->nt, stack->array[stack->top].nt);
				//printf("Head before %s\n", head->lexeme);
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
				//printf("Back into while of subtree %s and stack top is NT %s\n", lhs_sym->nt, stack->array[stack->top].nt);
     			//printf("Head before %s\n", head->lexeme);
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

// createParseTree function
parseTree* createParseTree(parseTree* t, tokenStream *head, llnode* G)
{	
	special st;
	st = createSubTree(&(G[0].sym) , head, G, 0);
	t = st.pt;
	if(t != NULL){
		printf("\n\nParse Tree Created Successfully\n\n");
	} 
	else printf("\n\nParse Tree Creation Failed\n\n");
	return t;
}

// printParseTree function
void printParseTree(parseTree* tree){
	if(tree == NULL) return;
	char star[3] = "***";
    //print for this node
    if(tree->sym->is_terminal){
        printf("|%-22s|terminal   |%-15s|%-22s|%-5d|%-5d|%-112s\n", tree->sym->nt, star, tree->tok.lexeme, tree->tok.line_num, tree->depth, star);
    }
    else{
        printf("|%-22s|nonterminal|", tree->sym->nt);
        if(tree->tex != NULL){
            printf("%-15s|", arr[tree->dtype]);
        }
		else printf("%-15s|",star);
        llnode * ptr = tree->ptr;
        printf("%-22s|%-5s|%-5d|",star,star,tree->depth);
		while(ptr != NULL){
            printf("%s ", ptr->sym.nt);
            ptr = ptr->next;
        }
		printf("\n");
    }
    //recurse in preorder
	if(!tree->sym->is_terminal){
		printParseTree(tree->child);
	}
	printParseTree(tree->sibling);
}

void printPerfectParseTree(parseTree* tree){
	//printf("Symbol Name\t\t\t\tT/NT\t\tTypeExpression\t\t\tLexeme\tLine Num\tDepth\tGrammar Rule\n");
	printf("----------------------------------------------------------------------------------------------------------------------------------------------------------\n");
	printf("|%-22s|%-11s|%-15s|%-22s|%-5s|%-5s|%-112s\n","Symbol Name","T/NT","TypeExpression","Lexeme","Line","Depth","Grammar Rule");
	printf("----------------------------------------------------------------------------------------------------------------------------------------------------------\n");
	printParseTree(tree);
	printf("----------------------------------------------------------------------------------------------------------------------------------------------------------\n");
	printf("\n\n Parse Tree printed successfully\n\n");
}

void calculateDepth(parseTree* tree, int depth){
	if(tree == NULL) return;
	tree->depth = depth;
	if(!tree->sym->is_terminal){
		calculateDepth(tree->child, depth+1);
	}
	calculateDepth(tree->sibling, depth);
}


void printTypeExpressionTable(typeElement* table){
	printf("----------------------------------------------------------------------------------------------------------------------------------------------------------\n");
    printf("FIELD1\t\tFIELD2\t\t\t\tFIELD3\t\tFIELD4\n");
	printf("----------------------------------------------------------------------------------------------------------------------------------------------------------\n");
	for(int i = 0; i < numvars; i++){
		printf("%s\t\t%s\t\t%s\t\t",table[i].varname, arr[table[i].dtype], brr[table[i].nature]);
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
	printf("----------------------------------------------------------------------------------------------------------------------------------------------------------\n");
    printf("\n\nTable printed successfully!\n\n");
}

void printTypeError(typeElement t1, typeElement t2, parseTree* tree, int line, char* lex){ //incomplete
	/*
	if((strcmp(t1.tex.prim_type,"integer") == 0 && ((t2.dtype == 2 || t2.dtype == 3)&& t2.tex.))
	||(strcmp(t2.tex.prim_type,"integer") == 0 && (t1.dtype == 2 || t1.dtype == 3))
	|| t2.dtype==2&&t1.dtype==3 || t1.dtype==3&&t2.dtype==2)
	{
		return;
	} */
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
        printf("Line Number %d\t", line);
        printf("Statement type : Assignment\t");
        printf("Operator %s\t", lex); 
        printf("First operand lexeme %s and type %s\t", t2.varname, type2);
        printf("Second operand lexeme %s and type %s    ", t1.varname, type1);
		printf("Depth of node in parse tree %d    ", tree->depth);
        printf("Type Error\n");
    }
	else if(t1.dtype == 1 && t2.dtype == 1){
		if(t1.tex.ra.dimensions != t2.tex.ra.dimensions){
		printf("Line Number %d\t", line);
        printf("Statement type : Assignment\t");
        printf("Operator %s\t", lex); 
        printf("First operand lexeme %s and %d-D type %s\t", t2.varname, t2.tex.ra.dimensions, type2);
        printf("Second operand lexeme %s and %d-D type %s    ", t1.varname, t1.tex.ra.dimensions, type1);
		printf("Depth of node in parse tree %d    ", tree->depth);
        printf("Type Error, different dimensions of rect array\n");
		}	
	}
	else if(t1.dtype == 2 && t2.dtype == 2){
		if(t1.tex.ja.dimensions != t2.tex.ja.dimensions){
		printf("Line Number %d\t", line);
        printf("Statement type : Assignment\t");
        printf("Operator %s\t", lex); 
        printf("First operand lexeme %s and %d-D type %s\t", t2.varname, t2.tex.ja.dimensions, type2);
        printf("Second operand lexeme %s and %d-D type %s    ", t1.varname, t1.tex.ja.dimensions, type1);
		printf("Depth of node in parse tree %d    ", tree->depth);
        printf("Type Error, different dimensions of jagged array\n");
		}
	}
    return;
}

void printTypeDefError(typeExp* tex, int line, int depth){

    printf("Line Number %d\t", line);
    printf("Statement type : Declaration\t");
    printf("Operator ***\t");
    printf("First operand lexeme *** and type ***\t");
    printf("Second operand lexeme *** and type ***    ");
    printf("Depth of node in parse tree %d    ", depth);
    printf("%d-D Jagged Array Type Definition Error\n", tex->ja.dimensions);
}

void printSizeMismatchError(typeExp* tex, int sz, int line, int depth){
    printf("Line Number %d\t", line);
    printf("Statement type : Declaration\t");
    printf("Operator ***\t");
    printf("First operand lexeme *** and type ***\t");
    printf("Second operand lexeme *** and type ***    ");
    printf("Depth of node in parse tree %d    ", depth);
    printf("%d-D Jagged Array Size Mismatch Error, size is %d\n", tex->ja.dimensions, sz);
}

void printRangeError(int decide, int line, int depth, int num, int range, int dim, char* varname){
        printf("Line Number %d\t", line);
        printf("Statement type : Assignment\t");
        printf("Operator ***\t"); 
        printf("First operand lexeme *** and type ***\t");
        printf("Second operand lexeme *** and type ***    ");
		printf("Depth of node in parse tree %d    ", depth);
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
		typeElement temp; 
		temp.varname = varname;
		temp.dtype = tree->dtype;
        //printf("\n\n%s\n\n",temp.varname);
		//printf("%s %s, %d\n", tree->sym->nt, varname, tree->dtype);
        if(temp.dtype == 1){
			rangePair* listHead = tree->tex->ra.rangeListHead;
			while(listHead != NULL) {
				char* l = listHead->lower;
				char* u = listHead->upper;
                //printf("\n\n%s %s %s\n\n",temp.varname,l,u);
				if(isalpha(l[0]) != 0 || isalpha(u[0]) != 0){
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
		/*
		if(strcmp(tree->tok.token_name,"num")==0){
			typeElement* numret = (typeElement*)malloc(sizeof(typeElement));
			numret->dtype = 0;
			//numret->varname = (char*)malloc(sizeof(char)*20);
			//printf("%s\n",tree->tok.lexeme);
			//strcpy(numret->varname,tree->tok.lexeme);
			strcpy(numret->tex.prim_type,"integer");
			return *numret;
		} 
		*/
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
			else if(tempo.dtype==2){
				int num = atoi(ptr->lexeme);
				int low = tempo.tex.ja.lower;
				int up = tempo.tex.ja.upper;
				if(num > up) printRangeError(1, line,tree->depth,num,up,1, tempo.varname);
				else if(num < low) printRangeError(0,line,tree->depth,num,low,1, tempo.varname);
				else{
					row* rowptr = tempo.tex.ja.rowListHead;
					ptr = ptr->next;
					int num2 = atoi(ptr->lexeme);
					int counter = num - low;
					while(counter--){
						rowptr = rowptr->next;
					}
					if(rowptr->size < num2)  printRangeError(1, line,tree->depth,num2,rowptr->size,2, tempo.varname);
					else{
						if(tempo.tex.ja.dimensions==3){
							ptr = ptr->next;
							int num3 = atoi(ptr->lexeme);
							int counter2 = rowptr->size - num2;
							innerSize* inptr = rowptr->innerSizeHead;
							while(counter2--){
								inptr = inptr->next;
							}
							if(inptr->innerdim < num3) printRangeError(1, line,tree->depth,num3,inptr->innerdim,3, tempo.varname);
						}
					}
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
        if(!tree->child->sibling->sym->is_terminal && strcmp(tree->child->sibling->child->sym->nt, "open_sq")==0){
        //bound checking 
			tokenStream* ptr = tree->child->sibling->child->tok.next;
			if(idtel.dtype==1){
				rangePair* bound = idtel.tex.ra.rangeListHead;
				int localdim = 1;
				while(strcmp(ptr->lexeme,"]")){
					int num = atoi(ptr->lexeme);
					int low = atoi(bound->lower);
					int up = atoi(bound->upper);
					if(num > up) printRangeError(1,line,tree->depth,num,up,localdim, idtel.varname);
					if(num < low) printRangeError(0,line,tree->depth,num,low,localdim, idtel.varname);
					ptr = ptr->next;
					bound = bound->next;
					localdim++;
				}
			}
			else if(idtel.dtype==2){
				int num = atoi(ptr->lexeme);
				int low = idtel.tex.ja.lower;
				int up = idtel.tex.ja.upper;
				if(num > up) printRangeError(1, line,tree->depth,num,up,1, idtel.varname);
				else if(num < low) printRangeError(0,line,tree->depth,num,low,1, idtel.varname);
				else{
					row* rowptr = idtel.tex.ja.rowListHead;
					ptr = ptr->next;
					int num2 = atoi(ptr->lexeme);
					int counter = num - low;
					while(counter--){
						rowptr = rowptr->next;
					}
					if(rowptr->size < num2)  printRangeError(1, line,tree->depth,num2,rowptr->size,2, idtel.varname);
					else{
						if(idtel.tex.ja.dimensions==3){
							ptr = ptr->next;
							int num3 = atoi(ptr->lexeme);
							int counter2 = rowptr->size - num2;
							innerSize* inptr = rowptr->innerSizeHead;
							while(counter2--){
								inptr = inptr->next;
							}
							if(inptr->innerdim < num3) printRangeError(1, line,tree->depth,num3,inptr->innerdim,3, idtel.varname);
						}
					}
				}
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

int getTableSize(parseTree* tree,int counter){
	if(tree == NULL || strcmp(tree->sym->nt,"ASSIGNMENT_STATEMENTS") == 0) return counter;
	if(strcmp(tree->tok.token_name,"id")==0) counter++;
	return counter + getTableSize(tree->sibling,counter) + getTableSize(tree->child,counter);
}

typeElement* traverseParseTree(typeElement* table, parseTree* tree){
	parseTree* otree = tree;
	calculateDepth(tree,0);
	//if(tree == otree) printf("Adit is right\n");
	traverseParseTreeA(otree);
    //printf("%d\n",tentativeTableSize);
    table = (typeElement*) malloc(sizeof(typeElement)*getTableSize(tree,0));
	printf("\n\n-------------------------------------------------------------\n");
	printf("\nERRORS: \n\n");
	traverseParseTreeB(table, tree);
	printf("\n\nTraversal Completed Successfully\n\n");
    return table;
}

void printMenu()
{
	printf("\n\n-------------------------------------------------------------\n");
	printf("\nSelect numbers between 0 to 4 for the following operations.\n");
	printf("Press 0 to Exit\n");
	printf("Press 1 to Create Parse Tree\n");
	printf("Press 2 to Traverse Parse Tree\n");
	printf("Press 3 to Print Parse Tree\n");
	printf("Press 4 to Print Type Expression Table\n");
	printf("-------------------------------------------------------------\n");
	printf("Enter: ");
}

int main(int argc, char* argv[])
{		
	printf("-------------------------------------------------------------\n");
	printf("Welcome! Hope you are having a good day!\n");
	llnode* G = readGrammar("grammar.txt", G);
	tokenStream* head = tokeniseSourcecode(argv[1], head);
	parseTree* tree;
	typeElement* table; 
	int option;
	while(1){
		numvars = 0, ind = 0;
		printMenu();
		scanf("%d",&option);
		if(option == 0 ){
			printf("\nExiting...\n");
			return 0;
		}
		else if(option == 1){
				tree = createParseTree(tree,head,G);
		}
		else if(option == 2){
			tree = createParseTree(tree,head,G);
			table = traverseParseTree(table,tree);
			}
		else if(option == 3){
			tree = createParseTree(tree,head,G); 
			table = traverseParseTree(table,tree);
			printPerfectParseTree(tree);
		}
		else if(option == 4){
			tree = createParseTree(tree,head,G);
			table = traverseParseTree(table,tree);
			printTypeExpressionTable(table);
		}
		else
		{
			printf("Enter number between 1 and 4\n");
		}
	}
    return 0;
}
