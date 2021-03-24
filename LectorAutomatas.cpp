#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <fstream>
#define epsilon "_"
using namespace std;
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
        bool equals(Estado *estado){
            return this->id == estado->id? true: false;
            
        }
};
/*clase que define una transicion, donde se almacena el simbolo
 y el conjunto de estados a los que se llega con ese simbolo desde el mismo estado*/
class Transicion{
    private:
        /*simbolo el cual define la transicion*/
        char symbol;
        /*conjunto de estados a los que se puede llegar con el mismo simbolo*/
        vector<int> *estadosSig;
    public:
        Transicion(char symbol){
            this->symbol = symbol;
            estadosSig = new vector<int>();
        }
        /*agrega un nuevo estado al que se puede llegar con el simbolo de la transicion*/
        void agregarEstadoSig(int edoSig){
            estadosSig->push_back(edoSig);
        }
        /*retorna el vector de estados a los que se puede llegar con un simbolo*/
        vector<int> * getEstadosSig(){
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
/*clase que define al grafo a través del cual se representa el automata definido*/
class Grafo{
    private:
        /*grafo que almacena como key el id del estado actual y como valor
        el vector de transiciones que puede tener ese estado*/
        map<int, vector<Transicion*>> *grafoAutomata;
    public:
        Grafo(){
            grafoAutomata = new map<int, vector<Transicion*>>();
        }
        /*Método para agregar una nueva transicion a un estado*/
        void agregarTransicion(int estado, int estadoSig, char symbol){
            bool existeTransicion = false;
            vector<Transicion*> *temporal = &grafoAutomata->operator[](estado);
           
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
        /*metodo que devuelve el vector de estados a los que se llega desde un estado 
        con un simbolo*/
        vector<int>* obtenerSiguientesEstados(int estadoActual, char symbol){
            vector<Transicion *> temporal = grafoAutomata->at(estadoActual);
            if(!temporal.empty()){
                for(int i = 0; i<temporal.size(); i++){
                    if(temporal[i]->getSymbol() == symbol){
                        return temporal[i]->getEstadosSig();
                    }
                }
            }
            return NULL;
        }
        map<int, vector<Transicion*>>* getGrafo(){return grafoAutomata;}
        vector<Transicion*>* operator[] (int key){return &grafoAutomata->at(key);}




};
/*clase que se encarga del manejo del archivo y entradas desde este al programa
para definir el autómata*/

/*clase que define al autómata leído*/
class Automata{
    private:
        /*vector de estados que tiene el automata*/
        vector<Estado*> *estados;
        /* vector de simbolos que tiene el automata*/
        vector<char> *simbolos;
        /*grafo que describe al automata*/
        Grafo *grafo;
        /*vector de string que almacena los recorridos al leer una cadena aceptada*/
        vector<string> *recorridos;
    public:
        Automata(){
            estados = new vector<Estado*>();
            simbolos = new vector<char>();
            grafo = new Grafo();
            recorridos = new vector<string>();
        };
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
        pair<string, string> aceptaCadena(string cadena){
            if(simbolos->empty()){
                cout<<"no hay simbolos en el alfabeto";
                return make_pair("error", "");
            }
            if(estados->empty()){
                cout<< "no hay estados en el automata";
                return make_pair("error", "");
            }
           
            for (auto c: cadena){
                bool estaEnAlfabeto = false;
                for(auto s: *simbolos){
                    if(c==s){
                        estaEnAlfabeto = true;
                    }
                }
                if(!estaEnAlfabeto){
                    cout<<"el simbolo "<< c<<" no pertenece al alfabeto";
                    return make_pair("error", "");
                }
            }
            string aceptacion = "";
            for(auto estado: *estados){
                vector<string> recorridos;
                string recorrido;
                if(estado->getEsInicial()){
                    pair<string, vector<string>*> camino = recorrerAutomata(aceptacion, cadena, 0, estado,0, recorrido);
                    return make_pair(camino.first, (camino.second!=nullptr?camino.second->operator[](0): ""));
                }
            }
            return make_pair("", "");
        }
        /*metodo que devuelve el vector de estados*/
        vector<Estado*> *getEstados(){
            return estados;
        }
        /*metodo que agrega el estado de error para las transiciones que faltan*/
        void agregarEstadoError(){
            int estadoError = 404;
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
            this->agregarEstado(estadoError);
            bool tieneSimbolo, recorrioSimbolos = false;
            string estadosCompletos;
            for(auto edoAct: *estados){
                for(auto symbol: *simbolos){
                    if(!recorrioSimbolos){
                        grafo->agregarTransicion(estadoError, estadoError, symbol);
                    }
                    tieneSimbolo = false;
                    for(auto trans: *grafo->operator[](edoAct->getID())){
                        if(symbol == trans->getSymbol()){
                            tieneSimbolo = true; 
                        }
                    }
                    if(!tieneSimbolo){
                        Transicion *transicion = new Transicion(symbol);
                        transicion->agregarEstadoSig(estadoError);
                        grafo->operator[](edoAct->getID())->push_back(transicion);
                    }
                }
                recorrioSimbolos = true;
            }
        }
        /*metodo que imprime un automata en un flujo de salida determinado*/
        void imprimirAutomata(ofstream *salida){
            string estados, simbolos, estadoInit, estadoFin;
            vector<tuple<int, char, int>> transiciones;
            bool recorrioSimbolos = false;
            for(auto g: *grafo->getGrafo()){
                estados.append(to_string(g.first)+",");
                for(auto e: *this->estados){
                    if(e->getID() == g.first){
                        if(e->getEsInicial()){
                            estadoInit = to_string(e->getID());
                        }
                        if(e->getEsFinal()){
                            estadoFin = to_string(e->getID())+",";
                        }
                    }
                }
                for(auto s: g.second){
                    char csymbol = s->getSymbol();
                    tuple<int, char, int> transicion;
                    if(!recorrioSimbolos){
                        simbolos.append(&csymbol);
                        simbolos.append(",");
                    }
                    for(auto t: *s->getEstadosSig()){
                        transiciones.push_back(make_tuple(g.first, csymbol, t));
                    }
                }
                recorrioSimbolos = true;
            }
            estados.erase(estados.size()-1);
            simbolos.erase(simbolos.size()-1);
            estadoFin.erase(simbolos.size()-2);
            *salida<<estados<<endl;
            *salida<<simbolos<<endl;
            *salida<<estadoInit<<endl;
            *salida<<estadoFin<<endl;
            for(auto c: transiciones){
                *salida<<get<0>(c)<<","<<get<1>(c)<<","<<get<2>(c)<<endl;
            }
        }
        /*método que devuelve el grafo completo*/
        map<int, vector<Transicion*>>* getGrafo(){return grafo->getGrafo();}
        private:
        /*metodo que sirve para recorrer un automata con recursividad dada una cadena
        y un punto de inicio*/
        pair<string, vector<string>*> recorrerAutomata(string aceptacion,string cadena, int posCadena, Estado *estadoAct, int numRecorrido, string recorrido){
            recorrido.append(to_string(estadoAct->getID()));
            if(estadoAct == NULL){
                cout<<"error en el recorrido"<<endl;
                return make_pair("error",recorridos);
            }
            if(cadena.size() == posCadena){
                if(estadoAct->getEsFinal()){
                    aceptacion.append("t");
                    recorridos->push_back(recorrido);
                    numRecorrido++;
                }else{
                    aceptacion.append("f");
                }
            }else{
                recorrido.append(",");
                for(auto v: *grafo->obtenerSiguientesEstados(estadoAct->getID(), cadena.at(posCadena))){
                    pair<string, vector<string>*> acepto =recorrerAutomata(aceptacion, cadena, posCadena+1, this->obtenerEstado(v), numRecorrido, recorrido);
                    if(acepto.first.find('t') != string::npos){
                        aceptacion = acepto.first;
                    }
                }
            }
            return make_pair(aceptacion, recorridos);
        }
        /*método que devuelve un estado a partir de su ID*/
        Estado* obtenerEstado(int id){
            for(int i = 0; i<estados->size(); i++){
                Estado *temp=estados->operator[](i);
                if(temp->getID()==id){                    
                    return temp;
                }
            }
            return NULL;
        }


};
class Archivo{
    private: 
        /*atributo que sirve como flujo de datos de un archivo determinado*/
        ifstream archivoAutomata;
        /*atributo que se encarga de la escritura del automata completo*/
        ofstream archivoAutomataCompleto;
        /*atributo en donde se almacena el autómata leído*/
        Automata automata;
    public:
        Archivo(string nombreArchivo){
            if(nombreArchivo.empty()|| nombreArchivo ==""){
                nombreArchivo = "entradas.txt";
            }else if(nombreArchivo.rfind(".txt")== string::npos){
                nombreArchivo.append(".txt");
            }
            archivoAutomata.open(nombreArchivo); 
            archivoAutomataCompleto.open("Completo_"+nombreArchivo);
        }
        /*metodo que  lee el autómata desde el archivo txt indicado*/
        void operator>>(Automata *automata){
            string estados, simbolos, transicion, edoInit, edoFin;
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
        void operator<<(string linea) {archivoAutomataCompleto<<linea<<endl; };
        
    private:  
        /*metodo que sirve para reconocer los estados que tendrá el 
        automata identificando como separador una ","*/  
        void leerEstados(string estados, Automata *automata){
            string estado ="";
            for(auto c: estados){
                if(c == ','|| c ==' '){
                    automata->agregarEstado(stoi(estado,nullptr, 10));
                    estado = "";
                }
                else{
                    estado.append(&c);
                }
            }
            automata->agregarEstado(stoi(estado,nullptr, 10));
        }
        /*metodo que sirve para reconocer los simbolos que tendrá el alfabeto
        del automata indentificando como separador una ","*/
        void leerSimbolos(string simbolos, Automata *automata){
            for(auto c: simbolos){
                if(c != ',' && c!= ' '){
                    automata->agregarSimbolo(c);
                }
            }
        }
        /*metodo que lee cual es el estado inicial*/
        void leerEstadoInicial(string estadoInit, Automata *automata){
            for(auto c: *automata->getEstados()){
                if(c->getID() == stoi(estadoInit)){
                    c->setEsInicial(true);
                    return;
                }
            }
        }
        /*metodo asigna el valor de los estados finales*/
        void leerEstadosFinales(string estadosFinales, Automata *automata){
            string estado="";
            for(int i = 0; i<estadosFinales.size();i++){
                char c = estadosFinales[i];
                if(c == ','|| c ==' '|| i == (estadosFinales.size()-1)){
                    if(estado.empty()&& c!=','){
                        estado.append(&c);
                    }
                    for(auto e: *automata->getEstados()){
                        if(e->getID()== stoi(estado)){
                            e->setEsFinal(true);
                        }
                    }
                    estado = "";
                }else{
                    estado.append(&c);
                }
            }
        }
        /*metodo que identifica las transiciones con un formato donde se separa
         cada parte de la funcion de transicion con una coma*/
        void leerTransicion(string transicion, Automata *automata){
            string buffer="";
            int edoAct, edoSig, comas = 0;
            char simbolo;
            for(int i = 0; i<transicion.size(); i++){
                char c = transicion[i];
                if(c == ',' || c== ' '||(i == transicion.size()-1)){
                    if((i == transicion.size()-1 )){
                        buffer.append(&c);
                    }
                    switch (comas)
                    {
                    case 0:
                        edoAct = stoi(buffer);
                        break;
                    case 1:
                        simbolo = buffer[0];
                        break;
                    case 2:
                        edoSig = stoi(buffer);
                        break;
                    default:
                        break;
                    }
                    buffer = "";
                    comas ++;
                }
                else{
                    buffer.append(&c);
                }
            }
            automata->agregarTransicion(edoAct, edoSig, simbolo);
        }

};

int main(){
    cout <<epsilon<<endl;
    cout<<"Ingresa el nombre del archivo donde se define el autómata,";
    cout<<" si deseas que sea el archivo por defecto (entradas.txt) ingresa 1"<<endl;
    string nombreArchivo = "1";
    cin>>nombreArchivo;
    if(nombreArchivo == "1" )
        nombreArchivo="";
    Archivo *archivo = new Archivo(nombreArchivo);
    Automata *automata = new Automata();
    *archivo>>automata;
    cout<<"ingresa la cadena a analizar"<<endl;
    string cadena;  
    cin>>cadena;
    pair<string, string> aceptacion = automata->aceptaCadena(cadena);
    if(aceptacion.first!="error"){
        bool acepto = (aceptacion.first.find('t') != string::npos);
        cout<<(acepto?"se acepto la cadena\ncon el recorrido: "
            +aceptacion.second: "no se acepto la cadena")<<endl; 
    }
}