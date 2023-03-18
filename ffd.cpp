#include <vector>
#include <GL/glut.h>
#include "Point.hpp"
#include "FFD_Box.hpp"

using namespace std;

/********************************************************************/
/*					 Déclaration des variables						*/
/********************************************************************/

FFD_Box Cloud;				// Mon nuage de Points pour la FFD !

int width 		{1200};		// La largeur de la fenêtre OpenGL
int height		{1200};		// La hauteur de la fenêtre OpenGL
int selected 	{-1};		// L'indice du Vertex sélectionné


/********************************************************************/
/*					Prototypes des méthodes 						*/
/********************************************************************/

void reshape(int w, int h);
void display();
int getRightPicked(int state, int x, int y, vector<Point> Pts);
void mouse(int button, int state, int x, int y);
void motion(int x, int y);
void keyboard(unsigned char c, int x, int y);


/********************************************************************/
/*					Implémentations			 						*/
/********************************************************************/


/** Gestion du redimensionnement ************************************/
void reshape(int w, int h)
{
	glViewport(0, 0, w, h); 			
	width = w;
	height = h;
	glMatrixMode(GL_PROJECTION); 
	glLoadIdentity(); 
	glOrtho(0.0f, width, 0.0f, height, -1.0f, 1.0f); 
	glMatrixMode(GL_MODELVIEW); 
	glLoadIdentity();
}


/** Gestion de l'affichage ******************************************/
void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Affichage du nuage avec toutes ses options
	Cloud.display();
    glutSwapBuffers();
    reshape(width, height);
}


/** Gestion du picking **********************************************/
int getRightPicked(int state, int x, int y, vector<Point> Pts)
{
	int s = -1;
	if (state == GLUT_DOWN) {
		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);
		GLuint *selectBuf = new GLuint[2*Pts.size()];
		GLuint *ptr;
		GLint hits;
		glSelectBuffer(2*Pts.size(), selectBuf);
		glRenderMode(GL_SELECT);
		glPushMatrix();
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			gluPickMatrix(x, viewport[3]-y, 12.0f, 12.0f, viewport);
			glOrtho(0.0f, viewport[2], 0.0f, viewport[3], -1.0f, 1.0f);
			glColor3f(0.0f, 0.96f, 0.0f);
			glPointSize(5.0f);
			glInitNames();
			glPushName(1);
			for (int i=0; i < Pts.size(); i++) {
				glLoadName(i);
				glBegin(GL_POINTS);
					glVertex2fv(Pts.at(i).get_Vertex2D());
				glEnd();			
			}
		glPopMatrix();
		glFlush();
		hits = glRenderMode(GL_RENDER);
		if (hits) {
			ptr = (GLuint *)selectBuf;
			ptr += 3;
			s = *ptr;
		}
	}
	return s;
}


/** Gestion des clics souris ****************************************/
void mouse(int button, int state, int x, int y)
{
	// On reste dans la zone de la fenêtre OpenGL
	if (x < 12) x = 12;
	if (x > width-12) x = width-12;
	if (y < 12) y = 12;
	if (y > height-12) y = height-12;

	// Si on est en mode "Gelée" ...
	if (Cloud.is_Gelled()) {
		// Le bouton droit sélectionne un crochet pour le bouger
		if (button == GLUT_RIGHT_BUTTON) {
			selected = getRightPicked(state, x, y, Cloud.get_Hooks());	
			if(state == GLUT_UP) selected = -1;
		}
	}
	// Sinon, on est en mode saisie/édition
	else {
		// Le bouton gauche ajoute un point
		if (button == GLUT_LEFT_BUTTON) {
			Point new_P;
			new_P.set_Coords(x, height-y);
			if (state == GLUT_UP) Cloud.add(new_P);
		}
		// Le bouton droit selectionne un point pour le bouger
		if (button == GLUT_RIGHT_BUTTON) {
			selected = getRightPicked(state, x, y, Cloud.get_Points());	
			if(state == GLUT_UP) selected = -1;
		}
	}
	reshape(width, height);
	glutPostRedisplay();
}


/** Gestion des déplacements de la souris ***************************/
void motion(int x, int y)
{
	// On reste dans la zone de la fenêtre OpenGL
	if (x < 12) x = 12;
	if (x > width-12) x = width-12;
	if (y < 12) y = 12;
	if (y > height-12) y = height-12;

	// Si on est en mode "Gelée" ...
	if (Cloud.is_Gelled()) {
		// Si on a selectionné un crochet pour le bouger, on déforme !
		if (selected!=-1) Cloud.change_Hook(selected, (float)x, (float)(height-y));
	}
	// Sinon, on est en mode édition/saisie
	else {
		// Si on a selectionné un point pour le bouger, on le bouge !
		if (selected!=-1) Cloud.change_Coords(selected, (float)x, (float)(height-y));
	}
	reshape(width, height);
	glutPostRedisplay();
}


/** Gestion des saisies clavier *************************************/
void keyboard(unsigned char c, int x, int y)
{
	switch (c) {

		case 'U' :		//--------------------- Augmenter le dégré horizontal
			Cloud.change_U_Degree(1);
			break;
		
		case 'u' : 		//--------------------- Diminuer le degré horizontal
			Cloud.change_U_Degree(-1);
			break;
			
		case 'V' :		//--------------------- Augmenter le degré vertical
			Cloud.change_V_Degree(1);
			break;
			
		case 'v' :		//--------------------- Diminuer le degré vertical
			Cloud.change_V_Degree(-1);
			break;

		case 'g' :		//--------------------- Changer le mode d'affichage (Saisie / Gelée)
			Cloud.change_Mode();
			break;

		case 'c' :		//--------------------- Autoriser ou non la déformation des lignes
			Cloud.change_Lines_Mode();
			break;

		case 'o' :		//--------------------- Orienter ou non la Bounding Box (hors mode Gelée)
			if (!Cloud.is_Gelled()) Cloud.change_Box_Mode();
			break;

		case 'q' :		//--------------------- Quitter
			exit(0);
			
		default :
			break;
	}
	reshape(width, height);	
	glutPostRedisplay();
}


/********************************************************************/
/*					Lanceur de l'application						*/
/********************************************************************/

int main (int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(1200, 1200);
	glEnable(GL_DEPTH_TEST);
    glutCreateWindow("Deformations");

	Cloud = FFD_Box();

    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
  	glutMouseFunc(mouse);
	glutMotionFunc(motion);	
	glutKeyboardFunc(keyboard);
  
    glutMainLoop();
    return 0;
}
