
#include "../constants.h"
#include <array>
#include <vector>

namespace tsunami_lab {
  namespace io {
    class Parser;
  }
}

class tsunami_lab::io::Parser{
  public:
    /**
     * Function for parsing runtime arguments.
     *
     * @param i_argc number of arguments
     * @param i_argv array of arguments
     * @param o_cellx output argument for cells in x direction
     * @param o_solverId output argument for solver (0=Roe, 1=Fwave) 
     **/
    static void parse(const int i_argc, const char *i_argv[], tsunami_lab::t_idx &o_cellx, tsunami_lab::t_idx &o_solverId);

};