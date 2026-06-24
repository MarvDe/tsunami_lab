#include "F_wave.h"
#include <cmath>
#include <iostream>

void tsunami_lab::solvers::Fwave::waveSpeeds(   t_real   i_hL, 
                                                t_real   i_hR, 
                                                t_real   i_uL, 
                                                t_real   i_uR, 
                                                t_real & o_waveSpeedL, 
                                                t_real & o_waveSpeedR ){
    t_real l_hL_safe = std::max(i_hL, t_real(0));
    t_real l_hR_safe = std::max(i_hR, t_real(0));


    t_real l_cL = (i_hL > 0) ? m_gSqrt * std::sqrt(l_hL_safe) : t_real(0);
    t_real l_cR = (i_hR > 0) ? m_gSqrt * std::sqrt(l_hR_safe) : t_real(0);

    if (i_hR <= 0){
        o_waveSpeedL = i_uL - l_cL;
        o_waveSpeedR = i_uL + 2 * l_cL;
        return;
    }
    if (i_hL <= 0) {
        o_waveSpeedL = i_uR - 2 * l_cR;
        o_waveSpeedR = i_uR + l_cR;
        return;
    }

    t_real l_hSqrtL = std::sqrt(i_hL);
    t_real l_hSqrtR = std::sqrt(i_hR);
    t_real l_hRoe   = 0.5f * (i_hL + i_hR);
    t_real l_uRoe   = (i_uL * l_hSqrtL + i_uR * l_hSqrtR) / (l_hSqrtL + l_hSqrtR);
    // t_real l_cRoe   = m_gSqrt * std::sqrt(l_hRoe);

    // also compute speed bounds
    //t_real lambdaL = i_uL - l_hSqrtL * m_gSqrt;
    //t_real lambdaR = i_uR + l_hSqrtR * m_gSqrt;

    // compute wave speeds
    t_real l_ghSqrtRoe = m_gSqrt * std::sqrt(l_hRoe);
    o_waveSpeedL = l_uRoe - l_ghSqrtRoe; // std::min(l_uRoe - l_ghSqrtRoe, lambdaL);
    o_waveSpeedR = l_uRoe + l_ghSqrtRoe; // std::max(l_uRoe + l_ghSqrtRoe, lambdaR);
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
    //t_real l_uL = (i_hL > 1e-12) ? i_huL / i_hL : t_real(0);
    //t_real l_uR = (i_hR > 1e-12) ? i_huR / i_hR : t_real(0);
    const t_real delta = 1e-3;  // tune to your h scale
    t_real l_uL = (std::sqrt(2.0) * i_hL * i_huL)
                / std::sqrt(i_hL*i_hL*i_hL*i_hL + std::max(i_hL*i_hL*i_hL*i_hL, delta*delta*delta*delta));
    t_real l_uR = (std::sqrt(2.0) * i_hR * i_huR)
                / std::sqrt(i_hR*i_hR*i_hR*i_hR + std::max(i_hR*i_hR*i_hR*i_hR, delta*delta*delta*delta));


    // compute wave speeds
    t_real l_sL;
    t_real l_sR;

    //if (i_hL > 1e-12 && i_hR > 1e-12){
        waveSpeeds( i_hL,
                i_hR,
                l_uL,
                l_uR,
                l_sL,
                l_sR);
    // }
    // else if (i_hR <= 1e-12){ // Einfeldt speeds
    //     // right cell dry: use left-state characteristics only
    //     t_real l_ghL = m_gSqrt * std::sqrt(i_hL);
    //     l_sL = l_uL - l_ghL;
    //     l_sR = l_uL + 2.0 * l_ghL;  // <-- this is always > 0, correct
    // }
    // else{
    //     // left cell dry: use right-state characteristics only
    //     t_real l_ghR = m_gSqrt * std::sqrt(i_hR);
    //     l_sL = l_uR - 2.0 * l_ghR;
    //     l_sR = l_uR + l_ghR;  
    // }
  
    
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

    // // --- REFINED ENTROPY FIX (KINETIC SCALING) ---
    // t_real l_cL = (i_hL > 1e-6) ? m_gSqrt * std::sqrt(i_hL) : t_real(0);
    // t_real l_cR = (i_hR > 1e-6) ? m_gSqrt * std::sqrt(i_hR) : t_real(0);
    
    // // Calculate the jump in characteristic speeds
    // t_real l_lambdaL1 = l_uL - l_cL;
    // t_real l_lambdaR1 = l_uR - l_cR;
    
    // // Check for sonic point transition (transonic rarefaction)
    // if (l_lambdaL1 < 0 && l_lambdaR1 > 0) {
    //     // Calculate the dissipative strength required to bridge the gap
    //     // This is a local adaptation of the Harten-Hyman idea for F-Waves
    //     t_real l_d = 0.5 * (l_lambdaR1 - l_lambdaL1);
        
    //     // Apply dissipation only to the component of the wave violating entropy
    //     // We modulate the flux by the local speed deficit
    //     o_netUpdateL[0] -= l_d * (i_hL - i_hR); 
    //     o_netUpdateL[1] -= l_d * (i_huL - i_huR);
        
    //     o_netUpdateR[0] += l_d * (i_hL - i_hR);
    //     o_netUpdateR[1] += l_d * (i_huL - i_huR);
    // }


    // --- ENTROPY FIX ---
    // Break stationary expansion shocks where Delta f = 0 but Delta q != 0
    t_real l_cL = (i_hL > 0) ? m_gSqrt * std::sqrt(i_hL) : t_real(0);
    t_real l_cR = (i_hR > 0) ? m_gSqrt * std::sqrt(i_hR) : t_real(0);
    
    // Check if either characteristic family represents an expansion fan crossing zero
    bool l_fan1 = (l_uL - l_cL < 0) && (l_uR - l_cR > 0);
    bool l_fan2 = (l_uL + l_cL < 0) && (l_uR + l_cR > 0);

    if (l_fan1 || l_fan2) {
        // Compute the maximum wave speed across the interface
        t_real l_sMax = std::max(std::abs(l_uL - l_cL), std::abs(l_uR + l_cR));
        
        // Calculate a Local Lax-Friedrichs style diffusion term based on the state jump
        t_real l_diff0 = t_real(0.5) * l_sMax * (i_hR - i_hL);
        t_real l_diff1 = t_real(0.5) * l_sMax * (i_huR - i_huL);
        
        // Apply diffusion symmetrically to the net updates
        o_netUpdateL[0] -= l_diff0;
        o_netUpdateL[1] -= l_diff1;
        
        o_netUpdateR[0] += l_diff0;
        o_netUpdateR[1] += l_diff1;
    }
}