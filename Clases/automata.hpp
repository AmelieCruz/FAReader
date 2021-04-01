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
#define epsilon 'e'
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
        int estadoError;
    public:
        Automata(){
            estados = new std::vector<Estado*>();
            simbolos = new std::vector<char>();
            grafo = new Grafo();
            recorridos = new std::vector<std::string>();
            recorridosError = new std::vector<std::string>();
            errores = new std::vector<std::string>();
            estadoError = 404;
        }
        /*metodo para agregar un estado*/
        void agregarEstado(int estado){
            estados->push_back(new Estado(estado));
        }
        /*metodo para agregar un simbolo*/
        void agregarSimbolo(char symbol){
            simbolos->push_back(symbol);
        }
        /*metodo para agregar una transicion al automata*/
        void agregarTransicion(int estadoAct, int estadoSig, char symbol){
            grafo->agregarTransicion(estadoAct, estadoSig, symbol);
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
                    std::tuple<std::string, std::vector<std::string>*,  std::vector<std::string>*> camino = recorrerAutomata(aceptacion, cadena, 0, estado, recorrido, "", grafo->obtenerSiguientesEstados(estado->getID(), cadena.at(0))->at(0)==estadoError);
                    std::string stringCamino =std::get<2>(camino)->operator[](0);
                    stringCamino.append(std::get<1>(camino)->operator[](0).empty()?"":" Manejo de errores en: "+std::get<1>(camino)->operator[](0));
                    std::string acepto = (!recorridos->empty()?"t":std::get<0>(camino));
                    return std::make_pair(acepto, stringCamino);
                }
            }
            return std::make_pair("error", "");
        }
        /*metodo que devuelve el vector de estados*/
        std::vector<Estado*> *getEstados(){
            return estados;
        }
        /*metodo que agrega el estado de error para las transiciones que faltan*/
        void agregarEstadoError(){
            bool estadoErrorExiste = true;
            while(estadoErrorExiste){
                estadoErrorExiste = false;
                for(auto c: *estados){
                    if(c->getID() == estadoError){
                        estadoError++;
                        estadoErrorExiste = true;
                    }
                }
            }
            bool tieneSimbolo, agregoEstadoError = false;
            for(auto edoAct: *estados){
                for(auto symbol: *simbolos){
                    tieneSimbolo = false;
                    if(!grafo->operator[](edoAct->getID())->empty()){
                        for(auto trans: *grafo->operator[](edoAct->getID())){
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
                        grafo->operator[](edoAct->getID())->push_back(transicion);
                    }
                }
                
            }
        }
        /*metodo que imprime un automata en un flujo de salida determinado*/
        void imprimirAutomata(std::ofstream *salida){
            std::string estados, simbolos, estadoInit, estadoFin;
            std::vector<std::tuple<int, char, int>> transiciones;
            bool recorrioSimbolos = false;
            for(auto g: *grafo->getGrafo()){
                estados.append(std::to_string(g.first)+",");
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
                        transiciones.push_back(std::make_tuple(g.first, csymbol, t));
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
        std::map<int, std::vector<Transicion*>>* getGrafo(){return grafo->getGrafo();}
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
        private:
        /*metodo que sirve para recorrer un automata con recursividad dada una cadena
        y un punto de inicio*/
        std::tuple<std::string,std::vector<std::string>*,std::vector<std::string>*> recorrerAutomata(std::string aceptacion,std::string cadena, int posCadena, Estado *estadoAct, std::string recorrido, std::string manejoErrores, bool caminoEpsilon){
            bool estaEnAlfabeto = false;
            if(estadoAct == NULL || recorrido.find("->)")!= std::string::npos){
                std::cout<<"error en el recorrido"<<std::endl;
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
                        ++posCadena;
                        recorrerAutomata(aceptacion, cadena, posCadena, estadoAct, recorrido, manejoErrores, grafo->obtenerSiguientesEstados(estadoAct->getID(), cadena.at(posCadena))->at(0)==estadoError);
                    }else{
                        recorrido.append(std::to_string(estadoAct->getID()));
                        recorrido.append("(");
                        std::stringstream streamSymbol; 
                        std::string symbol;
                        streamSymbol<<((caminoEpsilon&&(!grafo->obtenerSiguientesEstados(estadoAct->getID(), epsilon)->empty()))?epsilon:cadena.at(posCadena));
                        streamSymbol>> symbol;
                        recorrido.append(symbol);
                    }
                }
                if(cadena.size() == posCadena){
                    recorrido.append(std::to_string(estadoAct->getID()));
                    if(estadoAct->getEsFinal()){
                        aceptacion.append("t");
                        recorridos->push_back(recorrido);
                        errores->push_back(manejoErrores);
                        std::cout<<"recorrido bueno: "<<recorridos->back()<<std::endl;
                    }else{
                        aceptacion.append("f");
                        recorridosError->push_back(recorrido);
                        std::cout<<"recorrido mal: "<<recorridosError->back()<<std::endl;
                    }
                    if(!grafo->obtenerSiguientesEstados(estadoAct->getID(), epsilon)->empty()){
                        recorrido.append("(");
                        std::stringstream streamSymbol; 
                        std::string symbol;
                        streamSymbol<<epsilon;
                        streamSymbol>> symbol;
                        recorrido.append(symbol);
                        recorrido.append(")->");
                        for(auto v: *grafo->obtenerSiguientesEstados( estadoAct->getID(), epsilon )){
                            std::tuple<std::string, std::vector<std::string>*, std::vector<std::string>*> acepto = recorrerAutomata(aceptacion, cadena, posCadena, this->operator[](v), recorrido, manejoErrores, true);
                            if(std::get<0>(acepto).find('t') != std::string::npos){
                                aceptacion = std::get<0>(acepto);
                            }
                        }
                    }
                }else{
                    recorrido.append(")->");
                    if(!(grafo->obtenerSiguientesEstados( estadoAct->getID(), epsilon )->empty())){
                        for(auto v: *grafo->obtenerSiguientesEstados( estadoAct->getID(), epsilon )){
                             std::tuple<std::string, std::vector<std::string>*, std::vector<std::string>*> acepto = recorrerAutomata(aceptacion, cadena, posCadena, this->operator[](v), recorrido, manejoErrores, true);
                            if(std::get<0>(acepto).find('t') != std::string::npos){
                                aceptacion = std::get<0>(acepto);
                            }
                        }
                    }
                    for(auto v: *grafo->obtenerSiguientesEstados(estadoAct->getID(), cadena.at(posCadena))){
                        std::tuple<std::string, std::vector<std::string>*, std::vector<std::string>*> acepto = recorrerAutomata(aceptacion, cadena, posCadena+1, this->operator[](v), recorrido, manejoErrores, false);
                        if(std::get<0>(acepto).find('t') != std::string::npos){
                            aceptacion = std::get<0>(acepto);
                        }
                    }

                }
                return std::make_tuple(aceptacion, errores, (recorridos->empty()?recorridosError:recorridos));
            }
        }
};
#endif