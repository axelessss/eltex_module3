#include "contact.h"

typedef struct List
{
    struct Contact contact;
    struct List* next;
    struct List* prev;
}List;

List* InsertContact(List* head, Contact* contact);
List* ChangeContact(char* surname, char* name, List* head);
List* DeleteContact(char *surname, char *name, List* head);
List* DeleteList(List* head);
void PrintContact(char *surname, char *name, List* head);
void PrintList(List* head);
void WriteToFile(List *head);
List* ReadFromFile(List *head);