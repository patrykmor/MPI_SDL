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

Neighbours* Neighbours_create(int rank, int resX);