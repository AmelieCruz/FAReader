#ifndef INC_AUTOMATA_HPP
#define INC_AUTOMATA_HPP
#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <fstream>
#include "grafo.hpp"
#include "estado.hpp"
#include "transicion.hpp"
/*clase que define al autómata leído*/
class Automata{
    private:
        /*vector de estados que tiene el automata*/
        std::vector<Estado*> *estados;
        /* vector de simbolos que tiene el automata*/
        std::vector<char> *simbolos;
        /*grafo que describe al automata*/
        Grafo *grafo;
        /*vector de string que almacena los recorridos al leer una cadena aceptada*/
        std::vector<std::string> *recorridos;
        /*vector de string que almacena los recorridos al leer una cadena no aceptada*/
        std::vector<std::string> *recorridosError;
        /*vector que almacena los errores en cada recorrido*/
        std::vector<std::string> *errores;
        /*variable que indica el id del estado de error*/
        Estado* estadoError;
        /*constante que define el simbolo vacío*/
        const char epsilon = 'e';
    public:
        Automata(){
            estados = new std::vector<Estado*>();
            simbolos = new std::vector<char>();
            grafo = new Grafo();
            recorridos = new std::vector<std::string>();
            recorridosError = new std::vector<std::string>();
            errores = new std::vector<std::string>();
            estadoError = new Estado(404);
        }
        /*metodo que devuelve el vector de estados*/
        std::vector<Estado*> *getEstados(){
            return estados;
        }
        /*metodo para agregar un estado*/
        void agregarEstado(Estado* estado){
            estados->push_back(estado);
        }
        void agregarEstado(int estado){
            estados->push_back(new Estado(estado));
        }
        /*metodo para agregar un simbolo*/
        void agregarSimbolo(char symbol){
            simbolos->push_back(symbol);
        }
        /*metodo para agregar una transicion al automata*/
        void agregarTransicion(Estado* estadoAct, Estado* estadoSig, char symbol){
            grafo->agregarTransicion(estadoAct, estadoSig, symbol);
        }
        /*metodo que agrega el estado de error para las transiciones que faltan*/
        void agregarEstadoError(){
            bool estadoErrorExiste = true;
            while(estadoErrorExiste){
                estadoErrorExiste = false;
                for(auto c: *estados){
                    if(c->getID() == estadoError->getID()){
                        estadoError->setID( estadoError->getID()+1);
                        estadoErrorExiste = true;
                    }
                }
            }
            bool tieneSimbolo, agregoEstadoError = false;
            for(auto edoAct: *estados){
                for(auto symbol: *simbolos){
                    tieneSimbolo = false;
                    if(!grafo->operator[](edoAct)->empty()){
                        for(auto trans: *grafo->operator[](edoAct)){
                            if(symbol == trans->getSymbol()){
                                tieneSimbolo = true;
                            }
                        }
                    }

                    if(!tieneSimbolo){
                        if(!agregoEstadoError){
                            this->agregarEstado(estadoError);
                            for(auto s: *simbolos){
                                grafo->agregarTransicion(estadoError, estadoError, s);
                            }
                            agregoEstadoError = true;
                        }
                        Transicion *transicion = new Transicion(symbol);
                        transicion->agregarEstadoSig(estadoError);
                        grafo->agregarTransicion(edoAct, transicion);
                    }
                }
                
            }
        }
        /*metodo que indica si el automata acepta una cadena*/
        std::pair<std::string, std::string> aceptaCadena(std::string cadena){
            if(simbolos->empty()){
                std::cout<<"no hay simbolos en el alfabeto"<<std::endl;
                return std::make_pair("error", "");
            }
            if(estados->empty()){
                std::cout<< "no hay estados en el automata"<<std::endl;
                return std::make_pair("error", "");
            }
            recorridos->clear();
            std::string aceptacion = "";
            for(auto estado: *estados){
                std::string recorrido;
                if(estado->getEsInicial()){
                    std::tuple<std::string, std::vector<std::string>*,  std::vector<std::string>*> camino = recorrerAutomata(aceptacion, cadena, 0, estado, recorrido, "", grafo->obtenerSiguientesEstados(estado, cadena.at(0))->at(0)==estadoError);
                    std::string stringCamino =std::get<2>(camino)->operator[](0);
                    stringCamino.append(std::get<1>(camino)->operator[](0).empty()?"":" Manejo de errores en: "+std::get<1>(camino)->operator[](0));
                    std::string acepto = (!recorridos->empty()?"t":std::get<0>(camino));                  
                    return std::make_pair(acepto, stringCamino);
                }
            }  
            return std::make_pair("error", "");
        }
        /*metodo que imprime un automata en un flujo de salida determinado*/
        void imprimirAutomata(std::ofstream *salida){
            std::string estados, simbolos, estadoInit, estadoFin;
            std::vector<std::tuple<int, char, int>> transiciones;
            bool recorrioSimbolos = false;
            for(auto g: *grafo->getGrafo()){
                estados.append(std::to_string(g.first->getID())+",");
                Estado *estadoTemporal = this->operator[](g.first);
                if(estadoTemporal->getEsInicial()){
                    estadoInit = std::to_string(estadoTemporal->getID());
                }
                if(estadoTemporal->getEsFinal()){
                   estadoFin.append(std::to_string(estadoTemporal->getID())+",");
                }
                for(auto s: g.second){
                    char csymbol = s->getSymbol();
                    if(!recorrioSimbolos){
                        simbolos.append(&csymbol);
                        simbolos.append(",");
                    }
                    for(auto t: *s->getEstadosSig()){
                        transiciones.push_back(std::make_tuple(g.first->getID(), csymbol, t->getID()));
                    }
                }
                recorrioSimbolos = true;
            }
            estados.erase(estados.size()-1);
            simbolos.erase(simbolos.size()-1);
            estadoFin.erase(estadoFin.size()-1);
            *salida<<estados<<std::endl;
            *salida<<simbolos<<std::endl;
            *salida<<estadoInit<<std::endl;
            *salida<<estadoFin<<std::endl;
            for(auto c: transiciones){
                *salida<<std::get<0>(c)<<","<<std::get<1>(c)<<","<<std::get<2>(c)<<std::endl;
            }
        }
        /*método que devuelve el grafo completo*/
        std::map<Estado*, std::vector<Transicion*>>* getGrafo(){return grafo->getGrafo();}
        /*operador que devuelve un estado a partir de su ID*/
        Estado* operator[](int id){
            for(int i = 0; i<estados->size(); i++){
                Estado *temp=estados->operator[](i);
                if(temp->getID()==id){                    
                    return temp;
                }
            }
            return NULL;
        }
        Estado* operator[](Estado* estado){
            for(int i = 0; i<estados->size(); i++){
                Estado *temp=estados->operator[](i);
                if(temp->getID()==estado->getID()){                    
                    return temp;
                }
            }
            return NULL;
        }
        bool encontrarCiclos(){
            for(auto graph: *grafo->getGrafo()){
                for(auto transition: graph.second){
                    if(transition->getSymbol() == epsilon){
                        for(auto t: *transition->getEstadosSig()){
                            if(llegaAEstado(t, graph.first, std::to_string(graph.first->getID()).append(">"))){
                                return true;
                            }
                        }
                    }
                }
            }
            return false;
        }
        private:
        /*metodo que sirve para recorrer un automata con recursividad dada una cadena
        y un punto de inicio*/
        std::tuple<std::string,std::vector<std::string>*,std::vector<std::string>*> recorrerAutomata(std::string aceptacion,std::string cadena, int posCadena, Estado* estadoAct, std::string recorrido, std::string manejoErrores, bool caminoEpsilon){
            bool estaEnAlfabeto = false;
            if(estadoAct== nullptr || recorrido.find("->)")!= std::string::npos){
                return std::make_tuple("error",new std::vector<std::string>(),new std::vector<std::string>({recorrido}));
            }else{
                if(posCadena < cadena.size()){
                    for(auto s: *simbolos){
                        if(cadena.at(posCadena)==s){
                            estaEnAlfabeto = true;
                        }
                    }
                    if(!estaEnAlfabeto){
                        manejoErrores.append(std::to_string(estadoAct->getID())+"("+cadena.at(posCadena)+") ");
                        recorrerAutomata(aceptacion, cadena, posCadena + 1, estadoAct, recorrido, manejoErrores, false);
                    }else{
                        recorrido.append(std::to_string(estadoAct->getID()));
                        recorrido.append("(");
                        std::stringstream streamSymbol; 
                        std::string symbol;
                        streamSymbol<<((caminoEpsilon&&(!grafo->obtenerSiguientesEstados(estadoAct, epsilon)->empty()))?epsilon:cadena.at(posCadena));
                        streamSymbol>> symbol;
                        recorrido.append(symbol);
                        recorrido.append(")->");
                        if(!(grafo->obtenerSiguientesEstados( estadoAct, epsilon )->empty())){
                            for(auto v: *grafo->obtenerSiguientesEstados( estadoAct, epsilon )){
                                std::tuple<std::string, std::vector<std::string>*, std::vector<std::string>*> acepto = recorrerAutomata(aceptacion, cadena, posCadena, v, recorrido, manejoErrores, true);
                                if(std::get<0>(acepto).find('t') != std::string::npos){
                                    aceptacion = std::get<0>(acepto);
                                }
                            }
                        }
                        for(auto v: *grafo->obtenerSiguientesEstados(estadoAct, cadena.at(posCadena))){
                            std::tuple<std::string, std::vector<std::string>*, std::vector<std::string>*> acepto = recorrerAutomata(aceptacion, cadena, posCadena+1, v, recorrido, manejoErrores, false);
                            if(std::get<0>(acepto).find('t') != std::string::npos){
                                aceptacion = std::get<0>(acepto);
                            }
                        }
                    }
                    
                }else{
                    recorrido.append(std::to_string(estadoAct->getID()));
                    if(estadoAct->getEsFinal()){
                        aceptacion.append("t");
                        recorridos->push_back(recorrido);
                        errores->push_back(manejoErrores);
                    }else{
                        aceptacion.append("f");
                        recorridosError->push_back(recorrido);
                    }
                    if(!grafo->obtenerSiguientesEstados(estadoAct, epsilon)->empty()){
                        recorrido.append("(");
                        std::stringstream streamSymbol; 
                        std::string symbol;
                        streamSymbol<<epsilon;
                        streamSymbol>> symbol;
                        recorrido.append(symbol);
                        recorrido.append(")->");
                        for(auto v: *grafo->obtenerSiguientesEstados( estadoAct, epsilon )){
                            std::tuple<std::string, std::vector<std::string>*, std::vector<std::string>*> acepto = recorrerAutomata(aceptacion, cadena, posCadena, v, recorrido, manejoErrores, true);
                            if(std::get<0>(acepto).find('t') != std::string::npos){
                                aceptacion = std::get<0>(acepto);
                            }
                        }
                    }
                }
            }
                return std::make_tuple(aceptacion, errores, (recorridos->empty()?recorridosError:recorridos));
        }
        /*metodo que indica si se puede llegar desde un estado a otro*/
        bool llegaAEstado(Estado* estadoInicial, Estado* estadoFinal, std::string camino){
            camino.append(std::to_string(estadoInicial->getID())+">");
            for(auto symbol: *simbolos){
                for(auto transicion: *grafo->obtenerSiguientesEstados(estadoInicial, symbol)){
                    if(transicion == estadoFinal){
                        return true;
                    }else if(transicion == estadoError){ 
                        continue;
                    }else if(camino.find(std::to_string(transicion->getID())+">")!= std::string::npos){
                        return true;
                    }else{
                        return llegaAEstado(transicion, estadoFinal, camino);
                    }
                }
            }
             for(auto transicion: *grafo->obtenerSiguientesEstados(estadoInicial, epsilon)){
                    if(transicion == estadoFinal){
                        return true;
                    }else if(transicion == estadoError){
                        continue;
                    }else if(camino.find(std::to_string(transicion->getID())+">")!= std::string::npos){
                        return true;
                    }else{
                        return llegaAEstado(transicion, estadoFinal, camino);
                    }
             }
            return false;
        }
};
#endif