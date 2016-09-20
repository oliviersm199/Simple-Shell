#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "list.h"
/*
Olivier Simard-Morissette
260563480
ECSE 427
February 12th, 2016
*/

/*Code reused from the tutorial on how to create a linkedlist in c.
  Adaptations were made to have an extra method which finds a value
  given what it's index number as a command should be. 
*/

Node* head = NULL;
Node* tail = NULL;

//adds a new element to our linked list. 
int push(char* s[],int bg, int len) {
    if (head == NULL) {
        head = (Node*) malloc(sizeof(Node));
        tail = head;
    }
    else {
        tail->next = (Node*) malloc(sizeof(Node));
        tail = tail->next;
    }
    tail -> bg = bg;
    transferStrings(tail->data,s,20);
    return 1;
}

//function that faciliates the transfer of two arrays of strings.
int transferStrings(char**array1,char**array2,int sizeLen){
	int i;
	for(i = 0; i<sizeLen && array2[i]!= NULL; i++){
		int lengthStr = strlen(array2[i]);
		array1[i] = malloc(lengthStr*sizeof(char));
		strcpy(array1[i],array2[i]);
	}
	tail ->data[i+1] = NULL;
	return 1; 
}

//get a pointer to a node at a particular position in the linked list.
//if the next value in the string is null, just returns null.
//be careful if list is head is null.
Node *getNodeData(int pos){
	Node* temp = head;
	while(pos>1){
		if(temp -> next == NULL){
			//user entered a number greater
			//than list expected
			return NULL;
		}
		temp = temp -> next;
		pos--;
	}
	return temp;	
}

//method to help get the length of a list. 
int listLength(){
	int counter = 0;
	for(Node *it = head; it != NULL; it = it -> next){
		counter++;
	}
	return counter;
}
