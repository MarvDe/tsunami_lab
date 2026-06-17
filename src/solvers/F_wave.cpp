#include "F_wave.h"
#include <cmath>

// void tsunami_lab::solvers::Fwave::waveSpeeds(   t_real   i_hL, 
//                                                 t_real   i_hR, 
//                                                 t_real   i_uL, 
//                                                 t_real   i_uR, 
//                                                 t_real & o_waveSpeedL, 
//                                                 t_real & o_waveSpeedR ){
//     //precompute the sqare root ops
//     const t_real l_hSqrtL = std::sqrt(i_hL);
//     const t_real l_hSqrtR = std::sqrt(i_hR);
    
//     // compute the Roe averages
//     const t_real l_hRoe = t_real(0.5) * (i_hL + i_hR);
//     t_real l_uRoe = i_uL * l_hSqrtL + i_uR * l_hSqrtR;
//     l_uRoe /= l_hSqrtL + l_hSqrtR;

//     // compute wave speeds
//     const t_real l_ghSqrtRoe = m_gSqrt * std::sqrt(l_hRoe);
//     o_waveSpeedL = l_uRoe - l_ghSqrtRoe;
//     o_waveSpeedR = l_uRoe + l_ghSqrtRoe;
// }

// void tsunami_lab::solvers::Fwave::waveStrengths(t_real   i_hL, 
//                                                 t_real   i_hR,
//                                                 t_real   i_uL,
//                                                 t_real   i_uR,
//                                                 t_real   i_huL, 
//                                                 t_real   i_huR,
//                                                 t_real   i_bL,
//                                                 t_real   i_bR,
//                                                 t_real   i_waveSpeedL, 
//                                                 t_real   i_waveSpeedR, 
//                                                 t_real & o_strengthL, 
//                                                 t_real & o_strengthR){
//     //calculate the inverse l_rInv of R = [eigvec1, eigvec2]
//     const t_real l_rInvDet = t_real(1) / (i_waveSpeedR - i_waveSpeedL);

//     const t_real l_rInv00 = l_rInvDet * i_waveSpeedR;
//     const t_real l_rInv01 = - l_rInvDet;
//     const t_real l_rInv10 = - l_rInvDet * i_waveSpeedL;
//     const t_real l_rInv11 = l_rInvDet;

    
//     // bathymetry
//     const t_real l_psi = -m_g * t_real(0.5) * (i_hL + i_hR) * (i_bR - i_bL);
    
//     //calculate flux function for both sides
//     //calculate delta f for strength calculation

//     const t_real l_df0 = i_huR - i_huL;
//     const t_real l_fR1 = i_huR * i_uR + t_real(0.5) * m_g * i_hR * i_hR;
//     const t_real l_fL1 = i_huL * i_uL + t_real(0.5) * m_g * i_hL * i_hL;
//     const t_real l_df1 = l_fR1 - l_fL1 - l_psi;

//     //calculate: sigmas = l_rInv * df
//     o_strengthL = l_rInv00 * l_df0 + l_rInv01 * l_df1;

//     o_strengthR = l_rInv10 * l_df0 + l_rInv11 * l_df1;
// }

// void tsunami_lab::solvers::Fwave::netUpdates(   t_real i_hL,
//                                                 t_real i_hR,
//                                                 t_real i_huL,
//                                                 t_real i_huR,
//                                                 t_real i_bL,
//                                                 t_real i_bR,
//                                                 t_real o_netUpdateL[2],
//                                                 t_real o_netUpdateR[2]){
//     // calculate particle speed

//     constexpr t_real l_hMin = t_real(1e-6);

//     i_hL = std::max(i_hL, l_hMin);
//     i_hR = std::max(i_hR, l_hMin);

//     // if (i_hL < 1e-6f){
//     //     i_hL = 1e-6f;
//     // }
//     // if (i_hR < 1e-6f){
//     //     i_hR = 1e-6f;
//     // }

//     const t_real l_uL = i_huL / i_hL;
//     const t_real l_uR = i_huR / i_hR;

//     // compute wave speeds
//     t_real l_sL;
//     t_real l_sR;

//     waveSpeeds( i_hL,
//                 i_hR,
//                 l_uL,
//                 l_uR,
//                 l_sL,
//                 l_sR);
    
//     //compute wave strengths
//     t_real l_aL;
//     t_real l_aR;

//     waveStrengths(  i_hL,
//                     i_hR,
//                     l_uL,
//                     l_uR,
//                     i_huL,
//                     i_huR,
//                     i_bL,
//                     i_bR,
//                     l_sL,
//                     l_sR,
//                     l_aL,
//                     l_aR);
    
//     // t_real l_zL[2] = {0};
//     // t_real l_zR[2] = {0};

//     // l_zL[0] = l_aL;
//     // l_zL[1] = l_aL * l_sL;
    
//     // l_zR[0] = l_aR;
//     // l_zR[1] = l_aR * l_sR;

//     const t_real l_zL0 = l_aL;
//     const t_real l_zL1 = l_aL * l_sL;
//     const t_real l_zR0 = l_aR;
//     const t_real l_zR1 = l_aR * l_sR;
    
//     //init 
//     o_netUpdateL[0] = 0;
//     o_netUpdateL[1] = 0;
//     o_netUpdateR[0] = 0;
//     o_netUpdateR[1] = 0;

//     //1st wave
//     if (l_sL < 0){
//         o_netUpdateL[0] += l_zL0;
//         o_netUpdateL[1] += l_zL1;
//     }
//     else if (l_sL > 0) {
//         o_netUpdateR[0] += l_zL0;
//         o_netUpdateR[1] += l_zL1;
//     }

//     //2nd wave
//     if (l_sR > 0){
//         o_netUpdateR[0] += l_zR0;
//         o_netUpdateR[1] += l_zR1;

//     }
//     else if (l_sR < 0){
//         o_netUpdateL[0] += l_zR0;
//         o_netUpdateL[1] += l_zR1;
//     }
// }