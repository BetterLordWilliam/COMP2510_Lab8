#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 256

// A01372608

// enums
typedef enum dataTypes { 
    SHORT, INT, FLOAT, CHAR, STRING 
} DataType;

// Structs
typedef struct listNode {
    DataType t;
    size_t sLen;

    union nodeData {
        short shortInteger;
        int integer;
        float floating;
        char character;
        char* string;
    } data;

    struct listNode *next;
} ListNode;

// Global variables
FILE *in;
FILE *out;
size_t bytes;           // Size of the data in the LinkedList

// Method headers
void innitLinkedList(ListNode *head, int *dtP);
void createNode(ListNode *head, const char *data, int dataType);
void printErrorExit();
void freeList(ListNode *head);
void basicPrint(ListNode *head);

/**
 * main:            Drives the program.
 * param argc:      number of arguments
 * param argv:      the array of the arguments
*/
int main(int argc, char *argv[]){
    // Lab setup
    if (argc != 3)
        return 1;

    in = fopen(argv[1], "r");
    out = fopen(argv[2], "w");

    // Important variables
    int dataType = 0, *dtP;
    dtP = &dataType;
    ListNode *head = malloc(sizeof(ListNode));   // Actual list head
    head->next = NULL;

    // Bad file open
    if (in == NULL || out == NULL)
        return 1;

    // Begin the Process
    innitLinkedList(head, dtP);
    basicPrint(head);

    return 0;
}

/**
 * printErrorExit:      Prints 'exit' to out file and closes files. Terminates program.
*/
void printErrorExit() {
    if (out != NULL) {
        if (in != NULL) fclose(in);
        fprintf(out, "error\n");
        fclose(out);
    } else {
        printf("printErrorExit called incorrectly.\n");
    }
    exit(1);
}

/**
 * freeList:                    Frees memory used by the linkedlist.
 * param *head:                 the start of the list   
*/
void freeList(ListNode *head) {
    ListNode *itr = head->next;

    while (itr != NULL) {
        ListNode *temp = itr->next;
        free(itr);
        itr = temp;
    }
}

/**
 * basicPrint:                  basic print function, prints all student information. For testing purposes.
 * param *head:                 the start of the list
*/
void basicPrint(ListNode *head) {
    ListNode *itr = head->next;

    while(itr != NULL) {
        if (itr->t == STRING)
            printf("%s\n", itr->data.string);
        if (itr->t == CHAR)
            printf("%c\n", itr->data.character);
        if (itr->t == SHORT)
            printf("%d\n", itr->data.shortInteger);
        if (itr->t == INT)
            printf("%d\n", itr->data.integer);
        if (itr->t == FLOAT)
            printf("%f\n", itr->data.floating);
        itr = itr->next;
    }
}

/**
 * createNode:                  Creates a node in the LinkedList
 * param *head:                 Start of the list
 * param *data:                 Value of the list node
*/
void createNode(ListNode *head, const char *data, int dataType) {
    //printf("%s\n", data);
    ListNode *itr = head->next;
    ListNode *newNode = malloc(sizeof(struct listNode));
    newNode->next = NULL;

    switch (dataType) {
        case(1):
            newNode->t = SHORT;
            newNode->data.shortInteger = atoi(data);
            break;
        case(2):
            newNode->t = INT;
            newNode->data.integer = atoi(data);
            break;
        case(3):
            newNode->t = FLOAT;
            newNode->data.floating = atof(data);
            break;
        case(4):
            newNode->t = CHAR;
            newNode->data.character = *(data);
            break;
        case(5):
            int sSize = strlen(data);
            char *dest = malloc(sizeof(char) * (sSize + 1));
            *(dest + (int) sSize) = '\0';
            memcpy(dest, data, sSize);
            newNode->t = STRING;
            newNode->sLen = sSize + 1;
            newNode->data.string = dest;
            break;
    }

    if (itr == NULL) {
        head->next = newNode;
        return;
    } else {
        while (itr->next != NULL) {
            itr = itr->next;
        }
        itr->next = newNode;
    }
}

/**
 * innitLinkedList:             Reads the file and creates a linkedlist. Saves the option.
 * param *head:                 Pointer to the start of the linkedlist
 * param *dtP:                  Pointer to the address which holds the reference to the type of data
*/
void innitLinkedList(ListNode *head, int *dtP) {
    // Get the data type
    int optionT = 0;
    fscanf(in, "%d\n", &optionT);
    if (!(optionT >= 1 && optionT <= 5))
        printErrorExit();

    char line[MAX_LINE_LENGTH];     // Line with set size for input.
    char *array = 0;
    int elements = 0;
    *dtP = optionT;

    fseek(in, 2, SEEK_SET);         // Put it to the line with numbers.
    char delim = ',';
    if (fgets(line, sizeof(line), in) != NULL) {
        array = strtok(line, &delim);

        // Checks for delimiter
        while (array != 0) {
            createNode(head, array, *dtP);
            array = strtok(0, &delim);
            elements++;             // Increment element count.
        }
        if (elements == 0)
            printErrorExit();       // Not elements, bad input.

    } else {
        printErrorExit();           // Bad news.
    }
}