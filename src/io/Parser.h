
#ifndef TSUNAMI_LAB_IO_PARSER_H
#define TSUNAMI_LAB_IO_PARSER_H

#include "../constants.h"
#include "SetupConfig.h"
#include <string>
#include <unordered_map>
#include <set>

namespace YAML {
  class Node;
}

namespace tsunami_lab {
  namespace io {
    class Parser;
  }
}

/**
 * parsing runtime arguments
 **/
class tsunami_lab::io::Parser{
  private:

    // ! map of argName: argValue pairs
    std::unordered_map<std::string, std::string> m_args;

  public:

    /**
     * Constructor.
     * 
     * @param i_argc number of arguments
     * @param i_argv array of arguments
     */
    Parser(int i_argc, char *i_argv[]);

    /**
     * Function for retrieving arguments of type t_idx.
     *
     * @param i_name name of argument
     * @param i_fallback fallback value if the argument doesn't exist
     * 
     **/
    t_idx get(const std::string &i_name, t_idx i_fallback);

    /**
     * Function for retrieving arguments of type float.
     *
     * @param i_name name of argument
     * @param i_fallback fallback value if the argument doesn't exist
     * 
     **/
    float get(const std::string &i_name, float i_fallback);

    /**
     * Function for retrieving arguments of type string.
     *
     * @param i_name name of argument
     * @param i_fallback fallback value if the argument doesn't exist
     * 
     **/
    std::string get(const std::string &i_name, const std::string &i_fallback);

    /**
     * Function for parsing a yaml file as input args.
     * 
     * @param i_file yaml file location
     * @param o_solverName name of solver
     * @param o_setupName name of setup
     * @param o_formatName name of output format
     * @param o_dxy cellsize
     * @param o_bathymetryNcFilePath path to NetCDF bathymetry file
     * @param o_displacementNCFilePath path to NetCDF displacement file
     * @param o_nx cells in x direction
     * @param o_ny cells in y direction
     * @param o_endTime duration of simulation
     * @param o_stationsFilePath path to the station yaml file
     * @param o_left 
     * @param o_upper
     * @param o_outRes
     */
    void parseFile( std::string &i_file,
                    std::string &o_solverName,
                    std::string &o_setupName,
                    std::string &o_formatName,
                    tsunami_lab::t_real &o_dxy,
                    tsunami_lab::t_idx &o_nx,
                    tsunami_lab::t_idx &o_ny,
                    tsunami_lab::t_real &o_endTime,
                    std::string &o_stationsFilePath,
                    std::string &o_checkPointFile,
                    bool &o_appendFile,
                    tsunami_lab::t_idx &o_outRes,
                    tsunami_lab::t_real &o_manningFactor,
                    bool &o_useEntropyfix,
                    tsunami_lab::t_idx &o_timeSteps,
                    tsunami_lab::t_idx &o_outputInterval,
                    tsunami_lab::t_idx &o_compressionLevel,
                    tsunami_lab::t_idx &o_checkpointInterval,
                    std::vector<tsunami_lab::t_real> &o_snapshots,
                    SetupArgs &o_setupArgs
                  );

    ArgValue parseSetupValue(const YAML::Node &node, ArgType type);

    static const std::unordered_map<std::string, tsunami_lab::io::SetupDef> SETUP_DEFS;

    static const std::unordered_map<std::string, bool> knownFlags;
};



#endif
