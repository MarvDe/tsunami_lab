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
    t_idx m_cellsXBat = 0;
    t_idx m_cellsYBat = 0;

    //! number of cells in displacement grid
    t_idx m_cellsXDis = 0;
    t_idx m_cellsYDis = 0;

    //! size of cells
    t_real m_dxy = 1;
    t_real m_dxyBat = 1;
    t_real m_dxyDis = 1;

    //! coordinates 
    t_real m_left = 0;
    t_real m_upper = 0;
    t_real m_leftBat = 0;
    t_real m_upperBat = 0;
    t_real m_leftDis = 0;
    t_real m_upperDis = 0;

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
     * @param i_left coordinate of left most cell.
     * @param i_upper coordinate of upper most cell.
     * @param i_cellsXBat number of cells in x direction of bathymetry grid.
     * @param i_cellsYBat number of cells in y direction of bathymetry grid.
     * @param i_dxyBat size of cell of bathymetry grid.
     * @param i_leftBat coordinate of left most cell in bathymetry grid.
     * @param i_upperBat coordinate of upper most cell in bathymetry grid.
     * @param i_cellsXDis number of cells in x direction of displacement grid.
     * @param i_cellsYDis number of cells in y direction of displacement grid.
     * @param i_dxyDis size of cell of displacement grid.
     * @param i_leftDis coordinate of left most cell in displacement grid.
     * @param i_upperDis coordinate of upper most cell in displacement grid.
     * @param i_bathymetry input bathymetry data.
     * @param i_displacement input displacement data.
     */
    TsunamiEvent2d( t_idx i_cellsX, t_idx i_cellsY, t_real i_dxy, t_real i_left, t_real i_upper, t_idx i_cellsXBat, t_idx i_cellsYBat, t_real i_dxyBat, t_real i_leftBat, t_real i_upperBat,
                    t_idx i_cellsXDis, t_idx i_cellsYDis, t_real i_dxyDis, t_real i_leftDis, t_real i_upperDis, const t_real * i_bathymetry, const t_real * i_displacement);

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