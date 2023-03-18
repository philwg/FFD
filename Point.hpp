#ifndef POINT
#define POINT

#include <armadillo>

class Point {

private:

	float x;		// Abscisse du Point
	float y;		// Ordonnée du Point
	float z;		// Cote du Point
	float w;		// Poids du Points

	void set_X(float new_x);	// Fixe l'abscisse
	void set_Y(float new_y);	// Fixe l'ordonnée
	void set_Z(float new_z);	// Fixe la cote

public:

	Point();														// Constructeur par défaut ...
	Point(float init_x, float init_y);								// ... avec (x,y)
	Point(float init_x, float init_y, float init_z);				// ... avec (x,y,z)
	Point(float init_x, float init_y, float init_z, float init_w);	// ... acvec (x,y,z,w)
	Point(arma::vec P);												// ... à partir d'un vecteur

	~Point();											// Destructeur

	void set_Weight(float new_w);								// Fixe le Poids
	void set_Coords(float new_x, float new_y);					// Fixe (x,y)
	void set_Coords(float new_x, float new_y, float new_z);		// Fixe (x,y,z)
	void set_At(arma::vec new_P);								// Fixe aux coordonnées du vecteur

	float get_X();										// Renvoie l'abscisse
	float get_Y();										// Renvoie l'ordonnée
	float get_Z();										// Renvoie la cote
	float get_Weight();									// renvoie le Poids
	arma::vec get_Vec();								// Renvoie le Point sous forme de vecteur
	arma::rowvec get_RowVec();							// Renvoie le Point en vecteur ligne
	float* get_Vertex2D();								// Renvoie un Vertex 2D du Point 
	float* get_Vertex();								// Renvoie un vertex 3D du Point
	Point get_Image_T(arma::vec delta);					// Renvoie le translaté du Point
	Point get_Image_S(arma::vec sigma);					// Renvoie la mise à l'échelle du Point
	Point get_Image_R2D(Point center_P, float alpha);	// Renvoie l'image par rotation 2D du Point
	Point get_Middle_With(Point other_P);				// Renvoie le milieu avec un autre Point
	float get_Distance_To(Point other_P);				// Renvoie la distance à un autre Point

};

#endif