#ifndef TSUNAMI_LAB_SETUPS_SMALL_CITY_2D_H
#define TSUNAMI_LAB_SETUPS_SMALL_CITY_2D_H

#include "Setup.h"

namespace tsunami_lab{
    namespace setups{
        class SmallCity;
    }
}


/**
 * 2d small city setup.
 */
class tsunami_lab::setups::SmallCity : public Setup {
private:

    //! number of cells
    t_idx m_cellsX = 10;
    t_idx m_cellsY = 8;

    //! beach foot
    t_real m_x0 = 3;
    t_real m_d = 0.32;
    //! beach top
    t_real m_x1 = 5;
    t_real m_beachTop = 0.1;

    t_real m_waveX = 2;
    t_real m_H = 3;

    //! buildings
    t_real m_bHeight = 0.5;
    t_real m_bWidth = 0.5;
    //! first row
    t_real m_b00x = 5;
    t_real m_b00y = 1;

    t_real m_b01x = 6.5;
    t_real m_b01y = 1;
    
    t_real m_b02x = 8;
    t_real m_b02y = 1;
    //! second row
    t_real m_b10x = 5;
    t_real m_b10y = 3;

    t_real m_b11x = 6.5;
    t_real m_b11y = 3;
    
    t_real m_b12x = 8;
    t_real m_b12y = 3;
    //! third row
    t_real m_b20x = 5;
    t_real m_b20y = 5;

    t_real m_b21x = 6.5;
    t_real m_b21y = 5;
    
    t_real m_b22x = 8;
    t_real m_b22y = 5;
    //! fourth row
    t_real m_b30x = 5;
    t_real m_b30y = 7;

    t_real m_b31x = 6.5;
    t_real m_b31y = 7;
    
    t_real m_b32x = 8;
    t_real m_b32y = 7;




public:
    /**
     * Constructor.
     * 
     * 
     */
    SmallCity(){
    }

    ~SmallCity(){}

    /**
     * Gets the yaml argument definition of the setup.
     **/
    static tsunami_lab::io::SetupDef getSetupDef();

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
    t_real getMomentumX(t_real i_x,
                        t_real i_y) const;

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
    
};

#endif