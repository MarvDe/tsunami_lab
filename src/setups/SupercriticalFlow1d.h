#ifndef TSUNAMI_LAB_SETUPS_SUPERCRITICAL_FLOW_1D_H
#define TSUNAMI_LAB_SETUPS_SUPERCRITICAL_FLOW_1D_H

#include "Setup.h"

namespace tsunami_lab {
  namespace setups {
    class SupercriticalFlow1d;
  }
}


/**
 * 1d Supercritical flow setup
 */
class tsunami_lab::setups::SupercriticalFlow1d: public Setup {
private:
    //! momentum of the water
    t_real m_hu = 0.18;

public:
    /**
     * Constructor.
     */
    SupercriticalFlow1d();

    /**
     * Gets the water height at a given point.
     *
     * @param i_x x-coordinate of the queried point.
     * @return water height at the given point.
     **/
    t_real getHeight(t_real i_x, t_real) const;

    /**
     * gets the bathymetry.
     *
     * @param i_x x-coordinate of the queried point.
     * @return Bathymetry.
     **/
    t_real getBathymetry(t_real i_x, t_real) const;

    /**
     * Gets the momentum in x-direction.
     * 
     * @return momentum in x-direction.
     **/
    t_real getMomentumX(t_real, t_real) const;

    /**
     * Gets the momentum in y-direction.
     *
     * @return momentum in y-direction.
     **/
    t_real getMomentumY(t_real, t_real) const;

};

#endif