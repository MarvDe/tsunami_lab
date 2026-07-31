/**
 * @author Alexander Breuer (alex.breuer AT uni-jena.de)
 *
 * @file
 * @brief Two-dimensional circular dam-break problem.
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
 * Two-dimensional circular dam-break setup.
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

    //! bathymetry
    t_real * m_bathymetry = nullptr;

  public:
    /**
     * Constructor.
     *
     * @param i_heightCircle water height inside the circle.
     * @param i_bathymetry bathymetry array copied by the setup.
     * @param i_radius radius of the circle.
     * @param i_cellX number of cells in x-direction.
     * @param i_cellY number of cells in y-direction.
     * @param i_dxy cell size.
     **/
    CircularDamBreak2d( t_real i_heightCircle,
                        t_real * i_bathymetry,
                        t_real i_radius,
                        t_idx i_cellX,
                        t_idx i_cellY,
                        t_real i_dxy
                      );
    
    /** Destructor. */
    ~CircularDamBreak2d();

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
     * Gets the bathymetry.
     *
     * @param i_x x-coordinate of the queried point.
     * @param i_y y-coordinate of the queried point.
     * @return bathymetry at the given point.
     **/
    t_real getBathymetry( t_real i_x,
                          t_real i_y ) const;

};

#endif
