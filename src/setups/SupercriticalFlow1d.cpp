#include "SupercriticalFlow1d.h"


tsunami_lab::setups::SupercriticalFlow1d::SupercriticalFlow1d() {};

tsunami_lab::t_real tsunami_lab::setups::SupercriticalFlow1d::getHeight(t_real i_x, t_real) const {
    return - tsunami_lab::setups::SupercriticalFlow1d::getBathymetry(i_x, 0);
}

tsunami_lab::t_real tsunami_lab::setups::SupercriticalFlow1d::getBathymetry(t_real i_x, t_real) const {
    if (i_x > 8 && i_x < 12){
        return -0.13 - 0.05 * (i_x - 10) * (i_x - 10);
    }
    return -0.33;
}

tsunami_lab::t_real tsunami_lab::setups::SupercriticalFlow1d::getMomentumX(t_real, t_real) const {
    return m_hu;
}

tsunami_lab::t_real tsunami_lab::setups::SupercriticalFlow1d::getMomentumY(t_real, t_real) const {
    return 0;
}