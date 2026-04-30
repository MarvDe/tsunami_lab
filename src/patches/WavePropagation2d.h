/**
 * @author Alexander Breuer (alex.breuer AT uni-jena.de)
 *
 * @section DESCRIPTION
 * One-dimensional wave propagation patch.
 **/
#ifndef TSUNAMI_LAB_PATCHES_WAVE_PROPAGATION_2D
#define TSUNAMI_LAB_PATCHES_WAVE_PROPAGATION_2D

#include "WavePropagation.h"

namespace tsunami_lab {
  namespace patches {
    class WavePropagation2d;
  }
}

class tsunami_lab::patches::WavePropagation2d: public WavePropagation {
  private:
    //! current step which indicates the active values in the arrays below
    unsigned short m_step = 0;

    // flag to choose solver, 0=Roe, 1=F_wave
    tsunami_lab::t_idx m_solverId = 0;

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

    // ! ghost cell updating condition (0 = outflow, 1 = reflecting)
    t_idx m_ghost = 0;
    
    public:
    /**
     * Constructs the 1d wave propagation solver.
     *
     * @param i_xCells number of cells.
     * @param i_yCells number of cells.
     * @param i_solver_id flag to choose solver, 0=Roe, 1=F_wave.
     **/
    WavePropagation2d( t_idx i_xCells, t_idx i_yCells, tsunami_lab::t_idx i_solverId );

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
     * Gets the stride in y-direction. x-direction is stride-1.
     *
     * @return stride in y-direction.
     **/
    t_idx getStride(){
      return m_xCells+2;
    }

    /**
     * Gets cells' water heights.
     *
     * @return water heights.
     */
    t_real const * getHeight(){
      return m_h[m_step]+1;
    }

    /**
     * Gets the cells' momenta in x-direction.
     *
     * @return momenta in x-direction.
     **/
    t_real const * getMomentumX(){
      return m_hu[m_step]+1;
    }

    /**
     * Dummy function which returns a nullptr.
     **/
    t_real const * getMomentumY(){
      return m_hv[m_step]+1;
    }

    /**
     * Gets the cells' bathymetry.
     * 
     * @return bathymetry
     **/
    t_real const * getBathymetry() {
      return m_bathymetry+1;
    }

    /**
     * Sets the height of the cell to the given value.
     *
     * @param i_ix id of the cell in x-direction.
     * @param i_h water height.
     **/
    void setHeight( t_idx  i_ix,
                    t_idx  i_iy,
                    t_real i_h ) {
      m_h[m_step][i_ix + 1] = i_h;
    }

    /**
     * Sets the momentum in x-direction to the given value.
     *
     * @param i_ix id of the cell in x-direction.
     * @param i_iy id of the cell in x-direction.
     * @param i_hu momentum in x-direction.
     **/
    void setMomentumX( t_idx  i_ix,
                       t_idx  i_iy,
                       t_real i_hu ) {
      m_hu[m_step][i_ix+1] = i_hu;
    }

    /**
     * Sets the momentum in y-direction to the given value.
     *
     * @param i_ix id of the cell in x-direction.
     * @param i_iy id of the cell in x-direction.
     * @param i_hv momentum in y-direction.
     **/
    void setMomentumY( t_idx  i_ix,
                       t_idx  i_iy,
                       t_real i_hv) {};
    
    /**
     * Sets the cells' bathymetry.
     *
     * @param i_ix id of the cell in x-direction.
     * 
     * 
     * @return bathymetry
     **/
    void setBathymetry( t_idx i_ix, 
                        t_idx, 
                        t_real i_height);

};

#endif