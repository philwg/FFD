#include "FFD_Box.hpp"

using namespace std;
using namespace arma;

/********************************************************************/
/*					 		Méthodes privées						*/
/********************************************************************/

int FFD_Box::Pascal(int i, int n)	// Coefficient du triangle de Pascal
{
	if ((i<0)||(i>n)) return 0;
	else if ((i==0)||(i==n)) return 1;
	else return this->Pascal(i-1, n-1) + this->Pascal(i, n-1);
}

float FFD_Box::Bernstein(int i, int n, float t)		// Polynôme de Bernstein
{
	float value {0.0f};
	if ((t>=0.0f) && (t<=1.0f)) value = (float)this->Pascal(i, n)*pow(t,i)*pow(1.0f-t, n-i);
	if (value < 0.000000000001f) value = 0.0f;
	return value;
}

void FFD_Box::compute_UV()		// Calcul des coordonnées des points dans le repère de la Box
{
	// On nettoie les valeurs précédentes
	this->UV_Coords.clear();
	// On parcourt tous les Points saisis
	for (int i=0; i<this->Points.size(); i++) {

		// On se ramène au Corner_0 de la Box et on calcule u par produit scalaire
		vec U = this->Points.at(i).get_Vec()-this->Corner_0.get_Vec();
		U.set_size(3);
		float u = dot(U, this->get_Base_U()) / pow(norm(this->get_Base_U()), 2);
		// ... Même chose pour v !
		vec V = this->Points.at(i).get_Vec()-this->Corner_0.get_Vec();
		V.set_size(3);
		float v = dot(V, this->get_Base_V()) / pow(norm(this->get_Base_V()), 2);
		// On enregistre les coordonnées obtenues.
		vec UV {u, v};
		this->UV_Coords.push_back(UV);
	}
}

void FFD_Box::compute_Hooks()	// Calcul des crochet de déformation
{
	// On nettoie les précédents
	this->Hooks.clear();
	vector<Point> H_Line;
	Point Origin = this->Corner_0;
	// On découpe régulièrement la box ligne par ligne en fonction des degrés fixés en V
	for (int j=0; j <= this->V_degree; j++) {
		H_Line.clear();
		// ... et en U
		for (int i=0; i <= this->U_degree; i++) {
			vec Trans_U = (float)i / (float)this->U_degree * this->get_Base_U();
			vec Trans_V = (float)j / (float)this->V_degree * this->get_Base_V();
			H_Line.push_back(Origin.get_Image_T(Trans_U + Trans_V));
		}
		// On enregistre chaque ligne
		this->Hooks.push_back(H_Line);			
	}
}

Point FFD_Box::get_Point_at(vec UV)		// Méthode de calcul de la fonction de mélange en (u,v)
{
	Point P = Point();
	vec Pt = zeros(4);
	// On additionne les contributions de chaque crochet mû en polygone de contrôle
	for (int i=0; i <= this->U_degree; i++) {
		for (int j=0; j <= this->V_degree; j++) {
			Pt = Pt +   Bernstein(i, this->U_degree, UV(0))
					  * Bernstein(j, this->V_degree, UV(1))
					  * this->Hooks.at(j).at(i).get_Vec();
		}
	}
	P = Point(Pt);
	return P;
}

void FFD_Box::compute_FFD()		// Recalcul des Points saisis par la fonction de mélange du polygone de contrôle
{
	// On nettoie les valeurs précédentes
	this->FFD_Points.clear();
	// On recalcule à partir des couples (u,v) associés aux Points
	for (int k=0; k < this->UV_Coords.size(); k++) {
		this->FFD_Points.push_back(this->get_Point_at(UV_Coords.at(k)));
	}
}

void FFD_Box::display_Boundaries()		// Gestion de l'affichage des bords de la Box
{
	// En rouge
	glColor3f(0.96f, 0.0f, 0.0f);
	// On traite les quatres côtés simultanément
	// En partant des quatre coins (Points de départ)
	vec U0_Prev {0.0f, 0.0f};
	vec U1_Prev {1.0f, 0.0f};
	vec V0_Prev {0.0f, 0.0f};
	vec V1_Prev {0.0f, 1.0f};
	Point U0_Prev_Pt = this->get_Point_at(U0_Prev);
	Point U1_Prev_Pt = this->get_Point_at(U1_Prev);
	Point V0_Prev_Pt = this->get_Point_at(V0_Prev);
	Point V1_Prev_Pt = this->get_Point_at(V1_Prev);

	// On parcourt avec un pas d'un centième l'intervalle ]0, 1]
	for (float t=0.01f; t<=1.0f; t=t+0.01f) {

		// On calcule les points coorespondant par la fonction de mélange du polygone de contrôle
		vec U0_Crnt {0.0f, t};
		vec U1_Crnt {1.0f, t};
		vec V0_Crnt {t, 0.0f};
		vec V1_Crnt {t, 1.0f};
		Point U0_Crnt_Pt = this->get_Point_at(U0_Crnt);
		Point U1_Crnt_Pt = this->get_Point_at(U1_Crnt);
		Point V0_Crnt_Pt = this->get_Point_at(V0_Crnt);
		Point V1_Crnt_Pt = this->get_Point_at(V1_Crnt);

		// On relie les précédents au au Points courants sur les quatre bords
		glBegin(GL_LINES);
			glVertex2fv(U0_Prev_Pt.get_Vertex2D());
			glVertex2fv(U0_Crnt_Pt.get_Vertex2D());
			glVertex2fv(U1_Prev_Pt.get_Vertex2D());
			glVertex2fv(U1_Crnt_Pt.get_Vertex2D());
			glVertex2fv(V0_Prev_Pt.get_Vertex2D());
			glVertex2fv(V0_Crnt_Pt.get_Vertex2D());
			glVertex2fv(V1_Prev_Pt.get_Vertex2D());
			glVertex2fv(V1_Crnt_Pt.get_Vertex2D());
		glEnd();

		// On actualise la position des précédents
		U0_Prev_Pt = U0_Crnt_Pt;
		U1_Prev_Pt = U1_Crnt_Pt;
		V0_Prev_Pt = V0_Crnt_Pt;
		V1_Prev_Pt = V1_Crnt_Pt;
	}
}

void FFD_Box::display_Hooks()		// Affichage des crochets de déformation
{
	glPointSize(7.0f);
	// Affichage des bords
	this->display_Boundaries();

	// Pour chaque valeur licite de découpe selon les degrés en U et V
	for (int i=0; i <= this->U_degree; i++) {
		for(int j=0; j <= this->V_degree; j++) {

			// Si on n'est pas en "haut", on relie le crochet du dessus
			if (j < this->V_degree) {
				glColor3f(0.36f, 0.18f, 0.09f);
				glPushAttrib(GL_ENABLE_BIT); 
				glLineStipple(1, 0x6666);
				glEnable(GL_LINE_STIPPLE);
				glBegin(GL_LINES);
					glVertex2fv(this->Hooks.at(j).at(i).get_Vertex2D());
					glVertex2fv(this->Hooks.at(j+1).at(i).get_Vertex2D());
				glEnd();
				glPopAttrib();
			}

			// Si on n'est pas tout à "droite", on relie avec le crochet suivant
			if (i < this->U_degree) {
				glColor3f(0.36f, 0.18f, 0.09f);
				glPushAttrib(GL_ENABLE_BIT); 
				glLineStipple(1, 0x6666);
				glEnable(GL_LINE_STIPPLE);
				glBegin(GL_LINES);
					glVertex2fv(this->Hooks.at(j).at(i).get_Vertex2D());
					glVertex2fv(this->Hooks.at(j).at(i+1).get_Vertex2D());
				glEnd();
				glPopAttrib();
			}

			// On affiche le crochet courant
			glColor3f(0.48f, 0.24f, 0.12f);
			glBegin(GL_POINTS);
				glVertex2fv(this->Hooks.at(j).at(i).get_Vertex2D());
			glEnd();
		}
	}
}

void FFD_Box::display_Curve()	// Affichage des segments déformés
{
    glPointSize(5.0f);
    // Pour chaque Point saisi
	for (int i=0; i < this->FFD_Points.size(); i++) {

		// On affiche le Point
		glColor3f(0.0f, 0.96f, 0.0f);
		glBegin(GL_POINTS);
			glVertex2fv(FFD_Points.at(i).get_Vertex2D());
		glEnd();

		// ... et on retrace sa liaison au suivant en la paramétrisant dans [0, 1]
		if (i > 0) {
			glColor3f(0.96f, 0.48f, 0.96f);
			vec prev_UV = UV_Coords.at(i-1);
			Point prev_Pt = this->get_Point_at(prev_UV);
			// On applique encore un pas d'un centième sur chaque segment
			for (float t=0.01f; t<=1.0f; t=t+0.01f) {
				// Paramétrisation de la position courante en (u,v)
				vec crnt_UV = (1-t)*UV_Coords.at(i-1)+t*UV_Coords.at(i);
				// Calcul du Point correspondant par la fonction de mélange du polygone de contrôle
				Point crnt_Pt = this->get_Point_at(crnt_UV);
				glBegin(GL_LINES);
					glVertex2fv(prev_Pt.get_Vertex2D());
					glVertex2fv(crnt_Pt.get_Vertex2D());
				glEnd();
				prev_Pt = crnt_Pt;
			}
		}	
	}	
}

/********************************************************************/
/*					 		Constructeur							*/
/********************************************************************/

FFD_Box::FFD_Box()
{
	this->Oriented = false;
	this->Gelled = false;
	this->Curved = false;
	this->set_U_Degree(3);
	this->set_V_Degree(3);
}

/********************************************************************/
/*					 		Destructeur								*/
/********************************************************************/

FFD_Box::~FFD_Box()
{
	this->UV_Coords.clear();
	this->Hooks.clear();
	this->FFD_Points.clear();
}

/********************************************************************/
/*					 		Méthodes publiques						*/
/********************************************************************/

void FFD_Box::display()		// Gestion de l'affichage
{
	// Si il y a au moins un Point saisi
	if (this->Points.size()>0) {

		// Si on est en mode "Gelée"
		if (this->is_Gelled()) {

			// et si on doit déformer les segments, on le fait
			if (this->is_Curved()) this->display_Curve();
			// sinon, on utilise le tracé sans déformation
			else this->display_Line(this->FFD_Points);

			// DAns tous les cas on affiche les crochets de déformation
			this->display_Hooks();
		}

		// Sinon on utilise le tracé du parent Bounding_Box
		else Bounding_Box::display();
	}
}

void FFD_Box::set_Gelled()	// Activation du mode "Gelée"
{
	this->Gelled = true;	
	this->compute_UV();			// Recalcul des (u,v) de tous les Points
	this->compute_Hooks();		// Recalcul des crochets de déformation
	this->compute_FFD();		// Recalcul des points à partir des (u,v)
}

void FFD_Box::change_Mode()		// Changemnt de mode (saisie / Gelée)
{
	// Si on est en mode Gellée
	if (this->is_Gelled()) {
		// On annule toutes les déformations
		this->Gelled = false;
		this->Curved = false;
	}
	// Sinon on l'active (voir ci-dessus)
	else this->set_Gelled();
}

void FFD_Box::change_Lines_Mode()	// Changement du mode d'affichage des segments
{
	if (this->is_Curved()) this->Curved = false;
	else if (this->is_Gelled()) this->Curved = true;
}

void FFD_Box::set_U_Degree(int new_Hd)		// Fixation du degré en U
{
	if (new_Hd < 3) this->U_degree = 3;
	else this->U_degree = new_Hd;
	if (this->is_Gelled()) this->set_Gelled();
}

void FFD_Box::change_U_Degree(int delta_h)		// Changement du degré en U
{
	this->set_U_Degree(this->U_degree + delta_h);
}

void FFD_Box::set_V_Degree(int new_Vd)		// Fixation u degré en V
{
	if (new_Vd < 3) this->V_degree = 3;
	else this->V_degree = new_Vd;
	if (this->is_Gelled()) this->set_Gelled();
}

void FFD_Box::change_V_Degree(int delta_v)		// Changement du degré en V
{
	this->set_V_Degree(this->V_degree + delta_v);
}

bool FFD_Box::is_Gelled()		// Teste le mode courant (saisie / Gelée)
{
	return this->Gelled;
}

bool FFD_Box::is_Curved()		// Teste le mode courant de tracé des segments
{
	return this->Curved;
}

vector<Point> FFD_Box::get_Hooks()		// Renvoie les crochets de déformation pour le Picking
{
	// On crée un vecteur de Points à partir de la matrices des crochets de déformation
	vector<Point> Hks;
	Hks.clear();
	for (int j=0; j <= this->V_degree; j++) {
		for(int i=0; i <= this->U_degree; i++) {
			Hks.push_back(this->Hooks.at(j).at(i));
		}
	}
	return Hks;
}

void FFD_Box::change_Hook(int index, float x, float y)		// Modifie la position du crochet selectionné
{
	// On reconstitue à partir de son indice ses coordonnées dans la matrice des crochets
	int j = index / (this->U_degree + 1);
	int i = index - (j * (this->U_degree + 1));
	// On le modifie
	this->Hooks.at(j).at(i).set_Coords(x, y);
	// On recalcule tous les Points avec la nouvelle position
	this->compute_FFD();
}