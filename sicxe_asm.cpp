#include "sicxe_asm.h"

#define SET_E 0x100000
#define SET_P 0x2000
#define SET_B 0x4000
#define SET_3X 0x8000
#define SET_4X 0x800000
#define SET_3I 0x10000
#define SET_4I 0x1000000
#define SET_3N 0x20000
#define SET_4N 0x2000000

using namespace std;


int main(int argc, char** argv) {
    try {
        string exception;
        string file = "test.txt";
//        if(argc != 2) {
//            exception.append("Error: sicxeasm accepts 1 command line argument (filename)");
//            throw file_parse_exception(exception);
//        } else {
//            file = argv[1];
//        }
        sicxe_asm assembled = sicxe_asm(file);
        assembled.print_file(file);

    }
    catch (file_parse_exception e) {
        cout << e.getMessage() << endl;
    }
    catch (opcode_error_exception e){
        cout << e.getMessage() << endl;
    }
    catch (symtab_exception e){
        cout << e.getMessage() << endl;
    }
}

sicxe_asm::sicxe_asm(string file) {
    int line = do_first_pass(file);
    do_second_pass(file, line);
    print_list_file(file);
}

int sicxe_asm::do_first_pass(string file) {
    loc_ctr = 0;
    fp = new file_parser(file);
    opcodes = new opcodetab();
    labels = new symtab();
    fp->read_file();
    int line = find_start();
    assign_addresses(line);
    return line;
}

void sicxe_asm::do_second_pass(string file, int line) {
    base = "";
    bool end = false;
    string exception;
    string label;
    string opcode;
    string operand;
    string tmp;
    while(!end) {
        if(line > fp->size()) {
            convert_to_string << line;
            exception.append("Error at line: " + convert_to_string.str() + ". Failed to END program.");
            throw file_parse_exception(exception);
        }
        label = fp->get_token(line-1, 0);
        opcode = fp->get_token(line-1, 1);
        operand = fp->get_token(line-1, 2);
        if(label.empty() && opcode.empty() && operand.empty()) {
            line++;
            continue;
        }
        if(!opcode.empty()) {
            if(is_assembler_directive(opcode, line)) {
                if(opcodes->get_machine_code(opcode, line).compare("1") == 0) {
                    process_END(operand, line);
                    end = true;
                } else {
                    process_assembler_directive(opcode, operand, line);
                }
            } else {
                process_line(opcode, operand, line);
            }
        }
        line++;
    }
}

void sicxe_asm::process_assembler_directive(string opcode, string operand, int line) {
    string code = opcodes->get_machine_code(opcode, line);
    switch (atoi(code.c_str())) {
        case 0:
            //START is processed in the first pass
            break;
        case 1:
            //end will already be caught before this method is ever executed
            break;
        case 2:
            process_BYTE(operand, line);
            break;
        case 3:
            process_WORD(operand, line);
            break;
        case 4:
            //RESB does not generate machine code
            break;
        case 5:
            //RESW does not generate machine code
            break;
        case 6:
            process_BASE(operand, line);
            break;
        case 7:
            process_NOBASE(operand, line);
            break;
        case 8:
            //EQU does not generate machine code
            break;
        default:
            break;
    }
}

void sicxe_asm::process_BYTE(string operand, int line) {
    if(toupper(operand[0]) == 'C') {    //Storing characters
        string code = string_to_ascii(operand.substr(2, operand.size()-3));
        storage[line-1].machine_code = hex_to_int(code);
    } else if(toupper(operand[0] == 'X')) { //Storing hex digits
        storage[line-1].machine_code = hex_to_int(operand.substr(2, operand.size()-3));
    } else {                                //Storing decimal digits
        storage[line-1].machine_code = atoi(operand.substr(2).c_str());
    }
}

void sicxe_asm::process_WORD(string operand, int line) {
    if(operand[0] == '$') { //Operand is a hex value
        storage[line-1].machine_code = hex_to_int(operand.substr(1));
    } else {                //Operand is a decimal value
        storage[line-1].machine_code = atoi(operand.substr(1).c_str());
    }
}

void sicxe_asm::process_BASE(string operand, int line) {
    base = operand;
}

void sicxe_asm::process_NOBASE(string operand, int line) {
    base = "";
}

void sicxe_asm::process_END(string operand, int line) {
    //Might not need this method
}

void sicxe_asm::process_line(string opcode, string operand, int line) {
    int instruction_size = opcodes->get_instruction_size(opcode, line);
    switch(instruction_size) {
        case 1:
            process_format1(opcode, line);
            break;
        case 2:
            process_format2(opcode, operand, line);
            break;
        case 3:
            process_format3(opcode, operand, line);
            break;
        case 4:
            process_format4(opcode, operand, line);
            break;
        default:
            break;
    }
}

void sicxe_asm::process_format1(string opcode, int line) {
    int code = hex_to_int(opcodes->get_machine_code(opcode, line));
    storage[line-1].machine_code = code;
}

void sicxe_asm::process_format2(string opcode, string operand, int line) {
    //DEAL WITH SHIFT OPCODES HERE BECAUSE THEY ARE EXCEPTIONS
    
    
}

void sicxe_asm::process_format3(string opcode, string operand, int line) {
    int code = 0;
    bool indexed = false;
    string exception;
    //DEAL WITH RSUB HERE BECAUSE IT DOES NOT HAVE AN OPERAND
    if(operand.compare("RSUB") == 0) {
        code = hex_to_int("4F0000");
        storage[line-1].machine_code = code;
        return;
    }
    if(operand.find(',') != operand.npos) { //Offset (alpha,x) can you have 1000,x or $1000,x??
        code |= SET_3N;
        code |= SET_3I;
        code |= SET_3X;
        indexed = true;
    }
    if(operand[0] == '#') {      //Immediate (#alpha or #1000 or #$100)
        if(indexed) {
            convert_to_string << line;
            exception.append("Error at line: " + convert_to_string.str() + ". Invalid operand");
            throw file_parse_exception(exception);
        }
        code |= SET_3I;
        process_operand3(code, opcode, operand.substr(1), line);
    } else if (operand[0] == '@') {     //Indirect (@alpha or @1000 or @$100)
        if(indexed) {
            convert_to_string << line;
            exception.append("Error at line: " + convert_to_string.str() + ". Invalid operand");
            throw file_parse_exception(exception);
        }
        code |= SET_3N;
        process_operand3(code, opcode, operand.substr(1), line);
    } else {        //No addressing mode (alpha or 10000 or $100)
        if(indexed) {
            process_operand3(code, opcode, operand.substr(0, operand.size()-2), line);
        } else {
            code |= SET_3N;
            code |= SET_3I;
            process_operand3(code, opcode, operand, line);
        }
    }
    
    //validate flags
    
    int mc = hex_to_int(opcodes->get_machine_code(opcode, line));
    mc <<= 18;
    code |= mc;
    storage[line-1].machine_code = code;

}

void sicxe_asm::process_format4(string opcode, string operand, int line) {
    int code = 0;
    if(operand.compare("+RSUB")) {
        code = hex_to_int("4F000000");
        storage[line-1].machine_code = code;
        return;
    }
    bool indexed = false;
    string exception;
    if(operand.find(',') != operand.npos) {
        code |= SET_4N;
        code |= SET_4I;
        code |= SET_4X;
        indexed = true;
    }
    if(operand[0] == '#') {      //Immediate (#alpha or #1000 or #$100)
        if(indexed) {
            convert_to_string << line;
            exception.append("Error at line: " + convert_to_string.str() + ". Invalid operand");
            throw file_parse_exception(exception);
        }
        code |= SET_4I;
        process_operand4(code, opcode, operand.substr(1), line);
    } else if (operand[0] == '@') {     //Indirect (@alpha or @1000 or @$100)
        if(indexed) {
            convert_to_string << line;
            exception.append("Error at line: " + convert_to_string.str() + ". Invalid operand");
            throw file_parse_exception(exception);
        }
        code |= SET_4N;
        process_operand4(code, opcode, operand.substr(1), line);
    } else {        //No addressing mode (alpha or 10000 or $100)
        if(indexed) {
            process_operand4(code, opcode, operand.substr(0, operand.size()-2), line);
        } else {
            code |= SET_4N;
            code |= SET_4I;
            process_operand4(code, opcode, operand, line);
        }
    }
    
    //validate flags
    
    int mc = hex_to_int(opcodes->get_machine_code(opcode, line));
    mc <<= 26;
    code |= mc;
    storage[line-1].machine_code = code;
    

}

void sicxe_asm::process_operand3(int &code, string opcode, string operand, int line) {
    string exception;
    if(operand[0] == '$') {
        if(is_number(operand.substr(1))) {
            if(operand.size()-2 > 3) {
                convert_to_string << line;
                exception.append("Error at line: " + convert_to_string.str() + ". Value does not fit in 3 hex digits.");
                throw file_parse_exception(exception);
            } else {
                int immediate = hex_to_int(operand.substr(1));
                code += immediate;
            }
        } else {
            convert_to_string << line;
            exception.append("Error at line: " + convert_to_string.str() + ". Invalid operand syntax");
            throw file_parse_exception(exception);
        }
    } else if(is_number(opcode)) {
        int immediate = string_to_int(operand.substr(1));
        if(immediate < -2048 || immediate > 2047) {
            convert_to_string << line;
            exception.append("Error at line: " + convert_to_string.str() + ". Immediate value does not fit in 3 bytes");
            throw file_parse_exception(exception);
        } else {
            code += immediate;
        }
    } else {
        get_offset(code, operand, line);
    }
}

void sicxe_asm::process_operand4(int &code, string opcode, string operand, int line) {
    string exception;
    if(operand[0] == '$') {
        if(is_number(operand.substr(1))) {
            if(operand.size()-2 > 5) {
                convert_to_string << line;
                exception.append("Error at line: " + convert_to_string.str() + ". Value does not fit in 5 hex digits.");
                throw file_parse_exception(exception);
            } else {
                int immediate = hex_to_int(operand.substr(1));
                code += immediate;
            }
        } else {
            convert_to_string << line;
            exception.append("Error at line: " + convert_to_string.str() + ". Invalid operand syntax");
            throw file_parse_exception(exception);
        }
    } else if(is_number(opcode)) {
        int immediate = string_to_int(operand.substr(1));
        //Check if it fits in 5 bytes??
        code += immediate;
    } else {
        int destination = labels->gettab(operand);
        code += destination;
    }
}

void sicxe_asm::get_offset(int &code, string symbol, int line) {
    string exception;
    int source = storage[line-1].locctr;
    int destination = labels->gettab(symbol);
    int offset = source - (destination + 3);
    if(offset < -2048 || offset > 2047) {
        if(base != "") {
            offset = labels->gettab(base) - source;
            if(offset < 0) {
                convert_to_string << line;
                exception.append("Error at line: " + convert_to_string.str() + ". BASE negative offset");
                throw opcode_error_exception(exception);
            } else {
                code += offset;
                code |= SET_B;
            }
        } else {
            convert_to_string << line;
            exception.append("Error at line: " + convert_to_string.str() + ". BASE is not available");
            throw opcode_error_exception(exception);
        }
    } else {
        code += offset;
        code |= SET_P;
    }
}

bool sicxe_asm::is_number(string s) {
    for(int i = 0; i < s.length(); i++) {
        if(!isdigit(s[i])) {
            return false;
        }
    }
    return true;
}

bool sicxe_asm::is_assembler_directive(string opcode, int line) {
    return opcodes->get_instruction_size(opcode, line) == 0;
}

void sicxe_asm::print_list_file(string file) {
    print_file(file);
}

int sicxe_asm::find_start() {
    string exception;
    string label;
    string opcode;
    string operand;
    string switchtmp;
    int locctr = 0;
    int directive;
    struct list newline;
    int line = 1;
    bool start = false;
    while (!start) {
        if (line > fp->size()) {
            exception.append("You have failed to START the program");
            throw file_parse_exception(exception);//Wrong directive, must have START
        }
        newline.line = line;
        newline.locctr = locctr;
        
        label = fp->get_token(line-1, 0);
        opcode = fp->get_token(line-1, 1);
        operand = fp->get_token(line-1, 2);
        
        newline.comment = fp->get_token(line-1, 3);
        newline.label = label;
        newline.opcode = opcode;
        newline.operand = operand;
        if ((label.empty()) && (opcode.empty()) && (operand.empty())) {
            storage.push_back(newline);
            line++;
        } else {
            if (!opcode.empty()) {
                int tmp = opcodes->get_instruction_size(opcode, line);
                if (tmp != 0) {
                    convert_to_string << line;
                    exception.append("Error at line " + convert_to_string.str() +
                                     ". Cannot have an opcode, must be the START directive");
                    throw file_parse_exception(exception);//Cannot have an opcode, must be the START directive.
                }
                switchtmp = opcodes->get_machine_code(opcode, line);
                sscanf(switchtmp.c_str(), "%d", &directive);
                if (directive != 0) {
                    convert_to_string << line;
                    exception.append("Error at line " + convert_to_string.str() +
                                     ". Wrong directive, first directive must be START");
                    throw file_parse_exception(exception);//Wrong directive, must have START
                }
                loc_ctr = directive_handler(directive, operand, locctr, line);
                start = true;
            }
            if (!label.empty())
                labels->insert(label, newline.locctr);
            line++;
            storage.push_back(newline);
        }
    }
    return line;
}

void sicxe_asm::assign_addresses(int line) {
    string exception;
    struct list newline;
    string label;
    string opcode;
    string operand;
    string switchtmp;
    int directive;
    string base;
    bool end = false;
    while (!end) {
        if (line > fp->size()) {
            convert_to_string << line;
            exception.append("Error at line " + convert_to_string.str() + ". You have failed to END the program");
            throw file_parse_exception(exception);
        }
        
        newline.line = line;
        newline.locctr = loc_ctr;
        
        label = fp->get_token(line-1, 0);
        opcode = fp->get_token(line-1, 1);
        operand = fp->get_token(line-1, 2);
        
        newline.comment = fp->get_token(line-1, 3);
        newline.label = label;
        newline.opcode = opcode;
        newline.operand = operand;
        
        
        if ((label.empty()) && (opcode.empty()) && (operand.empty())) {
            storage.push_back(newline);
            line++;
        } else {
            
            if (!opcode.empty()) {
                int tmp = opcodes->get_instruction_size(opcode, line); //if 0, it is a directive
                if (tmp == 0) {
                    switchtmp = opcodes->get_machine_code(opcode, line);
                    sscanf(switchtmp.c_str(), "%d", &directive);
                    if (directive == 1)
                        end = true;
                    else if (directive == 6) {
                        base = operand;
                    } else if (directive == 7) {
                        base = "";
                    } else if (directive == 8 && label.empty()) {
                        convert_to_string << line;
                        exception.append("Error at line " + convert_to_string.str() +
                                         ". Cannot have EQU with no label.");
                        throw file_parse_exception(exception);
                    } else
                        loc_ctr = directive_handler(directive, operand, loc_ctr, line);
                } else {
                    loc_ctr = loc_ctr + tmp;
                }
            }
            
            if (!label.empty()) {
                if (labels->contains(label)) {
                    convert_to_string << line;
                    exception.append("Error at line " + convert_to_string.str() + ". Duplicate Symbol!");
                    throw symtab_exception(exception);//duplicate symbol
                }
                labels->insert(label, newline.locctr);
            }
            
            line++;
            storage.push_back(newline);
        }
        
    }
    
}

int sicxe_asm::directive_handler(int directive, string operand, int currentlcctr, int line){
    ostringstream convert_to_string;
    int value;
    int value2;
    string exception;
    switch(directive) {
        case 0: {
            if (operand.at(0) == '$') {
                operand = operand.substr(1, operand.length());
                is_operand_alnum(operand, line);
                sscanf(operand.c_str(), "%x", &value);
                return value;

            }
            is_operand_digit(operand, line);
            sscanf(operand.c_str(), "%d", &value2);
            return value2;
        }
        case 1:{
            return currentlcctr;
        }
        case 2: {
            bool space = false;
            int endquote = 0;
            int beginquote = 0;
            int size = 0;
            while (size < operand.length()) {
                if ((operand.at(size)) == 39) {
                    beginquote = endquote;
                    endquote = size;
                }
                if (operand.at(size) == ' ')
                    space = true;
                size++;
            }
            size--;
            beginquote++;
            endquote = size - beginquote;

            if ((operand.at(1) != 39 || (operand.at(size) != 39))) {
                convert_to_string << line;
                exception.append("Error at line " + convert_to_string.str() + ". Incorrect format for BYTE operand");
                throw file_parse_exception(exception);  //Should not have any space for BYTE
            }

            if (space) {
                convert_to_string << line;
                exception.append("Error at line " + convert_to_string.str() + ". No spaces in BYTE operand");
                throw file_parse_exception(exception);
            }

            if (((operand.at(0)) == 'x') || ((operand.at(0)) == 'X')) {

                if ((endquote&1) == 1) {
                    convert_to_string << line;
                    exception.append("Error at line " + convert_to_string.str() + ". Hex value can only be even");
                    throw opcode_error_exception(exception);//number is odd, can only be even for hex
                }
                value = (endquote >> 1) + currentlcctr;
                return value;
            } else if (operand.at(0) == 'c' || operand.at(0) == 'C') {
                value = 0;
                value = endquote + currentlcctr;
                return value;
            }
            convert_to_string << line;
            exception.append("Error at line " + convert_to_string.str() + ". Wrong initial argument for BYTE");
            throw file_parse_exception(exception);//Wrong operand for byte

        }
        case 3: {
            value = currentlcctr + 3;
            return value;
        }
        case 4: {
            if (operand.at(0) == '$') {
                operand = operand.substr(1, operand.length());
                is_operand_alnum(operand, line);
                sscanf(operand.c_str(), "%x", &value);
                value = value + currentlcctr;
                return value;

            }
            is_operand_digit(operand, line);
            sscanf(operand.c_str(), "%d", &value2);
            value2 = value2 + currentlcctr;
            return value2;
        }
        case 5: {
            if (operand.at(0) == '$') {
                operand = operand.substr(1, operand.length());
                is_operand_alnum(operand, line);
                sscanf(operand.c_str(), "%x", &value);
                value = (value * 3) + currentlcctr;
                return value;
            }
            is_operand_digit(operand, line);
            sscanf(operand.c_str(), "%d", &value2);
            value2 = (value2 * 3) + currentlcctr;
            return value2;
        }
        case 6: {
            return currentlcctr;
        }
        case 7: {
            return currentlcctr;
        }
        case 8: {
            return currentlcctr;
        }
    }
    return -1;
}

void sicxe_asm::is_operand_digit(string operand, int line){
    ostringstream convert_to_string;
    string exception;
    for(int i= 0; i < operand.length(); i++) {
        if (!isdigit(operand[i])) {
            convert_to_string << line;
            exception.append("Error at line " + convert_to_string.str() + ". Incorrect argument for this directive.");
            throw file_parse_exception(exception);
        }
    }

}

void sicxe_asm::is_operand_alnum(string operand, int line){
    ostringstream convert_to_string;
    string exception;
    for(int i= 0; i < operand.length(); i++) {
        if (!isalnum(operand[i])) {
            convert_to_string << line;
            exception.append("Error at line " + convert_to_string.str() + ". Incorrect argument for this directive.");
            throw file_parse_exception(exception);
        }
    }

}

string sicxe_asm::string_to_ascii(string s) {
    stringstream stream;
    for(int i = 0; i < s.size(); i++) {
        stream << hex << (int)s[i];
    }
    string temp = stream.str();
    transform(temp.begin(), temp.end(), temp.begin(), ::toupper);
    return temp;
}

int sicxe_asm::hex_to_int(string s) {
    int value;
    sscanf(s.c_str(),"%x",&value);
    return value;
}

int sicxe_asm::string_to_int(string s) {
    istringstream instr(s);
    int n;
    instr >> n;
    return n;
}

void sicxe_asm::print(){
    vector<list>::iterator v_iter;
    cout << std::left << setfill(' ') << setw(10) << "Line#" << setfill(' ') << setw(10) << "Address" << setfill(' ') << setw(10) << "Label" << setfill(' ') << setw(10) << "Opcode" << setfill(' ') << setw(10) << "Operand" << endl;
    cout << std::left << setfill(' ') << setw(10) << "=====" << setfill(' ') << setw(10) << "=======" << setfill(' ') << setw(10) << "=====" << setfill(' ') << setw(10) << "======" << setfill(' ') << setw(10) << "=======" << endl;

    for(v_iter = storage.begin(); v_iter != storage.end(); v_iter++) {
        stringstream out;
        out << setw(5) << setfill('0') << hex << (*v_iter).locctr;
        string hex = out.str();
        transform(hex.begin(), hex.end(), hex.begin(),::toupper);

            cout << std::left << setfill(' ') << setw(12) << (*v_iter).line << setfill(' ') << setw(8) << hex << setfill(' ') << setw(10) << (*v_iter).label << setfill(' ') << setw(10) << (*v_iter).opcode << setfill(' ') << setw(10) << (*v_iter).operand << endl;
        }
    }

void sicxe_asm::print_file(string file){

    string tmp = file;
    file = file.substr(0, file.length()-3);
    file+="lis";
    const char *cfile = file.c_str();
    ofstream myfile;
    myfile.open(cfile);
    vector<list>::iterator v_iter;
    myfile << std::right << setfill(' ') << setw(35) << "**" + tmp + "**" << endl;
    myfile << std::left << setfill(' ') << setw(10) << "Line#" << setfill(' ') << setw(10) << "Address" << setfill(' ') << setw(10) << "Label" << setfill(' ') << setw(10) << "Opcode" << setfill(' ') << setw(10) << "Operand" << endl;
    myfile << std::left << setfill(' ') << setw(10) << "=====" << setfill(' ') << setw(10) << "=======" << setfill(' ') << setw(10) << "=====" << setfill(' ') << setw(10) << "======" << setfill(' ') << setw(10) << "=======" << endl;

    for(v_iter = storage.begin(); v_iter != storage.end(); v_iter++) {
        stringstream out;
        out << setw(5) << setfill('0') << hex << (*v_iter).locctr;
        string hex = out.str();
        transform(hex.begin(), hex.end(), hex.begin(),::toupper);

        myfile << std::left << setfill(' ') << setw(12) << (*v_iter).line << setfill(' ') << setw(8) << hex << setfill(' ') << setw(10) << (*v_iter).label << setfill(' ') << setw(10) << (*v_iter).opcode << setfill(' ') << setw(10) << (*v_iter).operand << endl;
    }
    myfile.close();
}

