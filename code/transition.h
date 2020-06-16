#ifndef TRANSITION_H_INCLUDED
#define TRANSITION_H_INCLUDED
#include <string>
#include <iostream>
#include <queue>
#include <cstdlib>
#include <sstream>
#include <fstream>
#include <list>

class Transition{
private:
    std::string position;
    std::string transi;
    std::string destination;
public:
    Transition(std::string premier, std::string deuxieme, std::string troisieme);
    ~Transition();
    inline std::string getPosition()const {return position;};
    
    inline std::string getTransi() const {return transi;};
    
    inline std::string getDestination() const {return destination;};
    
    inline void setTransi(std::string Transi){ transi = Transi;};
    
    inline void setDestination(std::string Desti){ destination = Desti;};
    
    inline void afficher_transition() const { std::cout << position << " " << transi << " " << destination;};
};

#endif // TRANSITION_H_INCLUDED
