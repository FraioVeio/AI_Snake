#include "Snake.hpp"
#include <stdlib.h>
#include <string.h>
#include <time.h>

void Snake::step(int direction) {
    hunger --;
    if(hunger < 0)
        alive = false;
    
    if(alive) {
        int lastx = snakex[snakeDir-1];
        int lasty = snakey[snakeDir-1];
        for(int i=snakeSize-1;i>0;i--) {
            snakex[i] = snakex[i-1];
            snakey[i] = snakey[i-1];
        }
        if(direction != DIR_SAME) {
            snakeDir = direction;
        }
        switch(snakeDir) {
            case DIR_RIGHT:
                snakex[0] = snakex[1]+1;
                snakey[0] = snakey[1];
                break;
            case DIR_DOWN:
                snakex[0] = snakex[1];
                snakey[0] = snakey[1]+1;
                break;
            case DIR_LEFT:
                snakex[0] = snakex[1]-1;
                snakey[0] = snakey[1];
                break;
            case DIR_UP:
                snakex[0] = snakex[1];
                snakey[0] = snakey[1]-1;
                break;
        }


        // Collisione pareti
        if(snakex[0] < 0) {
            if(pacman)
                snakex[0] = gridSize-1;
            else
                alive = false;
        }
        if(snakex[0] > gridSize-1) {
            if(pacman)
                snakex[0] = 0;
            else
                alive = false;
        }
        if(snakey[0] < 0) {
            if(pacman)
                snakey[0] = gridSize-1;
            else
                alive = false;
        }
        if(snakey[0] > gridSize-1) {
            if(pacman)
                snakey[0] = 0;
            else
                alive = false;
        }

        // Collisione cookie
        if(snakex[0] == cookiex && snakey[0] == cookiey) {
            snakex[snakeSize] = lastx;
            snakey[snakeSize] = lasty;
            
            snakeSize ++;
            hunger = maxHunger;

            generateCookie();
        }

        if(isColliding()) {
            alive = false;
        }
    } else {
        // Death, do nothing
    }
}

void Snake::generateCookie() {
    
    bool ok = false;
    int cx;
    int cy;
    while(!ok) {
        ok = true;
        
        cx = getRandom();
        cy = getRandom();
        
        for(int i=0;i<snakeSize;i++) {
            if(cx == snakex[i] && cy == snakey[i])
                ok = false;
        }
    }

    cookiex = cx;
    cookiey = cy;
}

void Snake::setPacman(bool p) {
    pacman = p;
}

bool Snake::isColliding() {
    for(int i=1;i<snakeSize;i++) {
        if(snakex[0] == snakex[i] && snakey[0] == snakey[i])
            return true;
    }

    return false;
}

bool Snake::isAlive() {
    return alive;
}

int Snake::getDirection() {
    return snakeDir;
}

void Snake::getSnakeCoordinate(int i, int *x, int *y) {
    *x = snakex[i];
    *y = snakey[i];
}

int Snake::getSnakeSize() {
    return snakeSize;
}

void Snake::getCookieCoordinate(int *x, int *y) {
    *x = cookiex;
    *y = cookiey;
}

int Snake::getRandom() {
    int n = randomlist[randomlistindex++];
    if(randomlistindex >= randomlistsize)
        randomlistindex = 0;

    return n;
}

Snake::Snake(int size, int maxHunger, int *randomlist, int randomlistsize) {
    gridSize = size;

    snakeMaxSize = gridSize * gridSize;

    snakex = (int*) calloc(snakeMaxSize, sizeof(int));
    snakey = (int*) calloc(snakeMaxSize, sizeof(int));

    memset(snakex, 0, sizeof(snakex));
    memset(snakey, 0, sizeof(snakex));


    /* Initialize snake */
    snakeSize = 2;
    snakex[0] = 1;
    snakey[0] = 0;
    snakex[1] = 0;
    snakey[1] = 0;
    snakeDir = DIR_RIGHT;

    alive = true;
    pacman = false;

    srand (time(NULL));
    this->randomlist = randomlist;
    this->randomlistsize = randomlistsize;
    randomlistindex = 0;
    generateCookie();

    hunger = maxHunger;
    this->maxHunger = maxHunger;
}

Snake::~Snake() {
    free(snakex);
    free(snakey);
}