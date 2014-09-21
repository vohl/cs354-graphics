//Compile with g++ object3D.cpp -o object3D -lglut -lGL -lGLU -lGLEW
#include <GL/glut.h>  
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>

void init(void); 
void display(void);
void reshape(GLsizei width, GLsizei height);
 
int main(int argc, char** argv) {
   glutInit(&argc, argv);            
   glutInitDisplayMode(GLUT_DOUBLE); 
   glutInitWindowSize(512, 512);   
   glutInitWindowPosition(0, 0); 
   glutCreateWindow(argv[0]);          
   glutDisplayFunc(display);       
   glutReshapeFunc(reshape);       
   init();                     
   glutMainLoop();                 
   return 0;
}

void init() {
   glClearColor(0.3, 0.3, 0.3, 0.0); 
   glClearDepth(1.0);                 
   glEnable(GL_DEPTH_TEST);   
   glDepthFunc(GL_LEQUAL);    
}

void reshape(GLsizei width, GLsizei height) {  // GLsizei for non-negative integer
   // Compute aspect ratio of the new window
   if (height == 0) height = 1;                // To prevent divide by 0
   GLfloat aspect = (GLfloat)width / (GLfloat)height;
 
   // Set the viewport to cover the new window
   glViewport(0, 0, width, height);
 
   // Set the aspect ratio of the clipping volume to match the viewport
   glMatrixMode(GL_PROJECTION);  // To operate on the Projection matrix
   glLoadIdentity();             // Reset
   // Enable perspective projection with fovy, aspect, zNear and zFar
   gluPerspective(45.0f, aspect, 0.1f, 100.0f);
}

void display() {
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
   glMatrixMode(GL_MODELVIEW);     
 
   glLoadIdentity();                  // Reset the model-view matrix
   glTranslatef(0.0, 0.0, -6.0);  // Move left and into the screen
   glRotatef(45.0, 0.0, 1.0, 0.5);
 
   glBegin(GL_TRIANGLES);          
      
      glColor3f(1.0, 0.0, 0.0);     
      glVertex3f(0.0, -1.0, 0.0);
      glColor3f(0.0, 1.0, 0.0);     
      glVertex3f(1.0, 1.0, -1.0);
      glColor3f(0.0, 0.0, 1.0);     
      glVertex3f(-1.0, 1.0, -1.0);
 
      
      glColor3f(1.0, 0.0, 0.0);     
      glVertex3f(0.0, -1.0, 0.0);
      glColor3f(0.0, 0.0, 1.0);     
      glVertex3f(-1.0, 1.0, -1.0);
      glColor3f(0.0, 1.0, 0.0);     
      glVertex3f(-1.0, 1.0, 1.0);
 
      
      glColor3f(1.0, 0.0, 0.0);     
      glVertex3f(0.0, -1.0, 0.0);
      glColor3f(0.0, 1.0, 0.0);     
      glVertex3f(-1.0, 1.0, 1.0);
      glColor3f(0.0, 0.0, 1.0);     
      glVertex3f(1.0, 1.0, 1.0);
 
      
      glColor3f(1.0, 0.0, 0.0);       
      glVertex3f(0.0, -1.0, 0.0);
      glColor3f(0.0, 0.0, 1.0);       
      glVertex3f(1.0, 1.0, 1.0);
      glColor3f(0.0, 1.0, 0.0);       
      glVertex3f(1.0, 1.0, -1.0);
   glEnd();   
 
   glutSwapBuffers();  
}