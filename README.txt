Color analysis tool created by Wesley Chiou(wchiou1@umbc.edu), Adam Wendler(adamwe1@umbc.edu), Chen Kuo(ckuo1@umbc.edu)
This program is designed to compare colors along a color map to determine where there might be differences in the color distances in different parts of the map which can lead to misrepresentation of data if spikes or plateaus of color distance are unintentional.

Reads from the text file "Colors.txt"
Format for the color in the text file are in RGB[0-255] with spaces between

The program can contain up to 2048 different colors which will be displayed in the order they appear in the text file. Currently the text file contains the typical rainbow color map.

Tested in Code Blocks using GNU GCC Compiler, following C++11 ISO language standard
Requires Opengl32, glu32, glut32 libraries