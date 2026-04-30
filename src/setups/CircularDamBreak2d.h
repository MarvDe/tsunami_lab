/**
 * @author Alexander Breuer (alex.breuer AT uni-jena.de)
 *
 * @section DESCRIPTION
 * two-dimensional dam break problem.
 **/
#ifndef TSUNAMI_LAB_SETUPS_DAM_BREAK_2D_H
#define TSUNAMI_LAB_SETUPS_DAM_BREAK_2D_H

#include "Setup.h"

namespace tsunami_lab {
  namespace setups {
    class CircularDamBreak2d;
  }
}

/**
 * 1d dam break setup.
 **/
class tsunami_lab::setups::CircularDamBreak2d: public Setup {
  private:

    //! number of cells
    t_idx m_cellX = 0;
    t_idx m_cellY = 0;

    //! size of cell
    t_real m_dxy = 0;

    //! height of circle 
    t_real m_heightCircle = 0;
    
    //! radius of circle
    t_real m_radius = 0;

    //! 
    t_real * m_bathymetry = nullptr;

  public:
    /**
     * Constructor.
     *
     * @param i_heightCircle height of circle.
     * @param i_bathymetry bathymetry.
     * @param i_radius radius of circle.
     * @param i_cellX number of cells in x direction 
     * @param i_cellY number of cells in y direction
     * @param i_dxy size of cell
     **/
    CircularDamBreak2d( t_real i_heightCircle,
                        t_real * i_bathymetry,
                        t_real i_radius,
                        t_idx i_cellX,
                        t_idx i_cellY,
                        t_real i_dxy
                      );
    
    ~CircularDamBreak2d();

    /**
     * Gets the water height at a given point.
     *
     * @param i_x x-coordinate of the queried point.
     * @return height at the given point.
     **/
    t_real getHeight( t_real i_x,
                      t_real i_y    ) const;

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
    t_real getBathymetry( t_real i_x,
                          t_real i_y ) const;

};

#endif