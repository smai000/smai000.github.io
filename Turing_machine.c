/*
 ============================================================================
 Name        : Turing_machine.c
 Author      : Simon Mai
 Version     : 1.6
 Copyright   : 2015 Simon Mai All Rights Reserved
 Description : Turing Machine Simulation in c
			   Input: State Table, Initial Tape Content
			   Output: Modified Tape Content according to state transition
Sample Output:
<Staring contents of tape> aabbbbb
<Starting offset of machine head> 0
<Start state index> 0
<Halting state index> 9
<State index>  <Read>  <Write>  <Direction>  <New state index>
    0            a        -          1               1
    0            b        -          1               2
    0            -        -          1               6
    1            -        a          -1               3
    1            *        *          1               1
    2            -        b          -1               3
    2            *        *          1               2
    3            a        -          -1               4
    3            b        -          -1               5
    3            -        -          1               6
    4            -        a          1               0
    4            *        *          -1               4
    5            -        b          1               0
    5            *        *          -1               5
    6            a        -          -1               7
    6            b        -          -1               8
    6            -        -          0               9
    7            -        a          1               0
    8            -        b          1               0	
<Result> bbbbbaa--
<Last State> 9
 ============================================================================ */

#include <stdio.h>
#include <stdlib.h>

/*======================================Predefine constant==========================================================*/
#define DEFAULT_CONTENT_SIZE			128	//Staring content size(Will check back the actual size after assigned)

#define ACTION_TABLE_SIZE				40	//Default action table size to reallocate memory (40 bytes)

#define LEFT							-1	//Index to move tape LEFT
#define STAY							0	//Index to stay
#define RIGHT							1	//Index to move tape RIGHT

#define IGNORE							'*'	//Index to ignore read and write action

#define INPUT_PATH						"input.txt"
#define OUTPUT_PATH						"output.txt"

/* State table */
enum states {
	STATE_0,
	STATE_1,
	STATE_2,
	STATE_3,
	STATE_4,
	STATE_5,
	STATE_6,
	STATE_7,
	STATE_8,
	STATE_9,
}current_state;
/*===================================================================================================================*/

/*=======================Action table storing structure and functions=========================*/

/*
 * Action table structure
 * Used for storing state event for each state
 * @state_index		: original state
 * @read			: character to read from tape
 * @write			: character to write to tape
 * @direction		: direction where the tape move(Left/Right)
 * @new_state		: new state
 */
struct action_table {
	int state_index, direction, new_state;
	char read, write;
	struct action_table* next;
	struct action_table* prev;
};

/*
 * Check if no action contents in the list
 * return 1, otherwise return 0
 */
int noAction(struct action_table* head)
{
	if(head == NULL) {
		return 1;
	} else {
		return 0;
	}
}

/*
 * Insert first state action
 * @param: double referenced head, last pointer and state table parameters
 * @update: new head and last pointer
 */
void insertFirstAction(struct action_table** action_head, struct action_table** action_last,
		int state_index, char read, char write, int direction, int new_state)
{
	struct action_table* temp = (struct action_table*) malloc(sizeof(struct action_table*));
	temp->state_index = state_index ;
	temp->read = read;
	temp->write = write;
	temp->direction = direction;
	temp->new_state = new_state;
	*action_head = temp;
	*action_last = temp;
}

/*
 * Insert state actions
 * @param: double referenced head, last pointer and state table parameters
 * @update: new head and last pointer
 */
void inserttAction(struct action_table** action_head, struct action_table** action_last,
		int state_index, char read, char write, int direction, int new_state)
{
	if(noAction(*action_head)) {
		insertFirstAction(action_head, action_last, state_index, read, write, direction, new_state);
	} else {
		struct action_table* temp = (struct action_table*) malloc(ACTION_TABLE_SIZE);
		temp->state_index = state_index ;
		temp->read = read;
		temp->write = write;
		temp->direction = direction;
		temp->new_state = new_state;
		temp->next = NULL;
		temp->prev = *action_last;
		(*action_last)->next = temp;
		*action_last = temp;
	}
}

/*
 * Print action table
 * @param: action head pointer
 */
void printAction(struct action_table* action_head)
{
	printf("<State index>  <Read>  <Write>  <Direction>  <New state index>\n");
	while(action_head != NULL)
	{
		printf("    %d            %c        %c          %d               %d\n",
				action_head->state_index, action_head->read, action_head->write, action_head->direction, action_head->new_state);
		action_head = action_head->next;
	}
	printf("\n");
}

/*===================================================================================================================================*/

/*=========================================Tape content storing structure and functions==============================================*/

/*
 * Tape content structure
 * @info : tape content
 */
struct node
{
	char info;
	struct node* next;
	struct node* prev;
};

/*
 * Check whether tape is empty
 * @param: head pointer
 * @return: 1 for empty link list, 0 otherwise
 */
int tapeEmpty(struct node* head)
{
	if(head == NULL) {
		return 1;
	} else {
		return 0;
	}
}

/*
 * Insert first element to the tape
 * @param: double referenced head, last pointer and content
 * @return: new head and last pointer
 */
void insertFirst(struct node** head, struct node** last, char info)
{
	struct node* temp = (struct node*) malloc(sizeof(struct node*));
	temp->info = info;
	temp->prev = NULL;
	temp->next = NULL;
	*head = temp;
	*last = temp;
}

/*
 * Insert element to tape
 * @param: double referenced head, last pointer and tape content
 * @return: new head and last pointer
 */
void insert(struct node** head, struct node** last, char info)
{
	if(tapeEmpty(*head)) {
		insertFirst(head, last, info);
	} else {
		struct node* temp = (struct node*) malloc(sizeof(struct node*));
		temp->info = info;
		temp->next = NULL;
		temp->prev = *last;
		(*last)->next = temp;
		*last = temp;
	}
}

/*
 * Used to replace new head to tape
 * @param: double referenced head, last pointer and info
 * @return: new head location
 */
void insertHead(struct node** head, struct node** last, char info)
{
	struct node* temp = (struct node*) malloc(sizeof(struct node*));
	temp->info = info;
	temp->prev = NULL;
	temp->next = *head;
	(*head)->prev = temp;
	*head = temp;
}

/*
 * Print all contents in tape
 * @param: head pointer
 * @output: list of content
 */
void print(struct node* head)
{
	while(head != NULL)
	{
		printf("%c", head->info);
		head = head->next;
	}
	printf("\n");
}

/*
 * Move right as requested by state event
 * @param: double referenced head, last pointer and info
 * @update: current and last pointer
 */
void moveRight(struct node** head, struct node** last, struct node** curr)
{
	/*Check content reach right end*/
	if(*last == *curr) {
		/*Insert blank in right end position*/
		insert(head, last, '-');
		/*Update current pointer*/
		*curr = *last;
	} else {
		/*Move right if not reach right end*/
		*curr = (*curr)->next;
	}
}

/*
 * Move left as requested by state event
 * @param: double referenced head, last pointer and info
 * @update: current and head pointer
 */
void moveLeft(struct node** head, struct node** last, struct node** curr)
{
	/*Check content reach left end*/
	if (*head == *curr) {
		/*Insert blank in left end position*/
		insertHead(head, last, '-');
		/*Update current pointer*/
		*curr = *head;
	} else {
		/*Move left if not reach left end*/
		*curr = (*curr)->prev;
	}
}

/*
 * Design which direction the tape need to move
 * @param: head pointers from tape and action table
 * 		   current tape pointer
 * 		   current action table pointer
 * @update: head, last, curr and currAction_head pointer
 * @return: failure when discover null pointer, otherwise success
 */
int moveTape(struct node** head, struct node** last, struct node** curr,
		struct action_table** currAction_head)
{
	if(head == NULL || last == NULL || curr == NULL || currAction_head == NULL
			|| *currAction_head == NULL){
		return EXIT_FAILURE;
	}

	if((*currAction_head)->direction == LEFT) {
		moveLeft(head, last, curr);
	}
	else if ((*currAction_head)->direction == RIGHT) {
		moveRight(head, last, curr);
	}

	return EXIT_SUCCESS;
}
/*=========================================================================================================*/

/*
 * Write tape content to output file
 * @param: write file pointer and tape head pointer
 * @record: tape based on head pointer
 */
void writeTofile(FILE *wFile, struct node* head)
{
	if(wFile == NULL) {
		return;
	}

	while(head != NULL)
	{
		fprintf(wFile, "%c", head->info);
		head = head->next;
	}
	fprintf(wFile,"\n");
}

int main(void) {

	/*Initialize read & write file*/
	FILE *rFile;
	FILE *wFile;

	/*Initialize action table variable and pointer*/
	char tape_content[DEFAULT_CONTENT_SIZE];
	int  offset;
	int  ini_state;
	int  haf_state;
	struct action_table* action_head = NULL;
	struct action_table* action_last = NULL;
	struct action_table* currAction_head = NULL;
	int state_index, direction, new_state;
	char read, write;

	/*Initialize tape variable and pointer*/
	struct node* head = NULL;
	struct node* last = NULL;
	struct node* curr = NULL;
	int i, ini_tape_size = 0;

	/*============Retrieve information from input.txt=============*/
	/*Open input.txt*/
	rFile = fopen(INPUT_PATH, "r");
	/*Open output.txt*/
	wFile = fopen(OUTPUT_PATH, "w");
	/*Read and retrieve data from input.txt*/
	if (rFile != NULL) {
		/*Print initial condition*/
		fscanf(rFile, "%s\n"
				"%d\n"
				"%d\n"
				"%d\n",
				tape_content,
				&offset,
				&ini_state,
				&haf_state);
		fprintf(wFile,
				"<Staring contents of tape> %s\n"
				"<Starting offset of machine head> %d\n"
				"<Start state index> %d\n"
				"<Halting state index> %d\n",
				tape_content,
				offset,
				ini_state,
				haf_state);
		/*Print Action Table*/
		fprintf(wFile, "<State index>  <Read>  <Write>  <Direction>  <New state index>\n");
		while(!feof(rFile)) {
			fscanf(rFile,"%d %c %c %d %d\n", &state_index, &read, &write, &direction, &new_state);
			inserttAction(&action_head, &action_last, state_index, read, write, direction, new_state);
			fprintf(wFile, "    %d            %c        %c          %d               %d\n",
				state_index, read, write, direction, new_state);
		}
		fclose(rFile);
	} else {
		printf("Could not open the file.\n");
	}
	/*=================================================================================================*/

	/*=========Assign information to tape double link list structure==========*/
	/*Accumulate initial tape size*/
	for(i=0; i<DEFAULT_CONTENT_SIZE; i++) {
		if(tape_content[i] == '\0') {
			ini_tape_size = i;
			break;
		}
	}

	/*Insert tape content to double link list*/
	for(i=0; i<ini_tape_size; i++) {
		insert(&head, &last, tape_content[i]);
	}
	/*=========================================================================*/

	/*===============================State Machine logic=======================*/

	/*Set initial current pointer*/
	curr = head;
	/*Set initial state*/
	current_state = ini_state;
	/*Set current action head*/
	currAction_head = action_head;

	fprintf(wFile,"<Tape Transition Process>\n");
	/*
	 * State_Machine Core
	 * if found match state and read char -> perform corresponding state event
	 * else if read char equal to '*'     -> ignore read/write move tape according to action table (No state change)
	 * else increment or reset action table pointer to linear search match state and read char
	 */
	while(current_state < haf_state) {
		/*Find match action event*/
		if(currAction_head->state_index == current_state && currAction_head->read == curr->info) {
			/*Write*/
			curr->info = currAction_head->write;
			/*Change State*/
			current_state = currAction_head->new_state;
			/*Move Tape*/
			moveTape(&head, &last, &curr, &currAction_head);
			/*Reset to head*/
			currAction_head = action_head;
			/*Write new tape to file*/
			writeTofile(wFile, head);
		}
		/*Check for '*' event*/
		else if(currAction_head->state_index == current_state && currAction_head->read == IGNORE) {
			/*Move Tape*/
			moveTape(&head, &last, &curr, &currAction_head);
			/*Reset to head*/
			currAction_head = action_head;
			/*Write new tape to file*/
			writeTofile(wFile, head);
		} else {
			if(currAction_head == action_last) {
				/*Cannot find match reset to initial position*/
				currAction_head = action_head;
			} else {
				/*Cannot find match increment or reset current action head*/
				currAction_head = currAction_head->next;
			}
		}
	}
	/*======================================================================================================*/

	/*===========Record Result to output file========*/
	/*Record Final Tape*/
	fprintf(wFile,"\n<Result> ");
	writeTofile(wFile, head);
	/*Record Final State*/
	fprintf(wFile,"<Last State> %d\n", current_state);
	/*Close writing file*/
	fclose(wFile);
	/*===============================================*/

	return EXIT_SUCCESS;
}

