//
// Created by Omar Wardak on 9/26/2017.
//
#include "file_parser.h"

using namespace std;

file_parser::file_parser(string s) {
    filename = s;
}

void file_parser::read_file() {
    ifstream stream(filename.c_str());
    if(!stream.is_open()) {
        throw file_parse_exception("Unable to open source code file: " + filename);
    }
    string buffer;
    vector<string> file_lines;
    while(getline(stream, buffer)) {
        file_lines.push_back(buffer);
    }
    stream.close();
    tokenize_lines(file_lines);
}

void file_parser::tokenize_lines(vector<string> file_lines) {
    int line_num = 1;
    ostringstream convert_to_string;
    vector<string>::iterator v_iter;
    for(v_iter = file_lines.begin(); v_iter != file_lines.end(); v_iter++) {
        string exception;
        char *cstr = new char[(*v_iter).length() + 1]; // create pointer
        strcpy(cstr, (*v_iter).c_str());
        bool opcode = *cstr == ' ';
        bool commentmade = false;

        char *token = strtok(cstr, " \t");

        struct row newline;

        if(token == NULL) {
            newline.label = "";
            newline.opcode = "";
            newline.operand = "";
            newline.comment = "";
        }
        else {

            if (opcode) {
                newline.label = "";
            } else {
                //label***************************************************
                if (token[0] == '.') {
                    newline.label = "";
                    commentmade = true;
                    string strcomment1(token);
                    token = strtok(NULL, "\n");
                    if (token != NULL) {
                        string strcomment2(token);
                        string strcomment3 = strcomment1 + " " + strcomment2;
                        newline.comment = strcomment3;
                    } else
                        newline.comment = strcomment1;
                    token = strtok(NULL, " \t");
                }
                else if (!isalpha(token[0])) {
                    convert_to_string << line_num;
                    exception.append("Error at line " + convert_to_string.str() + ": First character of a label must be a letter.");
                    throw file_parse_exception(exception);
                }
                else {
                    string strlabel(token);
                    for (int j = 0; j < strlabel.length(); j++) {
                        if (!isalnum(strlabel[j])) {
                            convert_to_string << line_num;
                            exception.append("Error at line " + convert_to_string.str() + ": Invalid character in label. Not a letter or a number.");
                            throw file_parse_exception(exception);
                        }
                    }
                    newline.label = strlabel;
                    token = strtok(NULL, " \t");
                }
            }

            //opcode**********************************************

            if (token == NULL)
                newline.opcode = "";
            else if (token[0] == '.') {
                newline.opcode = "";
                commentmade = true;
                string strcomment1(token);
                token = strtok(NULL, "\n");
                if (token != NULL) {
                    string strcomment2(token);
                    string strcomment3 = strcomment1 + " " + strcomment2;
                    newline.comment = strcomment3;
                }
                else
                    newline.comment = strcomment1;

            }
            else {
                string stropcode(token);
                newline.opcode = stropcode;
            }

            token = strtok(NULL, " \t");


            //operand*********************************************

            if (token == NULL)
                newline.operand = "";
            else if (token[0] == '.') {
                newline.operand = "";
                commentmade = true;
                string strcomment1(token);
                token = strtok(NULL, "\n");
                if (token != NULL) {
                    string strcomment2(token);
                    string strcomment3 = strcomment1 + " " + strcomment2;
                    newline.comment = strcomment3;
                }
                else
                    newline.comment = strcomment1;
            }
            else {
                int quotes = 0;
                for (int i = 0; i < strlen(token); i++) {
                    if (token[i] == 39)
                        quotes++;
                }
                string stroper1(token);

                if (quotes == 0 || quotes == 2)
                    newline.operand = stroper1;
                else if (quotes == 1) {
                    bool endquote = false;
                    while (endquote != true) {
                        token = strtok(NULL, " ");
                        if(token == NULL){
                            convert_to_string << line_num;
                            exception.append("Error at line " + convert_to_string.str() +
                                             ": Missing the end quote for your operand");
                            throw file_parse_exception(exception);
                        }
                        for (int i = 0; i < strlen(token); i++) {
                            if (token[i] == 39)
                                endquote = true;
                        }
                        string str(token);
                        stroper1 = stroper1 + " " + str;
                    }
                    newline.operand = stroper1;
                }
            }
            token = strtok(NULL, " \t");

            //comment************************************************************

            if (commentmade == false) {
                if (token == NULL)
                    newline.comment = "";
                else if(token[0] == '.'){
                    string strcomment1(token);
                    token = strtok(NULL, "\n");
                    if (token != NULL) {
                        string strcomment2(token);
                        string strcomment3 = strcomment1 + " " + strcomment2;
                        newline.comment = strcomment3;
                    }
                    else
                        newline.comment = strcomment1;
                }
                else {
                    convert_to_string << line_num;
                    exception.append("Error at line " + convert_to_string.str() + ": 4th token can only be a comment.");
                    throw file_parse_exception(exception);
                }
            }
            token = strtok(NULL, " \t");
            if(token!=NULL) {
                convert_to_string << line_num;
                exception.append("Error at line " + convert_to_string.str() + ": Cannot have more than 4 tokens per line");
                throw file_parse_exception(exception);
            }
        }
        lines.push_back(newline);
        line_num++;
    }
}

string file_parser::get_token(unsigned int numrow, unsigned int numcol) {
    if(numrow >= lines.size() || numcol > 3) {
        return "";
    } else {
        switch(numcol) {
            case 0:
                return lines[numrow].label;
                break;
            case 1:
                return lines[numrow].opcode;
                break;
            case 2:
                return lines[numrow].operand;
                break;
            case 3:
                return lines[numrow].comment;
                break;
            default:
                return "";
                break;
        }
    }
}

void file_parser::print_file() {
    vector<row>::iterator v_iter;
    for(v_iter = lines.begin(); v_iter != lines.end(); v_iter++) {
        if((*v_iter).label.empty() && (*v_iter).opcode.empty() && (*v_iter).operand.empty() && !(*v_iter).comment.empty()) {
            cout << (*v_iter).comment << endl;
        } else {
            cout << std::left << setfill(' ') << setw(15) << (*v_iter).label << setfill(' ') << setw(10) << (*v_iter).opcode << setfill(' ') << setw(15) << (*v_iter).operand << (*v_iter).comment << endl;
        }
    }
}

int file_parser::size() {
    return (int) lines.size();
}
