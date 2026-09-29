#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 100

struct Node
{
    int id;
    struct Node *next;
};

struct User
{
    int id;
    char name[30];
    char city[30];
    char interest[30];
    struct Node *friends;
};

struct User users[MAX];
int n = 0;

/* Find User */

int findUser(int id)
{
    for(int i = 0; i < n; i++)
    {
        if(users[i].id == id)
            return i;
    }

    return -1;
}

/* Register User */

void registerUser()
{
    if(n >= MAX)
    {
        printf("User limit reached!\n");
        return;
    }

    printf("Enter User ID: ");
    scanf("%d", &users[n].id);

    if(findUser(users[n].id) != -1)
    {
        printf("User already exists!\n");
        return;
    }

    printf("Enter Name: ");
    scanf(" %[^\n]", users[n].name);

    printf("Enter City: ");
    scanf(" %[^\n]", users[n].city);

    printf("Enter Interest: ");
    scanf(" %[^\n]", users[n].interest);

    users[n].friends = NULL;

    n++;

    printf("User registered successfully!\n");
}

/* Display Users */

void displayUsers()
{
    if(n == 0)
    {
        printf("No users found!\n");
        return;
    }

    for(int i = 0; i < n; i++)
    {
        printf("\nID: %d", users[i].id);
        printf("\nName: %s", users[i].name);
        printf("\nCity: %s", users[i].city);
        printf("\nInterest: %s\n", users[i].interest);
    }
}

/* Add Friend */

void addFriend()
{
    int a, b;

    printf("Enter two User IDs: ");
    scanf("%d%d", &a, &b);

    int x = findUser(a);
    int y = findUser(b);

    if(x == -1 || y == -1)
    {
        printf("User not found!\n");
        return;
    }

    if(a == b)
    {
        printf("Cannot add yourself!\n");
        return;
    }

    struct Node *temp = users[x].friends;

    while(temp != NULL)
    {
        if(temp->id == b)
        {
            printf("Already friends!\n");
            return;
        }

        temp = temp->next;
    }

    struct Node *new1 = malloc(sizeof(struct Node));

    new1->id = b;
    new1->next = users[x].friends;
    users[x].friends = new1;

    struct Node *new2 = malloc(sizeof(struct Node));

    new2->id = a;
    new2->next = users[y].friends;
    users[y].friends = new2;

    printf("Friendship added!\n");
}

/* Display Friends */

void displayFriends()
{
    int id;

    printf("Enter User ID: ");
    scanf("%d", &id);

    int index = findUser(id);

    if(index == -1)
    {
        printf("User not found!\n");
        return;
    }

    struct Node *temp = users[index].friends;

    printf("Friends of %s:\n", users[index].name);

    if(temp == NULL)
    {
        printf("No friends!\n");
        return;
    }

    while(temp != NULL)
    {
        int i = findUser(temp->id);

        if(i != -1)
            printf("%s\n", users[i].name);

        temp = temp->next;
    }
}

/* Mutual Friends */

void mutualFriends()
{
    int a, b, count = 0;

    printf("Enter two User IDs: ");
    scanf("%d%d", &a, &b);

    int x = findUser(a);
    int y = findUser(b);

    if(x == -1 || y == -1)
    {
        printf("User not found!\n");
        return;
    }

    struct Node *temp = users[x].friends;

    printf("Mutual Friends:\n");

    while(temp != NULL)
    {
        struct Node *p = users[y].friends;

        while(p != NULL)
        {
            if(temp->id == p->id)
            {
                int i = findUser(temp->id);

                if(i != -1)
                    printf("%s\n", users[i].name);

                count++;
                break;
            }

            p = p->next;
        }

        temp = temp->next;
    }

    printf("Total Mutual Friends: %d\n", count);
}

/* Recommend Friends */

void recommendFriends()
{
    int id;

    printf("Enter Your User ID: ");
    scanf("%d", &id);

    int index = findUser(id);

    if(index == -1)
    {
        printf("User not found!\n");
        return;
    }

    printf("\nFriend Recommendations:\n");

    int count = 0;

    for(int i = 0; i < n; i++)
    {
        if(i == index)
            continue;

        struct Node *temp = users[index].friends;
        int alreadyFriend = 0;

        while(temp != NULL)
        {
            if(temp->id == users[i].id)
            {
                alreadyFriend = 1;
                break;
            }

            temp = temp->next;
        }

        if(alreadyFriend)
            continue;

        if(strcmp(users[index].city, users[i].city) == 0 ||
           strcmp(users[index].interest, users[i].interest) == 0)
        {
            printf("Name: %s\n", users[i].name);
            printf("City: %s\n", users[i].city);
            printf("Interest: %s\n\n", users[i].interest);

            count++;
        }
    }

    if(count == 0)
        printf("No recommendations found!\n");
}

/* Main Function */

int main()
{
    int choice;

    do
    {
        printf("\n===== SMART SOCIAL NETWORK =====\n");

        printf("1. Register User\n");
        printf("2. Display Users\n");
        printf("3. Add Friendship\n");
        printf("4. Display Friends\n");
        printf("5. Find Mutual Friends\n");
        printf("6. Recommend Friends\n");
        printf("7. Exit\n");

        printf("Enter Choice: ");
        scanf("%d", &choice);

        switch(choice)
        {
            case 1:
                registerUser();
                break;

            case 2:
                displayUsers();
                break;

            case 3:
                addFriend();
                break;

            case 4:
                displayFriends();
                break;

            case 5:
                mutualFriends();
                break;

            case 6:
                recommendFriends();
                break;

            case 7:
                printf("Thank You!\n");
                break;

            default:
                printf("Invalid Choice!\n");
        }

    } while(choice != 7);

    return 0;
}
