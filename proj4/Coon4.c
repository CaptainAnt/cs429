#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#if 0
   #define debugPrint(a) printf a
#else
   #define debugPrint(a) (void)0
#endif


char* c;		   //character holder for instantiation;
FILE* fp;		//file read variable
char* w1;      //first word;
char* w2;      //second word
char* w3;      //third word?? never implemented =(

typedef struct item{
   char* val;
   int count;
   struct item* tail; 
}item;

typedef struct node{
   struct item *value;
   struct node *next;
}node;

char* getwrd(char* wr,int spot){
	char* ret = (char*)malloc(100);
   	char* token = strtok(wr," \n\t\r\b");
   	int i;
   	for(i = 0; i<spot;i++){
		if(i == 2 && token != NULL)
			return ret;
		if(i == 1 && token == NULL)
			return token;
		if(i == spot-1 && token == NULL)
			ret = token;
		else
			strcpy(ret,token);
		token = strtok(NULL," \n\t\r\b");
   }
   return ret;
}

node* ex(char* str,node* r);

item* exi(char* str, item* r);

void testp(node* r);

node* searchlow(node* r);

main(int arg1, char *arg2[]){
   	int z = 1;
   	c = (char*)malloc(200);
	while(--arg1 > 0){
	fp = fopen(*++arg2, "r");}
	struct node* root = (node*)malloc(sizeof(struct node));
   	struct node* tail = (node*)malloc(sizeof(struct node));
	root->value = (item*)malloc(sizeof(struct item));
   	tail = root;
   	w1 = (char*)malloc(100);
   	w2 = (char*)malloc(100);

   debugPrint(("----------PROGRAM START------------\n"));
   while (fgets(c, 100, fp) != NULL){
		char* c1 = (char*)malloc(200);
		strcpy(c1,c);
		char* c2 = (char*)malloc(200);
		strcpy(c2,c);
		char* c3 = (char*)malloc(200);
		strcpy(c3,c);
	  debugPrint(("----------loop #%d-----------\n",z));
	  z++;
	  if(getwrd(c1,3)!=NULL){
		printf("Error: Bad input, too many strings\n");
		return;
	  }
	  if(getwrd(c2,2)==NULL){
		printf("Error: Bad input, not enough strings\n");
		 return;
	  }
	  w2 = getwrd(c,2);
	  w1 = getwrd(c,1);
	  debugPrint(("w1 = %s w2 = %s\n",w1,w2));
	  /*FIRST WORD STUFF*/
	  if(ex(w1,root)->value == NULL){ //if the word doesn't exist already....
		 //debugPrint(("got inside first word allocation\n"));
		 if(root->value->val == NULL){ //if root doens't have a value
			debugPrint(("root does not already have a node, adding...\n"));
			root->value->val = w1;
			debugPrint(("set root node to %s\n",w1));
		 }   
		 else{
			debugPrint(("%s does not already have a node, adding...",w1));
			struct node* new = (node*)malloc(sizeof(struct node)+1);
			new->value = (item*)malloc(sizeof(struct item));
			new->next = NULL;
			new->value->val = w1;
			tail->next = new;
			tail = new;
			debugPrint(("added new node for %s\n",w1));
		 }
	  }
	  else{
		 debugPrint(("Node %s already exists\n",w1));
	  }
	  /*SECOND WORD STUFF*/
	  if(ex(w2,root)->value == NULL){ //if word doens't exist in list yet
		 //debugPrint(("got into second word node allocaton\n"));
		 /*NEW NODE CODE*/
		 struct node* new = (node*)malloc(sizeof(struct node)+1);
		 new->value = (item*)malloc(sizeof(struct item)+1);
		 new->value->val = (char*)malloc(100);
		 new->next = NULL;
		 new->value->val = w2;
		 new->value->count = 1; 
		 tail->next = new;
		 tail = new;
		 /*END NEW NODE CODE*/
		 debugPrint(("added new node %s\n",tail->value->val));
	  }
	  else{ //if word already exists
		 struct node* exs = ex(w2,root);
		 debugPrint(("Node %s already exists\nIncrementing count...\n",exs->value->val));
		 exs->value->count += 1;
		 debugPrint(("Node %s new count is %d\n",exs->value->val,exs->value->count));
	  }
	  /*SECOND NODE FOLLOW STUFF*/
	  struct node* cur = ex(w1,root);
	  struct item* fol = (item*)malloc(sizeof(struct item)+1);
	  debugPrint(("Current node = %s\n",cur->value->val));
	  if(cur->value->tail == NULL){ //if no tail on current
			debugPrint(("Nothing assigned to %s's tail...\n",cur->value->val));
			//debugPrint(("word 2 in memory: %s\n",tail->value->val));
			fol->val = w2;
			cur->value->tail = fol;
			struct item* x = ex(w2,root)->value;
			if(exi(w1,x)->val!=NULL){ //loop check code
				printf("Loop Error, bad input\n");
				return;
			}
	  }
	  else{ //if not under cur word but a node is 
		 debugPrint(("Node not assigned to %s's tail yet, adding...\n",cur->value->val));
		 struct item* n = (item*)malloc(sizeof(struct item));
		 fol->val = w2;
		 n = cur->value->tail;
		 while(n->tail != NULL){
			n = n->tail;
		 }
		 n->tail = fol;
		 struct item* x = ex(w2,root)->value;
		 if(exi(w1,x)->val!=NULL){ //loop check code
			printf("Loop Error, bad input\n");
			return;
		}
		 debugPrint(("Added %s to %s's tail\n",n->val,cur->value->val));
	  }
	  testp(root);
	  
   }

   debugPrint(("Making tail's next point to root for circular list\n"));
   tail->next = root;
   /*PRINTING STUFF*/
   debugPrint(("\n\n---------------Entered Printing portion---------------------\n"));
   struct node* low = (node*)malloc(sizeof(struct item));
   struct node* place = (node*)malloc(sizeof(struct item));
   place = root;
   while(place!=NULL){
      low = searchlow(place);
      while(place != NULL){
         if((place->next->value->val==low->value->val))
            break;
         if(place->value->count < low->value->count)
            low = place;
         if(strcmp(place->value->val,low->value->val)<0 && (place->value->val = low->value->val))
            low = place;
         place = place->next;
      }
      printf("%s\n",low->value->val);
      /*REMOVING FOLLOW COUNTS FROM LIST*/
      while(low->value->tail!=NULL){
         ex(low->value->tail->val,root)->value->count--;
         low->value->tail = low->value->tail->tail;
      }
      debugPrint(("Removed counts from %s's followers\n",low->value->val));
      /*REMOVING LOW FROM THE LIST*/
      if(place == place->next)
         break;
      place->next = place->next->next;
   } 
}

void testp(node* cur){
   struct node* r = (node*)malloc(sizeof(struct node));
   r = cur;
   struct item* n = (item*)malloc(sizeof(struct item));
   while(r != NULL){
      n = r->value->tail;
      debugPrint(("%s",r->value->val));
      debugPrint((" count = %d\n",r->value->count));
      debugPrint(("tails of %s:",r->value->val));
      while(n!=NULL){
         debugPrint((" %s",n->val));
         n = n->tail;
      }
      debugPrint(("\n"));
      r = r->next;
   }



}

node* searchlow(node* r){
   struct node* rr = (node*)malloc(sizeof(struct node)); //copy to preserve data
   rr = r;
   struct node* ret = (node*)malloc(sizeof(struct node)); //return node
   ret = rr;
   int l = rr->value->count;
   rr = rr->next;
   while(rr->value != ret->value){
      if(rr->value->count < ret->value->count)
         ret = rr;
      rr = rr->next;
   }
   return ret;
}

node* ex(char* str,node* r){
   node* cur = r;
   while(1){
      if(cur->value->val == NULL)
         break;
      if(strcmp(cur->value->val,str) == 0)
         return cur;
      if(cur->next != NULL)
         cur = cur->next;
      else
         break;
   }
   struct node* temp = (node*)malloc(sizeof(struct node));
   return temp;
}

item* exi(char* str,item* r){
   item* cur = r;
   while(cur->tail!=NULL){
      if(strcmp(cur->val,str)==0)
         return cur;
      cur = cur->tail;
   }
   struct item* temp = (item*)malloc(sizeof(struct item));
   return temp;
}