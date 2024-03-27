#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 256

#define MIN_SHORT -32767 
#define MAX_SHORT 32767
#define MIN_INT -2147483648
#define MAX_INT 2147483647

// A01372608

// enums
typedef enum dataTypes { 
    SHORT, INT, DOUBLE, CHAR, STRING 
} DataType;

// Structs
typedef struct listNode {
    DataType t;
    int isHead;
    int byte_size;

    union nodeData {
        short shortInteger;
        int integer;
        float doubleNum;
        char character;
        char* string;
    } data;

    struct listNode *next;
} ListNode;

// Global variables
FILE *in;
FILE *out;

// Basic Method headers
void innitLinkedList(ListNode *head, int *dtP, int *elemCount);
void createNode(ListNode *head, const char *data, int dataType);
void printErrorExit();
void freeList(ListNode *head);
void basicPrint(ListNode *head);
void exportList(ListNode *head);

// LinkedList sort methods
ListNode* sortedMerge(ListNode* a, ListNode* b);
double compareNodes(ListNode *a, ListNode *b);
void mergeSort(ListNode** headRef);
void frontBackSplit(ListNode* source,
                    ListNode** frontRef, ListNode** backRef);

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
    int elements = 0, *elemCount;
    elemCount = &elements;
    dtP = &dataType;
    ListNode *head = malloc(sizeof(ListNode));   // Actual list head
    head->next = NULL;
    head->isHead = 1;

    // Bad file open
    if (in == NULL || out == NULL)
        return 1;

    // Begin the Process
    innitLinkedList(head, dtP, elemCount);
    
    //basicPrint(head);
    //printf("\n");
    mergeSort(&(head->next));

    //basicPrint(head);
    exportList(head);

    freeList(head);
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
 * printNode:                   Prints the contents of a node.
 * param *node:                 ListNode we are printing the contents
*/
void printNode(ListNode *node) {
    if (node->t == STRING)
        printf("%s\n", node->data.string);
    if (node->t == CHAR)
        printf("%c\n", node->data.character);
    if (node->t == SHORT)
        printf("%d\n", node->data.shortInteger);
    if (node->t == INT)
        printf("%d\n", node->data.integer);
    if (node->t == DOUBLE)
        printf("%f\n", node->data.doubleNum);
    node = node->next;
}

/**
 * basicPrint:                  basic print function, prints all student information. For testing purposes.
 * param *head:                 the start of the list
*/
void basicPrint(ListNode *head) {
    ListNode *itr = head->next;

    while(itr != NULL) {
        printNode(itr);
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
    newNode->isHead = 0;

    switch (dataType) {
        case(1):
            newNode->t = SHORT;
            long long numberS = atoi(data);                                            // Range check
            if (!(numberS >= MIN_SHORT && numberS <= MAX_SHORT) || numberS == 0)       // In range of short and Atoi was successful
                printErrorExit();
            newNode->data.shortInteger = (short) numberS;
            break;
        case(2):
            newNode->t = INT;
            long long number = atoi(data);                                             // Range check
            if (!(number >= MIN_SHORT && number <= MAX_SHORT) || number == 0)          // In range of int and Atoi was successful
                printErrorExit();
            newNode->data.integer = (int) number;
            break;
        case(3):
            if (atof(data) == 0)
                printErrorExit();
            newNode->t = DOUBLE;
            newNode->data.doubleNum = atof(data);
            break;
        case(4):
            newNode->t = CHAR;
            newNode->data.character = *(data);
            break;
        case(5):
            char *dest;
            if (data[strlen(data) - 1] == '\n') {
                dest = malloc(sizeof(char) * strlen(data)-1);           // Get rid of the new line characters at the end of a string
                memcpy(dest, data, strlen(data)-1);
                dest[strlen(data)-1] = '\0';                            // Set end of string character manually
            } else {
                dest = malloc(sizeof(char) * strlen(data));             // Copy memory normally
                memcpy(dest, data, strlen(data));
            }
            newNode->t = STRING;
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
void innitLinkedList(ListNode *head, int *dtP, int *elemCount) {
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
    char delim[] = ",";
    if (fgets(line, sizeof(line), in) != NULL) {
        array = strtok(line, delim);

        // Checks for delimiter
        while (array != 0) {
            createNode(head, array, *dtP);
            array = strtok(0, delim);
            elements++;             // Increment element count.
        }
        if (elements == 0)
            printErrorExit();       // Not elements, bad input.

    } else {
        printErrorExit();           // Bad news.
    }
    *elemCount = elements;
}

/**
 * frontBackSplit:              Spilts a LinkedList in half, stores references to front and back references.
 * param *source:               The source of the list
 * param **frontRef:            address where we will put reference to front ref
 * param **backRef:             address where we will put references to back ref
*/
void frontBackSplit(ListNode* source,
                    ListNode** frontRef, ListNode** backRef) {
    ListNode* fast;
    ListNode* slow;
    slow = source;
    fast = source->next;

    /* Advance 'fast' two nodes, and advance 'slow' one node */
    while (fast != NULL) {
        fast = fast->next;
        if (fast != NULL) {
            slow = slow->next;
            fast = fast->next;
        }
    }

    /* 'slow' is before the midpoint in the list, so split it in two
    at that point. */
    *frontRef = source;
    *backRef = slow->next;
    slow->next = NULL;
}

/**
 * compareNodes:                Compares list node data in the linkedlist.
 * param *a:                    Node1
 * param *b:                    Node2
*/
double compareNodes(ListNode *a, ListNode *b) {
    switch (a->t) {
        case SHORT:
            return a->data.shortInteger - b->data.shortInteger;
        case INT:
            return a->data.integer - b->data.integer;
        case DOUBLE:
            return a->data.doubleNum - b->data.doubleNum;
        case CHAR:
            return a->data.character - b->data.character;
        case STRING:
            return strcmp(a->data.string, b->data.string);
        default:
            return 0;  // Unknown type
    }
}

/**
 * sortedMerge:                 Merges two sorted lists.
 * param *a:                    Start of list one
 * param *b:                    Start of list two
*/
ListNode* sortedMerge(ListNode* a, ListNode* b) {
    ListNode* result = NULL;    // Create a reference for result node

    if (a == NULL)
        return (b);             // We are at the end of a, rest of list is rest of b
    else if (b == NULL)
        return (a);             // We are at the end of b, rest of list is rest of a 

    // Pick either a or b, and recur 
    if (compareNodes(a, b) <= 0) {
        result = a;                                 // At this position, a is the link
        result->next = sortedMerge(a->next, b);     // Move a forward

    } else {        
        result = b;                                 // At this position, b is the link
        result->next = sortedMerge(a, b->next);     // Move b forward
    }
    return (result);
}

/**
 * mergeSort:                   Starts the merge sort process for a LinkedList.
 * param **headRef:             The reference to the start of the list
*/
void mergeSort(ListNode** headRef) {
    ListNode* head = *headRef;
    ListNode* a;
    ListNode* b;

    // Base case -- length 0 or 1
    if ((head == NULL) || (head->next == NULL)) {
        return;
    }

    // Split head into 'a' and 'b' sublists
    frontBackSplit(head, &a, &b);

    // Recursively sort the sublists
    mergeSort(&a);
    mergeSort(&b);

    // answer = merge the two sorted lists together
    *headRef = sortedMerge(a, b);
}

/**
 * exportList:                      Exports list to the output file.
 * param *head:                     The head of the LinkedList
*/
void exportList(ListNode *head) {
    ListNode *itr = head->next;
    int i = 0;
    while (itr != NULL) {
        if (i != 0) fprintf(out, ",");
        switch (itr->t) {
            case(SHORT):
                fprintf(out, "%d", itr->data.shortInteger);
                break;
            case(INT):
                fprintf(out, "%d", itr->data.integer);
                break;
            case(DOUBLE):
                fprintf(out, "%g", itr->data.doubleNum);
                break;
            case(CHAR):
                fprintf(out, "%c", itr->data.character);
                break;
            case(STRING):
                fprintf(out, "%s", itr->data.string);
                break;
        }
        itr = itr->next;
        i++;
    }
    fprintf(out, "\n");
}