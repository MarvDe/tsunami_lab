/**
 * @file
 * @brief Two-dimensional artificial-tsunami setup.
 */
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
 * Two-dimensional artificial-tsunami setup.
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
     * Constructs the setup with its fixed grid dimensions.
     */
    ArtificialTsunami2d(){
        m_stride = m_cellsX;
    }

    /** Destructor. */
    ~ArtificialTsunami2d(){}

    /**
     * Gets the YAML argument definition of the setup.
     *
     * @return setup argument definition.
     **/
    static tsunami_lab::io::SetupDef getSetupDef();

    /**
     * Gets the water height at a given point.
     * 
     * @param i_x x-coordinate of the queried point.
     * @param i_y y-coordinate of the queried point.
     * @return water height at the given point.
     */
    t_real getHeight(   t_real i_x,
                        t_real i_y) const;

    /**
     * Gets the momentum in x-direction, which is zero initially.
     * 
     * @return momentum in x-direction.
     */
    t_real getMomentumX(t_real,
                        t_real) const {
        return 0;
    }

    /**
     * Gets the momentum in y-direction, which is zero initially.
     * 
     * @return momentum in y-direction.
     */
    t_real getMomentumY(t_real,
                        t_real) const {
        return 0;
    }

    /**
     * Gets the bathymetry.
     * 
     * @param i_x x-coordinate of the queried point.
     * @param i_y y-coordinate of the queried point.
     * @return bathymetry at the given point.
     */
    t_real getBathymetry(   t_real i_x,
                            t_real i_y) const;
    
    /**
     * Gets the displacement.
     * 
     * @param i_x x-coordinate of the queried point.
     * @param i_y y-coordinate of the queried point.
     * @return displacement at the given point.
     */
    t_real getDisplacement( t_real i_x,
                            t_real i_y) const;
};

#endif
