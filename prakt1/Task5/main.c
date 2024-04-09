#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#define N 50

int main()
{
    List *head = NULL;
    int action;
    char surname[N];
    char name[N];
    head = ReadFromFile(head);

    while (true)
    {
        printf("\nДобро пожаловать в список контактов! Выберите желаемое действие:\n1-добавить контакт\n2-редактировать контакт\n3-удалить контакт\n4-очистить список\n5-просмотр контакта\n6-просмотр списка контактов\n");
        printf("Для закрытия программы нажмите любую другую кнопку\n");
        scanf("%d", &action);

        switch(action)
        {
            case 1:
                Contact contact;
                add_contact(&contact);
                head = InsertContact(head, &contact);
                break;

            case 2:
                printf("Введите фамилию контакта, который хотите отредактировать: ");
                scanf("%s", surname);

                printf("Введите имя контакта, который хотите отредактировать: ");
                scanf("%s", name);

                head = ChangeContact(surname, name, head);
                break;
            
            case 3:
                printf("Введите фамилию контакта, который хотите удалить: ");
                scanf("%s", surname);

                printf("Введите имя контакта, который хотите удалить: ");
                scanf("%s", name);

                head = DeleteContact(surname, name, head);

                break;

            case 4:
                head = DeleteList(head);
                break;
            
            case 5:
                printf("Введите фамилию контакта, который хотите просмотреть: ");
                scanf("%s", surname);

                printf("Введите имя контакта, который хотите просмотреть: ");
                scanf("%s", name);

                PrintContact(surname, name, head);
                break;

            case 6:
                PrintList(head);
                break;
                
            default:
                WriteToFile(head);
                return 0;
        }
    }
}