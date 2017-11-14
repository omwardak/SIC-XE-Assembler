/*  opcodetab.h
    CS530, Spring 2016
*/

#ifndef SYMTAB_H
#define SYMTAB_H
#include <string>
#include <map>
#include<utility>
#include<exception>
#include "opcode_error_exception.h"
#include <cstdlib>
#include <algorithm>
#include "symtab_exception.h"



using namespace std;

class symtab {
public:

    symtab();


    bool contains(string);


    void insert (string,int);

    int getsize();

    int gettab(string);


private:
    // your variables and private methods go here
    std::map<string, int> mymap;
    int size;


};

#endif
