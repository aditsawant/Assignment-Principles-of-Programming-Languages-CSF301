#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <limits.h>
#define num_rules 72
int numvars = 0;
typedef enum {Primitive, Rect_Array, Jagged_Array} dtype;

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
	dtype dtype;
	enum { Static, Dynamic, not_applicable } nature;
	typeExp tex;
} typeElement;

//////////////////////////////////////////////////////////////////////

typedef struct parseTree {
    symbol* sym;
    struct parseTree *child;   // point to children of this node
    struct parseTree *sibling;    // point to next node at same level
	tokenStream tok;			// for line num and lexeme, add in create parse tree
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
char *strstrip(char *s)
{
        size_t size;
        char *end;

        size = strlen(s);

        if (!size)
                return s;

        end = s + size - 1;
        while (end >= s && isspace(*end))
                end--;
        *(end + 1) = '\0';

        while (*s && isspace(*s))
                s++;

        return s;
}


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

	ret_st.verdict = fill_aux(aux,*lhs_sym, G, counter);
	if(ret_st.verdict == true) return ret_st;
	copy_stack(aux,stack);

//	printf("Start of subtree %s\n", lhs_sym->nt);
	parseTree* temp;
	while(!isEmpty(stack))
	{	
		int nextcounter = 0;
		
		if(head == NULL) break;
	
		if(strcmp(stack->array[stack->top].t,"epsilon") == 0){
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
					//temp->tok = NULL;
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
		printf("(%s %s)", tree->sym->t, tree->tok.lexeme);
	}
	printParseTree(tree->sibling);
}

void printTypeExpTable(typeElement* table){
	for(int i = 0; i < numvars; i++){
		printf("table[%d] : %s\n", i, table[i].varname);
	}
}


typeExp* populateTex(parseTree* tree){
	symbol sym = *(tree->sym);
	typeExp* tex = (typeExp*) malloc(sizeof(typeExp));
	if(sym.is_terminal){
		tree->dtype = 0;
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
			tree->dtype = 1;
			parseTree* temp = tree;
            tokenStream *token = &tree->child->tok;
            printf("%s\n", tree->child->sym->nt);
            printf("%s", token->lexeme);
            int counter = 0;
			rangePair* rtemp, * orig_rtemp = NULL;
			printf("%s\n", tree->child->tok.token_name);
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
			tree->dtype = 2;
			tree = tree->child;
			if(strcmp(tree->sym->nt, "TWO_JA") == 0){
				tex->ja.dimensions = 2;	
			}
			else{
				tex->ja.dimensions = 3;	
			}
			tree = tree->child->sibling; //now at array of tree
			tokenStream *token = &tree->tok;
			
			tex->ja.lower = atoi(token->next->next->lexeme);
			tex->ja.upper = atoi(token->next->next->next->next->lexeme);
			int counter = tex->ja.upper - tex->ja.lower + 1;
			// printf("counter %d\n", counter);
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
	if(strcmp(tree->sym->t,"declare") == 0){
		parseTree* temp = tree;
		while(temp->sibling != NULL) temp = temp->sibling;
		temp = temp->child->child;
		tree->tex = populateTex(temp);
		//printf("\n\ndeclare %d\n", tree->tex->ra.dimensions);
	}
	if(!tree->sym->is_terminal){
		traverseParseTreeA(tree->child);
	}
	traverseParseTreeA(tree->sibling); 
}

void printError(int e){
	/*
		Type Mismatch
		2D Jagged Array size error
		3D Jagged Array size error
		RectArray IOB
	*/
	switch(e){
		case 1: {
			printf("Type Mismatch\n");
			break;
		}
		case 2: {
			printf("2D JaggedArray size error\n");
			break;
		}
		case 3: {
			printf("3D JaggedArray size error\n");
			break;
		}
		case 4: {
			printf("Rect Array ind Out of Bounds error\n");
			break;
		}
		default: printf("Some other error\n");
	}

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
		printf("dtype: %d\n", temp.dtype);
		if(temp.dtype == 1){
			// write for nature;
		} else temp.nature = 2;
		temp.tex = *tree->tex;
		return temp;
}


// bool areTexEqual(typeExp t1, typeExp t2){
// 	return ();
// }

// bool areTypeElemsEqual(typeElement t1, typeElement t2){
// 	return (t1.varname == t2.varname && t1.dtype == t2.dtype && areTexEqual(t1.tex, t2.tex) && t1.nature == t2.nature);
// }


typeElement recursiveTraverse(typeElement* table, parseTree* tree){
	if(tree->child == NULL && tree->sibling == NULL){
		printf("Both NULL reached for %s.\n", tree->tok.lexeme);
		typeElement tempo = fetchTypeElement(table, tree->tok.lexeme); //Correct
		printf("%s\n", tempo.varname);
		return tempo;
	}
	else if(tree->child == NULL && tree->sibling != NULL){
		return recursiveTraverse(table, tree->sibling); // Correct
	} 
	else if(tree->child != NULL && tree->sibling == NULL){
		return recursiveTraverse(table, tree->child); // Correct?
	}
	else if(tree->child != NULL && tree->sibling != NULL){
		typeElement tel = recursiveTraverse(table, tree->sibling);
		typeElement ptr = recursiveTraverse(table, tree->child);
		if(memcmp(&tel.tex, &ptr.tex, sizeof(tel.tex)) != 0){
			printf("%s %s\n", tel.varname, ptr.varname);
			printf("telptr wala error\n");
			printError(1);
		}
		// if(tel != recursiveTraverse(table, tree->child)) {/* ERROR*/}
		else{
			return tel;
		}
	}

}

int ind = 0;

void traverseParseTreeB(typeElement* table, parseTree* tree){
	// For Assignment Statements
	if(tree == NULL) return;
	if(strcmp(tree->sym->nt,"ASSIGNMENT_STATEMENT") == 0){
		printf("In the Assign section\n");
		// printf("NUMVARS: %d\n", numvars);
		// printTypeExpTable(table);
		typeElement exptel;
		if(tree->child->sibling->sym->is_terminal){
			exptel = recursiveTraverse(table, tree->child->sibling->sibling->child); // For BooleanExp ka child
		}
		else {
			// printf("Exptel...\n");
			exptel = recursiveTraverse(table, tree->child->sibling->sibling->sibling->child); // For Arith ka child
			
		}
		typeElement idtel = fetchTypeElement(table, tree->child->tok.lexeme);

		printf("%s %s\n", exptel.varname, idtel.varname);
		// printf("%s %s\n", exptel.tex.prim_type, idtel.tex.prim_type);

		if(memcmp(&exptel.tex, &idtel.tex, sizeof(idtel.tex)) != 0){
			printf("exptel idtel wala error\n");
			// printError(5);
		}
		// if(tel != recursiveTraverse(table, tree->child)) {/* ERROR*/}
	}
	else{
		// For Declaration statements:
		if(strcmp(tree->sym->nt, "declare") == 0){
			printf("In the Declare section\n");
			parseTree* declareptr = tree;
			if(strcmp(tree->sibling->sym->nt, "id") == 0){
				printf("In the single var section\n");
				table[ind] = populateTypeElement(tree, tree->sibling->tok.lexeme);
				numvars++;
				printf("NUMVARS: %d\n\n", numvars);
				table = (typeElement*) realloc(table, sizeof(table) + sizeof(typeElement));
				ind++;
			} else{
				printf("In the List of vars section\n");
				parseTree* idlistptr = tree->sibling->sibling->sibling->sibling; // now pointing to idlist
				while(idlistptr->child->sibling != NULL){
					printf("In the while loop\n");
					typeElement temptel = populateTypeElement(tree, idlistptr->child->tok.lexeme);
					// printf("%d\n", ind);
					
					table[ind] = temptel;
					// printf("Out\n");
					idlistptr = idlistptr->child->sibling;
					numvars++;
					printf("NUMVARS: %d\n\n", numvars);
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
					printf("NUMVARS: %d\n\n", numvars);
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


void traverseParseTree(typeElement* table, parseTree* tree){
	traverseParseTreeA(tree);
	traverseParseTreeB(table, tree);
	printf("\nTraversal Completed Successfully\n");
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
	//printf("\n\n");
	//printParseTree(tree);
	
	typeElement* table = (typeElement*) malloc(sizeof(typeElement)*6); 
	traverseParseTree(table, tree);
	
	return 0;
}
