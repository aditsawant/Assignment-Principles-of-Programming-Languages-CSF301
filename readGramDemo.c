#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>


typedef struct{
    union
    {
        char* t;
        char* nt;
    };
    bool is_terminal;
} symbol;

typedef struct llnode{
	symbol sym;
	struct llnode* next;
} llnode;


void readGrammar(char* fname, llnode* grammar){
	FILE* fptr = fopen(fname, "r");

	int rule_num = 0;
	char buffer[200];

	while (fgets(buffer, sizeof(buffer), fptr) != NULL) {
		// printf("7");
		// printf(" %s ", buffer);
		char *sym_read;
		int i;

		sym_read = strtok(buffer, " \n");
		for (i = 0; sym_read != NULL; i++) {
			printf("8");
			if(i == 0){
				printf(" %s ", sym_read);
				// LHS
				// if(nt_array.contains(*sym_read)){}   This will be done in the source code checking
				llnode* temp = (llnode*) malloc(sizeof(llnode));
				temp->sym.is_terminal = false;
				strcpy(temp->sym.nt, sym_read);
				printf(" %s ", temp->sym.nt);
				temp->next = NULL;
				grammar[rule_num] = *temp;
			}
			else {
				printf(" %s ", sym_read);
				llnode* temp = (llnode*) malloc(sizeof(llnode));
				if(temp == NULL) printf("malloc unsuccessful");
				if(!strcmp(sym_read, "R1")) {
					temp->sym.is_terminal = true; 
					strcpy(temp->sym.t, sym_read);
					temp->next = NULL;
				}
				else {
					// Check if upper or lower case
					if(isupper(sym_read[0])){
						temp->sym.is_terminal = false; 
						strcpy(temp->sym.nt, sym_read);
						temp->next = NULL;
					}
					else {
						temp->sym.is_terminal = true; 
						strcpy(temp->sym.t, sym_read);
						temp->next = NULL;
					}
				}
				llnode temphead = grammar[rule_num];
				while(temphead.next != NULL){
					temphead = *temphead.next;
				}
				temphead.next = temp;
			}
			printf(" %s ", sym_read);
			sym_read = strtok(NULL, " \n");
		}
		printf("\n");
		rule_num++;
	}
	fclose(fptr);
}

int main(){
	printf("1");
	llnode* G = (llnode*) malloc(sizeof(llnode)*80);
	printf("2");
	readGrammar("grammar.txt", G);
	printf("3");
	llnode temp = G[0];
	printf("4");
	while(temp.next != NULL){
		printf("5");
		if(temp.sym.is_terminal) printf("%s ", temp.sym.t);
		else printf("%s ", temp.sym.nt);

		temp = *temp.next;
	}
	printf("6");
	return 0;
}