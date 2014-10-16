#include <math.h>
#include <stdlib.h>

#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>

#include <sys/time.h>

#define X 0
#define Y 1
#define Z 2

static struct timeval last_idle_time;

void display();

int axis = 3;

double y=0;
double x=0;
double z=0;

int lightnum = 0;

GLfloat mat_specular[] = {1.0, 1.0, 1.0, 1.0};
GLfloat mat_shininess[] = {70.0};
GLfloat light_positionA[] = {0.0, 1.0, -1.0, 0.0};
GLfloat light_positionB[] = {1.0, -1.0, -1.0, 0.0};
GLfloat light_positionC[] = {-1.0, -1.0, -1.0, 0.0};

void Init(void);
void display(void);
void AnimateScene(void);
void keyInput(unsigned char key, int x, int y);

int main(int argc, char* argv[]){
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutCreateWindow("Multiple Lights/Rotated");
    Init();
    glutDisplayFunc(display);
    glutIdleFunc(AnimateScene);
    glutKeyboardFunc(keyInput);
    glutMainLoop();
    return 0;
}

void Init(){
    glClearColor(0.3, 0.3, 0.3, 0.0);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glShadeModel(GL_FLAT);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    glLightfv(GL_LIGHT0, GL_POSITION, light_positionA);
    glColorMaterial(GL_FRONT, GL_DIFFUSE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
}

void display(){
    
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    
    glPushMatrix();
        glRotatef(x, 1, 0, 0);
        glRotatef(y, 0, 1, 0);
        glRotatef(z, 0, 0, 1);
        glColor3f(1.0, 0.0, 0.0);
        glutSolidCube(0.5);
    glPopMatrix();


    glFlush();
    glutSwapBuffers();   

}

void AnimateScene(void) {
    float dt;
    if(lightnum < 2){
        ++lightnum;
    }
    else{
        lightnum = 0;
    }

    // Figure out time elapsed since last call to idle function
    struct timeval time_now;
    gettimeofday(&time_now, NULL);
    dt = (float)(time_now.tv_sec  - last_idle_time.tv_sec) +
    1.0e-6*(time_now.tv_usec - last_idle_time.tv_usec); 
    // Animate the scene by Increment 2 degrees each call
    if(axis == X) x+=2;
    if(axis == Y) y+=2;
    if(axis == Z) z+=2;
    // Save time_now for next time
    last_idle_time = time_now;
    
    if(lightnum == 0){
        glLightfv(GL_LIGHT0, GL_POSITION, light_positionA);
        //glEnable(GL_LIGHT0);
    }
    else if(lightnum == 1){
        glLightfv(GL_LIGHT0, GL_POSITION, light_positionB);
        //glEnable(GL_LIGHT0);
    }
    else{
        glLightfv(GL_LIGHT0, GL_POSITION, light_positionC);
        //glEnable(GL_LIGHT0);
    }
    // Force redraw
    glutPostRedisplay();
}

void keyInput(unsigned char key, int x, int y)
{
    switch(key)
    {
        case 'x':
            axis = X;
            break;
        case 'y':
            axis = Y;
            break;
        case 'z':
            axis = Z;
            break;
        case 27:
        case 'q':
        case 'Q':
            exit(0);
            break;
        default:
            break;
    }
}