#ifndef ETAT_H_INCLUDED
#define ETAT_H_INCLUDED
#include "transition.h"


class Etat{
private:
    std::string nom;
    std::list<Transition*> transitions;
public:
    Etat(std::string valeur);
    ~Etat();
    Etat(Etat & ancien);
    
    void set_Transition(std::string ligne);
    void ajouterTransition(Transition* ajout);
    void effacerlistTransi();
    void recherche_asynchrone_transi(std::queue<Transition*>* asynchrone);
    void verifier_complet_transi(std::queue<std::string>* stock, std::list<std::string> auto_alphabet);
    void transition_deterministe(std::queue<std::string>* stock_deterministe, std::list<std::string> auto_alphabet);
    void determiniser_Etat(std::queue<std::string>* stock_nouv_etat, std::list<Etat*> etats_automate, std::list<std::string> alphabet);
    
    //Permet de changer le nom d'un etat.
    inline void setNom(std::string nouv_nom){nom = nouv_nom;};
    //Permet de recupere le nom d'un etat
    inline std::string getNom() {return nom;};
    //Permet de recuperer un pointeur sur la liste de transitions
    inline std::list<Transition*>* getTransitions(){return &transitions;};
    
    bool chercherDansNom(std::string recherche);
    bool recherche_standard(std::string cherche);
    
    int completer_etat(std::list<std::string> alphabet);
    
    std::string rechercheTransition(std::string Lettre);
    std::string retourne_ligne_transition(std::list<std::string> auto_alphabet);
    
    std::queue<std::string> reconnaitre_etat_origine(char separateur);
};

//Structure utilise pour la minimisation.
typedef struct Case_minimal{
    std::list<Transition> definition;
    std::list<Etat*> stockage;
}case_minimal;

typedef struct Embranchement{
    std::list<std::string> groupe;
    std::list<case_minimal*> cases;
}embranchement;


#endif // ETAT_H_INCLUDED
