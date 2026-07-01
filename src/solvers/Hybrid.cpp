#include "Hybrid.h"
#include "F_wave.h"
#include "hlle.h"
#include <cmath>

#include <iostream>


void tsunami_lab::solvers::Hybrid::netUpdates(  t_real i_hL,
                                                t_real i_hR,
                                                t_real i_huL,
                                                t_real i_huR,
                                                t_real i_bL,
                                                t_real i_bR,
                                                t_real o_netUpdateL[2],
                                                t_real o_netUpdateR[2]){
    t_real l_hL2;
    t_real l_hR2;
    t_real l_huL2;
    t_real l_huR2;
    t_real l_sourceL;
    t_real l_sourceR;

    hydrostatic_reconstruction( i_bL,
                                i_bR,
                                i_hL,
                                i_hR,
                                i_huL,
                                i_huR,
                                
                                l_hL2,
                                l_hR2,
                                l_huL2,
                                l_huR2,
                                l_sourceL,
                                l_sourceR
                            );

    // 3. skip only if both reconstructed heights are negligible
    if( l_hL2 <= 0 && l_hR2 <= 0 ) {
        o_netUpdateL = {0};
        o_netUpdateR = {0};
        return;
    }

    bool l_isSupercritical = false;
    bool l_isDry = i_hL <= 1e-4 || i_hR <= 1e-4;
    
    if (!l_isDry){
        t_real l_hSqrtL = std::sqrt(i_hL);
        t_real l_hSqrtR = std::sqrt(i_hR);
        t_real l_hRoe   = 0.5f * (i_hL + i_hR);
        t_real l_uRoe   = ((i_huL/i_hL) * l_hSqrtL + (i_huR/i_hR) * l_hSqrtR) / (l_hSqrtL + l_hSqrtR);

        l_isSupercritical = std::abs(l_uRoe) / (std::sqrt(9.80665 * l_hRoe)) > 1;
    }
    if (l_isSupercritical) {
        solvers::Hlle::netUpdates(
          l_hL2,
          l_hR2,
          l_huL2,
          l_huR2,
          0,
          0,
          o_netUpdateL,
          o_netUpdateR 
        );
        o_netUpdateL[1] += l_sourceL;
        o_netUpdateR[1] += l_sourceR;
    } else if (l_isDry) {
        solvers::Fwave::netUpdates(
            l_hL2,
            l_hR2,
            l_huL2,
            l_huR2,
            t_real(0),
            t_real(0),
            o_netUpdateL,
            o_netUpdateR
        );
        o_netUpdateL[1] += l_sourceL;
        o_netUpdateR[1] += l_sourceR;
    } else {
        tsunami_lab::solvers::Fwave::netUpdates(
            i_hL,
            i_hR,
            i_huL,
            i_huR,
            i_bL,
            i_bR,
            o_netUpdateL,
            o_netUpdateR
        );
    }


}

void tsunami_lab::solvers::Hybrid::hydrostatic_reconstruction(  t_real i_bL,
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
                                                            ){
    t_real l_bHalf = std::max( i_bL, i_bR );
    o_hL   = std::max( t_real(0), i_hL + i_bL - l_bHalf );
    o_hR   = std::max( t_real(0), i_hR + i_bR - l_bHalf );
    o_huL  = ( i_hL > 0 ) ? o_hL * ( i_huL / i_hL ) : t_real(0);
    o_huR  = ( i_hR > 0 ) ? o_hR * ( i_huR / i_hR ) : t_real(0);

    // 2. source corrections (signs now correct)
    o_sourceL = t_real(0.5) * 9.81 * ( o_hL * o_hL - i_hL * i_hL );
    o_sourceR = t_real(0.5) * 9.81 * ( i_hR * i_hR - o_hR * o_hR );
}