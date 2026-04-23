/**
 * @section DESCRIPTION
 * One-dimensional shock-shock problem.
 */
#ifndef TSUNAMI_LAB_SETUPS_SHOCK_SHOCK_H
#define TSUNAMI_LAB_SETUPS_SHOCK_SHOCK_H

#include "Setup.h"

namespace tsunami_lab {
    namespace setups {
        class ShockShock1d;
    }
}

/**
 * 1d shock-shock setup.
 */

class tsunami_lab::setups::ShockShock1d: public Setup {
private:
    //! height of the water
    t_real m_height = 0;

    //! momentum of the water
    t_real m_momentum = 0;

    //! position of the discontinuity
    t_real m_locationDiscontinuity = 0;
public:
    /**
     * Constructor.
     * 
     * @param i_height water height.
     * @param i_momentum water momentum.
     * @param i_locationDiscontinuity location (x-coordinate) of the discontinuity
     * 
     */
    ShockShock1d(   t_real i_height,
                    t_real i_momentum,
                    t_real i_locationDiscontinuity);
    
    /**
     * Gets waterheight at a given point.
     * @return height at the given point.
     */
    t_real getHeight(t_real, t_real) const;

    /**
     * Gets the momentum in x-direction.
     * @param i_x location (x-coordinate) of the queried point.
     * @return momentum at the given point.
     */
    t_real getMomentumX(t_real i_x , t_real) const;

    /**
     * Gets the momentum in y-direction.
     * @return momentum at the given point.
     */
    t_real getMomentumY(t_real, t_real) const;

      /**
     * gets the bathymetry.
     *
     * @return Bathymetry.
     **/
    t_real getBathymetry( t_real,
                          t_real ) const {
        return 0;
    }
};

#endif
