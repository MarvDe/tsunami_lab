/**
 * @section DESCRIPTION
 * One-dimensional tsunami event.
 */
#ifndef TSUNAMI_LAB_SETUPS_TSUNAMI_EVENT_H
#define TSUNAMI_LAB_SETUPS_TSUNAMI_EVENT_H

#include "Setup.h"
#include <algorithm>
#include <cmath>

namespace tsunami_lab {
    namespace setups {
        class TsunamiEvent1d;
    }
}

/**
 * 1d tsunami event setup.
 */

class tsunami_lab::setups::TsunamiEvent1d: public Setup {
private:
    //! number of cells
    t_idx m_cellx = 0;

    //! input bathymetry data
    t_real * m_bIn = nullptr;

    //! size of cell
    t_real m_dx = 0;

    //! value for numerical stability 
    t_real m_delta = 0;

    /**
     * Vertical displacement
     * 
     * @param i_x x coordinate
     */
    t_real verticalDisplacement(t_real i_x) const;

public:
    /**
     * Constructor.
     * 
     * @param i_cellx number of cells.
     * @param i_bIn input bathymetry data.
     * @param i_dx size of cell.
     * 
     */
    TsunamiEvent1d( t_idx i_cellx, t_real const * i_bIn, t_real i_dx );
    
    /**
     * Destructor.
     */
    ~TsunamiEvent1d();

    /**
     * Gets waterheight at a given point.
     * @return height at the given point.
     */
    t_real getHeight(t_real, t_real) const;

    /**
     * Gets the momentum in x-direction.
     * @return momentum at the given point.
     */
    t_real getMomentumX(t_real , t_real) const;

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
                          t_real ) const;
};

#endif
