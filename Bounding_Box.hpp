#ifndef BOUNDING_BOX
#define BOUNDING_BOX

#define ARMA_USE_LAPACK				// La fonction arma::eig_sym() s'appuie sur LAPACK ...
#define ARMA_DONT_USE_WRAPPER		// ... mais sans WRAPPER !
#include <armadillo>
#include <vector>
#include <GL/glut.h>
#include "Point.hpp"

class Bounding_Box {

protected:

	std::vector<Point> Points;		// Le vecteur des Points saisis

	bool Oriented;			// la Bounding-Box est elle orienté par le nuage de Points ?

	Point Center;			// L'isobarycentre des Points saisis
	Point Corner_0;			// Le Point en bas à gauche de la Bounding-Box
	Point Corner_1;			// Le Point en bas à droite de la Bounding-Box
	Point Corner_2;			// Le Point en haut à droite de la Bounding-Box
	Point Corner_3;			// Le Point en haut à gauche de la Bounding-Box

	Point iso_Barycentre();						// Le calcul de l'isobarycentre
	void display_Line(std::vector<Point> Pts);	// L'affichage de la ligne reliant les Points
	void display_Box();							// L'affichage de la Bounding-Box
	void update();								// La mise à jour des données

public:

	Bounding_Box();			// Constructeur
	~Bounding_Box();		// Destructeur

	void display();										// Gestion de l'affichage global
	void add(Point new_P);								// Gestion de l'ajout d'un Point
	void change_Box_Mode();								// Gestion du modede la Bounding-Box
	void change_Coords(int index, float x, float y);	// Gestion des changements de coordonnées d'un Point

	arma::vec get_Base_U();		// Le premier vecteur de base du repère de la Bounding-Box
	arma::vec get_Base_V();		// ... et le deuxième !

	std::vector<Point> get_Points();	// Renvoie un vecteur contenant tous les Points pour le Picking
	bool is_Oriented();					// Teste le mode actif de la Bounding-Box

};

#endif