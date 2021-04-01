#pragma once
#ifndef INC_ESTADO_HPP
#define INC_ESTADO_HPP
/*clase que define un estado*/
class Estado{    
    private:
        /*identificador del estado*/
        int id;
        /*indica si el estado es inicial*/
        bool esInicial;
        /*indica si el estado es final*/
        bool esFinal;

    public:
        Estado(int idEdo){
           this->id = idEdo;
            esInicial = false;
            esFinal = false;
        }
        /*retorna el identificador del estado*/
        int getID(){return id;}
        /*indicar si el estado es inicial*/
        void setEsInicial(bool valor){esInicial = valor;}
        /*retorna un bool indicando si el estado es inicial*/
        bool getEsInicial(){return esInicial;}
        /*indicar si el estado es final*/
        void setEsFinal(bool valor){esFinal = valor;}
        /*retorna un bool indicando si el estado es final*/
        bool getEsFinal(){return esFinal;}
        bool operator==(Estado* estado){
            return this->id == estado->id? true: false;
            
        }
};
#endif