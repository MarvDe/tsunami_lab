#include "F_wave.h"
#include <cmath>
#include <iostream>

void tsunami_lab::solvers::Fwave::waveSpeeds(   t_real   i_hL, 
                                                t_real   i_hR, 
                                                t_real   i_uL, 
                                                t_real   i_uR, 
                                                t_real & o_waveSpeedL, 
                                                t_real & o_waveSpeedR ){
    //precompute the sqare root ops
    t_real l_hSqrtL = std::sqrt(i_hL);
    t_real l_hSqrtR = std::sqrt(i_hR);
    
    // compute the Roe averages
    t_real l_hRoe = 0.5f * (i_hL + i_hR);
    t_real l_uRoe = i_uL * l_hSqrtL + i_uR * l_hSqrtR;
    l_uRoe /= l_hSqrtL + l_hSqrtR;

    // compute wave speeds
    t_real l_ghSqrtRoe = m_gSqrt * std::sqrt(l_hRoe);
    o_waveSpeedL = l_uRoe - l_ghSqrtRoe;
    o_waveSpeedR = l_uRoe + l_ghSqrtRoe;
}

void tsunami_lab::solvers::Fwave::waveStrengths(t_real   i_hL, 
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
                                                t_real & o_strengthR){
    //calculate the inverse l_rInv of R = [eigvec1, eigvec2]
    t_real l_rInvDet = 1 / (i_waveSpeedR - i_waveSpeedL);

    t_real l_rInv00 = l_rInvDet * i_waveSpeedR;
    t_real l_rInv01 = - l_rInvDet;
    t_real l_rInv10 = - l_rInvDet * i_waveSpeedL;
    t_real l_rInv11 = l_rInvDet;

    
    // bathymetry
    t_real l_psi = -m_g * t_real(0.5) * (i_hL + i_hR) * (i_bR - i_bL);
    
    //calculate flux function for both sides
    //calculate delta f for strength calculation

    t_real l_df0 = i_huR - i_huL;
    t_real l_fR1 = i_huR * i_uR + t_real(0.5) * m_g * i_hR * i_hR;
    t_real l_fL1 = i_huL * i_uL + t_real(0.5) * m_g * i_hL * i_hL;
    t_real l_df1 = l_fR1 - l_fL1 - l_psi;

    //calculate: sigmas = l_rInv * df
    o_strengthL = l_rInv00 * l_df0 + l_rInv01 * l_df1;

    o_strengthR = l_rInv10 * l_df0 + l_rInv11 * l_df1;
}

void tsunami_lab::solvers::Fwave::netUpdates(   t_real i_hL,
                                                t_real i_hR,
                                                t_real i_huL,
                                                t_real i_huR,
                                                t_real i_bL,
                                                t_real i_bR,
                                                t_real o_netUpdateL[2],
                                                t_real o_netUpdateR[2]){
    // calculate particle speed
    t_real l_uL = (i_hL > 0) ? i_huL / i_hL : t_real(0);
    t_real l_uR = (i_hR > 0) ? i_huR / i_hR : t_real(0);

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
    t_real l_aL = 0;
    t_real l_aR = 0;

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
    
    t_real l_zL[2] = {0};
    t_real l_zR[2] = {0};

    l_zL[0] = l_aL;
    l_zL[1] = l_aL * l_sL;
    
    l_zR[0] = l_aR;
    l_zR[1] = l_aR * l_sR;

    //init 
    o_netUpdateL[0] = 0;
    o_netUpdateL[1] = 0;
    o_netUpdateR[0] = 0;
    o_netUpdateR[1] = 0;

    //1st wave
    if (l_sL < 0){
        o_netUpdateL[0] += l_zL[0];
        o_netUpdateL[1] += l_zL[1];
    }
    else if (l_sL > 0) {
        o_netUpdateR[0] += l_zL[0];
        o_netUpdateR[1] += l_zL[1];
    }

    //2nd wave
    if (l_sR > 0){
        o_netUpdateR[0] += l_zR[0];
        o_netUpdateR[1] += l_zR[1];
    }
    else if (l_sR < 0){
        o_netUpdateL[0] += l_zR[0];
        o_netUpdateL[1] += l_zR[1];
    }
}