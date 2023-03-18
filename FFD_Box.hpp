#ifndef FFD_BOX
#define FFD_BOX

#include <armadillo>
#include <GL/glut.h>
#include "Point.hpp"
#include "Bounding_Box.hpp"

class FFD_Box : public Bounding_Box {

private:

	bool Gelled;		// Gelée ou pas ?
	bool Curved;		// Déformation des segments ou pas ?

	int U_degree;		// nombre de découpe selon U
	int V_degree;		// nombre de découpe selon V

	std::vector<std::vector<Point>> Hooks;		// Les crochets pour déformer
	std::vector<arma::vec> UV_Coords;			// Les coordonnées (u,v) des Points dans la Bounding-Box
	std::vector<Point> FFD_Points;				// Les Points recalculés à partir de (u,v)

	int Pascal(int i, int n);					// Calcul des coefficients du triangle de Pascal
	float Bernstein(int i, int n, float t);		// Calcul des polynômes de Bernstein (--> Bézier)

	void compute_UV();				// Calcul des coordonnées dans le repère de la Box
	void compute_Hooks();			// Calcul des crochets de déformation selon les degrés
	void compute_FFD();				// Recalcul de tous les Points à partir des coordonnées (u,v)

	Point get_Point_at(arma::vec UV);	// Recalcul d'un Point à partir de ses coordonnées (u,v)
	void display_Hooks();				// Affichage des crochets de déformation
	void display_Boundaries();			// Affichage du Pourtour (éventuellement déformé)
	void display_Curve();				// Affcichages des segments déformés

public:

	FFD_Box();		// Constructeur
	~FFD_Box();		// Destructeur

	void display();		// Gestion de l'affichage global (surcharge)

	void set_Gelled();				// Fixation du mode "Gelée" 
	void change_Mode();				// Gestion du mode (saisie / Gelée)
	void change_Lines_Mode();		// Gestion du mode de tracé des segments

	void set_U_Degree(int new_Hd);			// Fixation du degré en U
	void change_U_Degree(int delta_h);		// Modification du degré en U
	void set_V_Degree(int new_Vd);			// Fixation du degré en V
	void change_V_Degree(int delta_v);		// Modification du degré en V

	bool is_Gelled();		// Renvoie le mode courant (saisie / Gelée)
	bool is_Curved();		// Renvoie le mode courant de tracé des segments

	std::vector<Point> get_Hooks();					// Renvoie les crochets pour le Picking
	void change_Hook(int index, float x, float y);	// Modifie la position des crochets en mode "Gelée"

};

#endif