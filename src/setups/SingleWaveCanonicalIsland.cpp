#include "SingleWaveCanonicalIsland.h"
#include <cmath>

using namespace tsunami_lab;

tsunami_lab::io::SetupDef tsunami_lab::setups::SingleWaveCanonicalIsland::getSetupDef() {
  return {
    "singleWaveCanonicalIsland",
    {
      
    },
    "setup:\n"
  };
}

t_real setups::SingleWaveCanonicalIsland::getHeight(  t_real i_x,
                                                t_real i_y) const {
    t_real l_k = std::sqrt((3 * m_H) / (4 * m_d * m_d * m_d));
    t_real l_cosh = std::cosh(l_k * (i_x - m_x1)); 
    t_real eta = m_H * 1 / (l_cosh * l_cosh);  
    t_real l_h = eta - getBathymetry(i_x, i_y); 
    return std::max<t_real>(l_h, 0);
}

t_real setups::SingleWaveCanonicalIsland::getBathymetry(  t_real i_x,
                                                    t_real i_y) const {
    
    t_real l_r = std::sqrt(
        (i_x - m_x0) * (i_x - m_x0) + (i_y - m_y0) * (i_y - m_y0)  
    );

    if (l_r <= m_rTop){
        return m_hCone - m_d;
    }
    else if (l_r <= m_rBase){
        return (m_rBase - l_r) / 4 - m_d;
    }
    else{
        return -m_d;
    }
}

t_real setups::SingleWaveCanonicalIsland::getMomentumX( t_real i_x,
                                                        t_real i_y) const {
    t_real l_k = std::sqrt((3 * m_H) / (4 * m_d * m_d * m_d));
    t_real l_cosh = std::cosh(l_k * (i_x - m_x1)); 
    t_real eta = m_H * 1 / (l_cosh * l_cosh);  
    t_real l_c = sqrt(9.81 * (m_d + m_H));
    t_real l_u = (l_c * eta) / (m_d + eta);
    return getHeight(i_x, i_y) * l_u;
}