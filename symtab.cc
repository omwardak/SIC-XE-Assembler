//
// Created by Omar Wardak on 10/16/2017.
//

#include "symtab.h"


symtab::symtab() {
    size = 0;

}

bool symtab::contains(string label) {
    if (mymap.count(label) > 0) {
        return true;
    }
    return false;
}

void symtab:: insert (string label, int locctr) {
    pair<string, int> tmp;
    tmp = make_pair(label, locctr);
    mymap.insert(tmp);
    size++;
}

int symtab:: getsize(){
    return size;
}

int symtab:: gettab(string key){
    int location;
    string exception;
    if(symtab::contains(key)) {
        location = mymap[key];
        return location;
    }
    else{
        exception.append("Key not found!");
        throw symtab_exception(exception);
    }
}
