
/**
 * @file
 * @brief Runtime-argument and YAML-configuration parsing.
 */
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
 * Parses command-line arguments and simulation configuration files.
 **/
class tsunami_lab::io::Parser{
  private:

    //! Map of argument-name and argument-value pairs.
    std::unordered_map<std::string, std::string> m_args;

  public:

    /**
     * Parses the command-line arguments.
     * 
     * @param i_argc number of command-line arguments.
     * @param i_argv array of command-line arguments.
     * @throws std::runtime_error if an argument is unknown or has an invalid value specification.
     */
    Parser(int i_argc, char *i_argv[]);

    /**
     * Retrieves an argument as an index value.
     *
     * @param i_name name of the argument.
     * @param i_fallback value returned if the argument was not specified.
     * @return parsed argument value or @p i_fallback.
     **/
    t_idx get(const std::string &i_name, t_idx i_fallback);

    /**
     * Retrieves an argument as a floating-point value.
     *
     * @param i_name name of the argument.
     * @param i_fallback value returned if the argument was not specified.
     * @return parsed argument value or @p i_fallback.
     **/
    float get(const std::string &i_name, float i_fallback);

    /**
     * Retrieves an argument as a string.
     *
     * @param i_name name of the argument.
     * @param i_fallback value returned if the argument was not specified.
     * @return argument value or @p i_fallback.
     **/
    std::string get(const std::string &i_name, const std::string &i_fallback);

    /**
     * Parses a YAML simulation configuration.
     * 
     * @param i_file path to the YAML file.
     * @param o_solverName configured solver name.
     * @param o_setupName configured setup name.
     * @param o_formatName configured output format name.
     * @param o_dxy cell size.
     * @param o_nx number of cells in x-direction.
     * @param o_ny number of cells in y-direction.
     * @param o_endTime simulation end time.
     * @param o_stationsFilePath path to the station YAML file.
     * @param o_checkPointFile path to the checkpoint file.
     * @param o_appendFile whether output is appended to an existing file.
     * @param o_outRes output resolution.
     * @param o_manningFactor Manning friction factor.
     * @param o_useEntropyfix whether the entropy fix is enabled.
     * @param o_timeSteps maximum number of time steps.
     * @param o_outputInterval time-step interval between regular outputs.
     * @param o_compressionLevel NetCDF compression level.
     * @param o_checkpointInterval time-step interval between checkpoints.
     * @param o_snapshots requested simulation times for snapshots.
     * @param o_setupArgs setup-specific arguments.
     * @throws std::runtime_error if required configuration values are missing or invalid.
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

    /**
     * Converts a YAML node to the requested setup-argument type.
     *
     * @param node YAML node containing the value.
     * @param type requested argument type.
     * @return converted setup-argument value.
     * @throws std::runtime_error if @p type is unknown.
     */
    ArgValue parseSetupValue(const YAML::Node &node, ArgType type);

    //! Setup definitions indexed by setup name.
    static const std::unordered_map<std::string, tsunami_lab::io::SetupDef> SETUP_DEFS;

    //! Known command-line flags and whether they require a value.
    static const std::unordered_map<std::string, bool> knownFlags;
};



#endif
