#ifndef INC_TRANSICION_HPP
#define INC_TRANSICION_HPP
#pragma once
#include <vector>
#include "estado.hpp"
/*clase que define una transicion, donde se almacena el simbolo
 y el conjunto de estados a los que se llega con ese simbolo desde el mismo estado*/
class Transicion{
    private:
        /*simbolo el cual define la transicion*/
        char symbol;
        /*conjunto de estados a los que se puede llegar con el mismo simbolo*/
        std::vector<Estado*> *estadosSig;
    public:
        Transicion(char symbol){
            this->symbol = symbol;
            estadosSig = new std::vector<Estado*>();
        }
        /*agrega un nuevo estado al que se puede llegar con el simbolo de la transicion*/
        void agregarEstadoSig(Estado* estadoSiguiente){
            estadosSig->push_back(estadoSiguiente);
        }
        /*retorna el vector de estados a los que se puede llegar con un simbolo*/
        std::vector<Estado*> * getEstadosSig(){
            return estadosSig;
        }
        /*establecer el simbolo de la transicion*/
        void setSymbol(char symbol){
            this->symbol = symbol;
        }
        /*retorna el simbolo de la transicion*/
        char getSymbol(){
            return symbol;
        }
};
#endif