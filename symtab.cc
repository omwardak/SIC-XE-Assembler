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
void symtab:: append (string label, string operand, int line){
    string exception;
    transform(operand.begin(), operand.end(), operand.begin(), ::toupper);
    transform(label.begin(), label.end(), label.begin(), ::toupper);
    if(contains(operand) && contains(label)) {
        mymap[label] = gettab(operand, line);
    } else{
        convert_to_string << line;
        exception.append("Error at line: " + convert_to_string.str() + ". Invalid EQU syntax");
        throw symtab_exception(exception);
    }
}

void symtab:: appendnum (string label, int value, int line){
    string exception;
    transform(label.begin(), label.end(), label.begin(), ::toupper);
    if(contains(label)){
        mymap[label] = value;
    } else{
        convert_to_string << line;
        exception.append("Error at line: " + convert_to_string.str() + ". Invalid EQU syntax");
        throw symtab_exception(exception);
    }
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
