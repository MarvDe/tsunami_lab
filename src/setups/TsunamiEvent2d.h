#ifndef TSUNAMI_LAB_SETUPS_TSUNAMI_EVENT_2D_H
#define TSUNAMI_LAB_SETUPS_TSUNAMI_EVENT_2D_H

#include <string>
#include "../io/NetCdf.h"
#include "Setup.h"

namespace tsunami_lab{
    namespace setups{
        class TsunamiEvent2d;
    }
}


/**
 * 2d Tsunami Event setup.
 */
class tsunami_lab::setups::TsunamiEvent2d : public Setup {
private:

    //! number of cells
    t_idx m_cellsX = 0;
    t_idx m_cellsY = 0;

    //! number of cells in bathymetry grid
    t_idx m_bCellsX = 0;
    t_idx m_bCellsY = 0;

    //! number of cells in displacement grid
    t_idx m_dCellsX = 0;
    t_idx m_dCellsY = 0;

    //! size of cells
    t_real m_dxy = 1;

    //! bathymetry data
    t_real * m_bathymetry = nullptr;
    //! displacement data
    t_real * m_displacement = nullptr;

    //! delta 
    t_real m_delta = 20;

public:
    /**
     * Constructor.
     * 
     * @param i_cellsX number of cells in x direction.
     * @param i_cellsY number of cells in y direction.
     * @param i_dxy size of cell.
     * @param i_bCellsX 
     * @param i_bCellsY
     * @param i_dCellsX
     * @param i_dCellsY
     * @param i_bathymetry input bathymetry data.
     * @param i_displacement input displacement data.
     */
    TsunamiEvent2d( t_idx i_cellsX, t_idx i_cellsY, t_real i_dxy, t_idx i_bCellsX, t_idx i_bCellsY, t_idx i_dCellsX, t_idx i_dCellsY, const t_real * i_bathymetry, const t_real * i_displacement);

    /**
     * Destructor.
     */
    ~TsunamiEvent2d();

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

};

#endif