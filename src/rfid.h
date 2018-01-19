#ifndef RFID_ACCESS_H
#define RFID_ACCESS_H

typedef struct Card {
	char *size;
	 char *uidArray;
	 char *user;
	struct Card *next;
} card_rfid;

void rfid_list_all_cards(void); 
void rfid_add_card(  card_rfid *card);
void rfid_read_card(  char *uid);
void rfid_remove_card( const char *uid, int len);
 card_rfid find( char *a);
#endif /* RFID_ACCESS_H */
