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

#define numFiles 12
unsigned int maps[2];
unsigned int mapSize[numFiles];
GLfloat colors[numFiles][2048][3];
CIEDE2000::LAB lab1, lab2;
float fLab1[3],fLab2[3];
float fRGB1[3],fRGB2[3];

GLint mouse_button = -1;
GLint mouse_state = -1;
void Find_Nearest(int, int);

GLdouble wx[4], wy[4], wz[4];
int ni, nj;
int in_box = -1;

float xorig = 50;
float yorig = 125;
float yRep= 250;

int height = 75;
int length = 400;

int width;
char files[numFiles][32]={"rainbow.txt","redgreen.txt","greenblue.txt","redblue.txt","orangeblue.txt","yellowpurple.txt","whitered.txt","whitegreen.txt","whiteblue.txt","gray.txt","separate.txt","rainbowseparate.txt"};
GLfloat topPoints[2][4] = {{100. , 225., 275., 400.},{100. , 225., 275., 400.}};
GLfloat connector[2][4] = {{100. , 225., 275., 400.},{100. , 225., 275., 400.}};
int comparison[2]={0,0};
int image [400][400];
int off_x;
int SELECTED = GL_FALSE;

void readColorsData(char fileName[], int cindex);
void readTopoData();
void drawMap(int cindex,int panelNum);
int getColorIndex(int x, int cindex);
void drawColorRect(int x, int y, int x2, int y2, int index, int cindex);
void setColor(int index, int cindex);
int getColorHeight(float h, int cindex);


void drawMapButtons(){
    unsigned int i,j;
    for(i=0;i<numFiles;i++){
        for(j=0;j<50;j++){
            drawColorRect(475+55*i+j,10,475+55*i+j+1,60,getColorHeight(.02*j,i),i);
        }
    }
}

int testMapButtons(int x, int y){
    unsigned int i;
    if(y>=10&&y<=60){
        for(i=0;i<numFiles;i++){
            if(x>=475+55*i&&x<=475+55*i+50)
                return i;
        }
    }
    return -1;
}

void drawTopoMap(int x, int y,int cindex){
    int maximium=255;
    unsigned int i,j;
    for(i=0;i<400;i++){
        for(j=0;j<400;j++){
            drawColorRect(x+i,y+j,x+i+1,y+j+1,getColorHeight(1.0*image[i][j]/maximium,cindex),cindex);
        }
    }
}
void draw_interface(int cindex, int panelNum)
{
  int i;

  drawMap(cindex,panelNum);
  glColor3f(0.5, 0.5, 0.5);
  glLineWidth(5.);
  float ytemp=yorig+panelNum*300;

  for(i=0;i<4;i++)
    {
      glColor3f(0.5, 0.5, 0.5);
      glBegin(GL_LINES);
      glVertex2f(topPoints[panelNum][i], ytemp+ height/2);
      glVertex2f(topPoints[panelNum][i], ytemp + height);
      glEnd();
      drawColorRect(topPoints[panelNum][i]-1,ytemp+height/2,topPoints[panelNum][i],ytemp+height,getColorIndex(topPoints[panelNum][i],cindex),cindex);
    }

  if(in_box>=4*panelNum&&in_box<4*(panelNum+1))
    {
      glLineWidth(5.0);
      glColor3f(1, 0, 0);
      glBegin(GL_LINES);
      glVertex2f(topPoints[panelNum][in_box%4] + 5, ytemp + height/2);
      glVertex2f(topPoints[panelNum][in_box%4] + 5, ytemp + height);

      glVertex2f(topPoints[panelNum][in_box%4] - 5, ytemp + height/2);
      glVertex2f(topPoints[panelNum][in_box%4] - 5, ytemp + height);
      glEnd();
    }

}

void draw_boxes(int cindex,int panelNum)
{
  int i;
  float ytemp=yRep+panelNum*300;
  glColor3f(1., 1., 1.);
  glBegin(GL_QUADS);
  glVertex2f(connector[panelNum][0], ytemp);
  glVertex2f(connector[panelNum][0], ytemp + height);
  glVertex2f(connector[panelNum][1], ytemp + height);
  glVertex2f(connector[panelNum][1], ytemp);

  glVertex2f(connector[panelNum][2], ytemp);
  glVertex2f(connector[panelNum][2], ytemp + height);
  glVertex2f(connector[panelNum][3], ytemp + height);
  glVertex2f(connector[panelNum][3], ytemp);
  glEnd();

  glLineWidth(3.);
  glBegin(GL_LINES);
  for(i=0;i<4;i++)
    {
      setColor(getColorIndex(topPoints[panelNum][i],cindex),cindex);
      glVertex2f(topPoints[panelNum][i], yorig + height+panelNum*300);
      glVertex2f(connector[panelNum][i], ytemp);
    }
  glEnd();

}
void text_Output(char const *hold = "Lorem", int ii=0, int vOff=30, int x=0,int cindex=0,int panelNum=0)
{
  int  jj;
  glColor3f(0.5, 0, 0.);
  float ytemp=yRep+panelNum*300;

  glRasterPos2f(connector[panelNum][ii] + 3+x, ytemp + vOff);
  for(jj = 0; jj < strlen(hold); jj++)
    {
      glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, hold[jj]);
    }
}

void init()
{
    maps[0]=0;
    maps[1]=2;
    unsigned int i;
    for(i=0;i<numFiles;i++)
        readColorsData(files[i],i);
    readTopoData();
}
void readTopoData(){
    std::fstream file ("colorado_elev.vit", std::ios::in|std::ios::binary);

  if (file.is_open())
  {
    char byte;
    file.seekg (268, std::ios::beg); //start reading after 268 byte

	for(int y=0;y<400;y++){
		for(int x=0;x<400;x++){
			file.read (&byte, 1);
			image[x][y]=(unsigned char)byte;
		}
	}
    file.close();
  }
  else std::cout << "Unable to open file";
}
void readColorsData(char fileName[],int cindex){
    std::ifstream infile(fileName);
    int r,g,b;
    mapSize[cindex]=0;
    for(mapSize[cindex]=0;mapSize[cindex]<2048&&infile >> r >> g >> b;mapSize[cindex]++){
        colors[cindex][mapSize[cindex]][0]=1.0*r/255;
        colors[cindex][mapSize[cindex]][1]=1.0*g/255;
        colors[cindex][mapSize[cindex]][2]=1.0*b/255;
        printf("Reading color %d: %d %d %d\n",mapSize[cindex],r,g,b);
    }
    infile.close();

}
void drawColorRect(int x, int y,int x2,int y2, int index, int cindex){
    setColor(index,cindex);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x, y2);
    glVertex2f(x2, y2);
    glVertex2f(x2, y);
    glEnd();
}

void setColor(int index, int cindex){
    glColor3f(colors[cindex][index][0],colors[cindex][index][1],colors[cindex][index][2]);
}

float getNormalizedDistance(int rect1,int rect2){
    //return sqrt(pow_(colors[rect1]));
    return 0.0;
}

float colorComparison(int rect1,int rect2,int cindex){
    char temp[256];
    RGB_to_LAB(colors[cindex][rect1],fLab1);
    RGB_to_LAB(colors[cindex][rect2],fLab2);
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

void drawComparisonGraph(int index,int cindex,int panelNum){
    float increment=1.0*length/mapSize[cindex];
    unsigned int i;
    float tempHeight;
    for(i=0;i<mapSize[cindex];i++){
        tempHeight=colorComparison(index,i,cindex);
        drawColorRect(xorig+increment*i,yorig-tempHeight+panelNum*300,xorig+increment*(i+1),yorig+panelNum*300,index,cindex);
    }
}

void drawMap(int cindex,int panelNum){
    float increment=1.0*length/mapSize[cindex];
    unsigned int i;
    for(i=0;i<mapSize[cindex];i++){
        drawColorRect(xorig+increment*i,yorig+panelNum*300,xorig+increment*(i+1),yorig+height+panelNum*300,i,cindex);
    }
}

//Gets a hard x value and will return the index of the color at the location
int getColorIndex(int x,int cindex){
    float increment=1.0*length/mapSize[cindex];
    unsigned int i;
    for(i=0;i<mapSize[cindex];i++){
        if(x<xorig+increment*(i+1))
            return i;
    }
    return 0;
}

int getColorHeight(float h,int cindex){
    if(h<0||h>1.0)
        return 0;
    return (int)(1.0*(mapSize[cindex]-1)*h);
}

void drawColorInfo(int cindex,int panelNum){
    std::string hold;
    std::ostringstream strs;
    int index;
    unsigned int i;
    unsigned int panelLength=connector[panelNum][1]-connector[panelNum][0];
    float ytemp=yRep+panelNum*300;
    for(i=0;i<4;i+=2){
        drawColorRect(connector[panelNum][i] + 3,ytemp+3,connector[panelNum][i] + panelLength/2,ytemp+10+3,getColorIndex((int)topPoints[panelNum][i],cindex),cindex);
        drawColorRect(connector[panelNum][i] + panelLength/2,ytemp+3, connector[panelNum][i] + panelLength-3,ytemp+10+3,getColorIndex((int)topPoints[panelNum][i+1],cindex),cindex);

        //Print first rgb
        hold = "";
        strs.str("");
        strs.clear();
        index=getColorIndex((int)topPoints[panelNum][i],cindex);
        strs << roundf(colors[cindex][index][0]*255)<<" "<<roundf(colors[cindex][index][1]*255)<<" "<<roundf(colors[cindex][index][2]*255);
        std::string str = hold+strs.str();
        text_Output(str.c_str(), i, 23,0,cindex,panelNum);

        //Print first lab
        RGB_to_LAB(colors[panelNum][index],fLab1);
        hold = "";
        strs.str("");
        strs.clear();
        strs << roundf(fLab1[0])<<" "<<roundf(fLab1[1])<<" "<<roundf(fLab1[2]);
        str = hold+strs.str();
        text_Output(str.c_str(), i, 33,0,cindex,panelNum);

        //Print second rgb
        hold = "";
        strs.str("");
        strs.clear();
        index=getColorIndex((int)topPoints[panelNum][i+1],cindex);
        strs << roundf(colors[cindex][index][0]*255)<<" "<<roundf(colors[cindex][index][1]*255)<<" "<<roundf(colors[cindex][index][2]*255);
        str = hold+strs.str();
        text_Output(str.c_str(), i, 23,panelLength/2,cindex,panelNum);

        RGB_to_LAB(colors[panelNum][index],fLab2);
        hold = "";
        strs.str("");
        strs.clear();
        index=getColorIndex((int)topPoints[panelNum][i+1],0);
        strs << roundf(fLab2[0])<<" "<<roundf(fLab2[1])<<" "<<roundf(fLab2[2]);
        str = hold+strs.str();
        text_Output(str.c_str(), i, 33,panelLength/2,cindex,panelNum);

        hold = "CIEDE2000: ";
        strs.str("");
        strs.clear();
        strs << colorComparison(getColorIndex((int)topPoints[panelNum][i],0),getColorIndex((int)topPoints[panelNum][i+1],0),0);
        str = hold+strs.str();
        text_Output(str.c_str(), i, 43,0,cindex,panelNum);

        hold = "Distance: ";
        strs.str("");
        strs.clear();
        strs << getColorIndex((int)topPoints[panelNum][i+1],0)-getColorIndex((int)topPoints[panelNum][i],0);
        str = hold+strs.str();
        text_Output(str.c_str(), i, 53,0,cindex,panelNum);
    }
}

void display(void)
{
    glClearColor(.5f, .5f, .5f, 1.0f);
    glClear (GL_COLOR_BUFFER_BIT);
    draw_interface(maps[0],0);
    draw_boxes(maps[0],0);
    drawColorInfo(maps[0],0);
    drawComparisonGraph(comparison[0],maps[0],0);
    draw_interface(maps[1],1);
    draw_boxes(maps[1],1);
    drawColorInfo(maps[1],1);
    drawComparisonGraph(comparison[1],maps[1],1);
    drawTopoMap(475,100,maps[0]);
    drawTopoMap(880,100,maps[1]);
    drawMapButtons();
    glFlush();
}

void move_TopPoint(int button, int x, int y)
{
  int i;

  switch (button) {
  case GLUT_LEFT_BUTTON:
    if(in_box != -1) {
      if(in_box == 0){
        if(((x - off_x)  > xorig) && ((x - off_x) < topPoints[0][1]))
          topPoints[0][in_box] = (float)(x-off_x);
      }
      else if(in_box == 1){
        if((x - off_x)  > topPoints[0][0] && ((x - off_x) < topPoints[0][2]))
          topPoints[0][in_box] = (float)(x-off_x);
      }
      else if(in_box == 2){
        if(((x - off_x) > topPoints[0][1]) && (x - off_x)  < topPoints[0][3])
          topPoints[0][in_box] = (float)(x-off_x);
      }
      else if(in_box == 3){
        if(((x - off_x) < xorig+length)&&((x - off_x) > topPoints[0][2]))
          topPoints[0][in_box] = (float)(x-off_x);
      }
      else if(in_box == 4){
        if(((x - off_x)  > xorig) && ((x - off_x) < topPoints[1][1]))
          topPoints[1][in_box-4] = (float)(x-off_x);
      }
      else if(in_box == 5){
        if((x - off_x)  > topPoints[1][0] && ((x - off_x) < topPoints[1][2]))
          topPoints[1][in_box-4] = (float)(x-off_x);
      }
      else if(in_box == 6){
        if(((x - off_x) > topPoints[1][1]) && (x - off_x)  < topPoints[1][3])
          topPoints[1][in_box-4] = (float)(x-off_x);
      }
      else if(in_box == 7){
        if(((x - off_x) < xorig+length)&&((x - off_x) > topPoints[1][2]))
          topPoints[1][in_box-4] = (float)(x-off_x);
      }
      display();
    };
  default:
    break;
  }
}
void mouse(int button, int state, int x, int y)
{
  int i,temp;
  mouse_state = state;
  mouse_button = button;


  switch (button) {
  case GLUT_LEFT_BUTTON:
    if (mouse_state == GLUT_UP) {
      if(in_box != -1) {
        if(in_box == 0){
          if(((x - off_x)  > xorig) && ((x - off_x) < topPoints[0][1]))
            topPoints[0][in_box] = (float)(x-off_x);
        }
        else if(in_box == 1){
          if((x - off_x)  > topPoints[0][0] && ((x - off_x) < topPoints[0][2]))
            topPoints[0][in_box] = (float)(x-off_x);
        }
        else if(in_box == 2){
          if(((x - off_x) > topPoints[0][1]) && (x - off_x)  < topPoints[0][3])
            topPoints[0][in_box] = (float)(x-off_x);
        }
        else if(in_box == 3){
          if(((x - off_x) < xorig+length)&&((x - off_x) > topPoints[0][2]))
            topPoints[0][in_box] = (float)(x-off_x);
        }
        else if(in_box == 4){
          if(((x - off_x)  > xorig) && ((x - off_x) < topPoints[1][1]))
            topPoints[1][in_box-4] = (float)(x-off_x);
        }
        else if(in_box == 5){
          if((x - off_x)  > topPoints[1][0] && ((x - off_x) < topPoints[1][2]))
            topPoints[1][in_box-4] = (float)(x-off_x);
        }
        else if(in_box == 6){
          if(((x - off_x) > topPoints[1][1]) && (x - off_x)  < topPoints[1][3])
            topPoints[1][in_box-4] = (float)(x-off_x);
        }
        else if(in_box == 7){
          if(((x - off_x) < xorig+length)&&((x - off_x) > topPoints[1][2]))
            topPoints[1][in_box-4] = (float)(x-off_x);
        }
      }
      in_box = -1;
    }
    if (mouse_state == GLUT_DOWN) {

      in_box = -1;
      for(i=0; i<4; i++) {
        if( (x > (int)(topPoints[0][i] - 20.))
            && (y > (int)(yorig -  20.))
            && (x < (int)(topPoints[0][i] +  20.))
            && (y < (int)(yorig +  (20. + height))))
          {
            in_box = i;
            off_x = x - (int)(topPoints[0][i]);
            break;
          }
      }
      for(i=0; i<4; i++) {
        if( (x > (int)(topPoints[1][i] - 20.))
            && (y > (int)(yorig -  20.+350.))
            && (x < (int)(topPoints[1][i] +  20.))
            && (y < (int)(yorig +  (20. + height+350.))))
          {
            in_box = i+4;
            off_x = x - (int)(topPoints[1][i]);
            break;
          }
      }
      temp=testMapButtons(x,y);
      if(temp!=-1)
        maps[0]=temp;
    }
    break;
  case GLUT_RIGHT_BUTTON:
    if (mouse_state == GLUT_DOWN){
        //Clicked on the scales
        if(x>xorig&&x<xorig+length){
            if(y<300)
                comparison[0]=getColorIndex(x,maps[0]);
            else
                comparison[1]=getColorIndex(x,maps[1]);
        }
        //Clicked on the first map
        if(x>=475&&x<875&&y>=100&&y<500){
            comparison[0]=getColorHeight(1.0*image[x-475][y-100]/255.,maps[0]);
        }
        if(x>=880&&x<1280&&y>=100&&y<500){
            comparison[1]=getColorHeight(1.0*image[x-880][y-100]/255.,maps[1]);
        }
        temp=testMapButtons(x,y);
        if(temp!=-1)
            maps[1]=temp;
    }
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
  glOrtho(0.0, 1300.0, 650.0, 0.0, -10.0, 10.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void Find_Nearest(int x, int y)
{
  int i, j;
  int temp=0;
  GLint viewport[4];
  GLdouble mvmatrix[16], projmatrix[16];
  GLdouble td, dd;

  glGetIntegerv(GL_VIEWPORT, viewport);
  glGetDoublev(GL_MODELVIEW_MATRIX, mvmatrix);
  glGetDoublev(GL_PROJECTION_MATRIX, projmatrix);
  if(y<350)
    temp=1;
  for(i=0; i<4; i++)
    {
      gluProject((GLdouble)topPoints[temp][i],
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

  if(dd<150.) {
    SELECTED=GL_TRUE;
  }
}
void motion(int x, int y)
{
  {
    if(!SELECTED)
      {
        Find_Nearest(x, y);
      }
    if(true||SELECTED) {
      move_TopPoint(mouse_button, x, y);
      if(in_box != -1) {
        if(in_box == 0){
          if(((x - off_x)  > xorig)&&((x - off_x) < topPoints[0][1]))
            topPoints[0][in_box] = (float)(x-off_x);
        }

        else if(in_box == 1){
          if((x - off_x)  > topPoints[0][0] && ((x - off_x) < topPoints[0][2]))
            topPoints[0][in_box] = (float)(x-off_x);
        }
        else if(in_box == 2){
          if(((x - off_x) > topPoints[0][1]) && (x - off_x)  < topPoints[0][3])
            topPoints[0][in_box] = (float)(x-off_x);
        }

        else if(in_box == 3){
          if(((x - off_x) < xorig+length)&&((x - off_x) > topPoints[0][2]))
            topPoints[0][in_box] = (float)(x-off_x);
        }
        else if(in_box == 4){
          if(((x - off_x)  > xorig)&&((x - off_x) < topPoints[1][1]))
            topPoints[1][in_box-4] = (float)(x-off_x);
        }

        else if(in_box == 5){
          if((x - off_x)  > topPoints[1][0] && ((x - off_x) < topPoints[1][2]))
            topPoints[1][in_box-4] = (float)(x-off_x);
        }
        else if(in_box == 6){
          if(((x - off_x) > topPoints[1][1]) && (x - off_x)  < topPoints[1][3])
            topPoints[1][in_box-4] = (float)(x-off_x);
        }

        else if(in_box == 7){
          if(((x - off_x) < xorig+length)&&((x - off_x) > topPoints[1][2]))
            topPoints[1][in_box-4] = (float)(x-off_x);
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
   glutInitWindowSize (1300, 650);
   glutInitWindowPosition (100, 100);
   glutCreateWindow (argv[0]);
   init();
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutMouseFunc(mouse);
   glutMotionFunc(motion);

   glutKeyboardFunc (keyboard);
   glutTimerFunc(40, updateDisplay, 0);
   glutMainLoop();
   return 0;
}
