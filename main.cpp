#include <GL/glut.h>  // GLUT, include glu.h and gl.h
#include <stdio.h>
#include <thread>
#include <unistd.h>

#include "Snake.hpp"
#include "Brain.hpp"

Snake *s;
int gridSize = 20;
int refreshMills = 30; // refresh interval in milliseconds
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

    if(s != NULL) {
        for(int i=0;i<s->getSnakeSize();i++) {
            s->getSnakeCoordinate(i, &sx, &sy);
            drawSquare(gridSize, sx, sy, 1, 0, 0);
        }

        s->getCookieCoordinate(&sx, &sy);
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
    
}
 
/* Callback handler for special-key event */
void specialKeys(int key, int x, int y) {
    if(key == 100 && s->getDirection() != DIR_RIGHT) {
        snakeDir = DIR_LEFT;
    }
    if(key == 101 && s->getDirection() != DIR_DOWN) {
        snakeDir = DIR_UP;
    }
    if(key == 102 && s->getDirection() != DIR_LEFT) {
        snakeDir = DIR_RIGHT;
    }
    if(key == 103 && s->getDirection() != DIR_UP) {
        snakeDir = DIR_DOWN;
    }
}

void mainThread() {
    while(1) {
        while(s->isAlive()) {
            s->step(snakeDir);

            usleep(200000);
        }
        s = new Snake(gridSize);
        snakeDir = s->getDirection();
    }
}

/* Main function: GLUT runs as a console application starting at main() */
int main(int argc, char** argv) {
    Brain *b = new Brain(100, 4, 2, 50);
    genome_t genome = b->getGenome();
    printf("%i\n", genome.size);

    /*
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


    s = new Snake(gridSize);
    std::thread thr(mainThread);


    glutMainLoop();                 // Enter the infinite event-processing loop
    */
    return 0;
}