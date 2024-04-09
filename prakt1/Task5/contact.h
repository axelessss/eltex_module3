#include "fio.h"
#include "email.h"
#include "job.h"
#include "messenger.h"
#include "phone.h"
#include "social.h"

typedef struct Contact
{
    int id;
    FIO fio;
    Job job;
    Phone phone[5];
    Email email[5];
    Social socials[5];
    Messenger messengers[5];
}Contact;

void add_contact(Contact *contact);
void change_contact(Contact *contact);
void print_contact(Contact *contacts);
int cmp_contacts(Contact *contact1, Contact *contact2);
void copy_contact(Contact *contact1, Contact *contact2);