#include "Parser.h"
#include <cstdlib>
#include <string.h>
#include <iostream>


tsunami_lab::io::Parser::Parser(int i_argc, char *i_argv[]){
    // parsing arguments
    for (int l_flagIndex{1}; l_flagIndex < i_argc; l_flagIndex++){

        std::string l_currentFlag = i_argv[l_flagIndex];
        t_idx l_pos =  l_currentFlag.find('=');        
        
        if (l_pos != std::string::npos){
            std::string l_argName = l_currentFlag.substr(0, l_pos);
            std::string l_argValue =  l_currentFlag.substr(l_pos + 1);

            m_args[l_argName] = l_argValue;
        }

    }

}

tsunami_lab::t_idx tsunami_lab::io::Parser::get(const std::string &i_name, t_idx i_fallback){
    auto l_item = m_args.find(i_name);
    if (l_item == m_args.end()){
        return i_fallback;
    }
    return atoi(l_item->second.c_str());
}

std::string tsunami_lab::io::Parser::get(const std::string &i_name, const std::string &i_fallback){
    auto l_item = m_args.find(i_name);
    if (l_item == m_args.end()){
        return i_fallback;
    }
    return l_item->second;
}