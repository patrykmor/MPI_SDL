typedef struct rectangle{
    int width;
    int height;
    int x;
    int y;
    int speedX;
    int speedY;
} Rectangle;
Rectangle* Rectangle_create(int width, int height, int x, int y);
void Rectangle_move(Rectangle* self, Window* window);
int Rectangle_get_position_code(Rectangle* self, Window* window);
void Rectangle_destroy(Rectangle* self);
