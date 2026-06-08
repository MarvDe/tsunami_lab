#ifndef TSUNAMI_LAB_SETUPS_ARTIFICIAL_TSUNAMI_2D_H
#define TSUNAMI_LAB_SETUPS_ARTIFICIAL_TSUNAMI_2D_H

#include "Setup.h"

namespace tsunami_lab{
    namespace setups{
        class ArtificialTsunami2d;
        constexpr t_real PI = 3.14159265358979323846;
    }
}


/**
 * 2d artificial Tsunami setup.
 */
class tsunami_lab::setups::ArtificialTsunami2d : public Setup {
private:

    //! number of cells
    t_idx m_cellsX = 1000;
    t_idx m_cellsY = 1000;

    //! stride of the setup
    t_idx m_stride = 0;

public:
    /**
     * Constructor.
     * 
     * 
     */
    ArtificialTsunami2d(){
        m_stride = m_cellsX;
    }

    ~ArtificialTsunami2d(){}

    /**
     * Gets the water height at a given point.
     * 
     * @param i_x x-coordinate of the queried point.
     * @param i_y y-coordinate of the queried point.
     * @return height of the point.
     */
    t_real getHeight(   t_real i_x,
                        t_real i_y) const;

    /**
     * Gets the momentum in x-direction.
     * 
     * @return momentum in x-direction.
     */
    t_real getMomentumX(t_real,
                        t_real) const {
        return 0;
    }

    /**
     * Gets the momentum in y-direction.
     * 
     * @return momentum in y-drection.
     */
    t_real getMomentumY(t_real,
                        t_real) const {
        return 0;
    }

    /**
     * Gets the bathymetry
     * 
     * @param i_x x-coordinate of the queried point.
     * @param i_y y-coordinate of the queried point.
     * @return bathymetry of the point.
     */
    t_real getBathymetry(   t_real i_x,
                            t_real i_y) const;
    
    /**
     * Gets the displacement.
     * 
     * @param i_x x-coordinate of the queried point.
     * @param i_y y-coordinate of the queried point.
     * @return displaceent of the point.
     */
    t_real getDisplacement( t_real i_x,
                            t_real i_y) const;
};

#endif