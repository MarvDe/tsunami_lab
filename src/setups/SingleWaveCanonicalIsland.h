/**
 * @file
 * @brief Solitary-wave interaction with a canonical conical island.
 */
#ifndef TSUNAMI_LAB_SETUPS_SINGLE_WAVE_CANONICAL_ISLAND_2D_H
#define TSUNAMI_LAB_SETUPS_SINGLE_WAVE_CANONICAL_ISLAND_2D_H

#include "Setup.h"

namespace tsunami_lab{
    namespace setups{
        class SingleWaveCanonicalIsland;
    }
}


/**
 * Two-dimensional solitary-wave setup with a canonical conical island.
 * This is a benchmark setup defined by the NOAA.
 */
class tsunami_lab::setups::SingleWaveCanonicalIsland : public Setup {
private:

    //! number of cells
    t_idx m_cellsX = 25;
    t_idx m_cellsY = 30;

    //! location of island
    t_real m_x0 = 12.96;
    t_real m_y0 = 13.80;

    //! dimensions of island
    t_real m_rBase = 3.6;
    t_real m_rTop = 1.1;
    t_real m_hCone = 0.625;

    //! height of water
    t_real m_d = 0.32;

    //! initial height of wave
    t_real m_H = 5;

    //! initial position of wave
    t_real m_x1 = 3;

public:
    /**
     * Constructs the setup using the canonical benchmark parameters.
     */
    SingleWaveCanonicalIsland(){
    }

    /** Destructor. */
    ~SingleWaveCanonicalIsland(){}

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
     * Gets the momentum in x-direction.
     * 
     * @param i_x x-coordinate of the queried point.
     * @param i_y y-coordinate of the queried point.
     * @return momentum in x-direction.
     */
    t_real getMomentumX(t_real i_x,
                        t_real i_y) const;

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
    
};

#endif
