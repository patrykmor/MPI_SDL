/*  
*   x,y - Koordinaten der linken Oberen Ecke
*   speedX, speedY - in Pixel/Zeitabschnitt
*   width, height - in Pixel
*/
#include<stdlib.h>
#include<stdio.h>
#include<stdbool.h>
#include"Window.h"
typedef struct
{
    int width;
    int height;
    int x;
    int y;
    int speedX;
    int speedY;
} Rectangle;

Rectangle* Rectangle_create(int width, int height, int x, int y)
{
    Rectangle* ret = (Rectangle*) malloc(sizeof(Rectangle));
    ret->width=width;
    ret->height=height;
    ret->x = x;
    ret->y = y;
    ret->speedX = 1;
    ret->speedY = 1;
    return ret;
}

void Rectangle_move(Rectangle* self, Window* window){
    int newX = self->x + self->speedX;
    int newY = self->y + self->speedY;
    if(((window->leftBorder)&&newX<0)||((window->rightBorder)&&(newX+self->width)>=(window->windowWidth))){
        printf("BUMP!");
        self->speedX=-self->speedX;
        newX= self->x + self->speedX;     
    }
    if(((window->topBorder)&&newY<0)||((window->bottomBorder)&&(newY+(self->height)>=(window->windowHeight)))){
        printf("BUMP!");
        self->speedY=-self->speedY;
        newY = self->y + self->speedY;
    }
    self->x=newX;
    self->y=newY;
}

/*  Position des Rechtecks im Fenster
*   0 = oben links (sende Info ans obere und linke Fenster + ans Fenster links oben)
*   1 = oben mitte (sende Info ans obere Fenster)
*   ... 
*   8= unten rechts (sende Info an das untere und das rechte Fenster)
*/

int Rectangle_get_position_code(Rectangle* self, Window* window){
    int code=4;
    //zeile
    if(!(window->topBorder)&&(self->y<window->borderWidth)){
        code-=3;
    }
    else if (!(window->bottomBorder)&&((self->y+self->height)>=window->windowHeight-window->borderWidth))
    {
        code+=3;
    }
    //spalte
    if(!(window->leftBorder)&&((self->x<window->borderWidth))){
        code-=1;
    }
    else if (!(window->rightBorder)&&((self->x+self->width)>=window->windowWidth-window->borderWidth))
    {
        code+=1;
    }
    return code;
}
void Rectangle_destroy(Rectangle* self){
    free(self);
}