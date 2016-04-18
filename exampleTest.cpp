#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <iostream>
#include <string>
#include <vector>
#include <CIEDE2000.h>
#include <string>
#include <sstream>
#include <cstring>
#include <ColorConverter.h>
#include <fstream>

#ifdef __APPLE__
  #include <GLUT/glut.h>
#else
  #include <GL/glut.h>
#endif

unsigned int mapSize;
GLfloat colors[2048][3];
CIEDE2000::LAB lab1, lab2;
float fLab1[3],fLab2[3];
float fRGB1[3],fRGB2[3];

GLint mouse_button = -1;
GLint mouse_state = -1;
void Find_Nearest(int, int);

GLdouble wx[4], wy[4], wz[4];
int ni, nj;
int in_box = -1;

float xorig = 100;
float yorig = 100;
float yRep= 450;

int height = 75;
int length = 400;

int width;

GLfloat topPoints[4] = {150. , 200., 400., 450.};
GLfloat connector[4] = {150. , 250., 350., 450.};
int off_x;
int SELECTED = GL_FALSE;

void readFile();
void drawMap();

void draw_interface()
{
  int i;

    drawMap();

  glColor3f(0.5, 0.5, 0.5);
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
void text_Output(char const *hold = "Lorem", int ii=0, int vOff=30, int x=0)
{
  int  jj;
  glColor3f(0.5, 0, 0.);

  glRasterPos2f(connector[ii] + 3+x, yRep + vOff);
  for(jj = 0; jj < strlen(hold); jj++)
    {
      glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, hold[jj]);
    }
}

void init()
{
    readFile();
}
void readFile(){
    std::ifstream infile("Colors.txt");
    int r,g,b;
    mapSize=0;
    for(mapSize=0;mapSize<2048&&infile >> r >> g >> b;mapSize++){
        colors[mapSize][0]=1.0*r/255;
        colors[mapSize][1]=1.0*g/255;
        colors[mapSize][2]=1.0*b/255;
        printf("Reading color %d: %d %d %d\n",mapSize,r,g,b);
    }
    infile.close();

}
void drawColorRect(int x, int y,int x2,int y2, int index){
    glColor3f(colors[index][0],colors[index][1],colors[index][2]);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x, y2);
    glVertex2f(x2, y2);
    glVertex2f(x2, y);
    glEnd();
}

float getNormalizedDistance(int rect1,int rect2){
    //return sqrt(pow_(colors[rect1]));
    return 0.0;
}

float colorComparison(int rect1,int rect2){
    char temp[256];
    RGB_to_LAB(colors[rect1],fLab1);
    RGB_to_LAB(colors[rect2],fLab2);
    lab1={fLab1[0],fLab1[1],fLab1[2]};
    lab2={fLab2[0],fLab2[1],fLab2[2]};
    double result=CIEDE2000::CIEDE2000(lab1,lab2);
    result = roundf(result * 10000) / 10000;
    return result;
//    std::ostringstream strs;
//    strs << result;
//    std::string str = "Distance:"+strs.str();
//    strcpy(temp, str.c_str());
//    unsigned int i;
//    for(i=0;i<strlen(temp);i++){
//        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10,temp[i]);
//    }
}

void drawMap(){
    float increment=1.0*length/mapSize;
    unsigned int i;
    for(i=0;i<mapSize;i++){
        drawColorRect(xorig+increment*i,yorig,xorig+increment*(i+1),yorig+height,i);
    }
}

//Gets an x value and will return the index of the color at the location
int getColorIndex(int x){
    float increment=1.0*length/mapSize;
    unsigned int i;
    for(i=0;i<mapSize;i++){
        if(x<xorig+increment*(i+1))
            return i;
    }
}

void drawColorInfo(){
    std::string hold;
    std::ostringstream strs;
    int index;
    unsigned int i;
    for(i=0;i<4;i+=2){
        drawColorRect(connector[i] + 3,yRep+3,connector[i] + 3+47,yRep+10+3,getColorIndex((int)topPoints[i]));
        drawColorRect(connector[i] + 3+47,yRep+3, connector[i] + 3+94,yRep+10+3,getColorIndex((int)topPoints[i+1]));

        hold = "";
        strs.str("");
        strs.clear();
        index=getColorIndex((int)topPoints[i]);
        strs << roundf(colors[index][0]*255)<<" "<<roundf(colors[index][1]*255)<<" "<<roundf(colors[index][2]*255);
        std::string str = hold+strs.str();
        text_Output(str.c_str(), i, 23);

        hold = "";
        strs.str("");
        strs.clear();
        index=getColorIndex((int)topPoints[i+1]);
        strs << roundf(colors[index][0]*255)<<" "<<roundf(colors[index][1]*255)<<" "<<roundf(colors[index][2]*255);
        str = hold+strs.str();
        text_Output(str.c_str(), i, 23,47);

        hold = "CIEDE2000: ";
        strs.str("");
        strs.clear();
        strs << colorComparison(getColorIndex((int)topPoints[i]),getColorIndex((int)topPoints[i+1]));
        str = hold+strs.str();
        text_Output(str.c_str(), i, 33);

        hold = "Distance: ";
        strs.str("");
        strs.clear();
        strs << getColorIndex((int)topPoints[i+1])-getColorIndex((int)topPoints[i]);
        str = hold+strs.str();
        text_Output(str.c_str(), i, 43);
    }
}

void display(void)
{
    glClear (GL_COLOR_BUFFER_BIT);
    draw_interface();
    draw_boxes();
    drawColorInfo();
    glFlush();
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

void updateDisplay(int value){
    glutPostRedisplay();
    //Update logic here
    glutTimerFunc(40, updateDisplay, 0);
}

int main(int argc, char **argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB);
   glutInitWindowSize (600, 600);
   glutInitWindowPosition (100, 100);
   glutCreateWindow (argv[0]);
   init();
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutMouseFunc(mouse);
   glutMotionFunc(motion);

   glutKeyboardFunc (keyboard);
   //glutTimerFunc(40, updateDisplay, 0);
   glutMainLoop();
   return 0;
}
