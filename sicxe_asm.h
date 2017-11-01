
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include "file_parser.h"
#include "file_parse_exception.h"
#include "opcodetab.h"
#include "opcode_error_exception.h"
#include "symtab.h"

using namespace std;

class sicxe_asm{
public:
    sicxe_asm(string);
    void print();
    void print_file(string file);

private:
    struct list {
        int line;
        int locctr;
        string label;
        string opcode;
        string operand;
        string comment;

    };
    int loc_ctr;        //global location counter that will be updates as the file is parsed.
    vector<list> storage;
    int directive_handler(int directive, string operand, int currentlcctr, int line);
    void is_operand_digit(string operand, int line);
    void is_operand_alnum(string operand, int line);
    int find_start(file_parser, opcodetab, symtab);
    void assign_addresses(int line, file_parser fp, opcodetab opcodes, symtab labels);


    };

