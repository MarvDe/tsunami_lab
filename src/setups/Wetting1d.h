/**
 * @author Alexander Breuer (alex.breuer AT uni-jena.de)
 *
 * @section DESCRIPTION
 * One-dimensional wetting problem.
 **/
#ifndef TSUNAMI_LAB_SETUPS_WETTING_1D_H
#define TSUNAMI_LAB_SETUPS_WETTING_1D_H

#include "Setup.h"

namespace tsunami_lab {
  namespace setups {
    class Wetting1d;
  }
}

/**
 * 1d wetting setup.
 **/
class tsunami_lab::setups::Wetting1d: public Setup {
  private:
    //! height on the left side 
    t_real m_heightLeft = 0;
    
    //! height on the right side
    t_real m_heightRight = 0;

    //! location of the dam
    t_real m_locationDam = 0;

  public:
    /**
     * Constructor.
     *
     **/
    Wetting1d( );

    /**
     * Gets the water height at a given point.
     *
     * @param i_x x-coordinate of the queried point.
     * @return height at the given point.
     **/
    t_real getHeight( t_real i_x,
                      t_real      ) const;

    /**
     * Gets the momentum in x-direction.
     *
     * @return momentum in x-direction.
     **/
    t_real getMomentumX( t_real,
                         t_real ) const;

    /**
     * Gets the momentum in y-direction.
     *
     * @return momentum in y-direction.
     **/
    t_real getMomentumY( t_real,
                         t_real ) const;
    /**
     * gets the bathymetry.
     *
     * @return Bathymetry.
     **/
    t_real getBathymetry( t_real,
                          t_real ) const;

};

#endif