#ifndef SNAKE_HPP
#define SNAKE_HPP

#define DIR_SAME 0
#define DIR_RIGHT 1
#define DIR_DOWN 2
#define DIR_LEFT 3
#define DIR_UP 4

class Snake {
    private:
        int gridSize;
        int snakeSize;
        int snakeMaxSize;
        int *snakex;
        int *snakey;
        int snakeDir;
        int cookiex;
        int cookiey;
        bool alive;
        bool pacman;
        
        void generateCookie();
        bool isColliding();
    public:
        Snake(int size);
        ~Snake();
        void step(int direction);
        int getSnakeSize();
        void getSnakeCoordinate(int i, int *x, int *y);
        void getCookieCoordinate(int *x, int *y);
        bool isAlive();
        int getDirection();
        void setPacman(bool p);
};

#endif