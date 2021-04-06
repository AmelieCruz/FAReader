#include <iostream>
#include "estado.hpp"
#include <map>
using namespace std;

int main (){
    map<Estado*, string> prueba;
    Estado* e= new Estado(12),*f = new Estado(13), *g = new Estado(14);
    prueba[e]= "hola";
    prueba[f]= "como";
    prueba[g]= "estas";
    Estado *h= new Estado(12);
    cout<<((e == h)?"si":"no")<<endl;
    cout<<((e->getID() == h->getID())?"si":"no")<<endl;
}