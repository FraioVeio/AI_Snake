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
        int snakeMaxSize;
        int snakeDir;
        bool alive;
        bool pacman;
        int hunger;
        int maxHunger;

        void generateCookie();
        bool isColliding();
        int getRandom();
    public:
        Snake(int size, int maxHunger, int *randomlist, int randomlistsize);
        ~Snake();
        void step(int direction);
        int getSnakeSize();
        void getSnakeCoordinate(int i, int *x, int *y);
        void getCookieCoordinate(int *x, int *y);
        bool isAlive();
        int getDirection();
        void setPacman(bool p);


        int *snakex;
        int *snakey;
        int snakeSize;
        int cookiex;
        int cookiey;
        int *randomlist;
        int randomlistindex;
        int randomlistsize;
};

#endif