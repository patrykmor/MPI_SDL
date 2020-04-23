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

Window* Window_create(int height, int width, int borderWidth, int rank, int resX, int resY);
void Window_destroy(Window* self);