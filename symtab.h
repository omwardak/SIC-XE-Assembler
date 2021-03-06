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
#include <sstream>
#include "symtab_exception.h"



using namespace std;

class symtab {
public:

    symtab();


    bool contains(string);
    void appendnum(string label, int value, int line);
    void append (string label, string operand, int line);
    void insert (string,int);
    int getsize();

    int gettab(string, int);


private:
    // your variables and private methods go here
    std::map<string, int> mymap;
    int size;
    ostringstream convert_to_string;


};

#endif
