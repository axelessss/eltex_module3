#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "list.h"

List* InsertContact(List* head, Contact* contact) 
{
    List* newItem = (List*)malloc(sizeof(struct List));
    List* tmp = head;

    copy_contact(contact, &(newItem->contact));

    if(NULL == head) 
    {
        newItem->prev = newItem;
        newItem->next = newItem;
        head = newItem;
        return head;
    }

    do 
    {   
        if(cmp_contacts(&(newItem->contact), &(tmp->contact))<=0) 
        {
            newItem->next = tmp;
            newItem->prev = tmp->prev;
            tmp->prev->next = newItem;
            tmp->prev = newItem;

            if(tmp == head)
                head = newItem;

            return head;
            
        }
        tmp = tmp->next;

    } while(tmp != head);


    newItem->next = tmp;
    newItem->prev = tmp->prev;
    tmp->prev->next = newItem;
    tmp->prev = newItem;
    return head;
}

List* ChangeContact(char* surname, char* name, List* head)
{
    List* tmp = head;

    if(NULL == head) 
    {
        printf("\nList empty!\n");
        return head;
    }


    do 
    {
        if(strcmp(tmp->contact.fio.surname, surname)==0 && strcmp(tmp->contact.fio.name, name)==0) 
        {
            change_contact(&(tmp->contact));
            return head;
        }

        tmp = tmp->next;

    } while(tmp != head);

    printf("\nДанный контакт не существует\n");
    return head;
}

List* DeleteContact(char *surname, char *name, List* head) 
{
    List *tmp = head;

    if (NULL == head) 
    {
        printf("\nСписок пустой!\n");
        return head;
    }

    do 
    {
        if (strcmp(tmp->contact.fio.surname, surname)==0 && strcmp(tmp->contact.fio.name, name)==0) 
        {

            if (tmp->next == tmp->prev)
            {
                free(tmp);
                return NULL;
            }

            tmp->next->prev = tmp->prev;
            tmp->prev->next = tmp->next;

            if(tmp == head) 
                head = head->next;

            free(tmp);
            return head;
        }  
        tmp = tmp->next;
    } while (tmp != head);

    printf("\nДанный контакт не существует\n");
    return head;
}

List* DeleteList(List* head) 
{
    List* tmp = head;
    List* itemForRemove;
    if (NULL == head) 
    {
        return NULL;
    }   

    do 
    {
        itemForRemove = tmp;
        tmp = tmp->next;
        free(itemForRemove);
    } while (tmp != head);

    return NULL;
}

void PrintList(List* head) 
{
    List* tmp = head;
    if (NULL == head) 
    {
        printf("\nПустой список!\n");
        return;
    }

    do 
    {
        printf("\n\n");
        print_contact(&(tmp->contact));
        tmp = tmp->next;
    } while (tmp != head);

    
}

void PrintContact(char* surname, char* name, List* head)
{
    List *tmp = head;

    if (NULL == head) 
    {
        printf("\nList empty!\n");
        return;
    }

    do 
    {
        if (strcmp(tmp->contact.fio.surname, surname)==0 && strcmp(tmp->contact.fio.name, name)==0) 
        {
            print_contact(&tmp->contact);
            return;
        }  
        tmp = tmp->next;
    } while (tmp != head);


    printf("\nДанный контакт не существует\n");
}

void WriteToFile(List *head)
{
    List *tmp = head;

    if (NULL == head) 
    {
        printf("\nПустой список!\n");
        return;
    }

    int file = open("contacts.txt", O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR);
    if (file==-1)    
    {
        printf("Ошибка открытия файла! \n");
        return;
    }

    do 
    {
        write(file, &tmp->contact, sizeof(Contact));
   
        tmp = tmp->next;
    } while (tmp != head);
    close(file);
}

List* ReadFromFile(List *head)
{
    Contact contact;
    int file_pointer;

    int file = open("contacts.txt", O_RDONLY);
    if (file==-1)   
    {
        printf("Ошибка открытия файла, список контактов пуст \n");
        return NULL;
    }

    file_pointer = read(file, &contact, sizeof(Contact));
    if(file_pointer <= 0)
    {
        printf("Пустой файл, список контактов пуст \n");
        close(file);
        return NULL;
    }
    head = InsertContact(head, &contact);
    
    while ((file_pointer = read(file, &contact, sizeof(Contact))) > 0) 
    {
        head = InsertContact(head, &contact);
    }
    close(file);
    return head;
}