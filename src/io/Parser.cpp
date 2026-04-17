#include "Parser.h"
#include <cstdlib>
#include <string.h>
#include <iostream>

void tsunami_lab::io::Parser::parse(const int i_argc, const char *i_argv[], tsunami_lab::t_idx &o_cellx, tsunami_lab::t_idx &o_solverId){

    // standard output of flags
    o_cellx = 1;
    o_solverId = 0;

    // parsing arguments
    for (int i = 1; i < i_argc; i++){
        const char *current_flag = i_argv[i];
        int j = 0;
        const int max_flag_name_size = 20;
        char flag_name[max_flag_name_size] = "";
        bool parsed_name = false;

        // parse name
        while (j < max_flag_name_size && current_flag[j] != '\0'){
            if (current_flag[j] == '='){
                parsed_name = true;
                j++;
                break;
            }
            flag_name[j] = current_flag[j];
            j++;
        }
    
        // parse value
        if (parsed_name){
            // set solver 
            if ( strcmp(flag_name, "solver") == 0 ){
                int solverId = atoi(current_flag + j * sizeof(char));
                // check for invalid input 
                if (solverId >= 0 && solverId <= 1){
                    o_solverId = solverId;
                }
            }
            // set cellx 
            else if ( strcmp(flag_name, "cellx") == 0 ){
                int cellx = atoi(current_flag + j * sizeof(char));
                // check for invalid input 
                if (cellx > 0){
                    o_cellx = cellx;
                }
            }
        }

    }

}