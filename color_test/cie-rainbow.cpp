/* This program will produce a sequence colors in the rainbow color spectrum in RGB value
	The each adjacent pair of colors in the sequence will have the same cie2000 delta-e spacing
	This should ensure that color scale will have linear cie2000 delta-e slope, or at least locally.
*/

#include "CIEDE2000.h"
#include "ColorConverter.h"
#include <fstream>
#include <cmath>

using namespace std;
using namespace CIEDE2000;

double cie2000(const float c1[3], const float c2[3]);	//compute the delta-e between two colors in rgb
void copy(float c1[3], float c2[3]);	//copy c1 to c2
bool validLAB(float color[3]); //checks if a LAB color has valid RGB representation
void method_local_linear(const char* filename); 	//produce rainbow scale by making each adjacent color with the same delta-e difference
											//drawback: only works locally
											
void method_lab_rotation(const char* filename); 	//produce rainbow(?) scale by rotating in lab color sphere
											//advantage: lightness consistency might contribute to smoother slope
											
void method_single_reference(const char* filename, const float ref[3]);		//produce rainbow scale with linear delta-e by using a single color as reference

void method_r_g_b(const char* filename);

int main(){
	method_r_g_b("rgb_rainbow.txt");
	method_local_linear("local_linear_rainbow.txt");
	method_lab_rotation("lab_rotation.txt");
	float red[]={1,0,0};
	float green[]={0,1,0};
	float blue[]={0,0,1};
	method_single_reference("red_reference.txt",red);
	method_single_reference("green_reference.txt",green);
	method_single_reference("blue_reference.txt",blue);
	return 0;
}

double cie2000(const float c1[3], const float c2[3])
{
	float c1_lab[3];
	float c2_lab[3];
	RGB_to_LAB(c1,c1_lab);
	RGB_to_LAB(c2,c2_lab);
	CIEDE2000::LAB color1={c1_lab[0], c1_lab[1], c1_lab[2]};
	CIEDE2000::LAB color2={c2_lab[0], c2_lab[1], c2_lab[2]};
	return CIEDE2000::CIEDE2000(color1, color2);
	
}

void copy(float c1[3], float c2[3]){
	c2[0]=c1[0];
	c2[1]=c1[1];
	c2[2]=c1[2];
}

void method_r_g_b(const char* filename){
	ofstream outputFile(filename, ofstream::out|ofstream::trunc);
	for(int i=0;i<255;i++){
		outputFile<<255-i<<" "<<i<<" "<<0<<"\n";
	}
	for(int i=0;i<255;i++){
		outputFile<<0<<" "<<255-i<<" "<<i<<"\n";
	}
	for(int i=0;i<255;i++){
		outputFile<<i<<" "<<0<<" "<<255-i<<"\n";
	}
	
	outputFile<<255<<" "<<0<<" "<<0<<"\n";
	
}

void method_local_linear(const char* filename){
	const float delta_e_spacing=0.5;	//The delta-e spacing between two adjacent color
	const float step=0.001; //change in rgb value in each step of testing
	ofstream outputFile(filename, ofstream::out|ofstream::trunc);
	float color1[]={1, 0, 0};
	float color2[]={1, 0, 0};
	bool done=false;
	double delta_e;
	
	outputFile<<(int)(color1[0]*255+0.5)<<" "<<(int)(color1[1]*255+0.5)<<" "<<(int)(color1[2]*255+0.5)<<"\n";
	//from	255	0 	0
	//	to	255 255 0
	while(!done){
		
		color2[1]+=step;
		delta_e=cie2000(color1,color2);
		if(delta_e > delta_e_spacing){
			if(color2[1]>1){
				color2[1]=1;
				done=true;
			}
			copy(color2,color1);
			outputFile<<(int)(color1[0]*255+0.5)<<" "<<(int)(color1[1]*255+0.5)<<" "<<(int)(color1[2]*255+0.5)<<"\n";
		}
	}
	
	//from 	255	255	0
	//to 	0	255	0
	done=false;
	while(!done){
		
		color2[0]-=step;
		delta_e=cie2000(color1,color2);
		if(delta_e > delta_e_spacing){
			if(color2[0]<0){
				color2[0]=0;
				done=true;
			}
			copy(color2,color1);
			outputFile<<(int)(color1[0]*255+0.5)<<" "<<(int)(color1[1]*255+0.5)<<" "<<(int)(color1[2]*255+0.5)<<"\n";
		}
	}
	
	//from	0	255	0
	//to	0	255 255
	done=false;
	while(!done){
		
		color2[2]+=step;
		delta_e=cie2000(color1,color2);
		if(delta_e > delta_e_spacing){
			if(color2[2]>1){
				color2[2]=1;
				done=true;
			}
			copy(color2,color1);
			outputFile<<(int)(color1[0]*255+0.5)<<" "<<(int)(color1[1]*255+0.5)<<" "<<(int)(color1[2]*255+0.5)<<"\n";
		}
	}
	
	//from 	0 	255	255
	//to	0	0	255
	done=false;
	while(!done){
		
		color2[1]-=step;
		delta_e=cie2000(color1,color2);
		if(delta_e > delta_e_spacing){
			if(color2[1]<0){
				color2[1]=0;
				done=true;
			}
			copy(color2,color1);
			outputFile<<(int)(color1[0]*255+0.5)<<" "<<(int)(color1[1]*255+0.5)<<" "<<(int)(color1[2]*255+0.5)<<"\n";
		}
	}
	
	//from 	0	0	255
	//to 	255 0	255
	done=false;
	while(!done){
		
		color2[0]+=step;
		delta_e=cie2000(color1,color2);
		if(delta_e > delta_e_spacing){
			if(color2[0]>1){
				color2[0]=1;
				done=true;
			}
			copy(color2,color1);
			outputFile<<(int)(color1[0]*255+0.5)<<" "<<(int)(color1[1]*255+0.5)<<" "<<(int)(color1[2]*255+0.5)<<"\n";
		}
	}
	
	//(I feel like this part should be omitted because it makes the max and the min values the same color)
	//from 	255 0	255 
	//to 	255 0 	0
	done=false;
	while(!done){
		
		color2[2]-=step;
		delta_e=cie2000(color1,color2);
		if(delta_e > delta_e_spacing){
			if(color2[2]<0){
				color2[2]=0;
				done=true;
			}
			copy(color2,color1);
			outputFile<<(int)(color1[0]*255+0.5)<<" "<<(int)(color1[1]*255+0.5)<<" "<<(int)(color1[2]*255+0.5)<<"\n";
		}
	}
	outputFile.close();
}
bool validLAB(float color[3]){
	float rgb[3];
	LAB_to_RGB(color,rgb);
	for(int i=0;i<3;i++){
		if(rgb[i]<0||rgb[i]>1)
			return false;
	}
	return true;
}
void  method_lab_rotation(const char* filename){
	const float delta_e_spacing=0.5;	//The delta-e spacing between two adjacent color
	const float step=M_PI/500; //change in angle in each step of testing
	float init_angle = M_PI/4;
	float final_angle= 9*M_PI/4;
	float angle=init_angle;
	float radius; //min 0, max 100
	ofstream outputFile(filename, ofstream::out|ofstream::trunc);
	float color1[3];
	color1[0]=60;
	float color2[3];
	color2[0]=60;
	float rgb1[3];
	float rgb2[3];
	bool done=false;
	double delta_e;

	while(angle<final_angle){
		radius=100;
		color2[1]=radius*cos(angle);
		color2[2]=radius*sin(angle);
		while(!validLAB(color2)){
			radius-=1;
			color2[1]=radius*cos(angle);
			color2[2]=radius*sin(angle);
		}
		LAB_to_RGB(color1,rgb1);	
		LAB_to_RGB(color2,rgb2);
		delta_e=cie2000(rgb1,rgb2);
		if(delta_e>delta_e_spacing){
			copy(color2,color1);
			outputFile<<(int)(rgb2[0]*255+0.5)<<" "<<(int)(rgb2[1]*255+0.5)<<" "<<(int)(rgb2[2]*255+0.5)<<"\n";
		}
		angle+=step;
	}
	
	outputFile.close();
}

void method_single_reference(const char* filename, const float ref[3]){
	
	const float delta_e_spacing=0.5;	//The delta-e spacing between two adjacent color
	const float step=0.001; //change in rgb value in each step of testing
	ofstream outputFile(filename, ofstream::out|ofstream::trunc);
	float color[]={1, 0, 0};
	bool done=false;
	double delta_e_last=cie2000(color,ref);
	double delta_e;


	
	outputFile<<(int)(color[0]*255+0.5)<<" "<<(int)(color[1]*255+0.5)<<" "<<(int)(color[2]*255+0.5)<<"\n";
	int stage=0;
	//stage)	from	255	0 	0
	//0)	to	255 255 0 		[1]++
	
	//1)	to 	0	255	0		[0]--

	//2) 	to	0	255 255		[2]++

	//3)	to	0	0	255		[1]--

	//4) 	to 	255 0	255		[0]++

	//5)	to 	255 0 	0		[2]--;
	while(!done){
		if(stage==0){
			color[1]+=step;
			if(color[1]>1){
				color[1]=1;
				stage++;
			}
		}
		else if(stage==1){
			color[0]-=step;
			if(color[0]<0){
				color[0]=0;
				stage++;
			}
		}
		else if(stage==2){
			color[2]+=step;
			if(color[2]>1){
				color[2]=1;
				stage++;
			}
		}
		else if(stage==3){
			color[1]-=step;
			if(color[1]<0){
				color[1]=0;
				stage++;
			}
		}
		else if(stage==4){
			color[0]+=step;
			if(color[0]>1){
				color[0]=1;
				stage++;
			}
		}
		else if(stage==5){
			color[2]-=step;
			if(color[2]<0){
				color[2]=0;
				done=true;
			}
		}

		delta_e=cie2000(color,ref);
		if(delta_e<delta_e_last-delta_e_spacing){
			delta_e_last=delta_e;
			outputFile<<(int)(color[0]*255+0.5)<<" "<<(int)(color[1]*255+0.5)<<" "<<(int)(color[2]*255+0.5)<<"\n";
		
		}
		else if(delta_e>delta_e_last+delta_e_spacing){
			delta_e_last=delta_e;
			outputFile<<(int)(color[0]*255+0.5)<<" "<<(int)(color[1]*255+0.5)<<" "<<(int)(color[2]*255+0.5)<<"\n";
		}
	}
	
	
	
	
	outputFile.close();
}
