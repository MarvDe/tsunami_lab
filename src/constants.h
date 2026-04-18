/**
 * @author Alexander Breuer (alex.breuer AT uni-jena.de)
 *
 * @section DESCRIPTION
 * Constants / typedefs used throughout the code.
 **/
#ifndef TSUNAMI_LAB_CONSTANTS_H
#define TSUNAMI_LAB_CONSTANTS_H

#include <cstddef>

namespace tsunami_lab {
  //! integral type for cell-ids, pointer arithmetic, etc.
  typedef std::size_t t_idx;

  //! floating point type
  typedef float t_real;

  namespace solvers{
    //! solver ids
    const t_idx ROE = 0;
    const t_idx FWAVE = 1;
  }

  namespace setups{
    //! solver ids
    const t_idx DAM_BREAK = 0;
    const t_idx RARE_RARE = 1;
  }

}

#endif