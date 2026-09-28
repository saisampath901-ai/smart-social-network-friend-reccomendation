#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_USERS 100
#define MAX_NAME 50
#define MAX_CITY 50
#define MAX_INTERESTS 10
#define MAX_INTEREST_LEN 30
#define DATA_FILE "social_network.dat"



typedef struct FriendNode {
    int userId;
    struct FriendNode *next;
} FriendNode;

typedef struct {
    int id;
    char name[MAX_NAME];
    int age;
    char city[MAX_CITY];
    char interests[MAX_INTERESTS][MAX_INTEREST_LEN];
    int interestCount;
    FriendNode *friends;
} User;

typedef struct {
    User users[MAX_USERS];
    int userCount;
} Graph;

typedef struct {
    int userId;
    int mutualFriends;
    int commonInterests;
    int sameCity;
    int socialDistance;
    int score;
} Recommendation;

void clearInputBuffer(void){ int c; while((c=getchar())!='\n' && c!=EOF); }
void readLine(char *b,int s){ if(fgets(b,s,stdin)) b[strcspn(b,"\n")]='\0'; }
int getInt(const char *m){ int v; for(;;){ printf("%s",m); if(scanf("%d",&v)==1){clearInputBuffer();return v;} printf("Invalid input. Please enter a number.\n"); clearInputBuffer(); } }
int findUserIndex(Graph *g,int id){ for(int i=0;i<g->userCount;i++) if(g->users[i].id==id) return i; return -1; }
FriendNode *createFriendNode(int id){ FriendNode *n=malloc(sizeof(FriendNode)); if(!n){printf("Memory allocation failed.\n");exit(EXIT_FAILURE);} n->userId=id;n->next=NULL;return n; }
int areFriends(Graph *g,int a,int b){ int i=findUserIndex(g,a); if(i<0)return 0; for(FriendNode*n=g->users[i].friends;n;n=n->next) if(n->userId==b)return 1; return 0; }

void saveData(Graph *g);
void freeGraph(Graph *g);

void registerUser(Graph *g){
    if(g->userCount>=MAX_USERS){printf("\nMaximum user limit reached.\n");return;}
    int id=getInt("\nEnter User ID: ");
    if(findUserIndex(g,id)!=-1){printf("User ID already exists.\n");return;}
    User *u=&g->users[g->userCount]; u->id=id;
    printf("Enter Name: ");readLine(u->name,MAX_NAME);
    u->age=getInt("Enter Age: ");
    if(u->age<=0||u->age>120){printf("Invalid age.\n");return;}
    printf("Enter City: ");readLine(u->city,MAX_CITY);
    u->interestCount=getInt("Enter number of interests (0-10): ");
    if(u->interestCount<0||u->interestCount>MAX_INTERESTS){printf("Invalid number of interests.\n");u->interestCount=0;}
    for(int i=0;i<u->interestCount;i++){printf("Enter Interest %d: ",i+1);readLine(u->interests[i],MAX_INTEREST_LEN);}
    u->friends=NULL;g->userCount++;printf("\nUser registered successfully!\n");saveData(g);
}

void displayUser(Graph*g,int i){ User*u=&g->users[i]; printf("\n---------------------------------------------\nUser ID : %d\nName    : %s\nAge     : %d\nCity    : %s\nInterests: ",u->id,u->name,u->age,u->city); if(!u->interestCount)printf("None"); for(int j=0;j<u->interestCount;j++)printf("%s%s",u->interests[j],j<u->interestCount-1?", ":""); printf("\n---------------------------------------------\n"); }
void displayAllUsers(Graph*g){ if(!g->userCount){printf("\nNo users registered.\n");return;} printf("\n========== ALL USERS ==========\n");for(int i=0;i<g->userCount;i++)displayUser(g,i); }
void searchUser(Graph*g){int id=getInt("\nEnter User ID to search: ");int i=findUserIndex(g,id);if(i<0)printf("\nUser not found.\n");else displayUser(g,i);}

void addFriendship(Graph*g,int a,int b){
    int ia=findUserIndex(g,a),ib=findUserIndex(g,b);if(ia<0||ib<0){printf("\nOne or both users do not exist.\n");return;}if(a==b){printf("\nA user cannot be friends with themselves.\n");return;}if(areFriends(g,a,b)){printf("\nUsers are already friends.\n");return;}
    FriendNode*n=createFriendNode(b);n->next=g->users[ia].friends;g->users[ia].friends=n;n=createFriendNode(a);n->next=g->users[ib].friends;g->users[ib].friends=n;printf("\nFriendship added successfully!\n");saveData(g);
}
void removeFriendship(Graph*g,int a,int b){
    int ia=findUserIndex(g,a),ib=findUserIndex(g,b);if(ia<0||ib<0){printf("\nOne or both users do not exist.\n");return;}if(!areFriends(g,a,b)){printf("\nThese users are not friends.\n");return;}
    FriendNode **p=&g->users[ia].friends;while(*p){if((*p)->userId==b){FriendNode*t=*p;*p=(*p)->next;free(t);break;}p=&(*p)->next;}
    p=&g->users[ib].friends;while(*p){if((*p)->userId==a){FriendNode*t=*p;*p=(*p)->next;free(t);break;}p=&(*p)->next;}
    printf("\nFriendship removed successfully!\n");saveData(g);
}
void displayFriends(Graph*g,int id){int i=findUserIndex(g,id);if(i<0){printf("\nUser not found.\n");return;}printf("\nFriends of %s:\n",g->users[i].name);FriendNode*n=g->users[i].friends;if(!n){printf("No friends found.\n");return;}while(n){int j=findUserIndex(g,n->userId);if(j>=0)printf("ID: %d | Name: %s\n",g->users[j].id,g->users[j].name);n=n->next;}}

int bfsSocialDistance(Graph*g,int startId,int targetId){
    int s=findUserIndex(g,startId),t=findUserIndex(g,targetId);if(s<0||t<0)return -1;if(s==t)return 0;int vis[MAX_USERS]={0},dist[MAX_USERS];for(int i=0;i<MAX_USERS;i++)dist[i]=-1;int q[MAX_USERS],f=0,r=0;q[r++]=s;vis[s]=1;dist[s]=0;
    while(f<r){int cur=q[f++];for(FriendNode*n=g->users[cur].friends;n;n=n->next){int j=findUserIndex(g,n->userId);if(j>=0&&!vis[j]){vis[j]=1;dist[j]=dist[cur]+1;if(j==t)return dist[j];q[r++]=j;}}}return -1;
}
void bfsTraversal(Graph*g,int id){int s=findUserIndex(g,id);if(s<0){printf("\nUser not found.\n");return;}int vis[MAX_USERS]={0},q[MAX_USERS],f=0,r=0;q[r++]=s;vis[s]=1;printf("\nBFS Traversal:\n");while(f<r){int c=q[f++];printf("%s (%d) -> ",g->users[c].name,g->users[c].id);for(FriendNode*n=g->users[c].friends;n;n=n->next){int j=findUserIndex(g,n->userId);if(j>=0&&!vis[j]){vis[j]=1;q[r++]=j;}}}printf("END\n");}
void dfsRecursive(Graph*g,int i,int vis[]){vis[i]=1;printf("%s (%d) -> ",g->users[i].name,g->users[i].id);for(FriendNode*n=g->users[i].friends;n;n=n->next){int j=findUserIndex(g,n->userId);if(j>=0&&!vis[j])dfsRecursive(g,j,vis);}}
void dfsTraversal(Graph*g,int id){int s=findUserIndex(g,id);if(s<0){printf("\nUser not found.\n");return;}int vis[MAX_USERS]={0};printf("\nDFS Traversal:\n");dfsRecursive(g,s,vis);printf("END\n");}

int countMutualFriends(Graph*g,int a,int b){int ia=findUserIndex(g,a),ib=findUserIndex(g,b);if(ia<0||ib<0)return 0;int c=0;for(FriendNode*n=g->users[ia].friends;n;n=n->next)if(areFriends(g,n->userId,b))c++;return c;}
void displayMutualFriends(Graph*g,int a,int b){int ia=findUserIndex(g,a),ib=findUserIndex(g,b);if(ia<0||ib<0){printf("\nOne or both users do not exist.\n");return;}int c=0;printf("\nMutual Friends:\n");for(FriendNode*n=g->users[ia].friends;n;n=n->next)if(areFriends(g,n->userId,b)){int j=findUserIndex(g,n->userId);if(j>=0){printf("ID: %d | Name: %s\n",g->users[j].id,g->users[j].name);c++;}}if(!c)printf("No mutual friends found.\n");else printf("Total Mutual Friends: %d\n",c);}

int countCommonInterests(User*a,User*b){int c=0;for(int i=0;i<a->interestCount;i++)for(int j=0;j<b->interestCount;j++)if(strcmp(a->interests[i],b->interests[j])==0){c++;break;}return c;}
int calculateRecommendationScore(Graph*g,int sourceId,int candidateId){
    int s=findUserIndex(g,sourceId),c=findUserIndex(g,candidateId);if(s<0||c<0||sourceId==candidateId||areFriends(g,sourceId,candidateId))return -1;int mutual=countMutualFriends(g,sourceId,candidateId),common=countCommonInterests(&g->users[s],&g->users[c]),same=strcmp(g->users[s].city,g->users[c].city)==0,d=bfsSocialDistance(g,sourceId,candidateId);int score=mutual*5+common*3+(same?2:0);if(d==2)score+=5;else if(d==3)score+=3;else if(d==4)score+=1;return score;
}
void sortRecommendations(Recommendation r[],int n){for(int i=0;i<n-1;i++)for(int j=0;j<n-i-1;j++)if(r[j].score<r[j+1].score){Recommendation t=r[j];r[j]=r[j+1];r[j+1]=t;}}
void recommendFriends(Graph*g){
    if(g->userCount<2){printf("\nAt least two users are required.\n");return;}int sourceId=getInt("\nEnter User ID for recommendations: ");int s=findUserIndex(g,sourceId);if(s<0){printf("\nUser not found.\n");return;}
    Recommendation r[MAX_USERS];int n=0;for(int i=0;i<g->userCount;i++){int id=g->users[i].id;if(id==sourceId||areFriends(g,sourceId,id))continue;int score=calculateRecommendationScore(g,sourceId,id);if(score<0)continue;r[n].userId=id;r[n].mutualFriends=countMutualFriends(g,sourceId,id);r[n].commonInterests=countCommonInterests(&g->users[s],&g->users[i]);r[n].sameCity=strcmp(g->users[s].city,g->users[i].city)==0;r[n].socialDistance=bfsSocialDistance(g,sourceId,id);r[n].score=score;n++;}
    if(!n){printf("\nNo friend recommendations available.\n");return;}sortRecommendations(r,n);printf("\n====================================================\n              FRIEND RECOMMENDATIONS\n====================================================\n");for(int i=0;i<n;i++){int j=findUserIndex(g,r[i].userId);printf("\nRecommendation %d\n---------------------------------------------\nUser ID             : %d\nName                : %s\nCity                : %s\nMutual Friends      : %d\nCommon Interests    : %d\nSame City           : %s\nSocial Distance     : %s%d\nRecommendation Score: %d\n",i+1,g->users[j].id,g->users[j].name,g->users[j].city,r[i].mutualFriends,r[i].commonInterests,r[i].sameCity?"Yes":"No",r[i].socialDistance<0?"Not Connected (":"",r[i].socialDistance<0?0:r[i].socialDistance,r[i].score);}
}

void saveData(Graph*g){FILE*f=fopen(DATA_FILE,"wb");if(!f){printf("\nUnable to save data.\n");return;}fwrite(&g->userCount,sizeof(int),1,f);for(int i=0;i<g->userCount;i++){User*u=&g->users[i];fwrite(&u->id,sizeof(int),1,f);fwrite(u->name,sizeof(u->name),1,f);fwrite(&u->age,sizeof(int),1,f);fwrite(u->city,sizeof(u->city),1,f);fwrite(&u->interestCount,sizeof(int),1,f);fwrite(u->interests,sizeof(u->interests),1,f);int count=0;for(FriendNode*n=u->friends;n;n=n->next)count++;fwrite(&count,sizeof(int),1,f);for(FriendNode*n=u->friends;n;n=n->next)fwrite(&n->userId,sizeof(int),1,f);}fclose(f);printf("\nData saved successfully.\n");}
void loadData(Graph*g){FILE*f=fopen(DATA_FILE,"rb");if(!f)return;freeGraph(g);g->userCount=0;if(fread(&g->userCount,sizeof(int),1,f)!=1||g->userCount<0||g->userCount>MAX_USERS){g->userCount=0;fclose(f);return;}for(int i=0;i<g->userCount;i++){User*u=&g->users[i];u->friends=NULL;fread(&u->id,sizeof(int),1,f);fread(u->name,sizeof(u->name),1,f);fread(&u->age,sizeof(int),1,f);fread(u->city,sizeof(u->city),1,f);fread(&u->interestCount,sizeof(int),1,f);fread(u->interests,sizeof(u->interests),1,f);int count=0;fread(&count,sizeof(int),1,f);for(int j=0;j<count;j++){int id;if(fread(&id,sizeof(int),1,f)!=1)break;FriendNode*n=createFriendNode(id);n->next=u->friends;u->friends=n;}}fclose(f);printf("\nData loaded successfully.\n");}
void freeGraph(Graph*g){for(int i=0;i<g->userCount;i++){FriendNode*n=g->users[i].friends;while(n){FriendNode*t=n;n=n->next;free(t);}g->users[i].friends=NULL;}}

void displayMenu(void){printf("\n====================================================\n                  MAIN MENU\n====================================================\n 1. Register User\n 2. Display All Users\n 3. Search User\n 4. Add Friendship\n 5. Remove Friendship\n 6. Display Friends\n 7. Find Mutual Friends\n 8. Calculate Social Distance (BFS)\n 9. BFS Network Traversal\n10. DFS Network Traversal\n11. Recommend Friends\n12. Save Data\n13. Load Data\n14. Exit\n====================================================\n");}

int main(void){Graph graph;graph.userCount=0;for(int i=0;i<MAX_USERS;i++)graph.users[i].friends=NULL;loadData(&graph);printf("\n====================================================\n       SMART SOCIAL NETWORK SYSTEM\n       FRIEND RECOMMENDATION APPLICATION\n====================================================\n");int choice;do{displayMenu();choice=getInt("Enter your choice: ");switch(choice){case 1:registerUser(&graph);break;case 2:displayAllUsers(&graph);break;case 3:searchUser(&graph);break;case 4:{int a=getInt("Enter first User ID: "),b=getInt("Enter second User ID: ");addFriendship(&graph,a,b);break;}case 5:{int a=getInt("Enter first User ID: "),b=getInt("Enter second User ID: ");removeFriendship(&graph,a,b);break;}case 6:displayFriends(&graph,getInt("Enter User ID: "));break;case 7:{int a=getInt("Enter first User ID: "),b=getInt("Enter second User ID: ");displayMutualFriends(&graph,a,b);break;}case 8:{int a=getInt("Enter source User ID: "),b=getInt("Enter destination User ID: ");int d=bfsSocialDistance(&graph,a,b);printf(d<0?"\nNo connection exists between the users.\n":"\nSocial Distance = %d\n",d);break;}case 9:bfsTraversal(&graph,getInt("Enter starting User ID: "));break;case 10:dfsTraversal(&graph,getInt("Enter starting User ID: "));break;case 11:recommendFriends(&graph);break;case 12:saveData(&graph);break;case 13:loadData(&graph);break;case 14:saveData(&graph);printf("\nThank you for using Smart Social Network!\n");break;default:printf("\nInvalid choice. Please try again.\n");}}while(choice!=14);freeGraph(&graph);return 0;}
