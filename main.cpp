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
int podio = 15;
int hiddenLayers = 1;
int neuronsPerLayer = 100;
float sumFactor = 0.02;
float sumsize = 0.4;
float replaceFactor = 0.0001;
float mulFactor = 0.02;
float mulsize = 0.1;
/***/

Snake *gsnake;
int gridSize = 10;
int maxHunger = 100;
int refreshMills = 30; // refresh interval in milliseconds
bool displayBest = false;
bool pacman = false;
int gameMillis = 50;
int snakeDir = DIR_RIGHT;


Evolution *evo;
genome_t bestGenome;

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
    if(_x<0 || _x>=_gridSize || _y<0 || _y>=_gridSize)
        return;
    
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
        gsnake->getSnakeCoordinate(0, &sx, &sy);
        drawSquare(gridSize, sx, sy, 1, 0.75, 1);
        for(int i=1;i<gsnake->getSnakeSize();i++) {
            gsnake->getSnakeCoordinate(i, &sx, &sy);
            drawSquare(gridSize, sx, sy, 0.5, 0.5, 0.5);
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
    int stepcount = 0;

    float currentmindist = MAXFLOAT;
    int sizeold = 2;

    while(s->isAlive()) {
        memset(grid, 0, gridSize*gridSize*sizeof(float));
        grid[s->cookiex][s->cookiey] = 1;
        for(int i=1;i<s->snakeSize;i++) {   // Corpo -0.5
            grid[s->snakex[i]][s->snakey[i]] = -0.5;
        }
        grid[s->snakex[0]][s->snakey[0]] = -1;  // Testa
        
        memcpy(inputs, grid, gridSize*gridSize*sizeof(float));
        inputs[gridSize*gridSize] = s->getDirection()/2.0-1;

        b->compute(inputs, outputs);

        if(s->getSnakeSize() > sizeold) {
            sizeold = s->getSnakeSize();
            currentmindist = MAXFLOAT;
        }

        float cdist = sqrt((s->cookiex - s->snakex[0])*(s->cookiex - s->snakex[0]) + (s->cookiey - s->snakey[0])*(s->cookiey - s->snakey[0]));
        
        if(cdist < currentmindist) {
            currentmindist = cdist;
        }

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
            //printf("Occhio net strana\n");
        }

        if(us != 0)
            usleep(us*1000);
        stepcount ++;
    }

    float gridDiag = sqrt(gridSize*gridSize + gridSize*gridSize);
    float fitness = s->snakeSize-2 + (gridDiag-currentmindist)/gridDiag;


    return fitness;
}

void mainThread() {
    int generation = 1;
    while(1) {
        for(int bi=0;bi<population;bi++) {
            Brain *b = evo->getBrain(bi);

            float fitness = brainPlay(b, 0, false);

            evo->setResult(bi, fitness);
        }

        float mean = 0;
        for(int i=0;i<population;i++) {
            mean += evo->results[i]/population;
        }
        bestGenome = evo->getBrain(evo->getBestBrainIndex())->getGenome();

        FILE *f = fopen("lastgen.gen", "w+");
        int gs = evo->getBrain(0)->getGenome().size;
        fwrite(&gs, sizeof(int), 1, f);
        fwrite(&population, sizeof(int), 1, f);

        for(int i=0;i<population;i++) {
            genome_t g = evo->getBrain(i)->getGenome();
            fwrite(g.genome, 1, g.size, f);
        }

        fwrite(randomlist, sizeof(int), RANDOMLIST_SIZE, f);

        fclose(f);

        float podfit = evo->evolve(podio, sumFactor, mulFactor, replaceFactor, sumsize, mulsize);
        printf("Generation %i: %f, %.10f, %.10f\n", generation, evo->results[evo->getBestBrainIndex()], podfit, mean);
        generation ++;
    }
}

void gameThread() {
    while(1) {
        if(bestGenome.genome != NULL) {
            Brain *b = new Brain(gridSize*gridSize+1, 4, hiddenLayers, neuronsPerLayer);
            b->setGenome(bestGenome);
            brainPlay(b, 100, true);
        }
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

    evo = new Evolution(population, gridSize*gridSize+1, 4, hiddenLayers, neuronsPerLayer);

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
    std::thread thr2(gameThread);
    


    glutMainLoop();                 // Enter the infinite event-processing loop
    
    return 0;
}