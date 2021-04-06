#pragma once
#ifndef GRAFO_HPP
#define GRAFO_HPP
#include <map>
#include <vector>
#include"transicion.hpp"
#include "estado.hpp"
/*clase que define al grafo a través del cual se representa el automata definido*/
class Grafo{
    private:
        /*grafo que almacena como key el id del estado actual y como valor
        el vector de transiciones que puede tener ese estado*/
        std::map<Estado*, std::vector<Transicion*>> *grafoAutomata;
    public:
        Grafo(){
            grafoAutomata = new std::map<Estado*, std::vector<Transicion*>>();
        }
        /*Método para agregar una nueva transicion a un estado*/
        void agregarTransicion(Estado* estado, Estado* estadoSig, char symbol){
            bool existeTransicion = false;
            std::vector<Transicion*> *temporal = &grafoAutomata->operator[](estado);
            if(!temporal->empty()){
                for(int i = 0; i<temporal->size(); i++){
                    if(temporal->operator[](i)->getSymbol() == symbol){
                        temporal->operator[](i)->agregarEstadoSig(estadoSig);
                        existeTransicion = true;
                    }
                }
                if(!existeTransicion){
                    Transicion *nuevaTransicion = new Transicion(symbol);
                    nuevaTransicion->agregarEstadoSig(estadoSig);
                    temporal->push_back(nuevaTransicion);
                }
            }else{
                    Transicion *nuevaTransicion = new Transicion(symbol);
                    nuevaTransicion->agregarEstadoSig(estadoSig);
                    temporal->push_back(nuevaTransicion);
            }
        }
        void agregarTransicion(Estado* estado, Transicion* transicion){
            grafoAutomata->operator[](estado).push_back(transicion);
        }
        /*metodo que devuelve el vector de estados a los que se llega desde un estado 
        con un simbolo*/
        std::vector<Estado*>* obtenerSiguientesEstados(Estado* estadoActual, char symbol)
        {
            std::vector<Transicion *> temporal = grafoAutomata->at(estadoActual);
            if(!temporal.empty()){
                for(int i = 0; i<temporal.size(); i++){
                    if(temporal[i]->getSymbol() == symbol){
                        return temporal[i]->getEstadosSig();
                    }
                }
            }
            return new std::vector<Estado*>();
        }
        /*metodo que retorna el grafo completo*/
        std::map<Estado*, std::vector<Transicion*>>* getGrafo(){return grafoAutomata;}
        /*operador para acceder a una posicion del grafo dado un estado*/
        std::vector<Transicion*>* operator[] (Estado* key){return &grafoAutomata->operator[](key);}
};
#endif