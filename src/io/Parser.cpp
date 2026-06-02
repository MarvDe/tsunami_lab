#include "Parser.h"
#include <cstdlib>
#include <string.h>
#include <iostream>
#include <yaml-cpp/yaml.h>

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

float tsunami_lab::io::Parser::get(const std::string &i_name, float i_fallback){
    auto l_item = m_args.find(i_name);
    if (l_item == m_args.end()){
        return i_fallback;
    }
    return std::stof(l_item->second);
}

void tsunami_lab::io::Parser::parseFile(std::string &i_file,
                                        std::string &i_solverName,
                                        std::string &i_setupName,
                                        std::string &i_formatName,
                                        tsunami_lab::t_real &i_dxy,
                                        std::string &i_bathymetryNCFilePath,
                                        std::string &i_displacementNCFilePath,
                                        tsunami_lab::t_idx &i_nx,
                                        tsunami_lab::t_idx &i_ny,
                                        tsunami_lab::t_real &i_endTime,
                                        std::string &i_stationsFilePath,
                                        tsunami_lab::t_real &i_left,
                                        tsunami_lab::t_real &i_upper,
                                        std::string &i_checkPointFile,
                                        bool &i_appendFile
                                    ){
    YAML::Node l_file;
    try {
        l_file = YAML::LoadFile(i_file);

        auto  args = l_file["args"][0];
        i_solverName = args["solverName"].as<std::string>();
        i_formatName = args["formatName"].as<std::string>();
        i_setupName = args["setupName"].as<std::string>();
        i_endTime = args["endTime"].as<tsunami_lab::t_real>();
        i_stationsFilePath = args["stations"].as<std::string>();
        if (i_setupName.compare("checkPoint") == 0 ){
            i_checkPointFile = args["checkPointFile"].as<std::string>();
            i_appendFile = args["appendFile"].as<bool>();
            return;
        }
        i_left = args["startCoordX"].as<tsunami_lab::t_real>();
        i_upper = args["startCoordY"].as<tsunami_lab::t_real>();
        i_dxy = args["cellSize"].as<tsunami_lab::t_real>();
        i_nx = args["cellx"].as<tsunami_lab::t_idx>();
        i_ny = args["celly"].as<tsunami_lab::t_idx>();
        i_displacementNCFilePath = args["displacement"][0]["filePath"].as<std::string>();
        i_bathymetryNCFilePath = args["bathymetry"][0]["filePath"].as<std::string>();
    } catch (YAML::Exception& e){
        std::cerr << "YAML Error: " << e.what() << std::endl;
        return;
    }

}