#include "SolitaryWaveBeach1d.h"

#include <algorithm>
#include <cmath>

tsunami_lab::setups::SolitaryWaveBeach1d::SolitaryWaveBeach1d( t_real i_depth,
                                                               t_real i_waveHeight,
                                                               t_real i_slopeInv,
                                                               t_real i_shorelineX,
                                                               t_real i_waveCenterX,
                                                               t_real i_velocitySign ):
  m_depth(i_depth),
  m_waveHeight(i_waveHeight),
  m_slopeInv(i_slopeInv),
  m_shorelineX(i_shorelineX),
  m_waveCenterX(i_waveCenterX),
  m_velocitySign(i_velocitySign) {
}

tsunami_lab::io::SetupDef tsunami_lab::setups::SolitaryWaveBeach1d::getSetupDef() {
  return {
    "solitaryWaveBeach",
    {
      {"depth", tsunami_lab::io::ArgType::Real, false, tsunami_lab::t_real(1.0)},
      {"waveHeight", tsunami_lab::io::ArgType::Real, false, tsunami_lab::t_real(0.019)},
      {"slopeInv", tsunami_lab::io::ArgType::Real, false, tsunami_lab::t_real(19.85)},
      {"shorelineX", tsunami_lab::io::ArgType::Real, false, tsunami_lab::t_real(50.0)},
      {"waveCenterX", tsunami_lab::io::ArgType::Real, false, tsunami_lab::t_real(95.0)},
      {"velocitySign", tsunami_lab::io::ArgType::Real, false, tsunami_lab::t_real(-1.0)}
    },
    "setup:\n"
    " depth: ${float}\n"
    " waveHeight: ${float}\n"
    " slopeInv: ${float}\n"
    " shorelineX: ${float}\n"
    " waveCenterX: ${float}\n"
    " velocitySign: ${float}\n"
  };
}

tsunami_lab::t_real tsunami_lab::setups::SolitaryWaveBeach1d::getSurfaceElevation( t_real i_x ) const {
  t_real l_k = std::sqrt( t_real(3) * m_waveHeight / (t_real(4) * m_depth * m_depth * m_depth) );
  t_real l_arg = l_k * (i_x - m_waveCenterX);
  t_real l_sech = t_real(1) / std::cosh(l_arg);
  return m_waveHeight * l_sech * l_sech;
}

tsunami_lab::t_real tsunami_lab::setups::SolitaryWaveBeach1d::getBathymetry( t_real i_x,
                                                                             t_real      ) const {
  t_real l_xFromShore = i_x - m_shorelineX;

  if (l_xFromShore <= 0) {
    return -l_xFromShore / m_slopeInv;
  }

  return -std::min(m_depth, l_xFromShore / m_slopeInv);
}

tsunami_lab::t_real tsunami_lab::setups::SolitaryWaveBeach1d::getHeight( t_real i_x,
                                                                         t_real      ) const {
  t_real l_eta = getSurfaceElevation(i_x);
  t_real l_b = getBathymetry(i_x, 0);

  return std::max( t_real(0), l_eta - l_b );
}

tsunami_lab::t_real tsunami_lab::setups::SolitaryWaveBeach1d::getMomentumX( t_real i_x,
                                                                            t_real      ) const {
  t_real l_h = getHeight(i_x, 0);
  if (l_h <= 0) {
    return 0;
  }

  t_real l_eta = getSurfaceElevation(i_x);
  t_real l_celerity = std::sqrt( t_real(9.81) * (m_depth + m_waveHeight) );
  t_real l_u = m_velocitySign * l_celerity * l_eta / std::max(m_depth, t_real(1e-6));

  return l_h * l_u;
}

tsunami_lab::t_real tsunami_lab::setups::SolitaryWaveBeach1d::getMomentumY( t_real,
                                                                            t_real ) const {
  return 0;
}
