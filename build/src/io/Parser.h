
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
     * Function for retrieving arguments of type string.
     *
     * @param i_name name of argument
     * @param i_fallback fallback value if the argument doesn't exist
     * 
     **/
    std::string get(const std::string &i_name, const std::string &i_fallback);


};

#endif