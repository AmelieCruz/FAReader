#ifndef INC_ARCHIVO_HPP
#define INC_ARCHIVO_HPP
#pragma once
#include <sstream>
#include<fstream>
#include <string>
#include "automata.hpp"
/*clase que se encarga del manejo del archivo y entradas desde este al programa
para definir el autómata*/
class Archivo{
    private: 
        /*nombre del archivo de entrada*/
        std::string nombreArchivo;
        /*atributo que sirve como flujo de datos de un archivo determinado*/
        std::ifstream archivoAutomata;
        /*atributo que se encarga de la escritura del automata completo*/
        std::ofstream archivoAutomataCompleto;
    public:
        Archivo(std::string nombreArchivo){
            if(nombreArchivo.empty()|| nombreArchivo ==""){
                nombreArchivo = "entradas.txt";
            }else if(nombreArchivo.rfind(".txt")== std::string::npos){
                nombreArchivo.append(".txt");
            }
            this->nombreArchivo = nombreArchivo;
            archivoAutomata.open(nombreArchivo); 
            archivoAutomataCompleto.open("Completo_"+nombreArchivo);
        }
        /*operador que  lee el autómata desde el archivo txt indicado*/
        void operator>>(Automata *automata){
            std::string estados, simbolos, transicion, edoInit, edoFin;
            archivoAutomata>>estados>>simbolos>>edoInit>>edoFin;
            this->leerEstados(estados, automata);
            this->leerSimbolos(simbolos, automata);
            this->leerEstadoInicial(edoInit, automata);
            this->leerEstadosFinales(edoFin, automata);
            while(archivoAutomata>>transicion){
                this->leerTransicion(transicion, automata);
            }
            automata->agregarEstadoError();
            automata->imprimirAutomata(&archivoAutomataCompleto);
        }
        /*operador que se encarga de escribir en el archivo*/
        void operator<<(std::string linea){archivoAutomataCompleto<<linea<<std::endl; }
        /*metodo que retorna el nombre del archivo donde se genera el automata completo*/
        std::string getNombreArchivoSalida(){return "Completo_"+nombreArchivo;}
        
    private:  
        /*metodo que sirve para reconocer los estados que tendrá el 
        automata identificando como separador una ","*/  
        void leerEstados(std::string estados, Automata *automata){
            std::string estado ="";
            for(auto c: estados){
                if(c == ','|| c ==' '){
                    automata->agregarEstado(stoi(estado));
                    estado = "";
                }
                else{
                    std::stringstream streamEstado;
                    streamEstado<<c;
                    std::string tempEdo;
                    streamEstado>>tempEdo;
                    estado.append(tempEdo);
                }
            }
            automata->agregarEstado(std::stoi(estado));
        }
        /*metodo que sirve para reconocer los simbolos que tendrá el alfabeto
        del automata indentificando como separador una ","*/
        void leerSimbolos(std::string simbolos, Automata *automata){
            for(auto c: simbolos){
                if(c != ',' && c!= ' '){
                    automata->agregarSimbolo(c);
                }
            }
        }
        /*metodo que lee cual es el estado inicial*/
        void leerEstadoInicial(std::string estadoInit, Automata *automata){
            automata->operator[](std::stoi(estadoInit))->setEsInicial(true);
        }
        /*metodo asigna el valor de los estados finales*/
        void leerEstadosFinales(std::string estadosFinales, Automata *automata){
            std::string estado="";
            for(int i = 0; i<estadosFinales.size();i++){
                char c = estadosFinales[i];
                if(c == ','|| c ==' '|| i == (estadosFinales.size()-1)){
                    if(c!=','){
                        estado.append(&c);
                    }
                    automata->operator[](stoi(estado))->setEsFinal(true);
                    estado = "";
                }else{
                    estado.append(&c);
                }
            }
        }
        /*metodo que identifica las transiciones con un formato donde se separa
         cada parte de la funcion de transicion con una coma*/
        void leerTransicion(std::string transicion, Automata *automata){
            std::string buffer="";
            int edoAct, edoSig, comas = 0;
            char simbolo;
            for(int i = 0; i<transicion.size(); i++){
                char c = transicion[i];
                if(i==transicion.size()-1&&transicion.at(transicion.size()-1)!=','){
                    std::stringstream s;
                    s<<c;
                    std::string cBuffer;
                    s>>cBuffer;
                    buffer.append(cBuffer);
                }
                if(c == ',' || c== ' '||i==transicion.size()-1){
                    switch (comas)
                    {
                    case 0:
                        edoAct = std::stoi(buffer);
                        break;
                    case 1:
                        simbolo = buffer[0];
                        break;
                    case 2:
                        edoSig = std::stoi(buffer);
                        break;
                    default:
                        break;
                    }
                    buffer = "";
                    comas ++;
                }
                else{
                    std::stringstream s;
                    s<<c;
                    std::string cBuffer;
                    s>>cBuffer;
                    buffer.append(cBuffer);
                }
            }            
            automata->agregarTransicion(edoAct, edoSig, simbolo);
        }
};
#endif