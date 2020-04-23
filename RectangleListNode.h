typedef struct node{
    struct node* prev;
    struct node* next;
    Rectangle info;
} RectangleListNode;
void List_add(RectangleListNode** self, Rectangle rect);
void List_remove(RectangleListNode** self);
RectangleListNode* Node_create(Rectangle* info);
