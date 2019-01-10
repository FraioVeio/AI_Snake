#include <GL/glut.h>  // GLUT, include glu.h and gl.h
#include <stdio.h>
#include <thread>
#include <unistd.h>
#include <string.h>
#include <math.h>

#include "Snake.hpp"
#include "Evolution.hpp"


#define RANDOMLIST_SIZE 3000

int *randomlist;
int randomindex;


/* AI parameters */
int population = 1000;
int podio = 100;
int hiddenLayers = 2;
int neuronsPerLayer = 100;
float mutationFactor = 0.01;
float replaceFactor = 0.005;
float mutationsize = 0.1;
/***/

Snake *gsnake;
int gridSize = 10;
int maxHunger = 40;
int refreshMills = 30; // refresh interval in milliseconds
bool displayBest = false;
bool pacman = true;
int gameMillis = 100;
int snakeDir = DIR_RIGHT;

/* Initialize OpenGL Graphics */
void initGL() {
   // Set "clearing" or background color
   glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black and opaque
}

void drawGrid(int size) {
    float lineWidth = 2.5;
    for(int i=0;i<=size;i++) {
        glLineWidth(lineWidth);
        glColor3f(0.3, 0.3, 0.3);
        glBegin(GL_LINES);
            glVertex2f(-1, 1-(2.0/size)*i);
            glVertex2f(1, 1-(2.0/size)*i);
        glEnd();
    }

    for(int i=0;i<=size;i++) {
        glLineWidth(lineWidth);
        glColor3f(0.3, 0.3, 0.3);
        glBegin(GL_LINES);
            glVertex2f(1-(2.0/size)*i, -1);
            glVertex2f(1-(2.0/size)*i, 1);
        glEnd();
    }

}

void drawSquare(int _gridSize, int _x, int _y, float cr, float cg, float cb) {
    float dimension = 2.0/_gridSize;
    float x = 2*((float)_x)/_gridSize - 1;
    float y = 2*(-(float)_y)/_gridSize + 1;

    glBegin(GL_QUADS);
        glColor3f(cr, cg, cb);
        glVertex2f(x, y);
        glVertex2f(x, y-dimension);
        glVertex2f(x+dimension, y-dimension);
        glVertex2f(x+dimension, y);
    glEnd();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);   // Clear the color buffer with current clearing color
    int sx, sy;

    if(gsnake != NULL) {
        for(int i=0;i<gsnake->getSnakeSize();i++) {
            gsnake->getSnakeCoordinate(i, &sx, &sy);
            drawSquare(gridSize, sx, sy, 1, 0, 0);
        }

        gsnake->getCookieCoordinate(&sx, &sy);
        drawSquare(gridSize, sx, sy, 0, 1, 1);
    }
    drawGrid(gridSize);
 
    glFlush();  // Render now
}
 
/* Handler for window re-size event. Called back when the window first appears and
   whenever the window is re-sized with its new width and height */
void reshape(GLsizei width, GLsizei height) {  // GLsizei for non-negative integer
    // Compute aspect ratio of the new window
    if (height == 0) height = 1;                // To prevent divide by 0
    GLfloat aspect = (GLfloat)width / (GLfloat)height;
    
    // Set the viewport to cover the new window
    glViewport(0, 0, width, height);
    
    // Set the aspect ratio of the clipping area to match the viewport
    glMatrixMode(GL_PROJECTION);  // To operate on the Projection matrix
    glLoadIdentity();             // Reset the projection matrix
    if (width >= height) {
        // aspect >= 1, set the height from -1 to 1, with larger width
        gluOrtho2D(-1.0 * aspect, 1.0 * aspect, -1.0, 1.0);
    } else {
        // aspect < 1, set the width to -1 to 1, with larger height
        gluOrtho2D(-1.0, 1.0, -1.0 / aspect, 1.0 / aspect);
    }
}

void Timer(int value) {
   glutPostRedisplay();      // Post re-paint request to activate display()
   glutTimerFunc(refreshMills, Timer, 0); // next Timer call milliseconds later
}

/* Callback handler for normal-key event */
void keyboard(unsigned char key, int x, int y) {
    if(key == 'd') {
        displayBest = !displayBest;
        if(displayBest)
            printf("Display best on\n");
        else
            printf("Display best off\n");
    }
}
 
/* Callback handler for special-key event */
void specialKeys(int key, int x, int y) {
    /* Snake movement */
    /*
    if(key == 100 && gsnake->getDirection() != DIR_RIGHT) {
        snakeDir = DIR_LEFT;
    }
    if(key == 101 && gsnake->getDirection() != DIR_DOWN) {
        snakeDir = DIR_UP;
    }
    if(key == 102 && gsnake->getDirection() != DIR_LEFT) {
        snakeDir = DIR_RIGHT;
    }
    if(key == 103 && gsnake->getDirection() != DIR_UP) {
        snakeDir = DIR_DOWN;
    }*/
}

float brainPlay(Brain *b, int us, bool graphics) {
    Snake *s = new Snake(gridSize, maxHunger, randomlist, RANDOMLIST_SIZE);
    if(graphics)
        gsnake = s;
    s->setPacman(pacman);

    float inputs[gridSize*gridSize+1], outputs[4];
    float grid[gridSize][gridSize];

    while(s->isAlive()) {
        memset(grid, 0, gridSize*gridSize*sizeof(float));
        for(int i=1;i<s->snakeSize;i++) {   // Corpo -0.5
            grid[s->snakex[i]][s->snakey[i]] = -0.5;
        }
        grid[s->snakex[0]][s->snakey[0]] = -1;  // Testa
        grid[s->cookiex][s->cookiey] = 1;
        memcpy(inputs, grid, gridSize*gridSize*sizeof(float));
        inputs[gridSize*gridSize] = s->getDirection()/2.0-1;

        b->compute(inputs, outputs);
       
        if(outputs[0] > outputs[1] && outputs[0] > outputs[2] && outputs[0] > outputs[3]) { // DOWN
            if(s->getDirection() != DIR_UP)
                s->step(DIR_DOWN);
            else
                s->step(DIR_SAME);
        }else if(outputs[1] > outputs[2] && outputs[1] > outputs[3] && outputs[1] > outputs[0]) { // LEFT
            if(s->getDirection() != DIR_RIGHT)
                s->step(DIR_LEFT);
            else
                s->step(DIR_SAME);
        }else if(outputs[2] > outputs[3] && outputs[2] > outputs[0] && outputs[2] > outputs[1]) { // RIGHT
            if(s->getDirection() != DIR_LEFT)
                s->step(DIR_RIGHT);
            else
                s->step(DIR_SAME);
        }else if(outputs[3] > outputs[0] && outputs[3] > outputs[1] && outputs[3] > outputs[2]) { // UP
            if(s->getDirection() != DIR_DOWN)
                s->step(DIR_UP);
            else
                s->step(DIR_SAME);
        }else {
            s->step(DIR_SAME);
            printf("Occhio net strana\n");
        }
        
        if(us != 0)
            usleep(us*1000);
    }

    float fitness = s->snakeSize-2;


    return fitness;
}

void mainThread() {
    Evolution *evo = new Evolution(population, gridSize*gridSize+1, 4, hiddenLayers, neuronsPerLayer);
    int generation = 1;
    while(1) {
        for(int bi=0;bi<population;bi++) {
            Brain *b = evo->getBrain(bi);

            float fitness = brainPlay(b, 0, false);

            evo->setResult(bi, fitness);
        }
        printf("Generation %i: %f\n", generation, evo->results[evo->getBestBrainIndex()]);
        if(displayBest)
            brainPlay(evo->getBrain(evo->getBestBrainIndex()), 100, true);


        evo->evolve(podio, mutationFactor, replaceFactor, mutationsize);
        generation ++;
    }
}

/* Main function: GLUT runs as a console application starting at main() */
int main(int argc, char** argv) {
    
    randomlist = (int*) calloc(RANDOMLIST_SIZE, sizeof(int));
    randomindex = 0;
    srand (time(NULL));
    for(int i=0;i<RANDOMLIST_SIZE;i++) {
        randomlist[i] = random()%gridSize;
    }

    glutInit(&argc, argv);          // Initialize GLUT
    glutInitWindowSize(640, 480);   // Set the window's initial width & height - non-square
    glutInitWindowPosition(50, 50); // Position the window's initial top-left corner
    glutCreateWindow("AI Snake");  // Create window with the given title
    glutDisplayFunc(display);       // Register callback handler for window re-paint event
    glutReshapeFunc(reshape);       // Register callback handler for window re-size event

    glutSpecialFunc(specialKeys); // Register callback handler for special-key event
    glutKeyboardFunc(keyboard);   // Register callback handler for special-key event


    glutTimerFunc(0, Timer, 0);     // First timer call immediately
    initGL();                       // Our own OpenGL initialization

    /*
    gsnake = new Snake(gridSize);
    gsnake->setPacman(true);
    */
    std::thread thr(mainThread);


    glutMainLoop();                 // Enter the infinite event-processing loop
    
    return 0;
}