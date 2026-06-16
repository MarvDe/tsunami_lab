/**
 * @author Alexander Breuer (alex.breuer AT uni-jena.de)
 *
 * @section DESCRIPTION
 * One-dimensional wetting problem.
 **/
#include "Wetting1d.h"

tsunami_lab::setups::Wetting1d::Wetting1d(  ) {}

tsunami_lab::t_real tsunami_lab::setups::Wetting1d::getHeight( t_real i_x,
                                                                t_real      ) const {
  if( i_x < 10 ) {
    return 0.1;
  }
  else {
    return 0;
  }
}

tsunami_lab::t_real tsunami_lab::setups::Wetting1d::getMomentumX( t_real,
                                                                   t_real ) const {
  return 0;
}

tsunami_lab::t_real tsunami_lab::setups::Wetting1d::getMomentumY( t_real,
                                                                   t_real ) const {
  return 0;
}

tsunami_lab::t_real tsunami_lab::setups::Wetting1d::getBathymetry( t_real, t_real ) const {
    return 0;
}