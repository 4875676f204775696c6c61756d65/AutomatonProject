#include "automate.h"

//Declaration de prototypes de fonctions utilise plus tard.
void affichageCentre(std::string element, int taille_case);
bool chercherNomEtat(std::string recherche, std::list<Etat*> etat_anciens);

//Fonction permettant l'ouverture du fichier et construisant l'automate asoscie par appelle du constructeur. Retourne un pointeur sur l'automate cree.
Automate* creation_automate(){
    while(1){
        std::cout << "Veuillez choisir votre automate"<< std::endl;
        std::string choix;
        getline(std::cin, choix);
        std::string titre = "BN7-" + choix + ".txt";
        std::ifstream mon_fichier(titre.c_str());
        if(mon_fichier){
            std::cout << "le fichier de l'automate " + choix + " a bien ete ouvert"<<std::endl;
            Automate* nouv_automate = new Automate(mon_fichier);
            return nouv_automate;
        }
        else{
            std::cout << "le fichier de l'automate n'a pas reussi a s'ouvrir veuillez recommencer" << std::endl;
        }
    }
}

//Fonction permettant la conversion de n'importe quel type de base en string. Recoder (to_string()) pour eviter tout soucis de compatibilite.
template <class T>
std::string convert_to_string(T element){
    
    std::stringstream flux;
    flux << element;
    return flux.str();
    
}

//---------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------

//Constructeur de la classe automate. Permet a partir d'un flux entrant sur un fichier (.txt) de recolter ligne par ligne les informations necessaire puis de les traiter.
Automate::Automate(std::ifstream& mon_fichier){
    nom = "AF";
    std::string sortie;
    
    //On recuperer la premiere ligne. La strcture du fichier est celle presenter dans le sujet.
    std::getline(mon_fichier, sortie);
    nbr_symbole = atoi(sortie.c_str());
    
    //Ici en fonction du nombre de symbole on rempli notre alphabet sous forme de liste de string(chaine de caractere).
    for(int i = 0; i < nbr_symbole; i++){
        char ajout = 'a' + i;
        std::string ajout_string = convert_to_string(ajout);
        alphabet.push_back(ajout_string);
    }
    
    //On recupere ici la deuxieme ligne a savoir le nombre d'etats.
    std::getline(mon_fichier, sortie);
    nbr_etat = atoi(sortie.c_str());
    
    //Ici a l'aide du nombre d'etat nous remplisssons notre liste d'etats avec des pointeur sur des element de class Etat.
    for(int i = 0; i < nbr_etat; i++){
        std::string nom_Etat = convert_to_string(i);
        Etat* nouv = new Etat(nom_Etat);
        etats_automate.push_back(nouv);
    }
    
    //ici nous recuperons la troisieme ligne c'est a dire le nombre et le nom de tout les etats initiaux en prenant soin de la decouper en fonction des separateur (espaces) pour pouvoir traiter chaque information individuellement.
    std::getline(mon_fichier, sortie);
    std::string chaine = sortie;
    int position_prec = (int)chaine.find(" ", 0);
    nbr_etats_initiaux = std::atoi(chaine.substr(0, position_prec).c_str());
    
    for(int i = 0; i < nbr_etats_initiaux; i++){
        int position_suivante = (int)chaine.find(' ', position_prec + 1);
        if(position_suivante == -1){
            etats_initiaux.push_back(retourne_Etat(chaine.substr(position_prec + 1, chaine.size())));
        }
        else{
            etats_initiaux.push_back(retourne_Etat(chaine.substr(position_prec + 1, (position_suivante - position_prec - 1))));
            position_prec = position_suivante;
        }
    }
    
    //On recupere la quatrieme ligne selon le meme modele que le systeme precedent mais cette fois pour les etats terminaux.
    std::getline(mon_fichier, sortie);
    chaine = sortie;
    position_prec = (int)chaine.find(' ', 0);
    nbr_etats_terminaux = std::atoi(chaine.substr(0, position_prec).c_str());
    
    for(int i = 0; i < nbr_etats_terminaux; i++){
        int position_suivante = (int)chaine.find(' ', position_prec+1);
        if(position_suivante == -1){
            etats_terminaux.push_back(retourne_Etat(chaine.substr(position_prec + 1, chaine.size())));
        }
        else{
            etats_terminaux.push_back(retourne_Etat(chaine.substr(position_prec + 1, position_suivante - position_prec - 1)));
            position_prec = position_suivante;
        }
    }
    
    //On recupere maintenant la cinquieme ligne qui nous donne le nombre de transitions existante dans l'automate.
    std::getline(mon_fichier, sortie);
    nbr_transition = std::atoi(sortie.c_str());
    
    //Maintenant nous recuperons ligne par ligne chaque transitions jusqu'a la fin du fichier et nous l'envoyons a l'etat conerner pour qu'il cree la transitions asoscie (class transition).
    while(std::getline(mon_fichier, sortie)){
        int pos = (int)sortie.find(' ', 0);
        Etat* actuel = retourne_Etat(sortie.substr(0 , pos));
        actuel->set_Transition(sortie);
    }
    
    //Si l'automate est asynchrone nous ajoutons a l'alphabet le symbole '*' pour des raisons d'affichage (affichage AF).
    if(est_asynchrone()){
        alphabet.push_back("*");
        nbr_symbole++;
    }
}

//---------------------------------------------------------------------------------------------

//On itere sur la liste des etats et on detruit chaque element allouer dynamiquement.
Automate::~Automate(){
    if(!etats_automate.empty()){
        for(std::list<Etat*>::iterator it = etats_automate.begin(); it!=etats_automate.end(); ++it){
            if(*it != nullptr){
                delete *it;
            }
        }
        etats_automate.clear();
    }
}

//---------------------------------------------------------------------------------------------

//Cette methode nous permet de trouver et de retourner un pointeur sur un etat a partir de son nom passe en parametre (rechercher).
Etat* Automate::retourne_Etat(std::string rechercher){
    for(std::list<Etat*>::iterator it = etats_automate.begin(); it !=etats_automate.end(); ++it){
        
        if(rechercher == (*it)->getNom()){
            return *it;
        }
    }
    
    std::cout << "Erreur mauvaise recherche pour " << rechercher << std::endl;
    return nullptr;
}

//---------------------------------------------------------------------------------------------

//Cette methode permet d'afficher l'alphabet de notre automate. Elle est utilise dans les divers affichages.
void Automate::afficherAlphabet(){
    
    std::cout << "l'automate reconnait le langage suivant :";
    
    if (alphabet.empty()) {
        std::cout <<"Il n'y aucun alphabet."<< std::endl;
    }else{
        
        for(std::list<std::string>::iterator it = alphabet.begin(); it!=alphabet.end() ; ++it){
            
            //On n'affiche pas epsilon car ce n'est pas a proprement parle une lettre de l'alphabet reconnu mais il a ete stocke dans l'alphabet pour faciliter son utilisation.
            if((*it) != "*"){
                if(it != alphabet.begin()){
                    std::cout << " ";
                }
                std::cout << (*it);
            }
        }
        std::cout << "." << std::endl;
        
    }
    
}

//---------------------------------------------------------------------------------------------

//On affiche les informations sur les etats d'automate (nbr, liste des initiaux et terminaux).
void Automate::afficherEtat(){
    
    std::cout << "L'automate a " << nbr_etat << " etats." << std::endl;
    
    if(etats_initiaux.empty()){
        std::cout<< "Il n'y aucun etat initial." << std::endl;
    }
    else{
        
        if (etats_initiaux.size() == 1) {
            std::cout << "Il y a " << nbr_etats_initiaux << " etat initial:";
        }else{
            std::cout << "Il y a " << nbr_etats_initiaux << " etats initiaux:";
        }
        
        for(std::list<Etat*>::iterator it = etats_initiaux.begin(); it!= etats_initiaux.end(); ++it){
            if(it!=etats_initiaux.begin()){
                std::cout << " ; ";
            }
            std::cout <<(*it)->getNom();
        }
        std::cout << "." << std::endl;
    }
    if(etats_terminaux.empty()){
        std::cout<< "Il n'y aucun etat terminal." << std::endl;
    }
    else{
        
        if (etats_terminaux.size() == 1) {
            std::cout << "Il y a " << nbr_etats_terminaux << " etat terminal:";
        }else{
            std::cout << "Il y a " << nbr_etats_terminaux << " etats terminaux:";
        }
        
        for(std::list<Etat*>::iterator it = etats_terminaux.begin(); it!= etats_terminaux.end(); ++it){
            if(it!=etats_terminaux.begin()){
                std::cout << " ; ";
            }
            std::cout <<(*it)->getNom();
        }
        std::cout << "." << std::endl;
    }
    
}

//---------------------------------------------------------------------------------------------

//On creer ici un string contenant une interligne(separation) pour nos affichages de tableau de transitions.
std::string Automate::creer_interligneTab(int taille){
    
    std::string separation = "|";
    for(int i = 0; i < 10 + nbr_symbole*taille; i++){
        separation += "-";
    }
    separation += "|";
    
    return separation;
    
}


//---------------------------------------------------------------------------------------------

//Permet d'afficher toutes les informations demande pour un automate automate fini qu'il soit synchrone ou asynchrone.
void Automate::afficher_automate_AF(){
    std::cout << std::endl << std::endl;
    
    std::cout <<"Affichage de l'automate " << nom <<"."<< std::endl;
    
    afficherAlphabet();
    
    afficherEtat();
    
    std::cout << "Il y a " << nbr_transition << " transitions." << std::endl;
    
    std::cout << std::endl << std::endl;
    
    std::string separation = "";
    std::string dixespace = " ";
    std::string entrant = "    ->   ";
    std::string sortant = "    <-   ";
    std::string entrantsortant = "  ->/<-  ";
    
    for (int i = 0; i < 8; i++)
    {
        dixespace += " ";
    }
    
    separation += creer_interligneTab(11);
    
    std::cout << dixespace << separation << std::endl;
    
    std::cout << dixespace << "|    " << nom << "    |";
    
    for(std::list<std::string>::iterator it=alphabet.begin(); it!=alphabet.end(); ++it){
        std::cout << "     " << (*it) << "    |";
    }
    
    std::cout << std::endl;
    std::cout << dixespace << separation << std::endl;
    
    //Ici on va pour chaque etat avant d'afficher la ligne du tableau corrspondante verifier si il est entrant ou sortant ou bien les deux pour l'afficher sous forme de fleche avant la ligen.
    for(std::list<Etat*>::iterator it = etats_automate.begin(); it!=etats_automate.end(); ++it){
        
        bool init = false;
        bool termi = false;
        
        for (std::list<Etat *>::iterator it_init = etats_initiaux.begin(); it_init != etats_initiaux.end(); ++it_init)
        {
            
            if ((*it)->getNom() == (*it_init)->getNom())
            {
                init = true;
            }
            
        }
        
        for (std::list<Etat *>::iterator it_init = etats_terminaux.begin(); it_init != etats_terminaux.end(); ++it_init)
        {
            
            if ((*it)->getNom() == (*it_init)->getNom())
            {
                termi = true;
            }
            
        }
        
        if (init == false && termi == false)
        {
            std::cout << dixespace;
        }
        
        if (init == true && termi == false)
        {
            std::cout << entrant;
        }
        
        if (init == false && termi == true)
        {
            std::cout << sortant;
        }
        
        if (init == true && termi == true)
        {
            std::cout << entrantsortant;
        }
        
        std::cout << (*it)->retourne_ligne_transition(alphabet) << std::endl;
        std::cout << dixespace << separation << std::endl;
    }
    
    std::cout << std::endl << std::endl;
    
}

//----------------------------------------------------------------------------------------------------

//Fonction proche de la précédente a  ceci pres qu'elle est concu pour des automates deterministe et complet elle ne permet pas d'en afficher des asynchrone ou non deterministe. Elle permet par contre d'afficher les etats d'origine(AF) de chaque etat de l'automate AFDC.
void Automate::afficher_automate_AFDC()
{
    std::cout << std::endl << std::endl;
    
    std::cout << "Affichage de l'automate " << nom << "." << std::endl;

    afficherAlphabet();

    afficherEtat();
    
    std::cout << "Il y a " << nbr_transition << " transitions." << std::endl;

    std::cout << std::endl << std::endl;
    
    std::string separation = "|";
    std::string dixespace = " ";
    std::string entrant = "    ->   ";
    std::string sortant = "    <-   ";
    std::string entrantsortant = "  ->/<-  ";
    
    //On definit la taille des cases en fonction du plus long nom d'etat que l'on aura a afficher.
    int tailleCase = 6;
    for (std::list<Etat*>::iterator it = etats_automate.begin(); it != etats_automate.end(); ++it) {
        if (tailleCase < (*it)->getNom().size()) {
            tailleCase = (int)(*it)->getNom().size();
        }
    }
    tailleCase = tailleCase + 4;
    
    for (int i = 0; i < 8; i++)
    {
        dixespace += " ";
    }
    
    for (int i = 0; i < tailleCase + (tailleCase+1) * nbr_symbole; i++)
    {
        separation += "-";
    }
    
    separation += "|";
    std::cout << dixespace << separation << std::endl;
    std::cout << dixespace;
    std::cout << "|";
    
    affichageCentre(nom, tailleCase);
    
    for (std::list<std::string>::iterator it = alphabet.begin(); it != alphabet.end(); ++it)
    {
        
        affichageCentre(*it, tailleCase);
    
    }

    std::cout << std::endl;
    std::cout << dixespace << separation << std::endl;

    //Meme procedure que pour AF.
    for (std::list<Etat *>::iterator it = etats_automate.begin(); it != etats_automate.end(); ++it)
    {

        bool init = false;
        bool termi = false;
        
        for (std::list<Etat *>::iterator it_init = etats_initiaux.begin(); it_init != etats_initiaux.end(); ++it_init)
        {
            
            if ((*it)->getNom() == (*it_init)->getNom())
            {
                init = true;
            }
            
        }
        
        for (std::list<Etat *>::iterator it_init = etats_terminaux.begin(); it_init != etats_terminaux.end(); ++it_init)
        {
            
            if ((*it)->getNom() == (*it_init)->getNom())
            {
                termi = true;
            }
            
        }
        
        if (init == false && termi == false)
        {
            std::cout << dixespace;
        }
        
        if (init == true && termi == false)
        {
            std::cout << entrant;
        }
        
        if (init == false && termi == true)
        {
            std::cout << sortant;
        }
        
        if (init == true && termi == true)
        {
            std::cout << entrantsortant;
        }
        
        std::cout << "|";

        affichageCentre((*it)->getNom(), tailleCase);

        
        for (std::list<std::string>::iterator it_transi = alphabet.begin(); it_transi != alphabet.end(); it_transi++)
        {
            std::string desti = (*it)->rechercheTransition((*it_transi));
            
            affichageCentre(desti, tailleCase);
            
        }
        
        //On recupere et affiche maintenant les etat d'origine (de AF) de l'automate AFDC.
        std::queue<std::string> etat_origine = (*it)->reconnaitre_etat_origine('.');
        if (etat_origine.size() == 1)
        {
            std::cout << "Cet etat est issu de l'etat : " << etat_origine.front();
            etat_origine.pop();
        }
        else
        {
            std::cout << "Cet etat est issu des etats : ";
            
            while (!etat_origine.empty())
            {
                std::cout << etat_origine.front();
                etat_origine.pop();
                if (etat_origine.size() == 1)
                {
                    std::cout << " et ";
                }
                else if(etat_origine.size() > 1)
                {
                    std::cout << ", ";
                }
            }
        }
        std::cout << std::endl;
        std::cout << dixespace << separation << std::endl;
        
    }
    
    std::cout << std::endl << std::endl;
    
}

//---------------------------------------------------------------------------------------------

//Fonction proche des précédentes a ceci pres qu'elle est concu pour des automates deterministe et complet  et minimaux elle ne permet pas d'en afficher des asynchrone ou non deterministe. Elle permet par contre d'afficher les etats d'origine(AFDC) de chaque etat de l'automate AFDCM.
void Automate::afficher_automate_AFDCM()
{
    
    std::cout << std::endl << std::endl;
    
    std::cout << "Affichage de l'automate " << nom << "." << std::endl;

    afficherAlphabet();

    afficherEtat();
    
    std::cout << "Il y a " << nbr_transition << " transitions." << std::endl;

    std::cout << std::endl << std::endl;
    
    std::string separation = "|";
    std::string dixespace = " ";
    std::string entrant = "    ->   ";
    std::string sortant = "    <-   ";
    std::string entrantsortant = "  ->/<-  ";
    
    //Meme procedure que pour la fonction precedente.
    int tailleCase = 8;
    for (std::list<Etat*>::iterator it = etats_automate.begin(); it != etats_automate.end(); ++it) {
        if (tailleCase < (*it)->getNom().size()) {
            tailleCase = (int)(*it)->getNom().size();
        }
    }
    tailleCase += 2;
    
    for (int i = 0; i < 8; i++)
    {
        dixespace += " ";
    }
    
    for (int i = 0; i < tailleCase + (tailleCase+1) * nbr_symbole; i++)
    {
        separation += "-";
    }
    
    
    separation += "|";
    std::cout << dixespace << separation << std::endl;
    std::cout << dixespace;
    std::cout << "|";
    
    affichageCentre(nom, tailleCase);
    
    for (std::list<std::string>::iterator it = alphabet.begin(); it != alphabet.end(); ++it)
    {
        affichageCentre(*it, tailleCase);
    
    }

    std::cout << std::endl;
    std::cout << dixespace << separation << std::endl;

    //Meme procedure que pour la fonction precedente.
    for (std::list<Etat *>::iterator it = etats_automate.begin(); it != etats_automate.end(); ++it)
    {

        bool init = false;
        bool termi = false;
        
        for (std::list<Etat *>::iterator it_init = etats_initiaux.begin(); it_init != etats_initiaux.end(); ++it_init)
        {
            
            if ((*it)->getNom() == (*it_init)->getNom())
            {
                init = true;
            }
            
        }
        
        for (std::list<Etat *>::iterator it_init = etats_terminaux.begin(); it_init != etats_terminaux.end(); ++it_init)
        {
            
            if ((*it)->getNom() == (*it_init)->getNom())
            {
                termi = true;
            }
            
        }
        
        if (init == false && termi == false)
        {
            std::cout << dixespace;
        }
        
        if (init == true && termi == false)
        {
            std::cout << entrant;
        }
        
        if (init == false && termi == true)
        {
            std::cout << sortant;
        }
        
        if (init == true && termi == true)
        {
            std::cout << entrantsortant;
        }
        
        std::cout << "|";
        
        affichageCentre((*it)->getNom(), tailleCase);
        
        for (std::list<std::string>::iterator it_transi = alphabet.begin(); it_transi != alphabet.end(); it_transi++)
        {
            std::string desti = (*it)->rechercheTransition((*it_transi));
            
            affichageCentre(desti, tailleCase);
            
        }
        
        //Meme procedure que pour la fonction precedente. Mais cette fois le separateur n'est pas '.' mais '/'.
        std::queue<std::string> etat_origine = (*it)->reconnaitre_etat_origine('/');
        
        if (etat_origine.size() == 1)
        {
            std::cout << "Cet etat est issu de l'etat : " << etat_origine.front();
            etat_origine.pop();
        }
        else
        {
            std::cout << "Cet etat est issu des etats : ";
            
            while (!etat_origine.empty())
            {
                std::cout << etat_origine.front();
                etat_origine.pop();
                if (etat_origine.size() == 1)
                {
                    std::cout << " et ";
                }
                else if(etat_origine.size() > 1)
                {
                    std::cout << ", ";
                }
            }
        }
        std::cout << std::endl;
        std::cout << dixespace << separation << std::endl;
        
    }
    
    std::cout << std::endl << std::endl;
    
}

//---------------------------------------------------------------------------------------------

//Methode de test visant a verifier qu'un automate est ou non asynchrone (presence de '*').
bool Automate::est_asynchrone(){
    std::queue<Transition*>* asynchrone = new std::queue<Transition*>;
    
    //On utilise une methode de la classe etat pour rechercher dans chaque etat les transitions asynchrone qui sont stocke dns asynchrone (la file).
    for(std::list<Etat*>::iterator it = etats_automate.begin(); it != etats_automate.end(); it++){
        (*it)->recherche_asynchrone_transi(asynchrone);
    }

    //Si la file est vide c'est qu'il n'a aucune transitions asynchrone et donc que l'automate est synchrone.
    if(asynchrone->empty()){
        std::cout << "l'automate est synchrone." << std::endl;
        delete asynchrone;
        return false;
    }
    // Sinon c'est que l'automate est asynchrone et on en affiche les raisons (les transitions epsilon).
    else{
        std::cout << "l'automate est asynchrone car les transitions suivantes :";
        while(!asynchrone->empty()){
            asynchrone->front()->afficher_transition();
            asynchrone->pop();
            if(!asynchrone->empty()){
                std::cout << "/";
            }
        }
        std::cout << " sont des transitions asynchrones" << std::endl;
        delete asynchrone;
        return true;
    }
}

//---------------------------------------------------------------------------------------------

//Methode de test permettant de verifier qu'un automate est ou non complet (presence d'une transitions pour chaque lettre a chaque etat).
bool Automate::est_complet(){
    std::queue<std::string>* stock = new std::queue<std::string>;
    
    //On utilise une methode d'etat pour verifier que chaque etat est bien complet. Si une transitions est manquante elle est stocke dans stock.
    for(std::list<Etat*>::iterator it = etats_automate.begin() ; it != etats_automate.end(); ++it){
        (*it)->verifier_complet_transi(stock, alphabet);
    }
    
    //Si stock est vide alors l'automate est complet
    if(stock->empty()){
        std::cout << "L'automate est bien complet." << std::endl;
        delete stock;
        return true;
    }
    //Sinon on affiche les tansitions manquantes.
    else{
        if(stock->size() == 1){
            std::cout << "L'automate n'est pas complet il manque l'element suivant: " << std::endl;
        }
        else{
            std::cout << "L'automate n'est pas complet il manque les elements suivants: " << std::endl;
        }
        
        while(!stock->empty()){
            std::cout <<"-" << stock->front();
            stock->pop();
            if(!stock->empty()){
                std::cout << std::endl;;
            }
            else{
                std::cout << "." << std::endl;
            }
        }
        delete stock;
        return false;
    }
}

//---------------------------------------------------------------------------------------------

//Methode de test permettant de verifier qu'un automate est ou non deterministe (presence d'un seul etat inital et d'une seule transitions maximum par etat pour chaque lettre de l'alphabet.
bool Automate::est_deterministe(){
    bool test = true;
    
    //On comence par verifier le nombre d'etat initaux.
    if(nbr_etats_initiaux > 1){
        std::cout << "L'automate n'est pas deterministe car il y a plusieurs etats initiaux :";
        std::list<Etat*>::iterator it=etats_initiaux.begin();
        for(int i = 0; i < nbr_etats_initiaux; i++){
            std::cout << (*it)->getNom();
            it++;
            if(it != etats_initiaux.end()){
                std::cout << ";";
            }
        }
        std::cout <<"."<<std::endl;
        test = false;
    }
    
    //On verifie maintenant chaque etat et s'il n'est pas deterministe on stocke les transitions concerne dans stock.
    std::queue<std::string>* stock = new std::queue<std::string>;
    for(std::list<Etat*>::iterator it = etats_automate.begin(); it!=etats_automate.end(); ++it){
        (*it)->transition_deterministe(stock,alphabet);
    }
    
    //Si stock n'est pas vide l'automate n'est pas deterministe.
    if(!stock->empty()){
        if(test){
            test = false;
            std::cout << "L'automate n'est pas deterministe car l'etat :" <<std::endl;
        }
        else{
            std::cout << "L'automate n'est egalement pas deterministe car l'etat :" << std::endl;
        }
        while(!stock->empty()){
            std::cout << stock->front() << std::endl;
            stock->pop();
        }
    }
    
    //Si les deux test precedent n'ont pas abouti alors l'automate est bien deterministe.
    if(test){
        std::cout << "L'automate est deterministe." << std::endl;
    }
    delete stock;
    return test;
}

//---------------------------------------------------------------------------------------------

//Pour chaque transition manquante de chaque etat on cree une nouvelle transition vers un etat poubelle P, On cree egalement l'etat P et toutes ses transitions vers lui meme.
void Automate::completer_automate(){
    
    nom = "AFDC";
    nbr_etat++;
    etats_automate.push_back(new Etat("P"));
    std::list<Etat*>::iterator it_P = etats_automate.end();
    it_P--;
    for(std::list<std::string>::iterator it_alphabet = alphabet.begin(); it_alphabet != alphabet.end(); ++it_alphabet){
        (*it_P)->set_Transition("P " + (*it_alphabet) + " P");
        nbr_transition++;
    }
    for(std::list<Etat*>::iterator it = etats_automate.begin(); it != etats_automate.end(); ++it){
        nbr_transition += (*it)->completer_etat(alphabet);
    }
    
}

//---------------------------------------------------------------------------------------------

//Cette methode determinise un automate synchrone.
void Automate::determiniser_automate_synchrone(){
    
    std::queue<std::string>* stock_nouv_etat = new std::queue<std::string>;
    std::list<Etat*> nouveau_etats;
    
    if(nbr_etats_initiaux == 1){
        std::list<Etat*>::iterator it_initiaux = etats_initiaux.begin();
        stock_nouv_etat->push((*it_initiaux)->getNom());
    }
    //Ici on reunit les etats initiaux en un seul et unique etat initial.
    else{
        std::string nouv;
        for(std::list<Etat*>::iterator it_init = etats_initiaux.begin(); it_init != etats_initiaux.end();++it_init){
            if(it_init != etats_initiaux.begin()){
                nouv += ".";
            }
            nouv += (*it_init)->getNom();
        }
        stock_nouv_etat->push(nouv);
    }
    
    std::list<Etat*>::iterator it_nouv = nouveau_etats.begin();
    
    // Tant que notre liste de nouveaux etats n'est pas vide on continue de traiter les etats.
    while(!stock_nouv_etat->empty()){
        //Permet de verifier que l'etat n'est pas deja cree.
        bool presence = false;
        
        for(std::list<Etat*>::iterator it_nouv2 = nouveau_etats.begin(); it_nouv2 != nouveau_etats.end(); ++it_nouv2){
            if((stock_nouv_etat->front()) == ((*it_nouv2)->getNom())){
                presence = true;
            }
        }
        
        //En cas de doublons d'etat on passe a l'etat suivant.
        if(presence){
            stock_nouv_etat->pop();
        }
        //Si l'etat n'existe pas on le cree puis on cree ses transitions avec determiniser etat.
        else{
            nouveau_etats.push_back(new Etat(stock_nouv_etat->front()));
            it_nouv++;
            (*it_nouv)->determiniser_Etat(stock_nouv_etat, etats_automate, alphabet);
            stock_nouv_etat->pop();
        }
    }
    
    std::list<Etat*> nouveau_etats_terminaux;
    
    // Tous les etats compose a partir d'au moins un etat terminal deviennet terminaux.
    for(std::list<Etat*>::iterator it_ancien = etats_terminaux.begin(); it_ancien != etats_terminaux.end(); ++it_ancien){
        for(std::list<Etat*>::iterator it_nouv = nouveau_etats.begin(); it_nouv != nouveau_etats.end(); ++ it_nouv){
            if((*it_nouv)->chercherDansNom((*it_ancien)->getNom())){
                
                if(chercherNomEtat((*it_nouv)->getNom(), nouveau_etats_terminaux)){
                 
                    nouveau_etats_terminaux.push_back((*it_nouv));
                    
                }
                
            }
        }
    }
    
    
    
    etats_terminaux.clear();
    
    //On remplace ici nos anciens etats terminaux par les nouveuax.
    etats_terminaux = nouveau_etats_terminaux;
    
    nouveau_etats_terminaux.clear();
    
    etats_initiaux.clear();
    
    //On remplace les anciens etats initiaux par le nouvel (et unique) etat initial.
    etats_initiaux.push_back(*nouveau_etats.begin());
    
    for(std::list<Etat*>::iterator it_suppr = etats_automate.begin(); it_suppr != etats_automate.end(); ++it_suppr){
        if(*it_suppr != nullptr){
            delete *it_suppr;
        }
    }
    
    etats_automate.clear();
    //Ici on affecte les nouveaux etats a la liste d'etat de l'automate.
    etats_automate = nouveau_etats;
    nbr_etats_initiaux = 1;
    nbr_etats_terminaux = (int)etats_terminaux.size();
    nbr_etat = (int)etats_automate.size();
    
    //Si l'automate n'est pas complet on appelle la fonction de completion.
    if(!est_complet()){
        completer_automate();
    }else{
        nom = "AFDC";
    }
    
    //On calcule et stocke le nouveaux nombre de transitions.
    nbr_transition = nbr_etat*nbr_symbole;
    delete stock_nouv_etat;
    
}

//--------------------------------------------------------------------------------------------

//On cree l'automate reconnaissant le language complementaire de l'automate actuel.
void Automate::complementaire_automate(){
    
    std::cout << "Cet automate provient de l'automate " + nom + "." << std::endl;
    
    nom = "AComp";
    std::list<Etat*> nouveau_etat_terminaux;
    for(std::list<Etat*>::iterator it_etat = etats_automate.begin(); it_etat != etats_automate.end();++it_etat){
        bool presence = false;
        for(std::list<Etat*>::iterator it_terminaux = etats_terminaux.begin(); it_terminaux != etats_terminaux.end(); ++it_terminaux){
            //On verifie si l'etat parcouru est terminal.
            if((*it_etat)->getNom() == (*it_terminaux)->getNom()){
                presence = true;
            }
        }
        // S'il est non terminal alors il devient terminal dans notre nouvel automate.
        if(!presence){
            nouveau_etat_terminaux.push_back((*it_etat));
        }
    }
    etats_terminaux.clear();
    //Les anciens etat terminaux sont remplace par les nouveaux.
    etats_terminaux = nouveau_etat_terminaux;
    nbr_etats_terminaux = (int)etats_terminaux.size();
}

//---------------------------------------------------------------------------------------------

//Cette methode permet de verifier si un automate est standard.
bool Automate::est_standard(){
    std::string nom_initial = (*etats_initiaux.begin())->getNom();
    for(std::list<Etat*>::iterator it_etat = etats_automate.begin(); it_etat != etats_automate.end(); ++it_etat){
        //On regarde ici si pour chacun de nos etat l'un d'eux a une transitions qui aurait pour destinations l'etat initial. Si c'est le cas il n'est pas standard.
        if((*it_etat)->recherche_standard(nom_initial)){
            return false;
        }
    }
    //Si on a jamais trouve de transitions vers l'etat initial il est standard.
    return true;
}

//---------------------------------------------------------------------------------------------

//On standardise notre automate. Creation d'un etat 'i'.
void Automate::standardiser(){
    //On verfie qu'il ne l'est pas deja car on ne standardise pas un automate qui l'est deja.
    if(est_standard()){
        std::cout << "L'automate est deja standard." << std::endl;
        nom = "ACompStd";
    }
    //Sinon on standardise.
    else{
        nom = "ACompStd";
        std::cout << "L'automate n'etait pas standard." << std::endl;
        Etat * nouv = new Etat((*(*etats_initiaux.begin())));
        //On cree un nouvel etat initial 'i'.
        nouv->setNom("i");
        etats_automate.push_front(nouv);
        bool terminal = false;
        for(std::list<Etat*>::iterator it_terminaux = etats_terminaux.begin(); it_terminaux != etats_terminaux.end(); ++it_terminaux){
            //On verifie si l'etat initial est aussi terminal.
            if((*it_terminaux)->getNom() == (*etats_initiaux.begin())->getNom()){
                terminal = true;
            }
        }
        //Si c'est la cas on ajoute alors cet etat dans la liste des etat terminaux.
        if(terminal){
            nbr_etats_terminaux++;
            etats_terminaux.push_front(nouv);
        }
        //On remplace l'etat inital par le nouvel etat 'i'.
        etats_initiaux.clear();
        etats_initiaux.push_back(nouv);
        //On remet a jour le nombre d'etat.
        nbr_etat++;
        //On met a jour le nombre de transitions.
        nbr_transition += nbr_symbole;
    }
}

//---------------------------------------------------------------------------------------------

//Cette fonction permet de regarder si une transitions fait partie d'une liste de transition.
bool verifier_presence_liste(Transition verif, std::list<Transition*> liste){
    for(std::list<Transition*>::iterator it = liste.begin(); it != liste.end(); ++it){
        if((*it)->getPosition() == verif.getPosition() && (*it)->getTransi() == verif.getTransi() && (*it)->getDestination() == verif.getDestination()){
            return true;
        }
    }
    return false;
}

//---------------------------------------------------------------------------------------------

//Cette fonction permet de regarder si un etat fait partie d'une liste d'etats.
bool verifier_presence_liste_etat(std::string verif, std::list<Etat*> liste){
    for(std::list<Etat*>::iterator it = liste.begin(); it != liste.end(); ++it){
        if(verif == (*it)->getNom()){
            return true;
        }
    }
    return false;
}

//---------------------------------------------------------------------------------------------

//Permet d'ajouter une transition a un automate.
void Automate::ajouterTransition(Transition* ajout){
    for(std::list<Etat*>::iterator it_etat = etats_automate.begin(); it_etat != etats_automate.end(); ++it_etat){
        if(ajout->getPosition() == (*it_etat)->getNom()){
            (*it_etat)->ajouterTransition(ajout);
        }
    }
}

//---------------------------------------------------------------------------------------------

//Permet d'effacer toutes les transitions de notre automate.
void Automate::effacerTransition(){
    for(std::list<Etat*>::iterator it_etat = etats_automate.begin(); it_etat!= etats_automate.end(); ++it_etat){
        (*it_etat)->effacerlistTransi();
    }
    nbr_transition = 0;
}

//---------------------------------------------------------------------------------------------

//Permet de rendre synchrone un automate asynchrone (supprimer les transitions epsilon).
void Automate::synchroniser(){
    std::list<Transition*> reserve;
    Etat* position;
    //On parcourt tous les etats de notre d'automate.
    for(std::list<Etat*>::iterator it_etat = etats_automate.begin(); it_etat!=etats_automate.end(); ++it_etat){
        std::stack<Transition*> a_faire;
        //On place notre curseur position sur les différents etat de l'automate pour les traiter les uns apres les autres.
        position = (*it_etat);
        //On recupere toutes les transitions de notre automate pour les mettre en attente.
        for(std::list<Transition*>::iterator it_transition = position->getTransitions()->begin(); it_transition != position->getTransitions()->end(); ++it_transition){
            a_faire.push((*it_transition));
        }
        //Tant que la liste de traitement n'est pas vide on continu de traiter les transitions qu'elle contient.
        while(!(a_faire.empty())){
            std::list<Transition> stock;
            Transition* temp = (a_faire.top());
            stock.push_back(Transition(temp->getPosition(),temp->getTransi(),temp->getDestination()));
            a_faire.pop();
            while(!(stock.empty())){
                std::list<Transition>::iterator it_recup = stock.begin();
                //On verifie si la transitions observe méne a un etat terminal uniquement via des transitions epsilon. Si c'est la cas on ajoute notre etat d'origne a la liste des etats terminaux.
                if((*it_recup).getTransi() == "*"){
                    if(verifier_presence_liste_etat((*it_recup).getDestination(),etats_terminaux)){
                        if(!verifier_presence_liste_etat((*it_recup).getPosition(), etats_terminaux)){
                            etats_terminaux.push_back(retourne_Etat((*it_recup).getPosition()));
                        }
                    }
                }
                
                Transition conserver = (*it_recup);
                stock.erase(it_recup);
                position = retourne_Etat(conserver.getDestination());

                //S'il n'y a pas de transitions dans l'etat de destination de la transition, on cree la transition si cette derniere n'est pas une transitions epsilon ou n'existe pas deja.
                if(position->getTransitions()->empty()){
                    if(conserver.getTransi() != "*"){
                        if(!verifier_presence_liste(conserver, reserve)){
                            reserve.push_back(new Transition(conserver.getPosition(), conserver.getTransi(),conserver.getDestination()));
                        }
                    }
                }
                //S'il y a des transitions dans l'etat de destination.
                else{
                    //On va tester toutes les transitions de l'etat de destination de la transition.
                    for(std::list<Transition*>::iterator it_transi = position->getTransitions()->begin(); it_transi != position->getTransitions()->end(); ++it_transi){
                        Transition stationnaire = conserver;
                        //Si on observe actuellement une transitions epsilon on rentre ici.
                        if(stationnaire.getTransi() == "*"){
                            //Et que la transition suivante n'est pas une transition epsilon on cree notre nouvelle transition definitivement et on la remet dans le stock pour observer les transitions suivantes.
                            if((*it_transi)->getTransi() != "*"){
                                
                                stationnaire.setTransi((*it_transi)->getTransi());
                                stationnaire.setDestination((*it_transi)->getDestination());
                                if(!verifier_presence_liste(stationnaire, reserve)){
                                    stock.push_back(stationnaire);
                                }
                                
                            }
                            //Si la transition suivante est une epsilon on ajoute notre transition au stock pour observer les transitions suivante et on verifie que l'on a pas une boucle de transition epsilon ramenant au point d'origine. Ce qui bloquerai le code (boucle infinie).
                            else{
                                
                                if((stationnaire.getPosition()) != (*it_transi)->getDestination()){
                                
                                    stationnaire.setDestination((*it_transi)->getDestination());
                                    if(!verifier_presence_liste(stationnaire, reserve)){
                                        stock.push_back(stationnaire);
                                    }
                                }
                            }
                        }
                        //Si la transition  que l'on observe n'est pas une transition epsilon on entre dans la condition.
                        else{
                            //Si la transition suivante est une transition epsilon on met a jour la destination de notre transition et on la place dans le stock pour observer les transitions qui suivront et on cree definitivement la transition observee.
                            if((*it_transi)->getTransi() == "*"){
                                
                                if(!verifier_presence_liste(stationnaire, reserve)){
                                            reserve.push_back(new Transition(stationnaire.getPosition(), stationnaire.getTransi(),stationnaire.getDestination()));
                                }
                                    
                                stationnaire.setDestination((*it_transi)->getDestination());
                                if(!verifier_presence_liste(stationnaire, reserve)){
                                    stock.push_back(stationnaire);
                                }
                            }
                            //Si la transition suivante n'est pas une transition epsilon on cree la transition observee definitivement et on ne cherche pas les transitions suivante.
                            else{
                                if(!verifier_presence_liste(stationnaire, reserve)){
                                    reserve.push_back(new Transition(stationnaire));
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    //On supprime l'epsilon de l'alphabet.
    for(std::list<std::string>::iterator it_alphabet = alphabet.begin(); it_alphabet != alphabet.end(); ++it_alphabet){
        if((*it_alphabet) == "*"){
            alphabet.erase(it_alphabet);
            nbr_symbole--;
        }
    }
    
    //on supprime toutes les transitions de notre alphabet.
    effacerTransition();
    
    //Puis on cree les nouvelles transtions.
    while(!reserve.empty()){
        ajouterTransition((*reserve.begin()));
        reserve.erase(reserve.begin());
        nbr_transition++;
    }
    
    //On met a jour notre nombre d'etats terminaux.
    nbr_etats_terminaux = (int)etats_terminaux.size();
    
    afficher_automate_AF();
    
    //On determinise et complete l'automate si besoin.
    if(!est_deterministe()){
        
        determiniser_automate_synchrone();
        
    } else if(!est_complet()){
        
        completer_automate();
        
    }
    
}

//---------------------------------------------------------------------------------------------

//Cette fonction permet de demander un mot a l'utilisateur.
void Automate::demander_mot(){
    std::string entree;
    while(1){
        std::cout << "Veuillez entree un mot a lire dans l'automate. S'il n'y a plus de mots a reconnaitre tapez #exit ." << std::endl;
        std::getline(std::cin, entree);
        //Si l'utiliseur rentre #exit on arrete le traitement.
        if(entree == "#exit"){
            return;
        }
        //Si jamais l'utilisateur n'a rien entre alors on applique le traitement pour le mot vide.
        if(entree.empty()){
            Etat* initial = *etats_initiaux.begin();
            int presence = false;
            for(std::list<Etat*>::iterator it_terminaux = etats_terminaux.begin(); it_terminaux != etats_terminaux.end(); ++it_terminaux){
                //Pour traiter le mot vide on regarde si l'etat initial est aussi terminal.
                if(initial == (*it_terminaux)){
                    presence = true;
                }
            }
            if(!presence){
                std::cout << "L'auomate ne reconnait pas le mot vide." <<std::endl;
                return;
            }
            else{
                std::cout << "L'automate reconnait le mot vide." <<std::endl;
                return;
            }
        }
        //On appelle la fonction lire mot qui verifie si le mot peut etre lu par l'automate.
        int resultat = lire_mot(entree);
        
        if(resultat == 1){
            std::cout << "Le mot " << entree << " a bien ete reconnu."<<std::endl;
        }
        //On cherche a trouver le mot le plus long qui aurait pu etre reconnu puis on l'affiche.
        else if(resultat == 0){
            std::cout << "Le mot " << entree << " n'a pas ete reconnu."<<std::endl;
            for(int i = 1; i < (int)entree.size();i++){
                if(lire_mot(entree.substr(0, (int)entree.size() - i))){
                    std::cout << "Le plus grand mot qui aurait pu etre reconnu a partir de ce mot est : " << entree.substr(0, (int)entree.size() - 1) << std::endl;
                    i = (int)entree.size();
                }
            }
        }
        //Si les lettres entre par l'utilisateur ne sont pas reconnu par l'automate on affiche l'alphabet reconnu par l'automate.
        else{
            std::cout << "Les lettres reconnu par l'automate sont : ";
            for(std::list<std::string>::iterator it_alphabet = alphabet.begin(); it_alphabet != alphabet.end(); ++it_alphabet){
                if(it_alphabet != alphabet.begin()){
                    std::cout << ", ";
                }
                std::cout << (*it_alphabet);
            }
            std::cout<<"."<<std::endl;
        }
    }
}

//---------------------------------------------------------------------------------------------

//Cette methode permet de verifier si un mot est reconnu ou non par l'automate.
int Automate::lire_mot(std::string mot){
    //Le pointeur position est notre etat intial au debut puis il itere sur chaque etat.
    Etat* position = (*etats_initiaux.begin());
    //On itere sur chaque lettre du mot dans l'ordre.
    for(int i =0; i < (int)mot.size(); i++){
        std::list<Transition*>::iterator it_transi = position->getTransitions()->begin();
        bool fin = false;
        //On regarde toutes les transitions de notre etat pour voir si l'une d'elle correspond a notre lettre.
        while(!fin){
            //Si on a parcouru toutes les transitions existante de notre etat sans trouver de transtions qui accepte notre lettre alors cela signifie que l'utilisateur a entre une lettre non reconnu par l'automate (car l'automate et complet et deterministe).
            if(it_transi == position->getTransitions()->end()){
                std::cout << "Vous avez rentre un caractere non reconnu par l'automate : " << mot.substr(i, 1) << std::endl;
                return 2;
            }
            //Si jamais une des transitions est egale à la lettre de notre mot on avance et notre position devient alors la destination de la transition correspondante.
            else if((*it_transi)->getTransi() == mot.substr(i, 1)){
                position = retourne_Etat((*it_transi)->getDestination());
                fin = 1;
            }
            //Si la transitions ne correspond pas on passe a la suivante.
            else{
                it_transi++;
            }
        }
    }
    int resultat = 0;
    //Une fois arrive a la fin du mot on regarde si l'on est sur un etat terminal si c'est le cas le mot est reconnu sinon il ne l'est pas.
    for(std::list<Etat*>::iterator it_termi = etats_terminaux.begin(); it_termi != etats_terminaux.end(); ++it_termi){
       
        if(position == (*it_termi)){
            resultat = 1;
        }
    }
    return resultat;
}

//---------------------------------------------------------------------------------------------

//Cette fonction permet de regarder si dans le nom passe(base) en parametre on retrouve le nom passer en paramtetre (destination).
bool chercher_string_dans_string(std::string base, std::string destination){
    int premiere_pos = (int)base.find("/", 0);
    if(premiere_pos == -1){
        if(base == destination){
            return true;
        }
        else{
            return false;
        }
    }
    if(destination == base.substr(0, premiere_pos)){
        return true;
    }
    else{
        bool stop = true;
        while(stop){
        int deuxieme_pos = (int)base.find("/", premiere_pos + 1);
            if((deuxieme_pos == -1) && (destination == base.substr(premiere_pos+1, base.size()))){
                return true;
            }
            else if(deuxieme_pos == -1){
                return false;
            }
            else if(destination == base.substr(premiere_pos+1, deuxieme_pos - premiere_pos - 1)){
                return true;
            }
            else{
                premiere_pos = deuxieme_pos;
            }
        }
    }
    return false;
}

//---------------------------------------------------------------------------------------------

//Permet de verifier si les transtitions d'une partition correpondent aux partitions deja creees.
bool verifier_existence_partition(std::list<case_minimal*>* tableau, Etat* ajout){
    if((int)tableau->size() == 0){
        return true;
    }
    else{
        for(std::list<case_minimal*>::iterator it_case = tableau->begin(); it_case != tableau->end(); ++it_case){
            std::list<Transition*>::iterator it_transi = ajout->getTransitions()->begin();
            bool trouve = true;
            for(std::list<Transition>::iterator it_def = (*it_case)->definition.begin(); it_def != (*it_case)->definition.end(); ++it_def){
                bool presence_nom = chercher_string_dans_string((*it_def).getDestination(), (*it_transi)->getDestination());
                if(presence_nom && (*it_def).getTransi() == (*it_transi)->getTransi()){
                    it_transi++;
                }
                else{
                    it_def = (*it_case)->definition.end();
                    it_def--;
                    trouve = false;
                }
            }
            if(trouve){
                (*it_case)->stockage.push_back(ajout);
                return false;
            }
        }
        return true;
    }
}

//---------------------------------------------------------------------------------------------

//Permet de creer une partition ainsi que les transitions associees.
void creer_partition(Etat* ajout,std::list<embranchement>* liste_emb, std::list<std::string>* alphabet, case_minimal* nouvelle){
    //Cette boucle permet de creer les transitions pour chaque lettre de l'alphabet reconnu par l'automate
    for(std::list<std::string>::iterator it_alphabet = alphabet->begin(); it_alphabet != alphabet->end(); ++it_alphabet){
        std::string position = "";
        std::string transition = (*it_alphabet);
        std::string destination;
        Transition* notre_transition;
        //On parcourt les transitions pour trouver celles correspondant a notre lettre de l'alphabet.
        for(std::list<Transition*>::iterator it_transition = ajout->getTransitions()->begin(); it_transition != ajout->getTransitions()->end(); ++it_transition){
            if((*it_alphabet) == (*it_transition)->getTransi()){
                notre_transition = (*it_transition);
            }
        }

        //On parcourt nos diffenrentes partitions pour trouver celle qui contient la destination de la transition trouvee.
        for(std::list<embranchement>::iterator it_verif = liste_emb->begin(); it_verif != liste_emb->end(); ++it_verif){
            bool presence = false;
            for(std::list<std::string>::iterator it_groupe = (*it_verif).groupe.begin(); it_groupe != (*it_verif).groupe.end(); ++it_groupe){
                if((*it_groupe) == notre_transition->getDestination()){
                    presence = true;
                }
                if((int)destination.size() != 0){
                    destination += "/";
                }
                destination += (*it_groupe);
            }
            if(presence){
                it_verif = liste_emb->end();
                it_verif--;
            }
            else{
                destination = "";
            }
        }
        //On ajoute la transition a notre partition celle ci contient la lettre de l'alphabet ainsi que la partition de la destination.
        nouvelle->definition.push_back(Transition(position, transition, destination));
    }
    //On ajoute notre etat comme faisant partie de cette partition.
    nouvelle->stockage.push_back(ajout);
}

//---------------------------------------------------------------------------------------------

//Cette fonction permet calculer le nombre partition presente dans notre tableau.
int taille_liste_embranchement(std::list<embranchement>* liste_embranchement){
    int taille = 0;
    for(std::list<embranchement>::iterator it_emb = liste_embranchement->begin(); it_emb != liste_embranchement->end(); ++it_emb){
        taille += (int)(*it_emb).cases.size();
    }
    return taille;
}

//---------------------------------------------------------------------------------------------

//Permet d'effacer un tableau de minimisation.
void effacer_tableau(std::list<embranchement>* liste_efface){
    for(std::list<embranchement>::iterator it_emb = liste_efface->begin(); it_emb != liste_efface->end(); ++it_emb){
        for(std::list<case_minimal*>::iterator it_case = (*it_emb).cases.begin(); it_case != (*it_emb).cases.end() ; ++it_case){
            (*it_case)->definition.clear();
            (*it_case)->stockage.clear();
            delete (*it_case);
        }
        (*it_emb).groupe.clear();
        (*it_emb).cases.clear();
    }
    liste_efface->clear();
}

//---------------------------------------------------------------------------------------------

//Permet de supprimer un tableau de minimisation (represente par des listes).
void delete_Etat(std::list<Etat*>* liste_a_effacer){
    for(std::list<Etat*>::iterator it_Etat = liste_a_effacer->begin(); it_Etat != liste_a_effacer->end(); ++it_Etat){
        for(std::list<Transition*>::iterator it_transitions = (*it_Etat)->getTransitions()->begin(); it_transitions != (*it_Etat)->getTransitions()->end(); ++it_transitions){
            delete (*it_transitions);
        }
        (*it_Etat)->getTransitions()->clear();
        delete (*it_Etat);
    }
}

//---------------------------------------------------------------------------------------------

//Permet de remplir notre tableau de minimisation en creant une partition si besoin ou en ajoutant l'etat dans une partition deja exitante.
void Automate::remplir_tableau_minimal(std::list<embranchement>* nouveau_tableau){
    for(std::list<Etat*>::iterator it_etat = etats_automate.begin(); it_etat != etats_automate.end(); ++it_etat){
            for(std::list<embranchement>::iterator it_emb = nouveau_tableau->begin(); it_emb != nouveau_tableau->end(); ++it_emb){
                //Permet de selectionner le bon embranchement dans lequel mettre chaque etat.
                bool presence = false;
                for(std::list<std::string>::iterator it_nom = (*it_emb).groupe.begin(); it_nom != (*it_emb).groupe.end(); ++it_nom){
                    if((*it_etat)->getNom() == (*it_nom)){
                        presence = true;
                    }
                }
                if(presence){
                    bool autorisation = verifier_existence_partition(&((*it_emb).cases) , (*it_etat));
                    if(autorisation){
                        case_minimal* nouvelle_case = new case_minimal;
                        (*it_emb).cases.push_back(nouvelle_case);
                        creer_partition((*it_etat),nouveau_tableau,&alphabet, nouvelle_case);
                    }
                }
            }
    }
}

//---------------------------------------------------------------------------------------------

//Permet de creer les etat de notre nouvel automate minimal a partir d'un tableau de minimisation.
void Automate::creer_nouveaux_etats(std::list<embranchement>* nouveaux_etat){
    for(std::list<embranchement>::iterator it_emb = nouveaux_etat->begin(); it_emb != nouveaux_etat->end(); ++it_emb){
        std::string nouv_nom;
        for(std::list<std::string>::iterator it_nom = (*it_emb).groupe.begin(); it_nom != (*it_emb).groupe.end(); ++it_nom){
            if(it_nom != (*it_emb).groupe.begin()){
                nouv_nom += "/";
            }
            nouv_nom += (*it_nom);
        }
        Etat * nouvel_etat = new Etat(nouv_nom);
        for(std::list<Transition>::iterator it_transi = (*(*it_emb).cases.begin())->definition.begin(); it_transi != (*(*it_emb).cases.begin())->definition.end(); ++it_transi){
            Transition* nouv_transi = new Transition(nouv_nom,(*it_transi).getTransi(),(*it_transi).getDestination());
            nouvel_etat->ajouterTransition(nouv_transi);
        }
        etats_automate.push_back(nouvel_etat);
    }
}

//---------------------------------------------------------------------------------------------

//Permet de creer la liste des etats terminaux de notre nouvel automate minimal.
void Automate::nouveau_etat_terminaux(){
    std::list<Etat*> nouv_etat_terminaux;
    for(std::list<Etat*>::iterator it_etat = etats_automate.begin(); it_etat != etats_automate.end(); ++it_etat){
        for(std::list<Etat*>::iterator it_etat_term = etats_terminaux.begin(); it_etat_term != etats_terminaux.end(); ++it_etat_term){
            bool ajout = chercher_string_dans_string((*it_etat)->getNom(),(*it_etat_term)->getNom());
            if(ajout){
                if(!verifier_presence_liste_etat((*it_etat)->getNom(),nouv_etat_terminaux)){
                    nouv_etat_terminaux.push_back((*it_etat));
                }
            }
        }
    }
    etats_terminaux.clear();
    etats_terminaux = nouv_etat_terminaux;
}

//---------------------------------------------------------------------------------------------

//Permet de creer l'etat initial de notre nouvel automate minimal.
void Automate::nouvel_etat_initial(){
    std::list<Etat*> nouv_etat_initial;
    for(std::list<Etat*>::iterator it_etat = etats_automate.begin(); it_etat != etats_automate.end(); ++it_etat){
        for(std::list<Etat*>::iterator it_etat_init = etats_initiaux.begin(); it_etat_init != etats_initiaux.end(); ++it_etat_init){
            bool ajout = chercher_string_dans_string((*it_etat)->getNom(),(*it_etat_init)->getNom());
            if(ajout){
                nouv_etat_initial.push_back((*it_etat));
            }
        }
    }
    etats_initiaux.clear();
    etats_initiaux = nouv_etat_initial;
}

//----------------------------------------------------------------------------------------------

//Permet de trouver la plus grande case de notre tableau pour ameliorer l'affichage(eviter de deborder des cases).
int taille_partition_max(std::list<embranchement>* partitions){
    int taille_max = 0;
    for(std::list<embranchement>::iterator it_emb = partitions->begin(); it_emb != partitions->end(); ++it_emb){
        int taille = 0;
        for(std::list<std::string>::iterator it_groupe = (*it_emb).groupe.begin(); it_groupe != (*it_emb).groupe.end(); ++it_groupe){
            if(it_groupe != (*it_emb).groupe.begin()){
                taille++;
            }
            taille += (*it_groupe).size();
        }
        if(taille > taille_max){
            taille_max = taille;
        }
        for(std::list<case_minimal*>::iterator it_case = (*it_emb).cases.begin(); it_case != (*it_emb).cases.end(); ++it_case){
            taille = 0;
            for(std::list<Etat*>::iterator it_etat = (*it_case)->stockage.begin(); it_etat != (*it_case)->stockage.end(); ++it_etat){
                if(it_etat != (*it_case)->stockage.begin()){
                    taille++;
                }
                taille += (*it_etat)->getNom().size();
            }
            if(taille > taille_max){
                taille_max = taille;
            }
            for(std::list<Transition>::iterator it_transi = (*it_case)->definition.begin(); it_transi != (*it_case)->definition.end(); ++it_transi){
                taille = 0;
                taille += (*it_transi).getDestination().size();
            }
            if(taille > taille_max){
                taille_max = taille;
            }
        }
    }
    
    return taille_max;
}

//----------------------------------------------------------------------------------------------

//Permet d'afficher les tableaux de minimisation.
void afficher_partition(std::list<embranchement>* partitions, std::list<std::string>* alphabet, int compteur){
    std::cout << std::endl;
    int taille_case = taille_partition_max(partitions);
    if(taille_case < 6){
        taille_case = 6;
    }
    int taille_ligne = (int)(2 + alphabet->size())*(taille_case);
    std::string separation = "|";
    for(int i = 0; i < taille_ligne + (1 + (int)alphabet->size()); i++){
        separation += "-";
    }
    separation +="|";
    std::cout<<separation<<std::endl;
    std::string numero_table = "table" + convert_to_string(compteur);
    std::cout << "|";
    affichageCentre(numero_table, taille_case);
    for(int i = 0; i < taille_case; i++){
        std::cout << " ";
    }
    std::cout << "|";
    for(std::list<std::string>::iterator it_alpha = alphabet->begin(); it_alpha != alphabet->end(); ++it_alpha){
        affichageCentre(*it_alpha, taille_case);
    }
    std::cout<<std::endl<<separation<<std::endl;
    for(std::list<embranchement>::iterator it_emb = partitions->begin(); it_emb != partitions->end(); ++it_emb){
        std::string nom;
        for(std::list<std::string>::iterator it_groupe = (*it_emb).groupe.begin(); it_groupe != (*it_emb).groupe.end(); ++it_groupe){
            if(it_groupe != (*it_emb).groupe.begin()){
                nom += "/";
            }
            nom += (*it_groupe);
        }
        std::cout << "|";
        affichageCentre(nom, taille_case);
        for(std::list<case_minimal*>::iterator it_case = (*it_emb).cases.begin(); it_case != (*it_emb).cases.end(); ++it_case){
            for(std::list<Etat*>::iterator it_nom = (*it_case)->stockage.begin(); it_nom != (*it_case)->stockage.end(); ++it_nom){
                if(it_case != (*it_emb).cases.begin() || it_nom != (*it_case)->stockage.begin()){
                    std::cout << "|";
                    for(int i = 0; i < taille_case; i++){
                        std::cout << " ";
                    }
                    std::cout << "|";
                }
                affichageCentre((*it_nom)->getNom(), taille_case);
                for(std::list<Transition>::iterator it_transi = (*it_case)->definition.begin(); it_transi != (*it_case)->definition.end(); ++it_transi){
                    affichageCentre((*it_transi).getDestination(), taille_case);
                }
                std::cout << std::endl;
            }
        }
         std::cout<<separation<<std::endl;
    }
    std::cout << std::endl;
}

//----------------------------------------------------------------------------------------------

//Permet de minimiser notre automate en utilisant les fonctions decrite ci-dessus.
void Automate::minimiser_automate(){
    
    int compteur= 1;
    
    std::list<embranchement> premiere_liste;
    for(int i =0; i < 2 ; i++){
        embranchement nouv;
        premiere_liste.push_back(nouv);
    }
    //Separe les etats de l'automate en terminaux et non terminaux.
    for(std::list<Etat*>::iterator it_etat = etats_automate.begin(); it_etat != etats_automate.end(); ++it_etat){
        bool presence = false;
        for(std::list<Etat*>::iterator it_etat_term = etats_terminaux.begin(); it_etat_term != etats_terminaux.end(); ++it_etat_term){
            if(*it_etat == *it_etat_term){
                presence = true;
            }
        }
        if(presence){
            (*premiere_liste.begin()).groupe.push_back((*it_etat)->getNom());
        }
        else{
            std::list<embranchement>::iterator it_temp = premiere_liste.begin();
            it_temp++;
            (*it_temp).groupe.push_back((*it_etat)->getNom());
        }
    }
    remplir_tableau_minimal(&premiere_liste);
    bool stop = false;
    
    //si on avait pas d'etat erminaux ou non terminaux on supprime l'embranchement associe.
    for (std::list<embranchement>::iterator it_liste = premiere_liste.begin(); it_liste != premiere_liste.end(); ++it_liste) {
        if ((*it_liste).cases.empty()) {
            premiere_liste.erase(it_liste);
        }
    }
    
    std::cout << "Affichage des partitions :" << std::endl;
    
    afficher_partition(&premiere_liste, &alphabet, compteur);
    
    //On fait maintenant des tableau successif tant que l'on a pas le tableau minimal.
    while(!stop){
        std::list<embranchement> deuxieme_liste;
        //On cree les embranchements de notre tableau suivant.
        for(std::list<embranchement>::iterator it_emb = premiere_liste.begin(); it_emb != premiere_liste.end(); ++it_emb){
            for(std::list<case_minimal*>::iterator it_case = (*it_emb).cases.begin(); it_case != (*it_emb).cases.end(); ++it_case){
                embranchement nouvel_embranchement;
                for(std::list<Etat*>::iterator it_etat = (*it_case)->stockage.begin(); it_etat != (*it_case)->stockage.end(); ++it_etat){
                    nouvel_embranchement.groupe.push_back((*it_etat)->getNom());
                }
                deuxieme_liste.push_back(nouvel_embranchement);
            }
        }
        remplir_tableau_minimal(&deuxieme_liste);
        
        int nbr_case1 = taille_liste_embranchement(&premiere_liste);
        int nbr_case2 = taille_liste_embranchement(&deuxieme_liste);
        
        effacer_tableau(&premiere_liste);
        
        premiere_liste = deuxieme_liste;
        compteur++;
        
        afficher_partition(&deuxieme_liste, &alphabet, compteur);
        deuxieme_liste.clear();
        
        //Si on a autant de partitions dans notre tableau que celui precedent alors on a l'automate minimal.
        if(nbr_case1 == nbr_case2){
           stop = true;
        }
    }
    
    
    int nbr_etat_nouveau = taille_liste_embranchement(&premiere_liste);

    //Si l'automate etait deja terminal on ne le recree pas on change juste le nom.
    if(nbr_etat_nouveau == nbr_etat){
        std::cout << "L'automate etait deja minimal." << std::endl;
        effacer_tableau(&premiere_liste);
        nom = "AFDCM";
    }
    //Sinon on remplace notre automate par son equivalent minimal.
    else{
        nom = "AFDCM";
        nbr_etat = nbr_etat_nouveau;
        std::list<Etat*> ancien_etat = etats_automate;
        etats_automate.clear();
        creer_nouveaux_etats(&premiere_liste);
        nouveau_etat_terminaux();
        nouvel_etat_initial();
        delete_Etat(&ancien_etat);
        nbr_etats_terminaux = (int)etats_terminaux.size();
        nbr_transition = nbr_etat * nbr_symbole;
    }
}

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------

//Reprend le meme affichage que afficher AFDC et AFDCM mais sans indiquer les etats d'origines. Utilise pour AComp et ACompStd
void Automate::afficher_automate_Autre()
{
    std::cout << std::endl << std::endl;
    
    std::cout << "Affichage de l'automate " << nom << "." << std::endl;

    afficherAlphabet();

    afficherEtat();
    
    std::cout << "Il y a " << nbr_transition << " transitions." << std::endl;

    std::cout << std::endl << std::endl;
    
    std::string separation = "|";
    std::string separationPlus = "|";
    std::string dixespace = " ";
    std::string entrant = "    ->   ";
    std::string sortant = "    <-   ";
    std::string entrantsortant = "  ->/<-  ";
    
    int tailleCase = 6;
    for (std::list<Etat*>::iterator it = etats_automate.begin(); it != etats_automate.end(); ++it) {
        if (tailleCase < (*it)->getNom().size()) {
            tailleCase = (int)(*it)->getNom().size();
        }
    }
    tailleCase = tailleCase + 4;
    
    for (int i = 0; i < 8; i++)
    {
        dixespace += " ";
    }
    
    for (int i = 0; i < tailleCase + (tailleCase+1) * nbr_symbole; i++)
    {
        separation += "-";
    }
    
    for (int i = 0; i < tailleCase; i++)
    {
        separationPlus += "-";
    }
    
    separation += "|";
    std::cout << dixespace << separation << std::endl;
    std::cout << dixespace;
    std::cout << "|";
    
    affichageCentre(nom, tailleCase);
    
    for (std::list<std::string>::iterator it = alphabet.begin(); it != alphabet.end(); ++it)
    {
        
        affichageCentre(*it, tailleCase);
    
    }

    std::cout << std::endl;
    std::cout << dixespace << separation << std::endl;

    for (std::list<Etat *>::iterator it = etats_automate.begin(); it != etats_automate.end(); ++it)
    {

        bool init = false;
        bool termi = false;
        
        for (std::list<Etat *>::iterator it_init = etats_initiaux.begin(); it_init != etats_initiaux.end(); ++it_init)
        {
            
            if ((*it)->getNom() == (*it_init)->getNom())
            {
                init = true;
            }
            
        }
        
        for (std::list<Etat *>::iterator it_init = etats_terminaux.begin(); it_init != etats_terminaux.end(); ++it_init)
        {
            
            if ((*it)->getNom() == (*it_init)->getNom())
            {
                termi = true;
            }
            
        }
        
        if (init == false && termi == false)
        {
            std::cout << dixespace;
        }
        
        if (init == true && termi == false)
        {
            std::cout << entrant;
        }
        
        if (init == false && termi == true)
        {
            std::cout << sortant;
        }
        
        if (init == true && termi == true)
        {
            std::cout << entrantsortant;
        }
        
        std::cout << "|";

        affichageCentre((*it)->getNom(), tailleCase);

        
        for (std::list<std::string>::iterator it_transi = alphabet.begin(); it_transi != alphabet.end(); it_transi++)
        {
            std::string desti = (*it)->rechercheTransition((*it_transi));
            
            affichageCentre(desti, tailleCase);
            
        }
        
        
        std::cout << std::endl;
        std::cout << dixespace << separation << std::endl;
        
    }
    
    std::cout << std::endl << std::endl;
    
}

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------

//Permet de verifier la presence d'un etat avec son nom dans une liste d'etat.
bool chercherNomEtat(std::string recherche, std::list<Etat*> etat_anciens){
                       
     for(std::list<Etat*>::iterator it = etat_anciens.begin(); it != etat_anciens.end(); ++it ){
                           
          if(recherche == (*it)->getNom()){
                               
              return false;
                               
          }
                           
      }
                       
      return true;
                       
}
              
//---------------------------------------------------------------------------------------------

//Permet d'afficher un element de maniere centre dans une case de taille donne.
void affichageCentre(std::string element, int taille_case){
    
    int diff = (int)(taille_case - element.size());
    
    if (diff < 0) {
        diff = 0;
    }
    
    
    for (int i = 0; i < ( (diff / 2) + (diff%2)) ; i++){
        std::cout << " ";
    }
        
        std::cout << element;
    for (int i = 0; i < ( diff / 2); i++){
        std::cout << " ";
    }
    std::cout << "|";

}

//---------------------------------------------------------------------------------------------
