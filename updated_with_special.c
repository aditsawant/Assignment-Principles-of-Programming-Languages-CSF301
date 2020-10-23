#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <limits.h>
#define num_rules 67

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

typedef struct parseTree {
    symbol* sym;
    struct parseTree *child;   // point to children of this node
    struct parseTree *sibling;    // point to next node at same level
	//union
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
}special;

///////////////////////////////////////////////////////////////////
void readGrammar(char* fname, llnode* grammar){
	//grammar = 

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

void fill_aux(Stack* aux, symbol lhs_sym, llnode* G, int counter){
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
				return;
			}
			else counter--;
		} 
	} 
	return;
}

void copy_stack(Stack* aux, Stack* stack){
	while(!isEmpty(aux)){
		push(stack, *pop(aux));
	}
}

special createSubTree(symbol* lhs_sym, tokenStream *head, llnode* G, int counter){
	special st;
	st.pt = NULL;
	if(head == NULL) return st;
	parseTree* t = (parseTree*)malloc(sizeof(parseTree));
	t->sym = lhs_sym;
	t->sibling = NULL;
	Stack* stack = createStack(100);
	Stack* aux = createStack(100);
	fill_aux(aux,*lhs_sym, G, counter);
	printf("4\n");
	copy_stack(aux,stack);
	//symbol temp = stack->array[stack->top];
	printf("5\n");
	parseTree* temp;
	int nextcounter = 0;
	while(!isEmpty(stack))
	{	
		printf("6\n");
		if(head == NULL) return st;

		if(stack->array[stack->top].is_terminal)
		{
			if(strcmp(stack->array[stack->top].t,head->token_name) == 0){
				head = head->next;
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
				pop(stack);
			}
			else{
				return st; // if NULL is returned, something is wrong
			}
		}
		else{
			//calls createsubtree, but verifies before attaching.
			//also backtrack code 
			if(t->child == NULL){
				do{
					st = createSubTree(&(stack->array[stack->top]), head, G, nextcounter);
					t->child = st.pt;
					nextcounter++;
				} while(t->child == NULL);
				//printf("out of while\n");
				//if(endhead == NULL || *endhead == NULL) printf("null\n");
				printf("%s\n", head->lexeme);
				head = st.endhead;
				printf("%s\n", head->lexeme);
				//printf("caller %s\n", (*endhead)->lexeme);
			}
			else{
				do{
					st = createSubTree(&(stack->array[stack->top]), head, G, nextcounter);
					temp->sibling = st.pt;
					nextcounter++;
				} while(temp->sibling == NULL);
				head = st.endhead;
				temp = temp->sibling;
				temp->sibling = NULL;
				temp->child = NULL;
			}
		}
	}
	st.pt = t;
	st.endhead = head;
	//printf("%s\n",head->lexeme);
	//printf("callee %s\n", (*endhead)->lexeme);
	return st;
}

void createParseTree(parseTree* t, tokenStream *head, llnode* G)
{
	// t = (parseTree*)malloc(sizeof(parseTree));
	// t->sym = G[0].sym;
	// t->sibling = NULL;
	special st;
	st = createSubTree(&(G[0].sym) , head, G, 0);
	t = st.pt;
	// for(int i=0;i<numRules;i++)
	// {
	// 	if(strcmp(G[i].sym.nt,"MAINPROG") == 0)
	// 	{
	// 		temp = G+i;
	// 		break;
	// 	}
	// }
}

int main(){
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
	head = tokeniseSourcecode("sourcecode.txt", head);
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
	createParseTree(tree,head,G);

	printf("%s", tree->child->sibling->sibling->sibling->child->child->child->sym->nt);

	return 0;
}
