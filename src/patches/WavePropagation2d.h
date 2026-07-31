/**
 * @author Alexander Breuer (alex.breuer AT uni-jena.de)
 *
 * @file
 * @brief Two-dimensional wave-propagation patch.
 **/
#ifndef TSUNAMI_LAB_PATCHES_WAVE_PROPAGATION_2D
#define TSUNAMI_LAB_PATCHES_WAVE_PROPAGATION_2D

#include "WavePropagation.h"

namespace tsunami_lab {
  namespace patches {
    class WavePropagation2d;
  }
}

/** Advances the two-dimensional shallow-water equations on a Cartesian grid. */
class tsunami_lab::patches::WavePropagation2d: public WavePropagation {
  private:
    //! current step which indicates the active values in the arrays below
    unsigned short m_step = 0;

    //! Selected Riemann solver.
    tsunami_lab::solvers::Ids m_solverId = tsunami_lab::solvers::FWAVE;

    //! number of cells discretizing the computational domain
    t_idx m_xCells = 0;
    t_idx m_yCells = 0;

    //! water heights for the current and next time step for all cells
    t_real * m_h[2] = { nullptr, nullptr };

    //! momenta for the current and next time step for all cells
    t_real * m_hu[2] = { nullptr, nullptr };
    t_real * m_hv[2] = { nullptr, nullptr };

    //! bathymetry data 
    t_real * m_bathymetry = nullptr;

    //! ghost cell updating condition (0 = outflow, 1 = reflecting)
    t_idx m_ghost = 0;

    //! Time-step size.
    t_real m_dt = 0.1;

    t_real m_manningFactor = 0;
    
    bool m_useEntropyFix = true;
    
    
    public:
    /**
     * Constructs the 2d wave propagation solver.
     *
     * @param i_xCells number of cells.
     * @param i_yCells number of cells.
     * @param i_solverId flag to choose solver.
     * @param i_useEntropyFix whether to apply the entropy fix.
     * @param i_manningFactor Manning friction factor.
     * @param i_ghost ghost cell updating condition (0 = outflow, 1 = reflecting)
     **/
    WavePropagation2d( t_idx i_xCells, t_idx i_yCells, tsunami_lab::solvers::Ids i_solverId, bool i_useEntropyFix = true, t_real i_manningFactor = 0.02, t_idx i_ghost = 0);

    /**
     * Destructor which frees all allocated memory.
     **/
    ~WavePropagation2d();

    /**
     * Performs a time step.
     *
     * @param i_scaling scaling of the time step (dt / dx).
     **/
    void timeStep( t_real i_scaling );

    /**
     * Sets the values of the ghost cells according to outflow boundary conditions.
     **/
    void setGhostOutflow();

    /**
     * Sets the time-step size used by source-term updates.
     *
     * @param i_dt time-step size.
     **/
    void setDt(t_real i_dt);

    /**
     * Gets the stride in y-direction. x-direction is stride-1.
     *
     * @return stride in y-direction.
     **/
    t_idx getStride() {
      return m_xCells + 2;
    }

    /**
     * Gets the cells' water heights.
     *
     * @return water heights.
     */
    t_real const * getHeight(){
      return m_h[m_step]+(1 + getStride());
    }

    /**
     * Gets the cells' momenta in x-direction.
     *
     * @return momenta in x-direction.
     **/
    t_real const * getMomentumX(){
      return m_hu[m_step]+(1 + getStride());
    }

    /**
     * Gets the cells' momenta in y-direction.
     *
     * @return momenta in y-direction.
     **/
    t_real const * getMomentumY(){
      return m_hv[m_step]+(1 + getStride());
    }

    /**
     * Gets the cells' bathymetry.
     * 
     * @return bathymetry.
     **/
    t_real const * getBathymetry() {
      return m_bathymetry+(1 + getStride());
    }

    /**
     * Sets the height of the cell to the given value.
     *
     * @param i_ix index of the cell in x-direction.
     * @param i_iy index of the cell in y-direction.
     * @param i_h water height.
     **/
    void setHeight( t_idx  i_ix,
                    t_idx i_iy,
                    t_real i_h ) {
      m_h[m_step][i_ix + 1 + (i_iy + 1) * getStride()] = i_h;
    }

    /**
     * Sets the momentum in x-direction to the given value.
     *
     * @param i_ix index of the cell in x-direction.
     * @param i_iy index of the cell in y-direction.
     * @param i_hu momentum in x-direction.
     **/
    void setMomentumX( t_idx  i_ix,
                       t_idx i_iy,
                       t_real i_hu ) {
      m_hu[m_step][i_ix+1 + (i_iy + 1) * getStride()] = i_hu;
    }

    /**
     * Sets the momentum in y-direction to the given value.
     *
     * @param i_ix index of the cell in x-direction.
     * @param i_iy index of the cell in y-direction.
     * @param i_hv momentum in y-direction.
     **/
    void setMomentumY( t_idx i_ix,
                       t_idx  i_iy,
                       t_real i_hv) {
      m_hv[m_step][i_ix+1 + (i_iy + 1) * getStride()] = i_hv;
    };
    
    /**
     * Sets the cells' bathymetry.
     *
     * @param i_ix index of the cell in x-direction.
     * @param i_iy index of the cell in y-direction.
     * @param i_height bathymetry value.
     * 
     **/
    void setBathymetry( t_idx i_ix, 
                        t_idx i_iy, 
                        t_real i_height);

};

#endif
