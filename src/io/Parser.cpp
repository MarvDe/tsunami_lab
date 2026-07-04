#include "Parser.h"
#include "../setups/DamBreak1d.h"
#include "../setups/ArtificialTsunami2d.h"
#include "../setups/CheckPoint.h"
#include "../setups/CircularDamBreak2d.h"
#include "../setups/RareRare1d.h"
#include "../setups/ShockShock1d.h"
#include "../setups/SubcriticalFlow1d.h"
#include "../setups/SupercriticalFlow1d.h"
#include "../setups/TsunamiEvent1d.h"
#include "../setups/TsunamiEvent2d.h"

#include <cstdlib>
#include <string.h>
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <yaml-cpp/yaml.h>

namespace {
    std::unordered_map<std::string, tsunami_lab::io::SetupDef> createSetupDefs() {
        tsunami_lab::io::SetupDef l_damBreak = tsunami_lab::setups::DamBreak1d::getSetupDef();
        tsunami_lab::io::SetupDef l_artificialTsunami2d = tsunami_lab::setups::ArtificialTsunami2d::getSetupDef();
        tsunami_lab::io::SetupDef l_checkPoint = tsunami_lab::setups::CheckPoint::getSetupDef();
        tsunami_lab::io::SetupDef l_circularDamBreak2d = tsunami_lab::setups::CircularDamBreak2d::getSetupDef();
        tsunami_lab::io::SetupDef l_rareRare = tsunami_lab::setups::RareRare1d::getSetupDef();
        tsunami_lab::io::SetupDef l_shockShock = tsunami_lab::setups::ShockShock1d::getSetupDef();
        tsunami_lab::io::SetupDef l_subcritical = tsunami_lab::setups::SubcriticalFlow1d::getSetupDef();
        tsunami_lab::io::SetupDef l_supercritical = tsunami_lab::setups::SupercriticalFlow1d::getSetupDef();
        tsunami_lab::io::SetupDef l_tsunamiEvent1d = tsunami_lab::setups::TsunamiEvent1d::getSetupDef();
        tsunami_lab::io::SetupDef l_tsunamiEvent2d = tsunami_lab::setups::TsunamiEvent2d::getSetupDef();


        return {
            {l_damBreak.name, l_damBreak},
            {l_artificialTsunami2d.name, l_artificialTsunami2d},
            {l_checkPoint.name, l_artificialTsunami2d},
            {l_checkPoint.name, l_checkPoint},
            {l_circularDamBreak2d.name, l_circularDamBreak2d},
            {l_rareRare.name, l_rareRare},
            {l_shockShock.name, l_shockShock},
            {l_subcritical.name, l_subcritical},
            {l_supercritical.name, l_supercritical},
            {l_tsunamiEvent1d.name, l_tsunamiEvent1d},
            {l_tsunamiEvent2d.name, l_tsunamiEvent2d}
        };
    }
}

const std::unordered_map<std::string, tsunami_lab::io::SetupDef> tsunami_lab::io::Parser::SETUP_DEFS = createSetupDefs();
const std::unordered_map<std::string, bool> tsunami_lab::io::Parser::knownFlags = {
    {"help", false}, // flagName, hasValue
    {"h", false},
    {"printSetups", false},
    {"printSetup", true},
    {"args", true},
};

tsunami_lab::io::Parser::Parser(int i_argc, char *i_argv[]){
    // parsing arguments
    for (int l_flagIndex{1}; l_flagIndex < i_argc; l_flagIndex++){
        std::string l_currentFlag = i_argv[l_flagIndex];
        t_idx l_pos =  l_currentFlag.find('=');        
        
        if (l_pos != std::string::npos){
            std::string l_argName = l_currentFlag.substr(0, l_pos);
            std::string l_argValue =  l_currentFlag.substr(l_pos + 1);
            auto l_flag = knownFlags.find(l_argName);
            if (l_flag != knownFlags.end()){
                if (l_flag -> second){
                    m_args[l_argName] = l_argValue;
                } else {
                    throw std::runtime_error(
                        "flag '" +
                        l_argName +
                        "' has no value!\n"
                    );
                }
            } else {
                throw std::runtime_error(
                    "flag '" +
                    l_argName +
                    "' does not exist!\n"
                );
            }
            
        } else {
            auto l_flag = knownFlags.find(l_currentFlag);
            if (l_flag != knownFlags.end()){
                if (!l_flag -> second){
                    m_args[l_currentFlag] = "";
                } else {
                    throw std::runtime_error(
                        "flag '" +
                        l_currentFlag +
                        "' needs a value!\n"
                    );
                }
            } else {
                throw std::runtime_error(
                    "flag '" +
                    l_currentFlag +
                    "' does not exist!\n"
                );
            }
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

tsunami_lab::io::ArgValue tsunami_lab::io::Parser::parseSetupValue(  const YAML::Node &node,
                                                                    tsunami_lab::io::ArgType type) {
    switch (type) {
        case tsunami_lab::io::ArgType::Bool: return node.as<bool>();
        case tsunami_lab::io::ArgType::Int: return node.as<tsunami_lab::t_idx>();
        case tsunami_lab::io::ArgType::String: return node.as<std::string>();
        case tsunami_lab::io::ArgType::Real: return node.as<tsunami_lab::t_real>();
    }
    throw std::runtime_error("unknown argument type");
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
                                        std::string &o_checkPointFile,
                                        bool &o_appendFile,
                                        tsunami_lab::t_idx &o_outRes,
                                        tsunami_lab::t_real &o_manningFactor,
                                        bool &o_useEntropyfix,
                                        tsunami_lab::t_idx &o_timeSteps,
                                        tsunami_lab::io::SetupArgs &o_setupArgs
                                    ){
    YAML::Node l_file;
    printf("start parseFile\n");
    try {
        l_file = YAML::LoadFile(i_file);

        auto  args = l_file["args"][0];

        o_solverName = args["solverName"].as<std::string>();
        o_left = args["startCoordX"].as<tsunami_lab::t_real>();
        o_upper = args["startCoordY"].as<tsunami_lab::t_real>();
        o_formatName = args["formatName"].as<std::string>();
        o_dxy = args["cellSize"].as<tsunami_lab::t_real>();
        o_nx = args["cellx"].as<tsunami_lab::t_idx>();
        o_ny = args["celly"].as<tsunami_lab::t_idx>();
        o_endTime = args["endTime"].as<tsunami_lab::t_real>();
        o_timeSteps = args["timeSteps"].as<tsunami_lab::t_idx>();
        o_stationsFilePath = args["stations"].as<std::string>();
        o_displacementNCFilePath = args["displacement"].as<std::string>();
        o_bathymetryNCFilePath = args["bathymetry"].as<std::string>();
        o_setupName = args["setupName"].as<std::string>();
        
        //setup args
        o_setupArgs.name = o_setupName;
        o_setupArgs.values.clear();
        auto l_setupNode = l_file["setup"];

        auto l_setupDefIt = SETUP_DEFS.find(o_setupName);
        if (l_setupDefIt == SETUP_DEFS.end()) {
            std::cerr << "Unknown setup in yaml: " << o_setupName << std::endl;
            return;
        }

        const tsunami_lab::io::SetupDef &l_setupDef = l_setupDefIt->second;

        for (const tsunami_lab::io::SetupArgDef &l_argDef : l_setupDef.args) {
            YAML::Node l_valueNode = l_setupNode[l_argDef.name];

            if (l_valueNode) {
                o_setupArgs.values[l_argDef.name] = parseSetupValue(l_valueNode, l_argDef.type);
            }
            else if (l_argDef.fallback.has_value()) {
                o_setupArgs.values[l_argDef.name] = l_argDef.fallback.value();
            }
            else if (l_argDef.required) {
                throw std::runtime_error("Missing required setup argument '"
                                            + l_argDef.name
                                            + "' for setup '"
                                            + o_setupName
                                            + "'."
                                            + "\n");
            }
        }

        if (o_setupName.compare("checkPoint") == 0 ){
            o_checkPointFile = args["checkPointFile"].as<std::string>();
            o_appendFile = args["appendFile"].as<bool>();
            return;
        }
        
        // tweaks
        o_outRes = args["outputResolution"].as<tsunami_lab::t_idx>();
        o_useEntropyfix = args["useEntropyFix"].as<bool>();
        o_manningFactor = args["manningFactor"].as<t_real>();
        

    } catch (YAML::Exception& e){
        std::cerr << "YAML Error: " << e.what() << std::endl;
        std::cerr << "Line: " << e.mark.line
                  << ", Column: " << e.mark.column << std::endl;
        return;
    }
    printf("end parseFile\n");

}
