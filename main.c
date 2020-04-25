#include<stdlib.h>
#include<stdbool.h>
#include"mpi/mpi.h"
#include<SDL2/SDL.h>

#include"Neighbours.h"
#include"Window.h"
#include"Rectangle.h"
#include"RectangleListNode.h"


#define WINDOW_WIDTH 480
#define WINDOW_HEIGHT 320
#define RECTANGLE_WIDTH 30
#define RECTANGLE_HEIGHT 30
#define BORDER_WIDTH 40


typedef struct {
    int x;
    int y;
}Offset;

Offset offsetN={0, -WINDOW_HEIGHT};
Offset offsetNE={WINDOW_WIDTH, -WINDOW_HEIGHT};
Offset offsetNW={-WINDOW_WIDTH, -WINDOW_HEIGHT};
Offset offsetE={WINDOW_WIDTH, 0};
Offset offsetW={-WINDOW_WIDTH, 0};
Offset offsetS={0, WINDOW_HEIGHT};
Offset offsetSE={WINDOW_WIDTH, WINDOW_HEIGHT};
Offset offsetSW={-WINDOW_WIDTH, WINDOW_HEIGHT};

int windowsVertically, windowsHorizontally;

Window* this;
SDL_Window* sdlWindow;
SDL_Renderer* renderer; 
RectangleListNode* head = NULL;
Neighbours* neighbours;

MPI_Datatype mpi_rectangle;

bool board[WINDOW_WIDTH+2*BORDER_WIDTH+2][WINDOW_HEIGHT+2*BORDER_WIDTH+2];

void processLocalRectangles();
void drawRectangle(Rectangle* rect);
void processNeighbourRectangles();
void resetBoard();
void processColisions();

int main(int argc, char *argv[])
{

    if(argc<3){
        printf("Invalid arguments! 1");
        return 0;
    }
    windowsHorizontally=atoi(argv[1]);
    windowsVertically=atoi(argv[2]);
    if(windowsVertically==0||windowsHorizontally==0){
        printf("Invalid arguments! %d %d ", windowsHorizontally, windowsVertically);
        return 0;
    }

    int numtasks, rank;
    
    
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    List_add(&head, *Rectangle_create(RECTANGLE_WIDTH, RECTANGLE_HEIGHT, 0, 0));
   
    
    if(windowsHorizontally*windowsVertically!=numtasks){
        printf("Invalid arguments! 3");
        MPI_Finalize();
        return 0;
    }
    this=Window_create(WINDOW_HEIGHT, WINDOW_WIDTH, BORDER_WIDTH, rank, windowsHorizontally, windowsVertically);
    printf("\nrank %d \n", this->rank);
    SDL_Init(SDL_INIT_VIDEO);
    char number[4];
    sprintf(number, "%d", rank+1);
    sdlWindow = SDL_CreateWindow(number, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_ALLOW_HIGHDPI);
    renderer= SDL_CreateRenderer(sdlWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    //get the rank numbers of neighbours
    neighbours=Neighbours_create(rank, windowsHorizontally);
    
    //creating mpi datatype
    int blocklengths[6]={1,1,1,1,1,1};
    MPI_Datatype types[6] ={MPI_INT, MPI_INT, MPI_INT, MPI_INT, MPI_INT, MPI_INT};
    MPI_Aint offsets[6];

    offsets[0]=offsetof(Rectangle, width);
    offsets[1]=offsetof(Rectangle, height);
    offsets[2]=offsetof(Rectangle, x);
    offsets[3]=offsetof(Rectangle, y);
    offsets[4]=offsetof(Rectangle, speedX);
    offsets[5]=offsetof(Rectangle, speedY);

    MPI_Type_create_struct(6, blocklengths, offsets, types, &mpi_rectangle);
    MPI_Type_commit(&mpi_rectangle);  


    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
    SDL_Event event;
    bool running=true;
    while(running){
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = false;
            }
            if(event.type==SDL_MOUSEBUTTONDOWN&&event.button.button==SDL_BUTTON_LEFT){
                int x= event.button.x;
                int y= event.button.y;
                if(x+RECTANGLE_WIDTH>=WINDOW_WIDTH){
                    x=WINDOW_WIDTH-RECTANGLE_WIDTH-1;
                }
                if(y+RECTANGLE_HEIGHT>=WINDOW_HEIGHT){
                    y=WINDOW_HEIGHT-RECTANGLE_HEIGHT-1;
                }
                List_add(&head, *Rectangle_create(RECTANGLE_WIDTH, RECTANGLE_HEIGHT, x, y));
                printf("Added rectangle at x=%d, y=%d\n", x, y);
            }
        }
        if(!running){
            MPI_Abort(MPI_COMM_WORLD, 0);
            break;
        }
        resetBoard();
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        //printf("Processing local rectangles...\n");
        processLocalRectangles();
        //printf("Barrier 1 \n");
        //MPI_Barrier(MPI_COMM_WORLD);
        //printf("Processing neighbour rectangles...\n");
        processNeighbourRectangles();
        processColisions();
        //printf("Barrier 2 \n");
        //MPI_Barrier(MPI_COMM_WORLD);
        SDL_RenderPresent(renderer);
        
    }
    MPI_Finalize();
}

void processColisions(){
    RectangleListNode* current =head;
    while (current!=NULL)
    {
        Rectangle* rect = &current->info;
        int x = rect->x+rect->speedX;
        int y = rect->y;
        if(rect->speedX<0){
            bool collision=false;
            for(int i=y; i<y+RECTANGLE_HEIGHT; i++){
                if(board[x+BORDER_WIDTH+1][i+BORDER_WIDTH+1]){
                    collision=true;
                    break;
                }
            }
            if(collision){
                printf("Colision...\n");
                rect->speedX=-rect->speedX;
            }
        }else{
            bool collision=false;
            for(int i=y; i<y+RECTANGLE_HEIGHT; i++){
                if(board[x+RECTANGLE_WIDTH+BORDER_WIDTH+1][i+BORDER_WIDTH+1]){
                    collision=true;
                    break;
                }
            }
            if(collision){
                printf("Colision...\n");
                rect->speedX=-rect->speedX;
            }
        } 
        x = rect->x;
        y = rect->y+rect->speedY;
        if(rect->speedY<0){
            bool collision=false;
            for(int i=x; i<x+RECTANGLE_WIDTH; i++){
                if(board[i+BORDER_WIDTH+1][y+BORDER_WIDTH+1]){
                    collision=true;
                    break;
                }
            }
            if(collision){
                printf("Colision...\n");
                rect->speedY=-rect->speedY;
            }
        }else{
            bool collision=false;
            for(int i=x; i<x+RECTANGLE_WIDTH; i++){
                if(board[i+BORDER_WIDTH+1][y+RECTANGLE_HEIGHT+BORDER_WIDTH+1]){
                    collision=true;
                    break;
                }
            }
            if(collision){
                printf("Colision...\n");
                rect->speedY=-rect->speedY;
            }
        }
        current=current->next;
    }
    
}
void resetBoard(){
    for(int i=0; i<WINDOW_WIDTH+2*BORDER_WIDTH+2; i++){
        memset(board[i], 0, sizeof board[i]);
    }
}
void receiveInfo(int rank, Offset offsets);
void processNeighbourRectangles(){
    if(!(this->topBorder)){
        receiveInfo(neighbours->n, offsetN);
        if(!(this->leftBorder)){
            receiveInfo(neighbours->nw, offsetNW);
        }
        if(!(this->rightBorder)){
            receiveInfo(neighbours->ne, offsetNE);
        }
    }
    if(!(this->leftBorder)){
        receiveInfo(neighbours->w, offsetW);
    }
    if(!(this->rightBorder)){
        receiveInfo(neighbours->e, offsetE);
    }
    if(!(this->bottomBorder)){
        receiveInfo(neighbours->s, offsetS);
        if(!(this->leftBorder)){
            receiveInfo(neighbours->sw, offsetSW);
        }
        if(!(this->rightBorder)){
          receiveInfo(neighbours->se, offsetSE);
        }
    }
}
void sendRectangleInfo(Rectangle* rect, int* msgCounter);

void processLocalRectangles(){
    //0=NW, 1=N, 2=NE, 3=W, 4=E, 5=SW, 6=S, 7=SE
    int msgCounter[8]={0,0,0,0,0,0,0,0};
    RectangleListNode** current = &head;
    while((*current)!=NULL){
        Rectangle* rect=&((*current)->info);
        Rectangle_move(rect, this);
        drawRectangle(rect);
        sendRectangleInfo(rect, msgCounter);
        RectangleListNode** next = &((*current)->next);
        if(rect->x<0||rect->y<0||rect->x>=WINDOW_WIDTH||rect->y>=WINDOW_HEIGHT){
            printf("Rank %d, removing: Rect x=%d, y=%d, speedx=%d, speedy=%d \n",this->rank,  rect->x, rect->y, rect->speedX, rect->speedY);
            List_remove(current);
        }else{
            current=next;
        }
        
    }
    if(!(this->topBorder)){
        MPI_Send(&msgCounter[1], 1, MPI_INT, neighbours->n, 0, MPI_COMM_WORLD);
        if(!(this->leftBorder)){
            MPI_Send(&msgCounter[0], 1, MPI_INT, neighbours->nw, 0, MPI_COMM_WORLD);
        }
        if(!(this->rightBorder)){
            MPI_Send(&msgCounter[2], 1, MPI_INT, neighbours->ne, 0, MPI_COMM_WORLD);
        }
    }
    if(!(this->leftBorder)){
        MPI_Send(&msgCounter[3], 1, MPI_INT, neighbours->w, 0, MPI_COMM_WORLD);
    }
    if(!(this->rightBorder)){
        MPI_Send(&msgCounter[4], 1, MPI_INT, neighbours->e, 0, MPI_COMM_WORLD);
    }
    if(!(this->bottomBorder)){
        MPI_Send(&msgCounter[6], 1, MPI_INT, neighbours->s, 0, MPI_COMM_WORLD);
        if(!(this->leftBorder)){
            MPI_Send(&msgCounter[5], 1, MPI_INT, neighbours->sw, 0, MPI_COMM_WORLD);
        }
        if(!(this->rightBorder)){
            MPI_Send(&msgCounter[7], 1, MPI_INT, neighbours->se, 0, MPI_COMM_WORLD);
        }
    }
}

void receiveInfo(int rank, Offset offset){
    int count;
    MPI_Status* status;
    //printf("Receiving from %d...\n", rank);
    MPI_Recv(&count, 1, MPI_INT, rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    //printf("Current rank %d, received %d rectangles from rank %d\n", this->rank, count, rank);
    for(int i=1; i<=count; i++){
        Rectangle rect;
        MPI_Recv(&rect, 1, mpi_rectangle, rank, i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        rect.x+=offset.x;
        rect.y+=offset.y;
        
        drawRectangle(&rect);
        //Berechnungen übernehmen
        if(rect.x>=0&&(rect.x<this->windowWidth)&&rect.y>=0&&(rect.y<this->windowHeight)){
            printf("GOTCHA\n");
            List_add(&head, rect);
            printf("Rank %d received, Rect x=%d, y=%d, speedx=%d, speedy=%d \n",this->rank,  rect.x, rect.y, rect.speedX, rect.speedY);
            
        }
    }
}

void sendRectangleInfo(Rectangle* rect, int* msgCounter){
    int pos= Rectangle_get_position_code(rect, this);
    switch(pos){
        //nw
        case 0:
            MPI_Send(rect, 1, mpi_rectangle, neighbours->n, ++*(msgCounter+1), MPI_COMM_WORLD);
            MPI_Send(rect, 1, mpi_rectangle, neighbours->w, ++*(msgCounter+3), MPI_COMM_WORLD);
            MPI_Send(rect, 1, mpi_rectangle, neighbours->nw, ++*(msgCounter+0), MPI_COMM_WORLD);
            break;
        case 1:
            MPI_Send(rect, 1, mpi_rectangle, neighbours->n, ++*(msgCounter+1), MPI_COMM_WORLD);
            break;
        case 2:
            MPI_Send(rect, 1, mpi_rectangle, neighbours->n, ++*(msgCounter+1), MPI_COMM_WORLD);
            MPI_Send(rect, 1, mpi_rectangle, neighbours->e, ++*(msgCounter+4), MPI_COMM_WORLD);
            MPI_Send(rect, 1, mpi_rectangle, neighbours->ne, ++*(msgCounter+2), MPI_COMM_WORLD);
        case 3:
            MPI_Send(rect, 1, mpi_rectangle, neighbours->w, ++*(msgCounter+3), MPI_COMM_WORLD);
            break;
        //  MPI_Send(&msgCounter[7], 1, MPI_INT, neighbours->se, 0, MPI_COMM_WORLD);4 ist die Mitte des Fensters
        case 5:
            MPI_Send(rect, 1, mpi_rectangle, neighbours->e, ++*(msgCounter+4), MPI_COMM_WORLD);
            break;
        case 6:
            MPI_Send(rect, 1, mpi_rectangle, neighbours->w, ++*(msgCounter+3), MPI_COMM_WORLD);
            MPI_Send(rect, 1, mpi_rectangle, neighbours->s, ++*(msgCounter+6), MPI_COMM_WORLD);
            MPI_Send(rect, 1, mpi_rectangle, neighbours->sw, ++*(msgCounter+5), MPI_COMM_WORLD);
            break;
        case 7:
            MPI_Send(rect, 1, mpi_rectangle, neighbours->s, ++*(msgCounter+6), MPI_COMM_WORLD);
            break;
        case 8:
            MPI_Send(rect, 1, mpi_rectangle, neighbours->s, ++*(msgCounter+6), MPI_COMM_WORLD);
            MPI_Send(rect, 1, mpi_rectangle, neighbours->e, ++*(msgCounter+4), MPI_COMM_WORLD);
            MPI_Send(rect, 1, mpi_rectangle, neighbours->se, ++*(msgCounter+7), MPI_COMM_WORLD);
            break;
    }
}

void drawRectangle(Rectangle* rect){
    SDL_Rect srect;
    srect.x=rect->x;
    srect.y=rect->y;
    srect.w=rect->width;
    srect.h=rect->height;
    SDL_RenderFillRect(renderer, &srect);
    for(int x = srect.x; x<srect.w+srect.x; x++){
        if(x<-BORDER_WIDTH-1){
            continue;
        }
        if(x>=(WINDOW_WIDTH+BORDER_WIDTH+1)){
            break;
        }
        for(int y=srect.y; y<srect.h+srect.y; y++){
            if(y>=(WINDOW_HEIGHT+BORDER_WIDTH+1)){
                break;
            }
            if(y<-BORDER_WIDTH-1){
                    continue;
            }
            board[x+BORDER_WIDTH+1][y+BORDER_WIDTH+1]=true;
        }
    }
}



