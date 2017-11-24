
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include "file_parser.h"
#include "file_parse_exception.h"
#include "opcodetab.h"
#include "opcode_error_exception.h"
#include "symtab.h"
#include "symtab_exception.h"


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
        string machine_code;
        string label;
        string opcode;
        string operand;
        string comment;

    };
    int loc_ctr;        //global location counter that will be updates as the file is parsed.
    vector<list> storage;
    string base;
    int directive_handler(int directive, string operand, int currentlcctr, int line);
    void is_operand_digit(string operand, int line);
    void is_operand_alnum(string operand, int line);
    int find_start();
    void assign_addresses(int line);
    int do_first_pass(string file);
    void do_second_pass(string file, int line);
    void print_list_file(string file);
    bool is_assembler_directive(string opcode, int line);
    void process_assembler_directive(string opcode, string operand, int line);
    void process_BYTE(string operand, int line);
    void process_WORD(string operand, int line);
    void process_BASE(string operand, int line);
    void process_NOBASE(string operand, int line);
    void process_END(string operand, int line);
    void process_line(string opcode, string operand, int line);
    void process_format1(string opcode, int line);
    void process_format2(string opcode, string operand, int line);
    void process_format3(string opcode, string oper, int line);
    void process_format4(string opcode, string operand, int line);
    string string_to_ascii(string s);
    int hex_to_int(string s);
    bool is_number(string s);
    int string_to_int(string s);
    void get_offset(int &code, string operand, int line);
    void process_operand3(int &code, string opcode, string operand, int line);
    void process_operand4(int &code, string opcode, string operand, int line);
    bool is_hexnumber(string s);
    bool is_hexdigit(char hex);
    string to_uppercase(string s);
    string int_to_hex(int num, int width);
    int op_to_int(string s);
    string pad_string(string code, int width);

    file_parser *fp;
    opcodetab *opcodes;
    symtab *labels;
    ostringstream convert_to_string;
};

