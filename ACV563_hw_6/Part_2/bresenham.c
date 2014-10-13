#ifdef __APPLE__  // include Mac OS X verions of headers
#  include <GLUT/glut.h>
#else // non-Mac OS X operating systems
#  include <GL/freeglut.h>
#  include <GL/freeglut_ext.h>
#endif  // __APPLE__

#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>

void myinit(void);
void display(void);
void draw_pixel(int, int);
void bres(int, int, int, int);
void mouse(int, int, int, int);
void keyboard(unsigned char, int, int);

int startX, startY, stopX, stopY;
int pointnum;

int main(int argc, char** argv) {
    /* standard GLUT initialization */
    glutInit(&argc,argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB); /* default, not needed */
    glutInitWindowSize(500,500); /* 500x500 pixel window */
    glutInitWindowPosition(0,0); /* place window top left on display */
    glutCreateWindow("Bresenham's Algorithm"); /* window title */

    glClearColor(1.0, 1.0, 1.0, 0.0);

    glutDisplayFunc(display); /* display callback invoked when window opened */
    glutMouseFunc(mouse);
    glutKeyboardFunc(keyboard);

    myinit();

    glutMainLoop(); /* enter event loop */
    return(0);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    bres(startX, startY, stopX, stopY);
    glutSwapBuffers();
}

void draw_pixel(int ix, int iy) {
    glBegin(GL_POINTS);
    glVertex2i( ix, iy);
    glEnd();
}

void bres(int x1,int y1,int x2,int y2) {
    int dx, dy, i, e;
    int incx, incy, inc1, inc2;
    int x,y;
    
    dx = x2 - x1;
    dy = y2 - y1;
    
    incx = 1;
    incy = 1;
    if(dx < 0){ 
        dx = -dx;
        incx = -1;
    }
    if(dy < 0){
        dy = -dy;
        incy = -1;
    }  

    if(dx > dy) {
        x=x1;
        y=y1;
        draw_pixel(x,y);
        e = 2*dy - dx;
        inc1 = 2*( dy - dx);
        inc2 = 2*dy;
        for(i = 0; i < dx; i++) {
            if(e >= 0) {
                y += incy;
                e += inc1;
            }
            else{
                e += inc2;
            }
            x += incx;
            draw_pixel(x,y);
        }
    } 
    else {
        x=x1;
        y=y1;
        draw_pixel(x,y);
        e = 2*dx - dy;
        inc1 = 2*( dx - dy);
        inc2 = 2*dx;
        for(i = 0; i < dy; i++) {
            if(e >= 0) {
                x += incx;
                e += inc1;
            }
            else {
                e += inc2;
            }
            y += incy;
            draw_pixel(x,y);
        }
    }
}

void mouse(int button, int state, int x, int y){
    switch(button){
        case GLUT_LEFT_BUTTON:
            if(state == GLUT_DOWN){
                printf("x = %d, y = %d\n", x, 500 - y);
                if(pointnum == 0){
                    startX = x;
                    startY = 500 - y;
                    pointnum = 1;
                }
                else{
                    stopX = x;
                    stopY = 500 - y;
                    pointnum = 0;
                    //printf("startX = %d startY = %d\nstopX = %d stopY = %d\n", startX, startY, stopX, stopY);
                    glutPostRedisplay();
                }
            }   
            break;   
        case GLUT_RIGHT_BUTTON:
            if(state == GLUT_DOWN)
                exit(0);
            break;
        default:
            break;
    }
}

void keyboard(unsigned char key, int x, int y){
    switch(key){
        case 'q':
        case 'Q':
            exit(0);
            break;
        default:
            break;
    }
}

void myinit(){
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glColor3f(1.0, 0.0, 0.0);
    glPointSize(1.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, 499.0, 0.0, 499.0);
}
