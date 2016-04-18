#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>


#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

GLint mouse_button = -1;
GLint mouse_state = -1;
void Find_Nearest(int, int);

GLdouble wx[4], wy[4], wz[4];
int ni, nj;
int in_box = -1;

float xorig = 100;
float yorig = 100;
float yRep= 450;

int height = 100;
int length = 400;

int width;

GLfloat topPoints[4] = {150. , 200., 400., 450.};
GLfloat connector[4] = {150. , 250., 350., 450.};
int off_x;
int SELECTED = GL_FALSE;

void draw_interface()
{
  int i;

  glColor3f(1., 1., 1.);
  glBegin(GL_QUADS);
  glVertex2f(xorig, yorig);
  glVertex2f(xorig, yorig + height);
  glVertex2f(xorig + length, yorig + height);
  glVertex2f(xorig + length, yorig);
  glEnd();

  glColor3f(0., 0., 0.);
  glLineWidth(5.);

  glBegin(GL_LINES);
  for(i=0;i<4;i++)
    {
      glVertex2f(topPoints[i], yorig);
      glVertex2f(topPoints[i], yorig + height);
    }
  glEnd();

  if(SELECTED)
    {
      glLineWidth(5.0);
      glColor3f(1, 0, 0);
      glBegin(GL_LINES);
      glVertex2f(topPoints[in_box] + 5, yorig);
      glVertex2f(topPoints[in_box] + 5, yorig + height);

      glVertex2f(topPoints[in_box] - 5, yorig);
      glVertex2f(topPoints[in_box] - 5, yorig + height);
      glEnd();
    }

}
void draw_boxes(void)
{
  int i;
  glColor3f(1., 1., 1.);
  glBegin(GL_QUADS);
  glVertex2f(connector[0], yRep);
  glVertex2f(connector[0], yRep + height);
  glVertex2f(connector[1], yRep + height);
  glVertex2f(connector[1], yRep);

  glVertex2f(connector[2], yRep);
  glVertex2f(connector[2], yRep + height);
  glVertex2f(connector[3], yRep + height);
  glVertex2f(connector[3], yRep);
  glEnd();

  glLineWidth(3.);
  glBegin(GL_LINES);
  for(i=0;i<4;i++)
    {
      glVertex2f(topPoints[i], yorig + height);
      glVertex2f(connector[i], yRep);
    }
  glEnd();

}
void text_Output(char const *hold = "Lorem", int ii=0, int vOff=30)
{
  int  jj;
  glColor3f(0.5, 0, 0.);

  glRasterPos2f(connector[ii] + 3, yRep + vOff);
  for(jj = 0; jj < strlen(hold); jj++)
    {
      glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, hold[jj]);
    }
}
void display(void)
{
  char *point;
  std::string hold;
  int i;

  glClear (GL_COLOR_BUFFER_BIT);
  draw_interface();
  draw_boxes();

  for(i=0;i<4;i+=2){
    hold = "Pos: ";
    asprintf(&point, "%i", (int)topPoints[i]);
    hold.append(point);
    text_Output(hold.c_str(), i, 30);

    hold = "Pos: ";
    asprintf(&point, "%i", (int)topPoints[i+1]);
    hold.append(point);
    text_Output(hold.c_str(), i, 70);
  }

  glutPostRedisplay();
  glFlush();
}

void reshape(int w, int h)
{
  width=w;
  glViewport(0, 0, (GLsizei) w, (GLsizei) h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0, 600.0, 600.0, 0.0, -10.0, 10.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}
void move_TopPoint(int button, int x, int y)
{
  int i;

  switch (button) {
  case GLUT_LEFT_BUTTON:
    if(in_box != -1) {
      if(in_box == 0){
        if(((x - off_x)  > 100) && ((x - off_x) < topPoints[1]))
          topPoints[in_box] = (float)(x-off_x);
      }
      else if(in_box == 1){
        if((x - off_x)  > topPoints[0] && ((x - off_x) < topPoints[2]))
          topPoints[in_box] = (float)(x-off_x);
      }
      else if(in_box == 2){
        if(((x - off_x) > topPoints[1]) && (x - off_x)  < topPoints[3])
          topPoints[in_box] = (float)(x-off_x);
      }
      else if(in_box == 3){
        if(((x - off_x) < 500)&&((x - off_x) > topPoints[2]))
          topPoints[in_box] = (float)(x-off_x);
      }
      display();
    };
  default:
    break;
  }
}
void mouse(int button, int state, int x, int y)
{
  int i;
  mouse_state = state;
  mouse_button = button;


  switch (button) {
  case GLUT_LEFT_BUTTON:
    if (mouse_state == GLUT_UP) {
      if(in_box != -1) {
        if(in_box == 0){
          if(((x - off_x)  > 100) && ((x - off_x) < topPoints[1]))
            topPoints[in_box] = (float)(x-off_x);
        }
        else if(in_box == 1){
          if((x - off_x)  > topPoints[0] && ((x - off_x) < topPoints[2]))
            topPoints[in_box] = (float)(x-off_x);
        }
        else if(in_box == 2){
          if(((x - off_x) > topPoints[1]) && (x - off_x)  < topPoints[3])
            topPoints[in_box] = (float)(x-off_x);
        }
        else if(in_box == 3){
          if(((x - off_x) < 500)&&((x - off_x) > topPoints[2]))
            topPoints[in_box] = (float)(x-off_x);
        }

      }
      in_box = -1;
    }
    if (mouse_state == GLUT_DOWN) {

      in_box = -1;
      for(i=0; i<4; i++) {
        if( (x > (int)(topPoints[i] - 20.))
            && (y > (int)(yorig -  20.))
            && (x < (int)(topPoints[i] +  20.))
            && (y < (int)(yorig +  (20. + height))))
          {
            in_box = i;
            off_x = x - (int)(topPoints[i]);
            break;
          }
      }
    }
    break;

  default:
    break;
  }


  glFlush();
  display();

}
void Find_Nearest(int x, int y)
{
  int i, j;
  GLint viewport[4];
  GLdouble mvmatrix[16], projmatrix[16];
  GLdouble td, dd;

  glGetIntegerv(GL_VIEWPORT, viewport);
  glGetDoublev(GL_MODELVIEW_MATRIX, mvmatrix);
  glGetDoublev(GL_PROJECTION_MATRIX, projmatrix);

  for(i=0; i<4; i++)
    {
      gluProject((GLdouble)topPoints[i],
		 (GLdouble)yorig,
		 0,
		 mvmatrix, projmatrix, viewport,
		 &(wx[i]), &(wy[i]), &(wz[i]));
      wy[i]=(GLdouble)width-wy[i];

    };

  dd=9e+9;
  ni=0;
  for(i=0; i<4; i++)
    {
      td=((GLdouble)x -wx[i]) * ((GLdouble)x-wx[i]) +
        ((GLdouble)y -wy[i]) * ((GLdouble)y-wy[i]);
      if(td<dd) {
        dd=td;
        ni=i;
	//printf("ni=%d\n", ni);                                                     
      };
    }

  if(dd<50.) {
    SELECTED=GL_TRUE;
  }
}
void  motion(int x, int y)
{

  {
    if(!SELECTED)
      {
        Find_Nearest(x, y);
      }
    if(SELECTED) {
      move_TopPoint(mouse_button, x, y);
      if(in_box != -1) {
        if(in_box == 0){
          if(((x - off_x)  > 100)&&((x - off_x) < topPoints[1]))
            topPoints[in_box] = (float)(x-off_x);
	}

	else if(in_box == 1){
          if((x - off_x)  > topPoints[0] && ((x - off_x) < topPoints[2]))
            topPoints[in_box] = (float)(x-off_x);
        }
        else if(in_box == 2){
          if(((x - off_x) > topPoints[1]) && (x - off_x)  < topPoints[3])
            topPoints[in_box] = (float)(x-off_x);
	}

	else if(in_box == 3){
          if(((x - off_x) < 500)&&((x - off_x) > topPoints[2]))
            topPoints[in_box] = (float)(x-off_x);
        }

      }
    }
  }
}
void keyboard(unsigned char key, int x, int y)
{
  switch (key) {
  case 27:
    exit(0);
    break;
  }
}
int main(int argc, char **argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB);
  glutInitWindowSize (600, 600);
  glutInitWindowPosition (100, 100);
  glutCreateWindow (argv[0]);
  
  //init();                                                                         
  
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutMouseFunc(mouse);
  glutMotionFunc(motion);
  
  glutKeyboardFunc (keyboard);
  glutMainLoop();
  
  return 0;
}
