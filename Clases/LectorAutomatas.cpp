/*
ALUMNA:CRUZ GRANADOS ANDRE AMELIE
GRUPO: 3CV17
GENERADOR DE AUTOMATAS A PARTIR DE UN ARCHIVO QUE EL USUARIO INDICA
EL ARCHIVO TIENE EL FORMATO:
LINEA 1: IDENTIFICADORES NUMERICOS DE ESTADOS SEPARADOS POR UNA COMA
LINEA 2: SIMBOLOS DEL ALFABETO DEL AUTOMATA SEPARADOS POR UNA COMA
LINEA 3: IDENTIFICADOR DEL ESTADO INICIAL 
LINEA 4: IDENTIFICADORES DEL CONJUNTO DE ESTADOS FINALES SEPARADOS POR  COMA
LINEA 5 EN ADELANTE: DESCRIPCION DE TRANSICIONES DE LA MANERA: 
                    ESTADO_ACTUAL,SIMBOLO,ESTADO_SIGUIENTE
NOTAS: -NO DEBE INCLUIR ESPACIOS NINGUNA LINEA.
       -EL NOMBRE DEL ARCHIVO NO DEBE TENER ESPACIOS 
*/
#include <iostream>
#include <string>
#include "automata.hpp"
#include "archivo.hpp"
using namespace std;
int main(){
    cout<<"Ingresa el nombre del archivo donde se define el autómata,";
    cout<<" si deseas que sea el archivo por defecto (entradas.txt) ingresa 1"<<endl;
    string nombreArchivo;
    cin>>nombreArchivo;
    if(nombreArchivo == "1" )
        nombreArchivo="";
    Archivo archivo(nombreArchivo);
    Automata automata;
    if(archivo>>&automata){
        cout<<"El automata tiene ciclos, por favor revise la descripcion del automata"<<endl;
        return -1;
    }
    cout<<"el archivo con el automata completado se llama: "<<archivo.getNombreArchivoSalida()<<endl;
    bool termina = false;
    while(!termina){
        cout<<"ingresa la cadena a analizar"<<endl;
        string cadena;  
        cin>>cadena;    
            pair<string, string> aceptacion = automata.aceptaCadena(cadena);
            if(aceptacion.first!="error"){
            bool acepto = (aceptacion.first.find('t') != string::npos||!aceptacion.first.empty());
            cout<<(acepto?"se acepto la cadena\ncon el recorrido: "
                +aceptacion.second: "no se acepto la cadena")<<endl; 
            }
        
        cout<<"presione 1 para ingresar otra cadena\no cualquier tecla para terminar el programa"<<endl;
        string continua;
        cin>>continua;
        termina = !(continua == "1");
    }
}