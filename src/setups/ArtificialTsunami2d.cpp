#include "ArtificialTsunami2d.h"
#include <cmath>

using namespace tsunami_lab;

t_real setups::ArtificialTsunami2d::getHeight(  t_real i_x,
                                                t_real i_y) const {
    t_real l_out = -getBathymetry(i_x,i_y) + getDisplacement(i_x, i_y);
    if (l_out < 0){
        return 0;
    }
    else if (l_out < 1e-6){
        return static_cast<t_real>(1e-6);
    }
    return l_out;
}

t_real setups::ArtificialTsunami2d::getBathymetry(  t_real,
                                                    t_real) const {
    return -100;
}

t_real setups::ArtificialTsunami2d::getDisplacement(t_real i_x,
                                                    t_real i_y) const {
    // shift coordinates to [-500, 500]
    t_real l_x = i_x - 500.0;
    t_real l_y = i_y - 500.0;

    if( std::abs(l_x) > 500 || std::abs(l_y) > 500 ){
        return 0;
    }
    t_real l_f = std::sin(((l_x / (static_cast<t_real>(m_cellsX)/2) ) + 1) * setups::PI);
    t_real l_g = -((l_y / (static_cast<t_real>(m_cellsY)/ 2)) * (l_y / (static_cast<t_real>(m_cellsY)/ 2))) + 1;
    return 5 * l_f * l_g;
}