#include "sicxe_asm.h"

using namespace std;


int main(int argc, char** argv) {
    try {
        string exception;
        string file;
        if(argc != 2) {
            exception.append("Error: sicxeasm accepts 1 command line argument (filename)");
            throw file_parse_exception(exception);
        } else {
            file = argv[1];
        }
        sicxe_asm firstpass = sicxe_asm(file);
        firstpass.print();
        firstpass.print_file(file);
    }
    catch (file_parse_exception e) {
        cout << e.getMessage() << endl;
    }
    catch (opcode_error_exception e){
        cout << e.getMessage() << endl;
    }
}

sicxe_asm::sicxe_asm(string file) {
    loc_ctr = 0;
    file_parser fp(file);
    fp.read_file();
    opcodetab opcodes = opcodetab();
    symtab labels = symtab();

    int line = find_start(fp, opcodes, labels);
    assign_addresses(line, fp, opcodes, labels);
}

int sicxe_asm::find_start(file_parser fp, opcodetab opcodes, symtab labels) {
    string exception;
    string label;
    string opcode;
    string operand;
    string switchtmp;
    int locctr = 0;
    int directive;
    ostringstream convert_to_string;
    struct list newline;
    int line = 1;
    bool start = false;
    while (!start) {
        if (line > fp.size()) {
            exception.append("You have failed to START the program");
            throw file_parse_exception(exception);//Wrong directive, must have START
        }
        newline.line = line;
        newline.locctr = locctr;
        
        label = fp.get_token(line-1, 0);
        opcode = fp.get_token(line-1, 1);
        operand = fp.get_token(line-1, 2);
        
        newline.comment = fp.get_token(line-1, 3);
        newline.label = label;
        newline.opcode = opcode;
        newline.operand = operand;
        if ((label.empty()) && (opcode.empty()) && (operand.empty())) {
            storage.push_back(newline);
            line++;
        } else {
            if (!opcode.empty()) {
                int tmp = opcodes.get_instruction_size(opcode, line);
                cout << opcode;
                if (tmp != 0) {
                    convert_to_string << line;
                    exception.append("Error at line " + convert_to_string.str() +
                                     ". Cannot have an opcode, must be the START directive");
                    throw file_parse_exception(exception);//Cannot have an opcode, must be the START directive.
                }
                switchtmp = opcodes.get_machine_code(opcode, line);
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
                labels.insert(label, locctr);
            line++;
            storage.push_back(newline);
        }
    }
    return line;
}

void sicxe_asm::assign_addresses(int line, file_parser fp, opcodetab opcodes, symtab labels) {
    ostringstream convert_to_string;
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
        if (line > fp.size()) {
            convert_to_string << line;
            exception.append(
                             "Error at line " + convert_to_string.str() + ". You have failed to END the program");
            throw file_parse_exception(exception);
        }
        
        newline.line = line;
        newline.locctr = loc_ctr;
        
        label = fp.get_token(line-1, 0);
        opcode = fp.get_token(line-1, 1);
        operand = fp.get_token(line-1, 2);
        
        newline.comment = fp.get_token(line-1, 3);
        newline.label = label;
        newline.opcode = opcode;
        newline.operand = operand;
        
        
        if ((label.empty()) && (opcode.empty()) && (operand.empty())) {
            storage.push_back(newline);
            line++;
        } else {
            
            if (!opcode.empty()) {
                int tmp = opcodes.get_instruction_size(opcode, line); //if 0, it is a directive
                if (tmp == 0) {
                    switchtmp = opcodes.get_machine_code(opcode, line);
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
                if (labels.contains(label)) {
                    convert_to_string << line;
                    exception.append("Error at line " + convert_to_string.str() + ". Duplicate Symbol!");
                    throw file_parse_exception(exception);//duplicate symbol
                }
                labels.insert(label, loc_ctr);
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
            int endquote;
            int beginquote;
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

                if (endquote&1 == 1) {
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
