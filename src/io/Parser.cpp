#include "Parser.h"
#include <cstdlib>
#include <string.h>
#include <iostream>

void tsunami_lab::io::Parser::parse(int i_argc, char *i_argv[], tsunami_lab::t_idx &o_cellx, tsunami_lab::t_idx &o_solverId){

    // standard values 
    o_cellx = 1;
    o_solverId = 0;

    // parsing arguments
    for (int l_flagIndex{1}; l_flagIndex < i_argc; l_flagIndex++){
        const char *l_currentFlag = i_argv[l_flagIndex];
        
        char l_flagName[m_MaxFlagNameSize] = "";
        

        // parse name
        bool l_parsedName = false;
        tsunami_lab::t_idx l_charIndex {0};
        while (l_charIndex < m_MaxFlagNameSize && l_currentFlag[l_charIndex] != '\0'){
            if (l_currentFlag[l_charIndex] == '='){
                l_parsedName = true;
                l_charIndex++;
                break;
            }
            l_flagName[l_charIndex] = l_currentFlag[l_charIndex];
            l_charIndex++;
        }
    
        // parse value
        if (l_parsedName){
            // set solver 
            if ( strcmp(l_flagName, "solver") == 0 ){
                int solverId = atoi(l_currentFlag + l_charIndex * sizeof(char));
                // check for invalid input 
                if (solverId == tsunami_lab::ROE || solverId == tsunami_lab::FWAVE){
                    o_solverId = solverId;
                }
            }
            // set cellx 
            else if ( strcmp(l_flagName, "cellx") == 0 ){
                int cellx = atoi(l_currentFlag + l_charIndex * sizeof(char));
                // check for invalid input 
                if (cellx > 0){
                    o_cellx = cellx;
                }
            }
        }

    }

}