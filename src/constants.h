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
    enum Ids{
      ROE,
      FWAVE,
      FWAVE_HYDROSTATIC_RECONSTRUCTION
    };
  }

  namespace io{
    //! format ids
    enum Ids{
      CSV,
      NC,
      NONE
    };
  }

  namespace setups{
    //! setup ids
    enum Ids{
      DAM_BREAK,
      RARE_RARE,
      SHOCK_SHOCK,
      TSUNAMI_EVENT,
      SUBCRITICAL_FLOW,
      SUPERCRITICAL_FLOW,
      WETTING_1D,
      DAM_BREAK_2D,
      ARTIFICIAL_TSUNAMI_2D,
      TSUNAMI_EVENT_2D,
      CHECK_POINT,
    };
  }

}

#endif