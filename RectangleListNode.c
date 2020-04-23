#include<stdlib.h>
#include<stdbool.h>
#include<stdio.h>
#include"Window.h"
#include"Rectangle.h"



typedef struct node{
    struct node* prev;
    struct node* next;
    Rectangle info;
} RectangleListNode;

RectangleListNode* Node_create(Rectangle* info){
    RectangleListNode* ret = (RectangleListNode*) malloc(sizeof(RectangleListNode));
    (ret->prev)=NULL;
    ret->next=NULL;
    return ret;
}
void List_add(RectangleListNode** self, Rectangle rect){
    RectangleListNode* new_node;
    RectangleListNode* old_prev;
    new_node=(RectangleListNode*) malloc(sizeof(RectangleListNode));
    new_node->info=rect;
    new_node->next= *self;
    if(*self!=NULL){
        (*self)->prev=new_node;
    }
    *self = new_node;
}
void List_remove(RectangleListNode** self){
    RectangleListNode* next_node;
    if(*self==NULL){
        printf("Failed to remove!\n");
        return;
    }
    RectangleListNode* node=*self;
    next_node=node->next;
    if(next_node!=NULL){
        next_node->prev=node->prev;
    }
    free(*self);
    printf("Removed!\n");
    *self=next_node;
}