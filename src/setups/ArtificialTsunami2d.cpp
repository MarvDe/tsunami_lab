#include "ArtificialTsunami2d.h"
#include <cmath>

using namespace tsunami_lab;

t_real setups::ArtificialTsunami2d::getHeight(  t_real i_x,
                                                t_real i_y) const {
    t_real l_out = -getBathymetry(i_x,i_y);
    if (l_out < 0){
        return 0;
    }
    else if (l_out < 1e-6){
        return static_cast<t_real>(1e-6);
    }
    return l_out;
}

t_real setups::ArtificialTsunami2d::getBathymetry(  t_real i_x,
                                                    t_real i_y) const {
    t_real l_bIn = 0;
    if (l_bIn < 0){
        if (l_bIn > -1e-6){
            return 1e-6 + getDisplacement(i_x, i_y);
        }
        return l_bIn + getDisplacement(i_x, i_y);
    }
    else {
        if (l_bIn < 1e-6){
            return 1e-6 + getDisplacement(i_x, i_y);
        }
        return l_bIn + getDisplacement(i_x, i_y);
    }
}

t_real setups::ArtificialTsunami2d::getDisplacement(t_real i_x,
                                                    t_real i_y) const {
    t_real l_f = std::sin(((i_x / (static_cast<t_real>(m_cellsX)/2) ) + 1) * setups::PI);
    t_real l_g = -((i_y / (static_cast<t_real>(m_cellsY)/ 2)) * (i_y / (static_cast<t_real>(m_cellsY)/ 2))) + 1;
    return 5 * l_f * l_g;
}