#ifndef AUTOMATE_H_INCLUDED
#define AUTOMATE_H_INCLUDED
#include "etat.h"

class Automate{

private:
    
    std::string nom;
    int nbr_symbole;
    std::list<std::string> alphabet;
    int nbr_etat;
    int nbr_etats_initiaux;
    int nbr_etats_terminaux;
    std::list <Etat*> etats_initiaux;
    std::list <Etat*> etats_terminaux;
    int nbr_transition;
    std::list <Etat*> etats_automate;
    
public:
    
    Automate(std::ifstream& mon_fichier);
    ~Automate();
    
    //Cette methode permet de mettre le nom de l'autome sur AFDC.
    inline void setAFDC(){nom = "AFDC";};
    
    Etat* retourne_Etat(std::string rechercher);
    
    void afficherAlphabet();
    void afficherEtat();
    void afficher_automate_AF();
    void afficher_automate_AFDC();
    void afficher_automate_AFDCM();
    void afficher_automate_Autre();
    
    std::string creer_interligneTab(int taille);
    
    void completer_automate();
    void determiniser_automate_synchrone();
    void complementaire_automate();
    void standardiser();
    void synchroniser();
    void ajouterTransition(Transition* ajout);
    void effacerTransition();
    
    bool est_asynchrone();
    bool est_complet();
    bool est_deterministe();
    bool est_standard();
    
    int lire_mot(std::string mot);
    
    void demander_mot();
    void minimiser_automate();
    
    void remplir_tableau_minimal(std::list<embranchement>* nouveau_tableau);
    void creer_nouveaux_etats(std::list<embranchement>* nouveaux_etat);
    void nouveau_etat_terminaux();
    void nouvel_etat_initial();
    
};

Automate* creation_automate();

#endif // AUTOMATE_H_INCLUDED
