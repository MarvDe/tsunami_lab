/**
 * @file
 * @brief One-dimensional shock-shock problem.
 */
#include "ShockShock1d.h"

tsunami_lab::setups::ShockShock1d::ShockShock1d(t_real i_height,
                                                t_real i_momentum,
                                                t_real i_locationDiscontinuity ):   
                                                m_height(i_height),
                                                m_momentum(i_momentum),
                                                m_locationDiscontinuity(i_locationDiscontinuity) {}

tsunami_lab::t_real tsunami_lab::setups::ShockShock1d::getHeight(   t_real,
                                                                    t_real ) const {
    // if (i_x > 60 && i_x < 240) {
    //     return 0;
    // }
    // return m_height -getBathymetry(i_x, i_y);

    return m_height;
}

tsunami_lab::io::SetupDef tsunami_lab::setups::ShockShock1d::getSetupDef() {
  return {
    "shockShock",
    {
      {"waterHeight", tsunami_lab::io::ArgType::Real, false, tsunami_lab::t_real(30)},
      {"waterMomentum", tsunami_lab::io::ArgType::Real, false, tsunami_lab::t_real(10)},
      {"locationDiscontinuity", tsunami_lab::io::ArgType::Real, false, tsunami_lab::t_real(150)}
    },
    "setup:\n"
    " waterHeight: ${float}\n"
    " waterMomentum: ${float}\n"
    " locationDiscontinuity: ${float}\n"
  };
}

tsunami_lab::t_real tsunami_lab::setups::ShockShock1d::getMomentumX(t_real i_x,
                                                                    t_real) const{
    // if (i_x > 100 && i_x < 200) {
    //     return 0;
    // }
    if (i_x < m_locationDiscontinuity){
        return m_momentum;
    }
    else {
        return -m_momentum;
    }
}

tsunami_lab::t_real tsunami_lab::setups::ShockShock1d::getMomentumY(t_real,
                                                                    t_real) const {
    return 0;
}
