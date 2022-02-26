#include "BMP.h"

int main(int argc, char const *argv[])
{
int color; 
int gd , gm ; 
gd = VGA ; gm = VGAHI; 
 
initgraph(&gd,&gm,""); 
ShowBMP(0,0,"tune.bmp"); /* Enter File Name Here */ 
getch(); 
closegraph(); 
}
