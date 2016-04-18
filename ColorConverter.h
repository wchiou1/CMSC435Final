/* ColorConverter.h
** A header file with implementation of functions that convert between RGB and LAB color spaces using XYZ as an intermediate step
** Note: Many variations of RGB color system exist. 
**		 The particular RGB system used for this converter is sRGB, or Adobe RGB (1998) if #define USING_ADOBE. 
**       Other RGB system requires different conversion.
**		 RGB values here are in range [0,1] NOT in range [0,255]
**		Usage example: RGB_to_LAB(RGB,LAB); RGB is an array of size 3 with rgb value. LAB is an array of size3 that will be modified by the function to store Lab values.
*/

#ifndef COLORCONVERTER_H
#define COLORCONVERTER_H

#include <math.h>
#include <iostream>

//CIE standard
const float epsilon=0.008856;
const float kappa=903.3;

//XYZ reference white with illuminant=D65
const float refWhite[3]={95.047, 100.000, 108.883};

const float Gamma=2.2; //for Adobe RGB

//matrix for converting from Adobe RGB to XYZ
const float M[3][3] ={	{0.5767309,  0.1855540,  0.1881852},
						{0.2973769,  0.6273491,  0.0752741},
						{0.0270343,  0.0706872,  0.9911085}};

//matrix for converting from XYZ to Adobe RGB
const float M_inverse[3][3]= {	{2.0413690, -0.5649464, -0.3446944},
								{-0.9692660,  1.8760108,  0.0415560},
								{0.0134474, -0.1183897,  1.0154096}};

//for  sRGB
const float sRGB_M[3][3]={	{0.4124564,  0.3575761,  0.1804375},
							{0.2126729,  0.7151522,  0.0721750},
							{0.0193339,  0.1191920,  0.9503041}};
							
const float sRGB_M_inverse[3][3]={	{3.2404542, -1.5371385, -0.4985314},
									{-0.9692660,  1.8760108,  0.0415560},
									{0.0556434, -0.2040259,  1.0572252}};
							
								

								
//functions prototype 
void RGB_to_XYZ(float RGB[3], float result[3]);
void RGB_to_LAB(float RGB[3], float result[3]);
void XYZ_to_RGB(float XYZ[3], float result[3]);
void XYZ_to_LAB(float XYZ[3], float result[3]);
void LAB_to_RGB(float LAB[3], float result[3]);
void LAB_to_XYZ(float LAB[3], float result[3]);
void multiply(const float m[3][3],float* v, float result[3]);

//pow(x,y) returns nan when x=0? make it return 0 instead.
float pow_(float x,float y){ 
	if(x< 0.0000000001 && x> -0.0000000001)
		return 0;
	else 
		return (float) pow(x,y);
}

//functions implementation

#ifdef  USING_ADOBE //convert using Adobe RGB 

void RGB_to_XYZ(float RGB[3], float result[3]){
	float rgb[3]; //companding RGB => linear
	for(int i=0;i<3;i++)
		rgb[i]=pow_(RGB[i],Gamma);
	//[xyz]=[M][rgb]
	multiply(M,rgb,result);
}

void XYZ_to_RGB(float XYZ[3], float result[3]){
	float rgb[3]; //linear
	float* RGB=result; //nonlinear
	multiply(M_inverse,XYZ,rgb);
	for(int i=0;i<3;i++){
		RGB[i]=pow_(rgb[i],1/Gamma); 
	}
}

#else //convert using sRGB 

void RGB_to_XYZ(float RGB[3], float result[3]){
	float rgb[3]; //companding RGB => linear
	for(int i=0;i<3;i++){
		if(RGB[i] <= 0.04045)
			rgb[i]=RGB[i]/12.92;
		else 
			rgb[i]= pow_((RGB[i]+0.055)/1.055,2.4);
		rgb[i]*=100;
	}
	//[xyz]=[M][rgb]
	multiply(sRGB_M,rgb,result);
}

void XYZ_to_RGB(float XYZ[3], float result[3]){
	float rgb[3]; //linear
	float* RGB=result; //nonlinear

	multiply(sRGB_M_inverse,XYZ,rgb);
	for(int i=0;i<3;i++){
		rgb[i]/=100;
		if(rgb[i] <= 0.0031308)
			RGB[i]= 12.92*rgb[i];
		else
			RGB[i]= 1.055 * pow_(rgb[i],1/2.4)-0.055;
	}
}
	
#endif

void RGB_to_LAB(float RGB[3], float result[3]){
	float XYZ[3];
	RGB_to_XYZ(RGB, XYZ);
	XYZ_to_LAB(XYZ, result);
}

void XYZ_to_LAB(float XYZ[3], float result[3]){
	float ref_XYZ[3];
	for(int i=0;i<3;i++)
		ref_XYZ[i]=XYZ[i]/refWhite[i];
	
	float f[3];
	for(int i=0;i<3;i++){
		if(ref_XYZ[i] > epsilon){
			f[i]=pow_(ref_XYZ[i], (float)1/3);
		}
		else{
			f[i]= (kappa*ref_XYZ[i]+16)/116;
		}
	}

	float* Lab=result;
	Lab[0]=116*f[1]-16;
	Lab[1]=500*(f[0]-f[1]);
	Lab[2]=200*(f[1]-f[2]);
	
}

void LAB_to_RGB(float LAB[3], float result[3]){
	float XYZ[3];
	LAB_to_XYZ(LAB,XYZ);
	XYZ_to_RGB(XYZ,result);
}

void LAB_to_XYZ(float LAB[3], float result[3]){
	float f[3];
	f[1]=(LAB[0]+16)/116; //fy
	f[2]=f[1]-(LAB[2]/200); //fz
	f[0]= (LAB[1]/500)+f[1]; //fx
	
	float ref_XYZ[3];
	for(int i=0;i<3;i++){
		if(pow_(f[i],3) > epsilon)
			ref_XYZ[i] = pow_(f[i],3);
		else 
			ref_XYZ[i] = (116*f[i]-16)/kappa;
	}
	float* XYZ=result;
	for(int i=0;i<3;i++)
		XYZ[i]=ref_XYZ[i]*refWhite[i];
	

}

void multiply(const float m[3][3],float* v, float result[3]){
	float product[3]={0,0,0};
	for(int r=0;r<3;r++){
		for(int c=0;c<3;c++){
			product[r]+= m[r][c]*v[c];
		}
	}
	for(int i=0;i<3;i++)
		result[i]=product[i];
}


#endif