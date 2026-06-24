#include "hlle.h"
#include <cmath>
#include <iostream>

using namespace tsunami_lab;

void tsunami_lab::solvers::Hlle::waveSpeeds(
  t_real   i_hL,
  t_real   i_hR,
  t_real   i_uL,
  t_real   i_uR,
  t_real & o_waveSpeedL,
  t_real & o_waveSpeedR 
){
    //precompute the sqare root ops
    t_real l_hSqrtL = std::sqrt(i_hL);
    t_real l_hSqrtR = std::sqrt(i_hR);
    
    // compute the Roe averages
    t_real l_hRoe = 0.5f * (i_hL + i_hR);
    t_real l_uRoe = i_uL * l_hSqrtL + i_uR * l_hSqrtR;
    l_uRoe /= l_hSqrtL + l_hSqrtR;

    // also compute speed bounds
    t_real lambdaL = i_uL - l_hSqrtL * m_gSqrt;
    t_real lambdaR = i_uR + l_hSqrtR * m_gSqrt;

    // compute wave speeds
    t_real l_ghSqrtRoe = m_gSqrt * std::sqrt(l_hRoe);
    o_waveSpeedL = std::min(l_uRoe - l_ghSqrtRoe, lambdaL);
    o_waveSpeedR = std::max(l_uRoe + l_ghSqrtRoe, lambdaR);
}

void tsunami_lab::solvers::Hlle::netUpdates(
  t_real i_hL,   t_real i_hR,
  t_real i_huL,  t_real i_huR,
  t_real o_netUpdateL[2],
  t_real o_netUpdateR[2] 
){
  // 1. Dry-bed protection for velocities
  t_real l_uL = (i_hL > 1e-12) ? i_huL / i_hL : t_real(0);
  t_real l_uR = (i_hR > 1e-12) ? i_huR / i_hR : t_real(0);

  // 2. Enforce zero height for dry beds to prevent sqrt(negative)
  t_real l_hL = (i_hL > 0) ? i_hL : t_real(0);
  t_real l_hR = (i_hR > 0) ? i_hR : t_real(0);

  t_real l_sL = 0;
  t_real l_sR = 0;

  waveSpeeds(l_hL, l_hR, l_uL, l_uR, l_sL, l_sR);

  // 3. Compute left and right physical fluxes
  t_real l_fL[2] = { i_huL, l_hL * l_uL * l_uL + t_real(0.5) * m_g * l_hL * l_hL };
  t_real l_fR[2] = { i_huR, l_hR * l_uR * l_uR + t_real(0.5) * m_g * l_hR * l_hR };

  // 4. Compute Fluctuation Splitting (F_R - F_L splitting)
  if (l_sL >= 0) {
    // Purely supersonic right-going flow
    o_netUpdateL[0] = 0;
    o_netUpdateL[1] = 0;

    o_netUpdateR[0] = l_fR[0] - l_fL[0];
    o_netUpdateR[1] = l_fR[1] - l_fL[1];
  }
  else if (l_sR <= 0) {
    // Purely supersonic left-going flow
    o_netUpdateL[0] = l_fR[0] - l_fL[0];
    o_netUpdateL[1] = l_fR[1] - l_fL[1];

    o_netUpdateR[0] = 0;
    o_netUpdateR[1] = 0;
  }
  else {
    // Transonic/Subsonic straddling flow (Waves go both ways)
    t_real denom = 1.0f / (l_sR - l_sL);
    
    for(int c = 0; c < 2; ++c) {
      t_real delta_U = (c == 0) ? (l_hR - l_hL) : (i_huR - i_huL);
      t_real delta_F = l_fR[c] - l_fL[c];

      // Left-going fluctuations (A^- Delta Q)
      o_netUpdateL[c] = l_sL * (l_sR * delta_U - delta_F) * denom;

      // Right-going fluctuations (A^+ Delta Q)
      o_netUpdateR[c] = l_sR * (delta_F - l_sL * delta_U) * denom;
    }
  }
}

