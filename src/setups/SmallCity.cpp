#include "SmallCity.h"
#include <cmath>

using namespace tsunami_lab;

tsunami_lab::io::SetupDef tsunami_lab::setups::SmallCity::getSetupDef() {
  return {
    "smallCity",
    {
      
    },
    "setup:\n"
  };
}

t_real setups::SmallCity::getHeight(  t_real i_x,
                                                t_real i_y) const {
    t_real l_k = std::sqrt((3 * m_H) / (4 * m_d * m_d * m_d));
    t_real l_cosh = std::cosh(l_k * (i_x - m_waveX)); 
    t_real eta = m_H * 1 / (l_cosh * l_cosh);  
    t_real l_h = eta - getBathymetry(i_x, i_y); 
    return std::max<t_real>(l_h, 0);
}

t_real setups::SmallCity::getBathymetry(  t_real i_x,
                                                    t_real i_y) const {

    // check for buildings
    if ((std::abs(i_x - m_b00x) < m_bWidth && std::abs(i_y - m_b00y) < m_bWidth) ||
        (std::abs(i_x - m_b01x) < m_bWidth && std::abs(i_y - m_b01y) < m_bWidth) ||
        (std::abs(i_x - m_b02x) < m_bWidth && std::abs(i_y - m_b02y) < m_bWidth) ||
        (std::abs(i_x - m_b10x) < m_bWidth && std::abs(i_y - m_b10y) < m_bWidth) ||
        (std::abs(i_x - m_b11x) < m_bWidth && std::abs(i_y - m_b11y) < m_bWidth) ||
        (std::abs(i_x - m_b12x) < m_bWidth && std::abs(i_y - m_b12y) < m_bWidth) ||
        (std::abs(i_x - m_b20x) < m_bWidth && std::abs(i_y - m_b20y) < m_bWidth) ||
        (std::abs(i_x - m_b21x) < m_bWidth && std::abs(i_y - m_b21y) < m_bWidth) ||
        (std::abs(i_x - m_b22x) < m_bWidth && std::abs(i_y - m_b22y) < m_bWidth) || 
        (std::abs(i_x - m_b30x) < m_bWidth && std::abs(i_y - m_b30y) < m_bWidth) ||
        (std::abs(i_x - m_b31x) < m_bWidth && std::abs(i_y - m_b31y) < m_bWidth) ||
        (std::abs(i_x - m_b32x) < m_bWidth && std::abs(i_y - m_b32y) < m_bWidth)){
            return m_bHeight;
    }
    else if (i_x - m_x1 > 0){ // on the top of the beach
        return m_beachTop;
    }
    else if (i_x - m_x0 > 0){ // on the slope of the beach
        t_real l_b = (m_beachTop * m_x0 - (-m_d) * m_x1) / (m_x0 - m_x1);
        t_real l_a = (-m_d - l_b) / m_x0;
        return i_x * l_a + l_b;
    }
    else { // in the water before the beach
        return -m_d;
    }
}

t_real setups::SmallCity::getMomentumX( t_real i_x,
                                                        t_real i_y) const {
    t_real l_k = std::sqrt((3 * m_H) / (4 * m_d * m_d * m_d));
    t_real l_cosh = std::cosh(l_k * (i_x - m_waveX)); 
    t_real eta = m_H * 1 / (l_cosh * l_cosh);  
    t_real l_c = sqrt(9.81 * (m_d + m_H));
    t_real l_u = (l_c * eta) / (m_d + eta);
    return getHeight(i_x, i_y) * l_u;
}