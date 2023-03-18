#include "Bounding_Box.hpp"

using namespace std;
using namespace arma;

/********************************************************************/
/*							Méthodes Protégées						*/
/********************************************************************/

Point Bounding_Box::iso_Barycentre()	// Calcul de l'isobarycentre
{
	Point P;
	vec iB = zeros(4);
	for (int i=0; i<this->Points.size(); i++) iB = iB + this->Points.at(i).get_Vec();
	iB = iB / this->Points.size();
	P = Point(iB);
	return P;
}

void Bounding_Box::display_Line(vector<Point> Pts)	// Affichage de la ligne des Points
{
    glPointSize(5.0f);
    // On Parcourt tout le vecteur ...
	for (int i=0; i < Pts.size(); i++) {
		glColor3f(0.0f, 0.96f, 0.0f);
		// On affiche les Points ...
		glBegin(GL_POINTS);
			glVertex2fv(Pts.at(i).get_Vertex2D());
		glEnd();
		// ... et on relie au précédent
		if (i > 0) {
			glColor3f(0.96f, 0.48f, 0.96f);
			glBegin(GL_LINES);
				glVertex2fv(Pts.at(i-1).get_Vertex2D());
				glVertex2fv(Pts.at(i).get_Vertex2D());
			glEnd();
		}	
	}
}

void Bounding_Box::display_Box()	// Affichage de la Bounding-Box rectangulaire
{
	glColor3f(0.96f, 0.0f, 0.0f);
	glPushAttrib(GL_ENABLE_BIT); 
		glLineStipple(1, 0x6666);
		glEnable(GL_LINE_STIPPLE);
		// Affichage à partir des quatres sommets
		glBegin(GL_LINE_LOOP);
			glVertex2fv(this->Corner_1.get_Vertex2D());
			glVertex2fv(this->Corner_2.get_Vertex2D());
			glVertex2fv(this->Corner_3.get_Vertex2D());
			glVertex2fv(this->Corner_0.get_Vertex2D());
		glEnd();
	glPopAttrib();
}

void Bounding_Box::update()		// Gestion de la mise à jour des données
{
	// Recalcul de l'isobarycentre
	this->Center = this->iso_Barycentre();

	// Si on est en mode orienté par l'objet
	if (this->is_Oriented()) {

		// On mets tous les Points (en relatif / isoBarycentre) dans une matrice
		mat Data = zeros(this->Points.size(), 4);
		for (int i=0; i<this->Points.size(); i++) {
			Data.row(i) = this->Points.at(i).get_RowVec()-this->Center.get_RowVec();
		}
		// On calcule ses vecteurs propres qu'on limite ici au cas 2D
		vec Valeurs_Propres;
		mat Vecteurs_Propres;
		eig_sym(Valeurs_Propres, Vecteurs_Propres, cov(Data));
		Vecteurs_Propres = fliplr(Vecteurs_Propres);
		vec U = Vecteurs_Propres.col(0);
		U.resize(2);
		vec V = Vecteurs_Propres.col(1);
		V.resize(2);

		// J'impose que l'abscisse du premier vecteur propre soit positive
		if (U(0)<0.0f) {
			U = -U;
			V = -V;
		}
		// Je récupère le centre en 2D
		vec isoBary = this->Center.get_Vec();
		isoBary.resize(2);

		// On calcule les coordonnées min/max des points dans le repère
		// centré au centre de l'objet avec la base des vecteurs propres
		float uMin, uMax, vMin, vMax;
		for (int i=0; i<this->Points.size(); i++) {
			vec P = this->Points.at(i).get_Vec();
			P.resize(2);
			if (i==0) {
				// On utilise le produit scalaire
				uMin = dot(P-isoBary,U)/pow(norm(U),2);
				vMin = dot(P-isoBary,V)/pow(norm(V),2);
				uMax = dot(P-isoBary,U)/pow(norm(U),2);
				vMax = dot(P-isoBary,V)/pow(norm(V),2);
			}
			else {
				// On met à jours min & max en parcourant tous les Points
				uMin = std::min(uMin,(float)(dot(P-isoBary,U)/pow(norm(U),2)));
				vMin = std::min(vMin,(float)(dot(P-isoBary,V)/pow(norm(V),2)));
				uMax = std::max(uMax,(float)(dot(P-isoBary,U)/pow(norm(U),2)));
				vMax = std::max(vMax,(float)(dot(P-isoBary,V)/pow(norm(V),2)));
			}
		}
		// On en déduit les coordonnées des 4 sommets de la Box orientée
		// floor() et ceil() évitent les problèmes de bords sans perte visuelle
		this->Corner_0 = Point(isoBary + floor(uMin)*U + floor(vMin)*V);
		this->Corner_1 = Point(isoBary + ceil(uMax)*U + floor(vMin)*V);	
		this->Corner_2 = Point(isoBary + ceil(uMax)*U + ceil(vMax)*V);		
		this->Corner_3 = Point(isoBary + floor(uMin)*U + ceil(vMax)*V);	

	}

	// Sinon, on est en repère monde 
	else {
		// On parcourt aussi le vecteur de Point pour identifier min & max en 2D
		float xMin, xMax, yMin, yMax;
		for (int i=0; i<this->Points.size(); i++) {
			if (i==0) {
				xMin = this->Points.at(i).get_X();
				yMin = this->Points.at(i).get_Y();
				xMax = this->Points.at(i).get_X();
				yMax = this->Points.at(i).get_Y();
			}
			else {
				xMin = min(xMin, this->Points.at(i).get_X());
				yMin = min(yMin, this->Points.at(i).get_Y());
				xMax = max(xMax, this->Points.at(i).get_X());
				yMax = max(yMax, this->Points.at(i).get_Y());
			}
			// On fixe les 4 sommets de cette version de la Bounding-Box
			this->Corner_0 = Point(xMin, yMin);
			this->Corner_1 = Point(xMax, yMin);
			this->Corner_2 = Point(xMax, yMax);
			this->Corner_3 = Point(xMin, yMax);
		}
	}
}

/********************************************************************/
/*					 		Constructeur							*/
/********************************************************************/


Bounding_Box::Bounding_Box()
{
	this->Oriented = false;
}

/********************************************************************/
/*					 		Destructeur								*/
/********************************************************************/

Bounding_Box::~Bounding_Box()
{
	this->Points.clear();
}

/********************************************************************/
/*					 	Méthodes publiques							*/
/********************************************************************/

void Bounding_Box::display()
{
	this->display_Line(this->Points);	// Affiche la ligne des Points
	this->display_Box();				// ... et la Bounding-Box
}

void Bounding_Box::add(Point new_P)
{
	this->Points.push_back(new_P);		// Ajoute un Point dans le vecteur
	this->update();						// ... et on met à jour les données
}

void Bounding_Box::change_Box_Mode()
{
	// On change le mode d'affichage de la Bounding-Box
	if (this->is_Oriented()) this->Oriented = false;
	else this->Oriented = true;
	// On met à jour les données
	this->update();
}

void Bounding_Box::change_Coords(int index, float x, float y)
{
	this->Points.at(index).set_Coords(x, y);	// On change les coordonnées d'un Point
	this->update();								// ... et on met à jour les données
}

vec Bounding_Box::get_Base_U()
{
	// On définit le premier vecteur de la base de la Bounding-Box
	vec U = this->Corner_1.get_Vec() - this->Corner_0.get_Vec();
	U.set_size(3);
	return U;
}

vec Bounding_Box::get_Base_V()
{
	// On définit le second vecteur de la base de la Bounding-Box
	vec V = this->Corner_3.get_Vec() - this->Corner_0.get_Vec();
	V.set_size(3);
	return V;
}

vector<Point> Bounding_Box::get_Points() 	// Renvoie les Points pour le Picking
{
	return this->Points;
}

bool Bounding_Box::is_Oriented()	// Renvoie le mode actif de la Bounding-Box
{
	return this->Oriented;
}