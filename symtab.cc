//
// Created by Omar Wardak on 10/16/2017.
//

#include <iostream>
#include "symtab.h"


symtab::symtab() {
    size = 0;

}

bool symtab::contains(string label) {
    transform(label.begin(), label.end(), label.begin(), ::toupper);
    if (mymap.count(label) > 0) {
        return true;
    }
    return false;
}

void symtab:: insert (string label, int locctr) {
    transform(label.begin(), label.end(), label.begin(), ::toupper);
    pair<string, int> tmp;
    tmp = make_pair(label, locctr);
    mymap.insert(tmp);
    size++;
}

int symtab:: getsize(){
    return size;
}

int symtab:: gettab(string key, int line){
    transform(key.begin(), key.end(), key.begin(), ::toupper);
    int location;
    string exception;
    if(symtab::contains(key)) {
        location = mymap[key];
        return location;
    }
    else{
        convert_to_string << line;
        exception.append("Error at line: " + convert_to_string.str() + ". Label in operand not found.");
        throw symtab_exception(exception);
    }
}
