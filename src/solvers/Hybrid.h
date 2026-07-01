/**
 * @author Alexander Breuer (alex.breuer AT uni-jena.de)
 *
 * @section DESCRIPTION
 * F-wave Riemann solver for the one-dimensional shallow water equations.
 **/
#ifndef TSUNAMI_LAB_SOLVERS_HYBRID
#define TSUNAMI_LAB_SOLVERS_HYBRID

#include "../constants.h"

namespace tsunami_lab {
  namespace solvers {
    class Hybrid;
  }
}

class tsunami_lab::solvers::Hybrid {
  private:
    //! square root of gravity
    static t_real constexpr m_gSqrt = 3.131557121;
    static t_real constexpr m_g = 9.80665;
    static t_real constexpr m_delta = 0.1;

  public:
    /**
     * Computes the net-updates.
     *
     * @param i_hL height of the left side.
     * @param i_hR height of the right side.
     * @param i_huL momentum of the left side.
     * @param i_huR momentum of the right side.
     * @param i_huL momentum of the left side.
     * @param i_huR momentum of the right side.
     * @param o_netUpdateL will be set to the net-updates for the left side; 0: height, 1: momentum.
     * @param o_netUpdateR will be set to the net-updates for the right side; 0: height, 1: momentum.
     **/
    static void netUpdates( t_real i_hL,
                            t_real i_hR,
                            t_real i_huL,
                            t_real i_huR,
                            t_real i_bL,
                            t_real i_bR,
                            t_real o_netUpdateL[2],
                            t_real o_netUpdateR[2] );
    

    static void hydrostatic_reconstruction( t_real i_bL,
                                            t_real i_bR,
                                            t_real i_hL,
                                            t_real i_hR,
                                            t_real i_huL,
                                            t_real i_huR,
                                            
                                            t_real &o_hL,
                                            t_real &o_hR,
                                            t_real &o_huL,
                                            t_real &o_huR,
                                            t_real &o_sourceL,
                                            t_real &o_sourceR
                                        );
};

#endif
