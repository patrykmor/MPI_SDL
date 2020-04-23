#include<stdbool.h>
#include<stdlib.h>
#include<stdio.h>
typedef struct window{
    int rank;
    int borderWidth;
    int windowHeight;
    int windowWidth;
    bool topBorder;
    bool rightBorder;
    bool bottomBorder;
    bool leftBorder;
} Window;

Window* Window_create(int height, int width, int borderWidth, int rank, int resX, int resY){
    Window* ret = (Window*) malloc(sizeof(Window));
    printf("rank is %d, resX is %d, resY is %d, rank/resY is %d\n", rank, resX, resY, rank/resY);
    ret->rank=rank;
    ret->borderWidth=borderWidth;
    ret->windowHeight=height;
    ret->windowWidth=width;
    ret->topBorder=(rank/resX==0);
    if(ret->topBorder){
        printf("top\n");
    }
    (ret->bottomBorder)=((rank/resX)==(resY-1));
    if(ret->bottomBorder){
        printf("bot\n");
    }
    ret->rightBorder=rank%resX==resX-1;
    if(ret->rightBorder){
        printf("right\n");
    }
    ret->leftBorder=rank%resX==0;
    if(ret->leftBorder){
        printf("left\n");
    }
    return ret;
}

void Window_destroy(Window* self){
    free(self);
}
