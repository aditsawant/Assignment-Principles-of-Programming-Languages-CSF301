#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <limits.h>
#define num_rules 68

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
    int lower;
    int upper;
	struct rangePair* next;
} rangePair;

typedef struct innerSize {
	int innerdim;
	innerSize* next;
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
	tokenStream* tok;			// for line num and lexeme, add in create parse tree
	//union
	typeExp tex;
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
void readGrammar(char* fname, llnode* grammar){
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
					strcpy(temp->sym.t, sym_read);
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
						strcpy(temp->sym.t, sym_read);
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
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

char* identifyToken(char* c){
	char * s = (char*)malloc(sizeof(char)*30);
	if(s == NULL) printf("malloc unsuccessful\n");
	if(strcmp(c,"program") == 0) return s = "program";
	else if(strcmp(c,"()") == 0) return s = "brackets";
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

tokenStream* tokeniseSourcecode(char* fname, tokenStream *head){
	FILE* fptr = fopen(fname, "r");
	int line_num = 1;
	char buffer[200];
	head = (tokenStream*)malloc(sizeof(tokenStream));
	tokenStream* s;
	s = head;
	while (fgets(buffer, sizeof(buffer), fptr) != NULL) {
        
		char *sym_read;
		int i;
		sym_read = strtok(buffer, " \n");
		for (i = 0; sym_read != NULL; i++){
			tokenStream* temp = (tokenStream*)malloc(sizeof(tokenStream));
			temp->line_num = line_num;
			strcpy(temp->lexeme,sym_read);
			strcpy(temp->token_name, identifyToken(sym_read));
			temp->next = NULL;
			temp->before = s;
			s->next = temp;
			s = temp;
			sym_read = strtok(NULL, " \n");
		}
		line_num++; 
	} 
	fclose(fptr);
	head = head->next;
	head->before = NULL;
	return head;
}
//////////////////////////////////////////////////////////////////

bool fill_aux(Stack* aux, symbol lhs_sym, llnode* G, int counter){
	for(int i=0; i< num_rules; i++){ //define it
		//if(G[i].sym == lhs_sym){
		//printf("%d\n",strcmp(G[i].sym.nt, lhs_sym.nt));
		if(strcmp(G[i].sym.nt, lhs_sym.nt) == 0){
			if(counter==0){
				llnode* temp = G + i;
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


typeExp* populateTex(symbol sym, parseTree* tree){
	typeExp* tex = (typeExp*) malloc(sizeof(typeExp));
	if(sym.is_terminal){
		if(strcmp(sym.t, "integer") == 0){
			strcpy(tex->prim_type, "integer");
		}
		else if(strcmp(sym.t, "real") == 0){
			strcpy(tex->prim_type, "real");	
		}
		else if(strcmp(sym.t, "boolean") == 0){
			strcpy(tex->prim_type, "boolean");
		}
		else {
			tex = NULL;
		}
	}
	else{
		//sym is a non-terminal, then what?
		if(strcmp(sym.nt, "REC_ARRAY") == 0){
			parseTree* temp = tree;
			int counter = 0;
			rangePair* rtemp = (rangePair*) malloc(sizeof(rangePair));
			rangePair* orig_rtemp = rtemp;
			while(strcmp(temp->tok->lexeme, "of") != 0){
				counter++;
				if(strcmp(temp->tok->lexeme, "[") == 0){
					rtemp->next = tex->tok->rangeListHead;
					tex->tok->rangeListHead = (rangePair*) malloc(sizeof(rangePair));
					strcpy(tex->ra->rangeListHead->lower, temp->sibling->child->tok->lexeme);
				}
				if(strcmp(temp->tok->lexeme, "..") == 0){
					strcpy(tex->ra->rangeListHead->upper, temp->sibling->child->tok->lexeme);
					tex->tok->rangeListHead->next = NULL;
				}
				rtemp = rtemp->next;
				temp = temp->sibling;
			}
			tex->ra->dimensions = counter/5;
		}
		if(strcmp(sym.nt, "JAGGED_ARRAY") == 0){
			tex->ja->dimensions = _____;
			tex->ja->lower = _________;
			tex->ja->upper = _________;
			tex->ja->rowListHead = _________;
		}
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

	Stack* stack = createStack(100);
	Stack* aux = createStack(100);

	ret_st.verdict = fill_aux(aux,*lhs_sym, G, counter);
	if(ret_st.verdict == true) return ret_st;
	copy_stack(aux,stack);

	printf("Start of subtree %s\n", lhs_sym->nt);
	parseTree* temp;
	/*
	if(strcmp(lhs_sym->nt,"DIMS")==0 && strcmp(head->token_name,"of") == 0){
		
		ret_st.verdict = false;
		return ret_st;
	} 
	*/
	while(!isEmpty(stack))
	{	
		int nextcounter = 0;
		
		if(head == NULL) break;
	
		if(strcmp(stack->array[stack->top].t,"epsilon") == 0){
					printf("EPSILON found\n");
					temp = (parseTree*)malloc(sizeof(parseTree));
					t->child = temp;
					temp->sym = &(stack->array[stack->top]);
					temp->sibling = NULL;
					temp->child = NULL;
					temp->tok = NULL;
					temp->tex = populateTex(stack->array[stack->top]);
					pop(stack);
		}
		else if(stack->array[stack->top].is_terminal)
		{
//			printf("Entered While of subtree %s and stack top is T %s at line %d\n", lhs_sym->nt, stack->array[stack->top].t,head->line_num);

			if(strcmp(stack->array[stack->top].t,head->token_name) == 0){
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
				temp->tok = head;
				temp->tex = populateTex(stack->array[stack->top]);
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
					temp->tok = NULL;
					temp->tex = populateTex(stack->array[stack->top]);
				}
				/*
				else{
					//return
					ret_st.pt = NULL;
					return ret_st;
				}
				*/
//				printf("Head after %s\n", head->lexeme);
				
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
					temp->tok = NULL;
					temp->tex = populateTex(stack->array[stack->top]);
					temp = temp->sibling;
				}
				/*
				else{
					ret_st.pt = NULL;
					return ret_st;
				}
				*/
//				printf("Head after %s\n", head->lexeme);
				
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
	if(!tree->sym->is_terminal){
		printf("%s ", tree->sym->nt);
		printParseTree(tree->child);
	}
	else{
		printf("%s ", tree->sym->t);
	}
	printParseTree(tree->sibling);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/

void traverseParseTreeA(parseTree* tree, typeElement* table){
	if(tree == NULL) return;
	tree->tex = populateTex(tree->sym);
	if(!tree->sym->is_terminal){
		traverseParseTreeA(tree->child);
	}
	traverseParseTreeA(tree->sibling);
	//self
	// field 1 table->varname = 
	

}




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(){
	// bool val; // default false;
	// printf("%s", val ? "true" : "false");
	
	llnode* G = (llnode*) malloc(sizeof(llnode)*80);
	readGrammar("grammar.txt", G);
	/*
	for(int i=0; i<67; i++){
		llnode* temp = G+i;
		printf("\n");
		while(temp != NULL){
			if(temp->sym.is_terminal) printf("%s ", temp->sym.t);
			else printf("%s ", temp->sym.nt);

			temp = temp->next;
		}		
	}
	*/
	tokenStream* head;
	head = tokeniseSourcecode("sourcecode3.txt", head);
	/*
	while(head->next != NULL){
		printf("%s\n", head->token_name);
		head = head->next;
	}
	while(head != NULL){
		printf("%s\n", head->token_name);
		head = head->before;
	}
	*/
	
	parseTree* tree;
	tree = createParseTree(tree,head,G);
	printf("\n\n");
	printParseTree(tree);
	
	//printf("%s", tree->child->sibling->sibling->sibling->child->child->child->sym->nt);
	
	return 0;
}
