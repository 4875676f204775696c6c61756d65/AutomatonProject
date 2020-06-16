#include "transition.h"



//Constructeur de transitions : prend en argument en premier l'etat de départ, en second la lettre et en dernier l'eat d'arrivé de la trannsitions.


Transition::Transition(std::string premier, std::string deuxieme, std::string troisieme):position(premier), transi(deuxieme),destination(troisieme){};


//--------------------------------------------------------------------------------------------------------------------------------------


//Destructeur de transitions :
Transition::~Transition(){};


//--------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------
