/**
 * @section DESCRIPTION
 * One-dimensional tsunami event.
 */
#include "TsunamiEvent1d.h"

tsunami_lab::setups::TsunamiEvent1d::TsunamiEvent1d(t_idx i_cellx, t_real const * i_bIn, 
                                                    t_real i_dx ): m_dx(i_dx){
    m_cellx = i_cellx;
    m_bIn = new t_real[i_cellx];
    for (t_idx l_i = 0; l_i < i_cellx; l_i++){
        m_bIn[l_i] = i_bIn[l_i];
    }
    m_delta = 20;
}   

tsunami_lab::t_real tsunami_lab::setups::TsunamiEvent1d::getHeight( t_real i_x,
                                                                    t_real ) const {
    t_idx l_xIdx = (t_idx)(i_x / m_dx + 0.5); 
    // guard against invalid access
    if (l_xIdx >= m_cellx) return 0;

    if (m_bIn[l_xIdx] < 0){
        return std::max(-m_bIn[(int)(i_x / m_dx)], m_delta);
    }
    else{
        return 0;
    }
}

tsunami_lab::t_real tsunami_lab::setups::TsunamiEvent1d::getMomentumX(t_real,
                                                                      t_real) const {
    return 0;
}

tsunami_lab::t_real tsunami_lab::setups::TsunamiEvent1d::getMomentumY(t_real,
                                                                    t_real) const {
    return 0;
}

tsunami_lab::t_real tsunami_lab::setups::TsunamiEvent1d::getBathymetry( t_real i_x,
                                                                        t_real ) const {
    t_idx l_xIdx = (t_idx)(i_x / m_dx + 0.5); 
    // guard against invalid access
    if (l_xIdx >= m_cellx) return 0;
    
    if (m_bIn[l_xIdx] < 0){
        return std::min(m_bIn[l_xIdx], -m_delta) + verticalDisplacement(i_x);
    }
    else{
        return std::max(m_bIn[l_xIdx], m_delta) + verticalDisplacement(i_x);
    }
}

tsunami_lab::t_real tsunami_lab::setups::TsunamiEvent1d::verticalDisplacement(t_real i_x) const {
    if (175000 < i_x && i_x < 250000){
        return 10 * std::sin((i_x - 175000)/37500 * M_PI + M_PI);
    }
    else{
        return 0;
    }
}

tsunami_lab::setups::TsunamiEvent1d::~TsunamiEvent1d(){
    delete[] m_bIn;
}