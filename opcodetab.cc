
/*  opcodetab.h
    CS530, Spring 2016
*/

#include <sstream>
#include"opcodetab.h"

using namespace std;

// ctor
// creates a new dictionary structure and loads all of the opcodes for
// the SIC/XE architecture into the table.  Use the STL
// map for this.
opcodetab::opcodetab() {

    pair<string, pair<string, string> > tmp;

    const int num_opcodes = 68;
    string op_mnemonics [num_opcodes] = {
            "ADD",    "ADDF",   "ADDR",   "AND",    "CLEAR",  "COMP",   "COMPF",  "COMPR",  "DIV",    "DIVF",
            "DIVR",   "FIX",    "FLOAT",  "HIO",    "J",      "JEQ",    "JGT",    "JLT",    "JSUB",   "LDA",
            "LDB",    "LDCH",   "LDF",    "LDL",    "LDS",    "LDT",    "LDX",    "LPS",    "MUL",    "MULF",
            "MULR",   "NORM",   "OR",     "RD",     "RMO",    "RSUB",   "SHIFTL", "SHIFTR", "SIO",    "SSK",
            "STA",    "STB",    "STCH",   "STF",    "STI",    "STL",    "STS",    "STSW",   "STT",    "STX",
            "SUB",    "SUBF",   "SUBR",   "SVC",    "TD",     "TIO",    "TIX",    "TIXR",   "WD",     "START",
            "END",    "BYTE",   "WORD",   "RESB",   "RESW",   "BASE",   "NOBASE", "EQU"
    };
    string op_lengths [num_opcodes] = {
            "3",      "3",      "2",      "3",      "2",      "3",      "3",      "2",      "3",      "3",
            "2",      "1",      "1",      "1",      "3",      "3",      "3",      "3",      "3",      "3",
            "3",      "3",      "3",      "3",      "3",      "3",      "3",      "3",      "3",      "3",
            "2",      "1",      "3",      "3",      "2",      "3",      "2",      "2",      "1",      "3",
            "3",      "3",      "3",      "3",      "3",      "3",      "3",      "3",      "3",      "3",
            "3",      "3",      "2",      "2",      "3",      "1",      "3",      "2",      "3",      "0",
            "0",      "0",      "0",      "0",      "0",      "0",      "0",      "0"
    };
    string op_hexcodes [num_opcodes] = {
            "18",     "58",     "90",     "40",     "B4",     "28",     "88",     "A0",     "24",     "64",
            "9C",     "C4",     "C0",     "F4",     "3C",     "30",     "34",     "38",     "48",     "00",
            "68",     "50",     "70",     "08",     "6C",     "74",     "04",     "D0",     "20",     "60",
            "98",     "C8",     "44",     "D8",     "AC",     "4C",     "A4",     "A8",     "F0",     "EC",
            "0C",     "78",     "54",     "80",     "D4",     "14",     "7C",     "E8",     "84",     "10",
            "1C",     "5C",     "94",     "B0",     "E0",     "F8",     "2C",     "B8",     "DC",     "0",
            "1",      "2",      "3",      "4",      "5",      "6",      "7",      "8"
    };

    int i = 0;
    while (i < num_opcodes)
    {
        tmp = make_pair(op_mnemonics[i], make_pair(op_lengths[i], op_hexcodes[i]));
        mymap.insert(tmp);
        i++;
    }
}

// takes a SIC/XE opcode and returns the machine code
// equivalent as a two byte string in hexadecimal.
// Example:  get_machine_code("ADD") returns the value 18
// Note that opcodes may be prepended with a '+'.
// throws an opcode_error_exception if the opcode is not
// found in the table.
string opcodetab::get_machine_code(string str, int line) {
    validate(str, line);

    if(str.at(0)=='+')
        str = str.substr(1,str.length());
    transform(str.begin(), str.end(), str.begin(), ::toupper);
    string tmp = mymap[str].second;

    return tmp;

}

// takes a SIC/XE opcode and returns the number of bytes
// needed to encode the instruction, which is an int in
// the range 1..4.
// NOTE: the opcode must be prepended with a '+' for format 4.
// throws an opcode_error_exception if the opcode is not
// found in the table.

int opcodetab::get_instruction_size(string str, int line){
    int size = validate(str, line);
    return size;

}
//Takes any opcode string, validates the input, and returns the validated opcode's size in int.
int opcodetab::validate(string str, int line) {
    ostringstream convert_to_string;
    convert_to_string << line;

    bool plusone = false;
    if (str.at(0) == '+') {
        str = str.substr(1, str.length());
        plusone = true;
    }
    transform(str.begin(), str.end(), str.begin(), ::toupper);
    string tmp;
    string exception;
    if (mymap.count(str) > 0) {
        tmp = mymap[str].first;
    } else {
        exception.append("Error at line " + convert_to_string.str() + ". Opcode not found!");
        throw opcode_error_exception(exception);
    }

    int num = atoi(tmp.c_str());

    if (plusone && (num != 3)) {
        exception.append("Error at line " + convert_to_string.str() + ". This opcode cannot be denoted with a '+'!");
        throw opcode_error_exception(exception);
    }

    if (plusone && (str =="RSUB")){
        exception.append("Error at line " + convert_to_string.str() + ". RSUB cannot be denoted with a '+'!");
        throw opcode_error_exception(exception);
    }

    if(plusone)
        return num + 1;
    return num;

}