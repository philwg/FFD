#include "Point.hpp"

using namespace std;
using namespace arma;

/********************************************************************/
/*					 	Méthodes privées							*/
/********************************************************************/

void Point::set_X(float new_x)	// Fixe l'abscisse du Point
{
	this->x = new_x;
}

void Point::set_Y(float new_y)	// Fixe l'ordonnée du Point
{
	this->y = new_y;
}

void Point::set_Z(float new_z)	// Fixe la cote du Point
{
	this->z = new_z;
}

/********************************************************************/
/*					 		Constructeurs							*/
/********************************************************************/

Point::Point()	
{
	this->set_Coords(0.0f, 0.0f, 0.0f);		// Initialisation à l'origine ...
	this->set_Weight(1.0f);					// ... le poids de 1
}

Point::Point(float init_x, float init_y)
{
	this->set_Coords(init_x, init_y, 0.0f);		// Initialisation en (x,y) ...
	this->set_Weight(1.0f);						// ... z à 0 et le poids à 1
}

Point::Point(float init_x, float init_y, float init_z)
{
	this->set_Coords(init_x, init_y, init_z);	// Intialisation à (x,y,z) ...
	this->set_Weight(1.0f);						// ... le poids à 1
}

Point::Point(float init_x, float init_y, float init_z, float init_w)
{
	this->set_Coords(init_x, init_y, init_z);	// Initialisation à (x,y,z) ...
	this->set_Weight(init_w);					// ... le poids à w
}

Point::Point(arma::vec init_P)
{
	this->set_At(init_P);		// Initilisation à partir des coordonnées d'un vecteur
}

/********************************************************************/
/*					 			Destructeur							*/
/********************************************************************/

Point::~Point()
{}

/********************************************************************/
/*					 		Méthodes publiques						*/
/********************************************************************/

void Point::set_Weight(float new_w)		// Change le poids
{
	this->w = new_w;
}

void Point::set_Coords(float new_x, float new_y)	// change les coordonnées en 2D
{
	this->set_Coords(new_x, new_y, 0.0f);			
}

void Point::set_Coords(float new_x, float new_y, float new_z) 	// change les coordonnées en 3D
{
	this->set_X(new_x);
	this->set_Y(new_y);
	this->set_Z(new_z);
}

void Point::set_At(vec new_P)	// Change les coordonnées à partir d'un vecteur
{
	// On initialise la position à (0,0,0)
	this->set_Coords(0.0f, 0.0f, 0.0f);
	// et le poids à 1
	this->set_Weight(1.0f);
	// si il a plus de deux éléments,
	// On met à jour en fonction du nombre d'éléments disponibles dans le vecteur
	if (new_P.n_elem >= 2) {
		this->set_X(new_P(0));
		this->set_Y(new_P(1));
	}
	if (new_P.n_elem >= 3) this->set_Z(new_P(2));
	if (new_P.n_elem >= 4) this->set_Weight(new_P(3));
}

float Point::get_X()	// Renvoie l'abscisse du Point
{
	return this->x;
}

float Point::get_Y()	// Renvoie l'ordonnée du Point
{
	return this->y;
}

float Point::get_Z()	// Renvoie la cote du Point
{
	return this->z;
}

float Point::get_Weight()	// Renvoie le poids du Point
{
	return this->w;
}

vec Point::get_Vec()	// Renvoie le Point en vecteur colonne
{
	vec v = zeros(4);
	v(0) = this->get_X();
	v(1) = this->get_Y();
	v(2) = this->get_Z();
	v(3) = this->get_Weight();
	return v;
}

rowvec Point::get_RowVec()	// Renvoie le Point en vecteur ligne
{
	rowvec rv = zeros<rowvec>(4);
	rv(0) = this->get_X();
	rv(1) = this->get_Y();
	rv(2) = this->get_Z();
	rv(3) = this->get_Weight();
	return rv;
}

float* Point::get_Vertex2D()	// Renvoie un Vertex2D pour glVertex2fv()
{
	return new float[2] { this->get_X(), this->get_Y() };
}

float* Point::get_Vertex()		// Renvoie un Vertex 3D pour glVertex3fv()
{
	return new float[3] { this->get_X(), this->get_Y(), this->get_Z() };
}

Point Point::get_Image_T(vec delta)		// Renvoie un Point translaté
{
	// On initialise la translation à 0
	vec dlt = zeros(4);
	Point P;
	// On s'adapte à la taille de delta ...	
	if (delta.n_elem > 0) {
		for (int i=0; i<delta.n_elem; i++) {
			dlt(i) = delta(i);
		}
	}
	P.set_At(this->get_Vec() + dlt);
	return P;
}

Point Point::get_Image_S(vec sigma)		// Renvoie un Point mis à l'échelle
{
	// On initialise les coefficients à 1
	vec sgm = ones(4);
	Point P;
	// On s'adpate à la taille de sigma
	if (sigma.n_elem > 0) {
		for (int i=0; i<sigma.n_elem; i++) {
			sgm(i) = sigma(i);
		}
	}
	P.set_At(this->get_Vec() % sgm);
	return P;
}

Point Point::get_Image_R2D(Point center_P, float alpha)		// Renvoie l'image par rotation 2D
{
	Point P;
	vec V = this->get_Vec() - center_P.get_Vec();
	// On s'adapte à la taille par défaut qui est de 4
	mat R = { { cos(alpha), -sin(alpha), 0.0f, 0.0f},
			  { sin(alpha),  cos(alpha), 0.0f, 0.0f},
			  { 	  0.0f,		   0.0f, 1.0f, 0.0f},
			  {		  0.0f, 	   0.0f, 0.0f, 1.0f} };
	P.set_At(center_P.get_Vec() + V * R);
	return P;
}

Point Point::get_Middle_With(Point other_P)		// Renvoie le milieu avec un autre Point
{
	Point P;
	P.set_At((this->get_Vec() + other_P.get_Vec())/2.0f);
	return P;
}

float Point::get_Distance_To(Point other_P)		// Renvoie la distance euclidienne à autre Point
{
	vec delta = other_P.get_Vec() - this->get_Vec();
	// On ne considère que les coordonnées de position
	delta.set_size(3);
	return norm(delta);
}