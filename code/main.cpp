#include "automate.h"
#include <cstdlib>



void Pause(){
    
    std::string entre;
        
    std::cout << "Tapez sur entrer pour continuer..." << std::endl;
    getline(std::cin, entre);
    
    std::cout << std::endl << std::endl;
    
}

int main(){

    
    int fin = 0;
    
    while(fin != 1){
        
        
        Automate* mon_auto = creation_automate();
        
        mon_auto->afficher_automate_AF();
        
        Pause();
        
        if (mon_auto->est_asynchrone()) {
            
            mon_auto->synchroniser();
            
        }else{
            
            if (mon_auto->est_deterministe()) {
                
                if (mon_auto->est_complet()) {
                    
                    mon_auto->setAFDC();
                    
                }else{
                    
                    mon_auto->completer_automate();
                    
                }
                
            }else{
                
                mon_auto->determiniser_automate_synchrone();
                
            }
            
        }
        
        mon_auto->afficher_automate_AFDC();
        
        Pause();
        
        mon_auto->minimiser_automate();
        
        mon_auto->afficher_automate_AFDCM();
        
        Pause();
        
        mon_auto->demander_mot();
        
        mon_auto->complementaire_automate();
        
        mon_auto->afficher_automate_Autre();
        
        Pause();
        
        mon_auto->demander_mot();
        
        mon_auto->standardiser();
        
        mon_auto->afficher_automate_Autre();
        
        Pause();
        
        mon_auto->demander_mot();
        
        std::cout << "Voulez vous tester avec un autre automate ? (oui ou non)";
        
        std::string choix = "choix";
        
        do {
            getline(std::cin, choix);
            if (choix == "non") {
                fin = 1;
            }
        } while(choix != "oui" && choix != "non");
     
        delete mon_auto;
        
        
    }
         
    return 0;
}
