/**
 * @author Alexander Breuer (alex.breuer AT uni-jena.de)
 *
 * @section DESCRIPTION
 * two-dimensional dam break problem.
 **/
#include "CircularDamBreak2d.h"
#include <cmath>

tsunami_lab::setups::CircularDamBreak2d::CircularDamBreak2d(  t_real i_heightCircle,
                                                              t_real * i_bathymetry,
                                                              t_real i_radius,  
                                                              t_idx i_cellX,
                                                              t_idx i_cellY,
                                                              t_real i_dxy) {
  m_heightCircle = i_heightCircle;
  m_radius = i_radius;
  m_cellX = i_cellX;
  m_cellY = i_cellY;
  m_dxy = i_dxy;

  m_bathymetry = new tsunami_lab::t_real[m_cellX * m_cellY];
  for (t_idx l_y = 0; l_y < m_cellY; l_y++){
    for (t_idx l_x = 0; l_x < m_cellX; l_x++){
      m_bathymetry[l_x + l_y * m_cellX] = i_bathymetry[l_x + l_y * m_cellX];
    }
  }
}

tsunami_lab::setups::CircularDamBreak2d::~CircularDamBreak2d(){
  delete[] m_bathymetry;
}

tsunami_lab::t_real tsunami_lab::setups::CircularDamBreak2d::getHeight( t_real i_x,
                                                                        t_real i_y     ) const {
  t_real l_cx = m_dxy * m_cellX / 2 - i_x;
  t_real l_cy = m_dxy * m_cellY / 2 - i_y;

  t_real l_r = std::sqrt(l_cx * l_cx + l_cy * l_cy);
  if (l_r <= m_radius){
    return m_heightCircle;
  }
  else{
    t_idx l_ix = i_x / m_dxy;
    t_idx l_iy = i_y / m_dxy;
    return -m_bathymetry[l_ix + l_iy * m_cellX];
  }

}

tsunami_lab::t_real tsunami_lab::setups::CircularDamBreak2d::getMomentumX( t_real,
                                                                   t_real ) const {
  return 0;
}

tsunami_lab::t_real tsunami_lab::setups::CircularDamBreak2d::getMomentumY( t_real,
                                                                   t_real ) const {
  return 0;
}

tsunami_lab::t_real tsunami_lab::setups::CircularDamBreak2d::getBathymetry( t_real i_x,
                                                                            t_real i_y ) const{
  t_idx l_ix = i_x / m_dxy;
  t_idx l_iy = i_y / m_dxy;
  return m_bathymetry[l_ix + l_iy * m_cellX];
} 