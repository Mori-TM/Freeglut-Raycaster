#include <GL/freeglut.h> 
#define PI_2 2*3.1415926535
#define PI 3.1415926535
#define P2 PI/2
#define P3 3*PI/2
#define DR 0.0174533 //1 Grad in radians

float deltaMoveX = 0.0;
float deltaMoveY = 0.0;
float deltaMoveA = 0.0;
float deltaMoveRX = 0.0;
float deltaMoveRY = 0.0;
float px,py,pdx,pdy,pa; //Spieler Position

float MouseX; 
float MouseY; 

int w = 1024;
int h = 512;

//Fps
int FP = 0;
int final_time;
int init_time;
int fps = 0;

char test[32];
void drawPlayer()
{	
	glColor3f(1,0,1);
	glPointSize(8);
	glBegin(GL_POINTS);
		glVertex2i(px,py);
	glEnd();

	sprintf(test,"FPS:%d", fps);
	glutSetWindowTitle(test);
	
	glLineWidth(3);
	glBegin(GL_LINES);
		glVertex2i(px,py);
		glVertex2i(px+pdx*5, py+pdy*5);
	glEnd();
}

const int mapX = 8;
const int mapY = 8;
const int mapS = 64;
int map[]=
{
	1,1,1,1,1,1,1,1,
	1,0,0,0,0,0,0,1,
	1,0,1,1,0,1,0,1,
	1,0,0,0,0,0,0,1,
	1,1,1,1,0,0,0,1,
	1,0,0,0,0,0,0,1,
	1,0,0,1,0,0,0,1,
	1,1,1,1,1,1,1,1,
};

void drawMap2D()
{	
	int x,y,xo,yo;
	for(y=0;y<mapY;y++)
	{
		for(x=0;x<mapY;x++)
		{
			if(map[y*mapX+x]==1){ glColor3f(1,1,1);} else{ glColor3f(0,0,0);}
			
			xo=x*mapS; yo=y*mapS;
			glBegin(GL_QUADS);
				glVertex2i(xo     +1, yo     +1);
				glVertex2i(xo     +1, yo+mapS-1);
				glVertex2i(xo+mapS-1, yo+mapS-1);
				glVertex2i(xo+mapS-1, yo     +1);
			glEnd(); 
		}
	}
}

float dist(float ax,float ay, float bx,float by, float ang)
{
	return ( sqrt((bx-ax)*(bx-ax) + (by-ay)*(by-ay)) );
}

void drawRays3D()
{
	int r,mx,my,mp,dof; float rx,ry,ra,xo,yo,disT;
	ra=pa-DR*30;if(ra<0){ra+=2*PI;}if(ra>2*PI) {ra-=2*PI;}
	
	for(r=0;r<60;r++)
	{
		//Check Horizontal Lines
		dof=0;
		float disH=10000.0,hx=px,hy=py;
		// float aTan=-1 / tan(ra);
		
		float t = tan(ra);  
		float aTan;
		if (t != 0.0f) 
		{
			aTan = -1.0f / t;
		} else 
		{
			aTan = 0.0f;
		}
		
		if(ra>PI){ ry=(((int)py>>6)<<6)-0.0001; rx=(py-ry)*aTan+px; yo=-64; xo=-yo*aTan;}//Hoch Kucken
		if(ra<PI){ ry=(((int)py>>6)<<6)+64;     rx=(py-ry)*aTan+px; yo= 64; xo=-yo*aTan;}//Runter Kucken
		if(ra==0 || ra==PI){ rx=px; ry=py; dof=8;} //grade nach links oder rechts schaun
		while(dof<8)
		{
			mx=(int)(rx)>>6; my=(int)(ry)>>6; mp=my*mapX+mx;
			if(mp>0 && mp<mapX*mapY && map[mp]==1){ hx=rx; hy=ry; disH=dist(px,py,hx,hy,ra); dof=8;} //Wand berühren
			else{ rx+=xo; ry+=yo; dof+=1;} //nächste linie
		}
		
		//Check Vertical Lines
		
		//   dof=0;
		float disV = 10000.0f;
		float vx = px;
		float vy = py;
		float nTan = -tan(ra);
		
		
		if (ra > P2 && ra < P3) 
		{
			rx = (float)(  ( (int)px >> 6 )  << 6) - 0.0001; 
			ry = (px - rx) * nTan + py; 
			xo = -64; 
			yo = -xo * nTan;
		}
		if ((ra < P2) || (ra > P3)) 
		{
			rx = (  ( (int)px >> 6 ) << 6 ) + 64;
			ry = (px - rx) * nTan + py;
			xo = 64;
			yo = -xo * nTan;
		}
		
		
		dof = 0;
		if ( (ra == 0) || (ra == PI) ) 
		{
			rx = px;
			ry = py;
			dof = 8;   	
		}
		int mv;
		while ( dof < 8 ) 
		{
			mx = (int)(rx) >> 6;
			my = (int)(ry) >> 6;
			mp = my * mapX + mx;
			if ( (mp >= 0) && (mp < mapS) ) 
			{
				mv = map[mp];	
				if (map[mp] == 1) 
				{    		    
				vx = rx;
				vy = ry;
				disV = dist(px,py,vx,vy,ra);
				dof = 8;
				break;
				}
			}
		rx += xo; 
		ry += yo;
		dof += 1;
		}
		
		
		if (disV < disH)
		{
			rx = vx;
			ry = vy;
			disT = disV; glColor3f(0.0,0.0,0.9);
		} else if (disV >= disH) 
		{
			rx = hx;
			ry = hy;
			disT = disH; glColor3f(0.0,0.0,0.7);
		}
		
		
		glLineWidth(1);
		glBegin(GL_LINES);
			glVertex2i(px,py);
			glVertex2i(rx,ry);
		glEnd();
		
		//3D Wände erstellen
		
		
		
		float ca=pa-ra; if(ca<0){ ca+=2*PI;} if(ca>2*PI){ ca-=2*PI;} disT=disT*cos(ca);
		float lineH=(mapS*320)/disT; 
		if(lineH>320){lineH=320;} 		//linien Höhe
		float lineO=160-lineH/2;			
		glLineWidth(8);
		glBegin(GL_LINES);
			glVertex2i(r*8+530,lineO);
			glVertex2i(r*8+530,lineH+lineO);
		glEnd();
		ra+=DR; if(ra<0){ ra+=2*PI;} if(ra>2*PI){ ra-=2*PI;}
 	}
}

void display()
{
	glClearColor(0.3,0.3,0.3,0.0); 
	glClear(GL_COLOR_BUFFER_BIT); 
	
	px += deltaMoveX/2;
	py += deltaMoveY/2;
	
	pa += deltaMoveA;
	pdx += deltaMoveRX;
	pdy += deltaMoveRY;
	
	drawMap2D();
	drawPlayer();
	drawRays3D();
	glutSwapBuffers();
 	FP ++;
	final_time = time(NULL);
	if(final_time - init_time > 0)	
	{
		fps = FP/(final_time - init_time);
		FP = 0;
		init_time = final_time;
	}
}

void buttons(unsigned char key, int x, int y)
{
	if(key=='d') { deltaMoveA=0.1; if(pa<   0){ pa+=2*PI;} pdx=cos(pa)*5; pdy=sin(pa)*5;}
	if(key=='a') { deltaMoveA=-0.1; if(pa>2*PI){ pa-=2*PI;} pdx=cos(pa)*5; pdy=sin(pa)*5;}
	if(key=='w') { deltaMoveX = pdx; deltaMoveY = pdy;}
	if(key=='s') { deltaMoveX = -pdx; deltaMoveY = -pdy;}
	if(key== 27) { exit(0); } 
}
void buttonsUp(unsigned char key, int x, int y)
{
	if(key=='w') { deltaMoveX = 0; deltaMoveY = 0;}
	if(key=='s') { deltaMoveX = 0; deltaMoveY = 0;}
	if(key=='d') { deltaMoveA = 0; pdx=cos(pa)*5; if(pa<   0){ pa+=2*PI;} pdy=sin(pa)*5;}
	if(key=='a') { deltaMoveA = 0; pdx=cos(pa)*5; if(pa>2*PI){ pa-=2*PI;} pdy=sin(pa)*5;}
}

void Reshape(int w, int h)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0,w,h,0);
	glMatrixMode(GL_MODELVIEW);
	
	glutSetCursor(GLUT_CURSOR_NONE);
	px=300; py=300; pdx=cos(pa)*5; pdy=sin(pa)*5;
}

void MouseM(int x, int y)
{
	MouseX = x -w/2;
	MouseY += y -h/2;
	
	pa += MouseX/980;
	pdx=cos(pa)*5; if(pa<   0){ pa+=2*PI;} pdy=sin(pa)*5;
	pdx=cos(pa)*5; if(pa>2*PI){ pa-=2*PI;} pdx=cos(pa)*5; pdy=sin(pa)*5;
}

void Timer()
{
	glutPostRedisplay();
	glutTimerFunc(1000/60, Timer, 1);
	glutWarpPointer(w/2.0, h/2.0);
}

int main(int argc, char **argv)
{ 
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowPosition(400,0);
	glutInitWindowSize(w,h);
	glutCreateWindow("Raycaster");
	// glutFullScreen();
	// init(); 
	
	
	glutReshapeFunc(Reshape);
	glutDisplayFunc(display);
	
	
	glutKeyboardFunc(buttons);
	glutKeyboardUpFunc(buttonsUp);
	glutPassiveMotionFunc(MouseM);
	glutTimerFunc(0, Timer, 0);
	glutMainLoop(); 
}
