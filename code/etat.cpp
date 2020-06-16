#include "etat.h"

//--------------------------------------------------------------------------------------------------------------------------------------

//Protoytpe d'une fonction utilise plus bas.

std::string affichageCentreEtat(std::string element, int taille_case);

//--------------------------------------------------------------------------------------------------------------------------------------

//Constructeur d'état, prend en argument une chaine de caractere repésentant le nom de l'état.

Etat::Etat(std::string valeur):nom(valeur){
}

//--------------------------------------------------------------------------------------------------------------------------------------

//Destructeur d'état, delete toutes les transitions qui ont été allouer dynamiquement grace a la liste des etats.

Etat::~Etat(){
    if(!transitions.empty()){
        for(std::list<Transition*>::iterator it = transitions.begin(); it!=transitions.end(); ++it){
            if(*it != nullptr){
                delete *it;
            }
        }
        transitions.clear();
    }
}

//--------------------------------------------------------------------------------------------------------------------------------------

//Constructeur de copie, permet de construire un etat en copiant toutes les donnees d'un autre. Necessaire car les transitions sont allouer dynamiquement.

Etat::Etat(Etat & ancien){
    nom = ancien.nom;
    for(std::list<Transition*>::iterator it_transition = ancien.transitions.begin(); it_transition != ancien.transitions.end(); ++it_transition){
        transitions.push_back(new Transition((*it_transition)->getPosition(), (*it_transition)->getTransi(), (*it_transition)->getDestination()));
    }
}

//---------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------

//Setteur d'état permettant de creer une nouvelle transitions à partir d'une ligne type( ex : 1 a 4) et de l'ajouter a la liste de transistions.

void Etat::set_Transition(std::string ligne){
    
    //On commence par determiner la position des séparateur (les " ").
    int pos_prem = (int)ligne.find(' ', 0);
    int pos_deux = (int)ligne.find(' ', pos_prem + 1);
    int compteur = 0;
    
    //Conditions gerant la creation de la premiere transition de l'etat.
    if(transitions.empty()){
        compteur++;
        transitions.push_back(new Transition(ligne.substr(0, pos_prem), ligne.substr(pos_prem + 1, pos_deux - pos_prem - 1), ligne.substr(pos_deux + 1, ligne.size())));
        return;
    }
    //Conditions gérant l'ajout de nouvelle transitions a l'etat.
    else{
        for(std::list<Transition*>::iterator it_transitions = transitions.begin();it_transitions != transitions.end(); ++it_transitions){
            compteur++;
            
            //Si on atteint les transistions epsilon, qui sont placees en fin de liste, alors on inserer la transitions tout de suite.
            if((*it_transitions)->getTransi() == "*"){
                transitions.insert(it_transitions, new Transition(ligne.substr(0, pos_prem), ligne.substr(pos_prem + 1, pos_deux - pos_prem - 1), ligne.substr(pos_deux + 1, ligne.size())));
                return;
            }
            //Si notre transistion est une epsilon alors on la place directement a la fin de la liste.
            else if(ligne.substr(pos_prem + 1, pos_deux - pos_prem - 1) == "*"){
                transitions.push_back(new Transition(ligne.substr(0, pos_prem), ligne.substr(pos_prem + 1, pos_deux - pos_prem - 1), ligne.substr(pos_deux+1, ligne.size())));
                return;
            }
            //Cette conditions permet de placer les transitions en fonction de l'ordre aphabetique de leur lettre( d'abord 1 a 3 ensuite 1 b 2 par exemple).
            else if(ligne.substr(pos_prem + 1, pos_deux - pos_prem - 1) < (*it_transitions)->getTransi()){
                transitions.insert(it_transitions, new Transition(ligne.substr(0, pos_prem), ligne.substr(pos_prem + 1, pos_deux - pos_prem - 1), ligne.substr(pos_deux + 1, ligne.size())));
                return;
            }
            //Si on arrive en fin de liste on y place la transitions.
            else if(compteur == (int)transitions.size()){
                transitions.push_back(new Transition(ligne.substr(0, pos_prem), ligne.substr(pos_prem + 1, pos_deux - pos_prem - 1), ligne.substr(pos_deux+1, ligne.size())));
                return;
            }
         
            //Si on est rentrer dans aucune des conditions précédente on continu la boucle (on passe a la transitions suivante dans la liste).
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------------------

//Fonction préparant une ligne du tableau des transistions pour les automates AF. La ligne est cree dans un string qui est retourne a la fonction de Automate charge de l'affichage.

std::string Etat::retourne_ligne_transition(std::list<std::string> auto_alphabet){
    
    std::queue<Transition*> aucasou;
    int fin = 1;
    std::string ligne;
    
    //Cette partie gére l'affichage du nom de l'état.
    if (nom.size() == 1) {
        ligne += "|     " + nom + "    |";
    }else{
        ligne += "|    " + nom + "    |";
    }
    
    //Si il n'y a aucune transitions on remplis la ligne de case vide.
    if(transitions.empty()){
        for(int i = 0; i < (int)auto_alphabet.size(); i++){
            ligne += "          |";
        }
    }
    //Sinon on rentre dans cette conditions qui gére l'affichage des destinations.
    else{
        std::list<Transition*>::iterator it = transitions.begin();
        
        //On parcourt l'alphabet pour mettre dans chaque colonne de la ligne le ou les destinations correspondant a la transitions.
        for(std::list<std::string>::iterator it2 = auto_alphabet.begin(); it2!=auto_alphabet.end(); ++it2){
            if((*it2) == (*it)->getTransi()){
                while(((*it2) == (*it)->getTransi()) && (fin == 1)){
                    //On les place dans une file.
                    aucasou.push(*it);
                    it++;
                    if(it == transitions.end()){
                        fin = 0;
                        it--;
                    }
                }
                
                std::string aAfficher;
                
                //On vide la file et si plusieurs transitions sont trouvé pour la colonne on les sépare d'un (').
                while (!aucasou.empty()) {
                    if(!aAfficher.empty()){
                        aAfficher += "'";
                    }
                    aAfficher += aucasou.front()->getDestination();
                    aucasou.pop();
                }
                
                //On affiche la case (aAfficher) correspondant a la colonne actuelle (l'iteration de la boucle for).
                ligne += affichageCentreEtat(aAfficher, 10);
            }else{
                
                //Si il n'y a pas de transitions on affiche une case vide pour cet colonne.
                ligne += "          |";
                
            }
        }
    }
    //On retourne la ligne cree pour affichage.
    return ligne;
}

//---------------------------------------------------------------------------------------------

//Recherche pour chaque element de la liste si il y a une transitions epsilon symbolise par la lettre '*'. Si il y en a elle sont stocker dans la file pour etre afficher ultérieurement.
void Etat::recherche_asynchrone_transi(std::queue<Transition*>* asynchrone){
    for(std::list<Transition*>::iterator it = transitions.begin(); it!=transitions.end();++it){
        if((*it)->getTransi() == "*"){
            asynchrone->push((*it));
        }
    }
}

//---------------------------------------------------------------------------------------------

//Recherche si on retrouve le nom d'un etat de l'automate AF dans les etats deterministe.
bool Etat::chercherDansNom(std::string recherche){
    int pos_prec = (int)nom.find(".", 0);
    if(nom.substr(0, pos_prec) == recherche){
        return true;
    }
    while(pos_prec != -1){
        int pos_suiv = (int)nom.find(".", pos_prec+1);
        if(recherche == nom.substr(pos_prec+1, pos_suiv - pos_prec -1)){
            return true;
        }
        else{
            pos_prec = pos_suiv;
        }
    }
    return false;
}

//---------------------------------------------------------------------------------------------

//Recherche si on retrouve une transitions vers l'etat initial (cherche) dans cet etat.
bool Etat::recherche_standard(std::string cherche){
    for(std::list<Transition*>::iterator it_transi = transitions.begin(); it_transi != transitions.end(); ++it_transi){
        if(cherche == (*it_transi)->getDestination()){
            return true;
        }
    }
    return false;
}


//---------------------------------------------------------------------------------------------

// Verifie pour chaque transitions de l'etat si il y a bien une transistions pour chaque lettre de l'alphabet.
void Etat::verifier_complet_transi(std::queue<std::string>* stock, std::list<std::string> auto_alphabet){
    std::queue<std::string> alphabet;
    
    //Stocke tout l'alphabet dans une file pour plus de simplicité plus tard.
    for(std::list<std::string>::iterator it=auto_alphabet.begin(); it!=auto_alphabet.end(); ++it){
        alphabet.push((*it));
    }
    
    //Si il n'y aucune transitions pour cet etat on renvoie un message l'indiquant et listant les transitions manquantes(toutes) grace a la file stock.
    if(transitions.empty()){
        while(!alphabet.empty()){
            std::string ajout = nom + " n'a pas de transition " + alphabet.front();
            stock->push(ajout);
            alphabet.pop();
        }
    }
    
    //Pour chaque lettre de l'alphabet on verifie qu'il exite bien une transitions associe.
    while(!alphabet.empty()){
        std::string comparer = alphabet.front();
        bool verif = false;
        alphabet.pop();
        for(std::list<Transition*>::iterator it = transitions.begin(); it != transitions.end(); ++it){
            if(comparer == (*it)->getTransi()){
                verif = true;
            }
        }
        
        //Si il n'existe aucune transitions pour la lettre verifie on renvoi un message l'indiquant. (via le stock aussi).
        if(!verif){
            std::string ajout = nom + " n'a pas de transition " + comparer;
            stock->push(ajout);
        }
    }
}

//---------------------------------------------------------------------------------------------

//On verifie pour chaque transitions de l'etat qu'elle a bien au plus une seul transitions par lettre.
void Etat::transition_deterministe(std::queue<std::string>* stock_deterministe, std::list<std::string> auto_alphabet){
    if(!transitions.empty()){
        std::list<Transition*>::iterator it = transitions.begin();
        int compteur = 0;
        int fin = 1;
        for(std::list<std::string>::iterator it2 = auto_alphabet.begin(); it2 != auto_alphabet.end(); ++it2){
            while(((*it2) == (*it)->getTransi()) && ( fin == 1)){
                compteur++;
                it++;
                if(it == transitions.end()){
                    it--;
                    fin = 0;
                }
            }
            if(compteur > 1){
                stock_deterministe->push("-" + nom + " a plusieurs transitions " + (*it2));
            }
            compteur = 0;
        }
    }
}

//---------------------------------------------------------------------------------------------

//Pour chaque lettre de l'alphabet n'ayant aucune transitions on en creer une nouvelle vers l'etat poubelle P.
int Etat::completer_etat(std::list<std::string> alphabet){
    int nbr_transition = 0;
    
    //Si il n'y aucune transitions on creer une transitions vers P poour chaque lettre
    if(transitions.empty()){
        for(std::list<std::string>::iterator it_alphabet = alphabet.begin(); it_alphabet!= alphabet.end(); ++it_alphabet){
            set_Transition(nom + " " + (*it_alphabet) + " " + "P");
            nbr_transition++;
        }
    }
    else{
        for(std::list<std::string>::iterator it_alphabet = alphabet.begin(); it_alphabet!= alphabet.end(); ++it_alphabet){
            int compteur = 0;
            for(std::list<Transition*>::iterator it_transition = transitions.begin(); it_transition != transitions.end(); ++it_transition){
                compteur++;
                if((*it_alphabet) == (*it_transition)->getTransi()){
                    it_transition = transitions.end();
                    it_transition--;
                }
                //Si il n'y a pas de transitions pour cette lettre et qu'on a parcouru toutes la liste on rajoute une transitions vers P
                else{
                    if(compteur == (int)transitions.size()){
                        set_Transition(nom + " " + (*it_alphabet) + " " + "P");
                        nbr_transition++;
                    }
                }
            }
        }
    }
    return nbr_transition;
}

//---------------------------------------------------------------------------------------------

//Modifie les transitions pour permettre la determisation de l'automate. Cree les nouveuax etat necessaire pour que ces transitions fonctionnent.
void Etat::determiniser_Etat(std::queue<std::string>* stock_nouv_etat, std::list<Etat*> etats_automate, std::list<std::string> alphabet){
    
    std::string nouv = nom;
    int pos = 0;
    std::list<std::string> etat_compose;
    
    //On decompose le nom de l'etat pour recuperer ses etat d'origine (de AF).
    while(pos != -1){
        int pos_prec = pos;
        pos = (int)nouv.find(".", pos_prec+1);
        if((pos_prec) == 0 && (pos == -1)){
            etat_compose.push_back(nouv);
        }
        else if(pos_prec == 0){
            etat_compose.push_back(nouv.substr(0, pos));
        }
        else if(pos == -1){
            etat_compose.push_back(nouv.substr(pos_prec + 1, nouv.size()));
        }
        else{
            etat_compose.push_back(nouv.substr(pos_prec + 1, pos - pos_prec - 1));
        }
    }
    
    //On boucle sur l'alphabet pour creer toutes les transitions necessaire.
    for(std::list<std::string>::iterator it_alphabet = alphabet.begin(); it_alphabet != alphabet.end(); ++it_alphabet){
        std::string ajout;
        std::list<std::string> element_nom;
        
        //On boucle sur tout les états d'origine pour recurperer leur transitions.
        for(std::list<std::string>::iterator it_compose = etat_compose.begin(); it_compose != etat_compose.end(); ++it_compose){
            
            //On recherche cet etat dans la liste des etats.
            for(std::list<Etat*>::iterator it_Etat = etats_automate.begin(); it_Etat != etats_automate.end(); ++it_Etat){
                
                //Si on l'a trouve on rentre dans la boucle
                if((*it_Etat)->getNom() == (*it_compose)){
                    
                    //On boucle sur ses transitions.
                    for(std::list<Transition*>::iterator it_transi = (*it_Etat)->transitions.begin(); it_transi !=(*it_Etat)->transitions.end(); ++it_transi){
                        
                        //Si on trouve la ou les transitions correspondantes a la lettre de l'alphabet actuellement traite.
                        if((*it_alphabet) ==  (*it_transi)->getTransi()){
                            
                            bool presence = false;
                            
                            //On verifie que la destinations de la transitions n'a pas deja ete ajoute.
                            for(std::list<std::string>::iterator it_element = element_nom.begin(); it_element != element_nom.end();++it_element){
                                if((*it_transi)->getDestination() == (*it_element)){
                                    presence = true;
                                }
                            }
                            
                            // Si elle n'a pas encore ete ajouter on le fait.
                            if(!presence){
                                
                                int fin = 0;
                                bool bien_ajoute = false;
                                std::list<std::string>::iterator it_element = element_nom.begin();
                                
                                //On trie dans l'ordre croissant pour ne pas creer plusieurs fois le meme etat ecrit differemment. Ex: 1.2.3 et 2.3.1 = meme etat ecrit differemment.
                                while(!bien_ajoute && fin == 0){
                                    
                                    if(element_nom.empty()){
                                        element_nom.push_front((*it_transi)->getDestination());
                                        bien_ajoute = true;
                                    }
                                    else if((*it_transi)->getDestination() < (*it_element)){
                                        
                                        if(it_element != element_nom.begin()){
                                            element_nom.insert(it_element, (*it_transi)->getDestination());
                                            bien_ajoute = true;
                                        }
                                        else{
                                            element_nom.push_front((*it_transi)->getDestination());
                                            bien_ajoute = true;
                                        }
                                    }
                                    else{
                                        it_element++;
                                        
                                        if(it_element == element_nom.end()){
                                            fin = 1;
                                        }
                                    }
                                }
                                
                                if(!bien_ajoute){
                                    element_nom.push_back((*it_transi)->getDestination());
                                }
                            }
                        }
                    }
                }
            }
        }
        
        // On cree le nom de l'etat avec les destinations trouvees precedemment. On utilise le '.' comme separateur.
        for(std::list<std::string>::iterator it_ele = element_nom.begin(); it_ele != element_nom.end(); ++it_ele){
            
            if(it_ele != element_nom.begin()){
                ajout += "." + (*it_ele);
            }
            else{
                ajout = (*it_ele);
            }
        }
       
        //Si le nom de l'etat n'est pas vide on l'ajoute au stock de nouveaux etats crees.
        if(ajout.size()!= 0){
            set_Transition(nom + " " + (*it_alphabet)+ " " + ajout);
            stock_nouv_etat->push(ajout);
        }
    }
    
}

//---------------------------------------------------------------------------------------------

//Permet d'ajouter une transitions deja cree a l'etat.
void Etat::ajouterTransition(Transition* ajout){
    if(transitions.empty()){
        transitions.push_back(ajout);
    }
    else{
        for(std::list<Transition*>::iterator it_transi = transitions.begin(); it_transi != transitions.end(); ++it_transi){
            if(ajout->getTransi() < (*it_transi)->getTransi() && it_transi == transitions.begin()){
                transitions.push_front(ajout);
                return;
            }
            if(ajout->getTransi() < (*it_transi)->getTransi()){
                transitions.insert(it_transi,ajout);
                return;
            }
        }
        transitions.push_back(ajout);
    }
}

//---------------------------------------------------------------------------------------------

//Permet d'effacer la liste des transitions dans l'etat.
void Etat::effacerlistTransi(){
    if(!transitions.empty()){
        for(std::list<Transition*>::iterator it_transi = transitions.begin(); it_transi != transitions.end(); ++it_transi){
            if(*it_transi != nullptr){
                delete (*it_transi);
            }
        }
        transitions.clear();
    }
}

//---------------------------------------------------------------------------------------------

//Recherche et retourne la destination de la transitions de la lettre passe en parametre.
std::string Etat::rechercheTransition(std::string Lettre){
    for(std::list<Transition*>::iterator it = transitions.begin(); it != transitions.end(); ++it){
        if((*it)->getTransi()==Lettre){
            return (*it)->getDestination();
        }
    }
    //Si on ne trouve pas on retourne un espace.
    return " ";
}

//---------------------------------------------------------------------------------------------

//A partir d'un separateur permet de retrouver et de retourner les etat d'origine d'un etat compose (suite a une determinisation ou une minimisation). Par exemple 1.3.4 donnera 1 et 3 et 4.
std::queue<std::string> Etat::reconnaitre_etat_origine(char separateur)
{
    std::string atraiter = nom;
    std::queue<std::string> retour;
    int position = (int)atraiter.find(separateur, 0);
    retour.push(atraiter.substr(0, position));
    
    //On boucle tant que l'on est pas au bout de la chaine de caractere.
    while (position != -1)
    {
        int positionsuiv = (int)atraiter.find(separateur, position + 1);
        
        //On rentre a chaque fois les etat trouve dans la file qui sera retourner.
        
        if (positionsuiv == -1)
        {
            retour.push(atraiter.substr(position + 1, positionsuiv - position));
            position = positionsuiv;
        }
        else
        {
            retour.push(atraiter.substr(position + 1, (positionsuiv - position) - 1));
            position = positionsuiv;
        }
    }
    
    // On retourne la file
    return retour;
}

//---------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------

//Retourne une case dont la taille est donnee en parametre et dont l'element lui aussi donne en parametre est centre.
std::string affichageCentreEtat(std::string element, int taille_case){
    
    int diff = (int)(taille_case - element.size());
    
    if (diff < 0) {
        diff = 0;
    }
    
    std::string ligne = "";
    
    for (int i = 0; i < ( (diff / 2) + (diff%2)) ; i++){
        ligne += " ";
    }
        
    ligne += element;
    
    for (int i = 0; i < ( diff / 2); i++){
        ligne += " ";
    }
    ligne += "|";
    
    return ligne;

}
