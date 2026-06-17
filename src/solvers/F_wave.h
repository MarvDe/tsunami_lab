/**
 * @author Alexander Breuer (alex.breuer AT uni-jena.de)
 *
 * @section DESCRIPTION
 * F-wave Riemann solver for the one-dimensional shallow water equations.
 **/
#ifndef TSUNAMI_LAB_SOLVERS_FWAVE
#define TSUNAMI_LAB_SOLVERS_FWAVE

#include "../constants.h"

#include <cmath>
#include <algorithm>

namespace tsunami_lab {
  namespace solvers {
    class Fwave;
  }
}

class tsunami_lab::solvers::Fwave {
  private:
    //! square root of gravity
    static t_real constexpr m_gSqrt = 3.131557121;
    static t_real constexpr m_g = 9.80665;

    /**
     * Computes the wave speeds.
     *
     * @param i_hL height of the left side.
     * @param i_hR height of the right side.
     * @param i_uL particle velocity of the leftside.
     * @param i_uR particles velocity of the right side.
     * @param o_waveSpeedL will be set to the speed of the wave propagating to the left.
     * @param o_waveSpeedR will be set to the speed of the wave propagating to the right.
     **/
    __attribute__((always_inline))
    inline static void waveSpeeds( t_real   i_hL,
                            t_real   i_hR,
                            t_real   i_uL,
                            t_real   i_uR,
                            t_real & o_waveSpeedL,
                            t_real & o_waveSpeedR ){
      //precompute the sqare root ops
      const t_real l_hSqrtL = std::sqrt(i_hL);
      const t_real l_hSqrtR = std::sqrt(i_hR);
      
      // compute the Roe averages
      const t_real l_hRoe = t_real(0.5) * (i_hL + i_hR);
      t_real l_uRoe = i_uL * l_hSqrtL + i_uR * l_hSqrtR;
      l_uRoe /= l_hSqrtL + l_hSqrtR;

      // compute wave speeds
      const t_real l_ghSqrtRoe = m_gSqrt * std::sqrt(l_hRoe);
      o_waveSpeedL = l_uRoe - l_ghSqrtRoe;
      o_waveSpeedR = l_uRoe + l_ghSqrtRoe;
    }

    /**
     * Computes the wave strengths.
     *
     * @param i_hL height of the left side.
     * @param i_hR height of the right side.
     * @param i_uL particle velocity of the leftside.
     * @param i_uR particle velocity of the rightside.
     * @param i_huL momentum of the left side.
     * @param i_huR momentum of the right side.
     * @param i_bL bathymetry of the left side.
     * @param i_bR bathymetry of the right side.
     * @param i_waveSpeedL speed of the wave propagating to the left.
     * @param i_waveSpeedR speed of the wave propagating to the right.
     * @param o_strengthL will be set to the strength of the wave propagating to the left.
     * @param o_strengthR will be set to the strength of the wave propagating to the right.
     **/
    __attribute__((always_inline))
    inline static void waveStrengths( t_real   i_hL,
                               t_real   i_hR,
                               t_real   i_uL,
                               t_real   i_uR,
                               t_real   i_huL,
                               t_real   i_huR,
                               t_real   i_bL,
                               t_real   i_bR,
                               t_real   i_waveSpeedL,
                               t_real   i_waveSpeedR,
                               t_real & o_strengthL,
                               t_real & o_strengthR ) {
      //calculate the inverse l_rInv of R = [eigvec1, eigvec2]
      const t_real l_rInvDet = t_real(1) / (i_waveSpeedR - i_waveSpeedL);

      const t_real l_rInv00 = l_rInvDet * i_waveSpeedR;
      const t_real l_rInv01 = - l_rInvDet;
      const t_real l_rInv10 = - l_rInvDet * i_waveSpeedL;
      const t_real l_rInv11 = l_rInvDet;

      
      // bathymetry
      const t_real l_psi = -m_g * t_real(0.5) * (i_hL + i_hR) * (i_bR - i_bL);
      
      //calculate flux function for both sides
      //calculate delta f for strength calculation

      const t_real l_df0 = i_huR - i_huL;
      const t_real l_fR1 = i_huR * i_uR + t_real(0.5) * m_g * i_hR * i_hR;
      const t_real l_fL1 = i_huL * i_uL + t_real(0.5) * m_g * i_hL * i_hL;
      const t_real l_df1 = l_fR1 - l_fL1 - l_psi;

      //calculate: sigmas = l_rInv * df
      o_strengthL = l_rInv00 * l_df0 + l_rInv01 * l_df1;

      o_strengthR = l_rInv10 * l_df0 + l_rInv11 * l_df1;
    }

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
    __attribute__((always_inline))
    inline static void netUpdates( t_real i_hL,
                            t_real i_hR,
                            t_real i_huL,
                            t_real i_huR,
                            t_real i_bL,
                            t_real i_bR,
                            t_real o_netUpdateL[2],
                            t_real o_netUpdateR[2] ) {
    // calculate particle speed

    constexpr t_real l_hMin = t_real(1e-6);

    i_hL = std::max(i_hL, l_hMin);
    i_hR = std::max(i_hR, l_hMin);


    const t_real l_uL = i_huL / i_hL;
    const t_real l_uR = i_huR / i_hR;

    // compute wave speeds
    t_real l_sL;
    t_real l_sR;

    waveSpeeds( i_hL,
                i_hR,
                l_uL,
                l_uR,
                l_sL,
                l_sR);
    
    //compute wave strengths
    t_real l_aL;
    t_real l_aR;

    waveStrengths(  i_hL,
                    i_hR,
                    l_uL,
                    l_uR,
                    i_huL,
                    i_huR,
                    i_bL,
                    i_bR,
                    l_sL,
                    l_sR,
                    l_aL,
                    l_aR);

    const t_real l_zL0 = l_aL;
    const t_real l_zL1 = l_aL * l_sL;
    const t_real l_zR0 = l_aR;
    const t_real l_zR1 = l_aR * l_sR;
    
    //init 
    // o_netUpdateL[0] = 0;
    // o_netUpdateL[1] = 0;
    // o_netUpdateR[0] = 0;
    // o_netUpdateR[1] = 0;
    
    //1st wave
    o_netUpdateL[0] = (l_sL < 0)? l_zL0 : t_real(0);
    o_netUpdateL[1] = (l_sL < 0)? l_zL1 : t_real(0);
    o_netUpdateR[0] = (l_sL > 0)? l_zL0 : t_real(0);
    o_netUpdateR[1] = (l_sL > 0)? l_zL1 : t_real(0);
    
    //2nd wave
    o_netUpdateR[0] += (l_sR > 0)? l_zR0 : t_real(0);
    o_netUpdateR[1] += (l_sR > 0)? l_zR1 : t_real(0);
    o_netUpdateL[0] += (l_sR < 0)? l_zR0 : t_real(0);
    o_netUpdateL[1] += (l_sR < 0)? l_zR1 : t_real(0);
  }
};

#endif
