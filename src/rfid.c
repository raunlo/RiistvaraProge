#include "rfid.h"
#include <stdlib.h>
#include <string.h>
#include <avr/pgmspace.h>
#include "../lib/andygock_avr-uart/uart.h"
#include "hmi_msg.h"

/* method to acssess linked list items*/
card_rfid  *head;

  card_rfid  find(  char* a)
{
	card_rfid *curr = head;
	while(curr !=NULL) 
	 {
	 	if(strcmp(a, curr->uidArray)==0) {
     			 return curr;
     			 }
     			      		 
      	curr =curr->next;	
     	}
     	free(curr);
      	return NULL;
      	
}
		

/* method to list all card, which are listed*/
void rfid_list_all_cards(void)
{
	uint8_t queue_number = 1;
	if (head == NULL) 
	{
		uart0_puts_p(PSTR(LIST_EMPTY));
	} 
	else 
	{
		card_rfid *curr = head;		
		while (curr != NULL) 
		{//print out cards
			char *a;
			uart0_puts(itoa(queue_number,a, 10));
			uart0_puts_p(PSTR(PRINT_UID_START));
			uart0_puts(curr->size);
			uart0_puts_p(PSTR(PRINT_UID_END));
			uart0_puts(curr->uidArray);
			uart0_puts_p(PSTR(PRINT_HOLDER));
			uart0_puts(curr->user);
			uart0_puts_p(PSTR(LINE_BREAK));
			queue_number++;
			curr = curr->next;
		}
	}
}

/* Method to add new card to list*/
void  rfid_add_card( card_rfid *card)
{	
	 //checking if exsists
	 card_rfid *current = head;
	 while(current !=NULL) 
	 {
     	if (memcmp(current->uidArray, card->uidArray, *card->size) == 0)
     	{
     		uart0_puts_p(PSTR(CARD_ADDING_FAILED));
      		return;
     	}
      	current =current->next;
     } 	
	//copying data	
	card_rfid *new_card = (malloc(sizeof(card_rfid)));
	char *user = malloc(strlen(card->user));
	strcpy(user, card->user);
    new_card->user = user;
	char *size = malloc(strlen(card->size));
	strcpy(size, card->size );
    new_card->size = size;
	char *uidArray = malloc(strlen(card->uidArray));
	strcpy(uidArray, card->uidArray);
    new_card->uidArray = uidArray;
    new_card->next = NULL;    
    //updating linked list
    if(head == NULL) 
    {    
    	head = new_card;
        uart0_puts_p(PSTR(ADDED_SUCCESSFULLY));
        return;
    }
	card_rfid *temp = head;
	if (temp == NULL) 
	{
    	uart0_puts_p(PSTR(MEMORY_FAILED));
        return;
    }
    while(temp->next != NULL)
    {
    	temp = temp->next;
    }
    temp->next = new_card;
    uart0_puts_p(PSTR(ADDED_SUCCESSFULLY));
}

/* Method to remove card from list */
void rfid_remove_card(const char uid[10], int len) {
	card_rfid *prev = NULL;
	card_rfid *current = head;
	if(head == NULL)
	{
		uart0_puts_p(PSTR(LIST_EMPTY));
	}
		
	 while(current !=NULL) 
	 {
     	if(memcmp(current->uidArray, uid, len/2)== 0) 
      	{   //only 1 card is in list      		
      		if(prev == NULL && current->next == NULL) 
      		{     			
      		free(current);
      		head = NULL;
      		} 
      		else if(prev == NULL && current->next != NULL) 
      		{ // there is more than 2 cards in the list and deleting first card      															      			
      			free(current->uidArray);
      			free(current);
      			head = current-> next;
      		} 
      		else if(prev != NULL && current->next == NULL) 
      		{   // Deleted card is last in the list
      			prev->next = NULL;
      			free(current->uidArray);
      			free(current);
      		} 
      		else 
      		{   // the deleted card is middle of the list
      			prev->next = current->next;
      			free(current->uidArray);
      			free(current);
      		}
      		uart0_puts_p(PSTR(RM_successfully));	
      	} 
      	prev = current;
      	current =current->next;      	
      }             
}
