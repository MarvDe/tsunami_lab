
#ifndef TSUNAMI_LAB_IO_PARSER_H
#define TSUNAMI_LAB_IO_PARSER_H

#include "../constants.h"
#include <string>
#include <unordered_map>

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
     * @param i_solverName name of solver
     * @param i_setupName name of setup
     * @param i_formatName name of output format
     * @param i_dxy cellsize
     * @param i_bathymetryNcFilePath path to NetCDF bathymetry file
     * @param i_displacementNCFilePath path to NetCDF displacement file
     * @param i_nx cells in x direction
     * @param i_ny cells in y direction
     * @param i_endTime duration of simulation
     * @param i_stationsFilePath path to the station yaml file
     */
    void parseFile( std::string &i_file,
                    std::string &i_solverName,
                    std::string &i_setupName,
                    std::string &i_formatName,
                    tsunami_lab::t_real &i_dxy,
                    std::string &i_bathymetryNCFilePath,
                    std::string &i_displacementNCFilePath,
                    tsunami_lab::t_idx &i_nx,
                    tsunami_lab::t_idx &i_ny,
                    tsunami_lab::t_real &i_endTime,
                    std::string &i_stationsFilePath
                  );
};

#endif