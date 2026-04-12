#include "F_wave.h"
#include <cmath>

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
                                                t_real   i_waveSpeedL, 
                                                t_real   i_waveSpeedR, 
                                                t_real & o_strengthL, 
                                                t_real & o_strengthR){
    //calculate the inverse l_rInv of R = [eigvec1, eigvec2]
    t_real l_rInvDet = 1 / (i_waveSpeedR - i_waveSpeedL);
    t_real l_rInv[2][2] = {0};
    l_rInv[0][0] = l_rInvDet * i_waveSpeedR;
    l_rInv[0][1] = - l_rInvDet;
    l_rInv[1][0] = - l_rInvDet * i_waveSpeedL;
    l_rInv[1][1] = l_rInvDet;

    //calculate flux function for both sides
    t_real l_fR[2] = {i_huR, i_huR * i_uR + 0.5f * m_g * i_hR * i_hR};
    t_real l_fL[2] = {i_huL, i_huL * i_uL + 0.5f * m_g * i_hL * i_hL};

    //calculate delta f for strength calculation
    t_real l_df[2] = {l_fR[0] - l_fL[0], l_fR[1] - l_fL[1]};

    //calculate: sigmas = l_rInv * df
    o_strengthL = l_rInv[0][0] * l_df[0];
    o_strengthL += l_rInv[0][1] * l_df[1];

    o_strengthR = l_rInv[1][0] * l_df[0];
    o_strengthR += l_rInv[1][1] * l_df[1];
}

void tsunami_lab::solvers::Fwave::netUpdates(   t_real i_hL,
                                                t_real i_hR,
                                                t_real i_huL,
                                                t_real i_huR,
                                                t_real o_netUpdateL[2],
                                                t_real o_netUpdateR[2]){
    // calculate particle speed
    t_real l_uL = i_huL / i_hL;
    t_real l_uR = i_huR / i_hR;

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
    
    for (unsigned int l_pt = 0; l_pt < 2; l_pt++){
        //init 
        o_netUpdateL[l_pt] = 0;
        o_netUpdateR[l_pt] = 0;

        //1st wave
        if (l_sL < 0){
            o_netUpdateL[l_pt] += l_zL[l_pt];
        }
        else {
            o_netUpdateR[l_pt] += l_zL[l_pt];
        }

        //2nd wave
        if (l_sR > 0){
            o_netUpdateR[l_pt] += l_zR[l_pt];
        }
        else {
            o_netUpdateL[l_pt] += l_zR[l_pt];
        }
    }
}