#include <fcntl.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "contact.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define N 50
#define M 20
#define U 100

void add_contact(Contact *contact)
{
    int con;
    int operation;
    int count;
    char buffer[N];
    char buffer2[N];

    while (true)
    {
        printf("Введите фамилию: ");
        scanf("%s", buffer);

        if (strlen(buffer)==0)
        {
            printf("Поле не должно быть пустым!");
            continue;
        }

        else
        {
            strcpy(contact->fio.surname, buffer);
        }

        printf("Введите имя: ");
        scanf("%s", buffer2);

        if (strlen(buffer2)==0)
            printf("Поле не должно быть пустым!");

        else
        {
            strcpy(contact->fio.name, buffer2);
            break;
        }
    }
    
    strcpy(contact->fio.partonymic, "");
    strcpy(contact->job.corp, "");
    strcpy(contact->job.title, "");
    contact->phone[0].count = 0;
    contact->email[0].count = 0;
    contact->socials[0].count = 0;
    contact->messengers[0].count = 0;

    printf("Желаете ли продолжить заполнение? 1-да/другая кнопка-нет\n");
    scanf("%d", &con);
    if (con == 1)
        while (true)
        {       

            printf("Что заполнить:\n1-Отчество\n2-Место работы и должность\n3-Номер телефона\n4-E-mail\n5-Соцсети\n6-Мессенджеры\nЛюбая другая кнопка - Выход\n");
            scanf("%d", &operation);
            switch(operation)
            {
                case 1:
                    printf("Введите отчество: ");
                    scanf("%s", contact->fio.partonymic);
                    break;
                
                case 2:
                    printf("Введите  место работы: ");
                    scanf("%s", contact->job.corp);

                    printf("Введите  должность: ");
                    scanf("%s", contact->job.title);
                    break;
                
                case 3:
                    printf ("Введите количество номеров телефона: ");
                    scanf("%d", &count);
                    for(int i=0;i<count;i++)
                    {
                        contact->phone[i].count = count;
                        contact->phone[i].ID = i;

                        printf("Укажите тип телефона: ");      
                        scanf("%s", contact->phone[i].phone_type);

                        printf("Укажите номер телефона: ");      
                        scanf("%s", contact->phone[i].number);
                    }
                    break;

                case 4:
                    printf ("Введите количество e-mail адресов: ");
                    scanf("%d", &count);
                    for(int i=0;i<count;i++)
                    {
                        contact->email[i].count = count;
                        contact->email[i].ID = i;

                        printf("Введите электронный адрес: ");      
                        scanf("%s", contact->email[i].email);
                    }
                    break;

                case 5:
                    printf ("Введите количество профилей в соцсетях: ");
                    scanf("%d", &count);
                    for(int i=0;i<count;i++)
                    {
                        contact->socials[i].count = count;
                        contact->socials[i].ID = i;

                        printf("Укажите название соцсети: ");      
                        scanf("%s", contact->socials[i].name);

                        printf("Укажите ссылку на ваш профиль: ");      
                        scanf("%s", contact->socials[i].url);
                    }
                    break;

                case 6:
                    printf ("Введите количество профилей в мессенджерах: ");
                    scanf("%d", &count);
                    for(int i=0;i<count;i++)
                    {
                        contact->messengers[i].count = count;
                        contact->messengers[i].ID = i;

                        printf("Укажите название мессенджера: ");      
                        scanf("%s", contact->messengers[i].name);

                        printf("Укажите имя пользователя: ");      
                        scanf("%s", contact->messengers[i].nickname);
                    }
                    break;
                
                default:
                    return;
            }
        }
}

void change_contact(Contact *contact)
{
    int operation;
    int sub_operation;
    int ID;
    char buffer[N];
    int count_phones, count_email, count_socials, count_messengers;
    while (true)
    {       

        printf("Что заполнить:\n1-Фамилия\n2-Имя\n3-Отчество\n4-Место работы и должность\n5-Номер телефона\n6-E-mail\n7-Соцсети\n8-Мессенджеры\n");
        printf("Любая другая кнопка - Выход\n");
        scanf("%d", &operation);
        
        switch(operation)
        {
            case 1:
                while (true)
                {
                    printf("Введите фамилию: ");
                    scanf("%s", buffer);
                    if (strlen(buffer)==0)
                        printf("Поле не должно быть пустым!");
                    else
                    {
                        strcpy(contact->fio.surname, buffer);
                        break;
                    }
                }
                break;

            case 2:
                while (true)
                {
                    printf("Введите имя: ");
                    scanf("%s", buffer);
                    if (strlen(buffer)==0)
                        printf("Поле не должно быть пустым!");
                    else
                    {
                        strcpy(contact->fio.name, buffer);
                        break;
                    }
                }
                break;

            case 3:
                printf("Введите отчество: ");
                scanf("%s", contact->fio.partonymic);
                break;
                
            case 4:
                printf("Введите  место работы: ");
                scanf("%s", contact->job.corp);

                printf("Введите  должность: ");
                scanf("%s", contact->job.title);
                break;

            case 5:
                printf("1-изменить номер телефона 2-добавить номер телефона\n");
                scanf("%d", &sub_operation);

                count_phones = contact->phone[0].count;

                if (sub_operation==1)
                {
                    printf("Введите ID телефона для изменения: ");
                    scanf("%d", &ID);

                    if(ID >= count_phones)
                        printf("Данного номера не существует!");

                    else
                    {
                        printf("Укажите тип телефона: ");      
                        scanf("%s", contact->phone[ID].phone_type);

                        printf("Укажите номер телефона: ");      
                        scanf("%s", contact->phone[ID].number);
                    }
                }

                else if (sub_operation==2)
                {
                    if(count_phones == 5)
                        printf("Достигнуто максимальное количество номеров телефона!");
                    else
                    {    
                        printf("Укажите тип телефона: ");      
                        scanf("%s", contact->phone[count_phones].phone_type);

                        printf("Укажите номер телефона: ");      
                        scanf("%s", contact->phone[count_phones].number);

                        for(int i = 0; i <= count_phones; i++)
                            contact->phone[i].count++;
                        
                    }
                }
                break;

            case 6:
                printf("1-изменить адрес 2-добавить адрес\n");
                scanf("%d", &sub_operation);

                count_email = contact->email[0].count;

                if (sub_operation==1)
                {
                    printf("Введите ID электронного адреса для удаления: ");
                    scanf("%d", &ID);
                    
                    if(ID >= count_email)
                        printf("Данного адреса не существует!");

                    else
                    {
                        printf("Укажите номер телефона: ");      
                        scanf("%s", contact->email[ID].email);
                    }
                }

                else if (sub_operation==2)
                {
                    if(count_email == 5)
                        printf("Достигнуто максимальное количество адресов!");
                    else
                    {
                            printf("Укажите адрес: ");      
                            scanf("%s", contact->email[count_email].email);

                            for(int i = 0; i <= count_email; i++)
                                contact->email[i].count++;
                    }
                }
                break;

            case 7:
                printf("1-изменить соцсеть 2-добавить соцсеть\n");
                scanf("%d", &sub_operation);

                count_socials = contact->socials[0].count;

                if (sub_operation==1)
                {
                    printf("Введите ID соцсети для удаления: ");
                    scanf("%d", &ID);
                    
                    if(ID >= count_socials)
                        printf("Данной соцсети не существует!");

                    else
                    {
                        printf("Укажите соцсеть: ");      
                        scanf("%s", contact->socials[ID].name);

                        printf("Укажите ссылку на страницу: ");      
                        scanf("%s", contact->socials[ID].url);
                    }
                    
                }

                else if (sub_operation==2)
                {
                    if(count_socials == 5)
                        printf("Достигнуто максимальное количество соцсетей!");
                    else
                    {    
                            printf("Укажите название соцсети: ");      
                            scanf("%s", contact->socials[count_socials].name);

                            printf("Укажите ссылку на страницу: ");      
                            scanf("%s", contact->socials[count_socials].url);

                            for(int i = 0; i <= count_socials; i++)
                                contact->socials[i].count++;
                    }
                }
                break;

            case 8:
                printf("1-изменить мессенджер 2-добавить мессенджер\n");
                scanf("%d", &sub_operation);

                count_messengers = contact->messengers[0].count;

                if (sub_operation==1)
                {
                    printf("Введите ID мессенджера для удаления: ");
                    scanf("%d", &ID);
                    
                    if(ID >= count_messengers)
                        printf("Данной соцсети не существует!");

                    else
                    {
                        printf("Укажите мессенджер: ");      
                        scanf("%s", contact->messengers[ID].name);

                        printf("Укажите ник: ");      
                        scanf("%s", contact->messengers[ID].nickname);
                    }
                    
                }

                else if (sub_operation==2)
                {
                    if(count_messengers == 5)
                        printf("Достигнуто максимальное количество номеров телефона!");
                    else    
                    {
                            printf("Укажите мессенджер: ");      
                            scanf("%s", contact->messengers[count_messengers].name);

                            printf("Укажите ник: ");      
                            scanf("%s", contact->messengers[count_messengers].nickname);

                            for(int i = 0; i <= count_messengers; i++)
                                contact->messengers[i].count++;
                    }
                }
                break;
                
            default:
                return;     
        }
    }
}

void print_contact(Contact *contact)
{

    int phone_count = contact->phone[0].count;
    int email_count = contact->email[0].count;
    int social_count = contact->socials[0].count;
    int messenger_count = contact->messengers[0].count;

    printf("Фамилия: %s\n", contact->fio.surname);
    printf("Имя: %s\n", contact->fio.name);
    printf("Отчество: %s\n", contact->fio.partonymic);
    printf("Место работы: %s\n", contact->job.corp);
    printf("Должность: %s\n", contact->job.title);

    for (int j = 0; j < phone_count; j++)
    {
        printf("Phone ID: %d\n", contact->phone[j].ID);
        printf("Тип телефона: %s\n", contact->phone[j].phone_type);
        printf("Номер телефона: %s\n", contact->phone[j].number);
    }

    for (int j = 0; j < email_count; j++)
    {
        printf("Email ID: %d\n", contact->email[j].ID);
        printf("E-mail: %s\n", contact->email[j].email);
    }

    for (int j = 0; j < social_count; j++)
    {
        printf("Social ID: %d\n", contact->socials[j].ID);
        printf("Название: %s\n", contact->socials[j].name);
        printf("Ссылка: %s\n", contact->socials[j].url);
    }

    for (int j = 0; j < messenger_count; j++)
    {
        printf("Messenger ID: %d\n", contact->messengers[j].ID);
        printf("Мессенджер: %s\n", contact->messengers[j].name);
        printf("Имя пользователя: %s\n", contact->messengers[j].nickname);
    }  
}

int cmp_contacts(Contact *contact1, Contact *contact2)
{
    if (strcmp(contact1->fio.surname, contact2->fio.surname) > 0)
        return 1;
    else if (strcmp(contact1->fio.surname, contact2->fio.surname) < 0)
        return -1;
    
    else
        return strcmp(contact1->fio.name, contact2->fio.name);     
}

void copy_contact(Contact *contact1, Contact *contact2)
{
    strcpy(contact2->fio.surname, contact1->fio.surname);
    strcpy(contact2->fio.name, contact1->fio.name);
    strcpy(contact2->fio.partonymic, contact1->fio.partonymic);
    strcpy(contact2->job.corp, contact1->job.corp);
    strcpy(contact2->job.title, contact1->job.title);

    for (int j = 0; j < contact1->phone[0].count; j++)
    {
        contact2->phone[j].count = contact1->phone[j].count;
        contact2->phone[j].ID = contact1->phone[j].ID;

        strcpy(contact2->phone[j].phone_type, contact1->phone[j].phone_type);
        strcpy(contact2->phone[j].number, contact1->phone[j].number);
    }

    for (int j = 0; j < contact1->email[0].count; j++)
    {
        contact2->email[j].count = contact1->email[j].count;
        contact2->email[j].ID = contact1->email[j].ID;

        strcpy(contact2->email[j].email, contact1->email[j].email);
    }

    for (int j = 0; j < contact1->socials[0].count; j++)
    {
        contact2->socials[j].count = contact1->socials[j].count;
        contact2->socials[j].ID = contact1->socials[j].ID;

        strcpy(contact2->socials[j].name, contact1->socials[j].name);
        strcpy(contact2->socials[j].url, contact1->socials[j].url);

    }

    for (int j = 0; j < contact1->messengers[0].count; j++)
    {
        contact2->messengers[j].count = contact1->messengers[j].count;
        contact2->messengers[j].ID = contact1->messengers[j].ID;

        strcpy(contact2->messengers[j].name, contact1->messengers[j].name);
        strcpy(contact2->messengers[j].nickname, contact1->messengers[j].nickname);
    }  
}