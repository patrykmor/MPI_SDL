#include<stdlib.h>
typedef struct{
    int n;
    int ne;
    int nw;
    int e;
    int w;
    int s;
    int se;
    int sw;
} Neighbours;

Neighbours* Neighbours_create(int rank, int resX, int numtask){
    Neighbours* ret = (Neighbours*) malloc(sizeof(Neighbours));
    ret->n=rank-resX;
    ret->ne=rank-resX+1;
    ret->nw=rank-resX-1;
    ret->w=rank-1;
    ret->e=rank+1;
    ret->s=rank+resX;
    ret->se=rank+resX+1;
    ret->sw=rank+resX-1;
    return ret;
}