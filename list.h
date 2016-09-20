/*
Olivier Simard-Morissette
260563480
ECSE 427
February 12th, 2016
*/
typedef struct _Node {
    char* data[20];
    int bg;
    struct _Node* next;
} Node;

int push(char* s[],int bg ,int len);
Node *getNodeData(int pos);
int transferStrings(char **arr1, char **arr2, int sizeLen);
