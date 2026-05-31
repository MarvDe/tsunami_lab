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
                                        std::string &o_solverName,
                                        std::string &o_setupName,
                                        std::string &o_formatName,
                                        tsunami_lab::t_real &o_dxy,
                                        std::string &o_bathymetryNCFilePath,
                                        std::string &o_displacementNCFilePath,
                                        tsunami_lab::t_idx &o_nx,
                                        tsunami_lab::t_idx &o_ny,
                                        tsunami_lab::t_real &o_endTime,
                                        std::string &o_stationsFilePath,
                                        tsunami_lab::t_real &o_left,
                                        tsunami_lab::t_real &o_upper,
                                        tsunami_lab::t_idx &o_outRes
                                    ){
    YAML::Node l_file;
    try {
        l_file = YAML::LoadFile(i_file);

        auto  args = l_file["args"][0];
        o_solverName = args["solverName"].as<std::string>();
        o_left = args["startCoordX"].as<tsunami_lab::t_real>();
        o_upper = args["startCoordY"].as<tsunami_lab::t_real>();
        o_setupName = args["setupName"].as<std::string>();
        o_formatName = args["formatName"].as<std::string>();
        o_dxy = args["cellSize"].as<tsunami_lab::t_real>();
        o_nx = args["cellx"].as<tsunami_lab::t_idx>();
        o_ny = args["celly"].as<tsunami_lab::t_idx>();
        o_endTime = args["endTime"].as<tsunami_lab::t_real>();
        o_stationsFilePath = args["stations"].as<std::string>();
        o_displacementNCFilePath = args["displacement"]["filePath"].as<std::string>();
        o_bathymetryNCFilePath = args["bathymetry"]["filePath"].as<std::string>();
        o_outRes = args["outputResolution"].as<tsunami_lab::t_idx>();
    } catch (YAML::Exception& e){
        std::cerr << "YAML Error: " << e.what() << std::endl;
        return;
    }

}