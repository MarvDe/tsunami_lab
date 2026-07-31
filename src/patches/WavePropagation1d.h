/**
 * @author Alexander Breuer (alex.breuer AT uni-jena.de)
 *
 * @file
 * @brief One-dimensional wave-propagation patch.
 **/
#ifndef TSUNAMI_LAB_PATCHES_WAVE_PROPAGATION_1D
#define TSUNAMI_LAB_PATCHES_WAVE_PROPAGATION_1D

#include "WavePropagation.h"

namespace tsunami_lab {
  namespace patches {
    class WavePropagation1d;
  }
}

/** Advances the one-dimensional shallow-water equations on a Cartesian grid. */
class tsunami_lab::patches::WavePropagation1d: public WavePropagation {
  private:
    //! current step which indicates the active values in the arrays below
    unsigned short m_step = 0;

    //! Selected Riemann solver.
    tsunami_lab::solvers::Ids m_solverId = tsunami_lab::solvers::FWAVE;

    //! number of cells discretizing the computational domain
    t_idx m_nCells = 0;

    //! water heights for the current and next time step for all cells
    t_real * m_h[2] = { nullptr, nullptr };

    //! momenta for the current and next time step for all cells
    t_real * m_hu[2] = { nullptr, nullptr };

    //! bathymetry data 
    t_real * m_bathymetry = nullptr;

    //! Ghost-cell boundary conditions (0 = outflow, 1 = reflecting).
    t_idx m_ghostL = 1; 
    t_idx m_ghostR = 1;

    t_real m_dt = 0.1;

    t_real m_manningFactor = 0;

    bool m_useEntropyFix;

    void HydrostaticReconstruction(    
      t_real i_hL,  t_real i_hR,
      t_real i_huL, t_real i_huR,
      t_real i_bL,  t_real i_bR,
      t_real o_netUpdateL[2],
      t_real o_netUpdateR[2]);

  public:
    /**
     * Constructs the 1d wave propagation solver.
     *
     * @param i_nCells number of cells.
     * @param i_solverId selected Riemann solver.
     * @param i_useEntropyFix whether to apply the entropy fix.
     * @param i_manningFactor Manning friction factor.
     **/
    WavePropagation1d( t_idx i_nCells, tsunami_lab::solvers::Ids i_solverId, bool i_useEntropyFix, t_real i_manningFactor = 0 );

    /**
     * Constructs the 1d wave propagation solver.
     *
     * @param i_nCells number of cells.
     * @param i_solverId selected Riemann solver.
     * @param i_ghostL type of boundary for left ghost cell.
     * @param i_ghostR type of boundary for right ghost cell.
     * @param i_useEntropyFix whether to apply the entropy fix.
     * @param i_manningFactor Manning friction factor.
     **/
    WavePropagation1d( t_idx i_nCells, tsunami_lab::solvers::Ids i_solverId, t_idx i_ghostL, t_idx i_ghostR, bool i_useEntropyFix, t_real i_manningFactor = 0 );

    /**
     * Destructor which frees all allocated memory.
     **/
    ~WavePropagation1d();

    /**
     * Performs a time step.
     *
     * @param i_scaling scaling of the time step (dt / dx).
     **/
    void timeStep( t_real i_scaling );

    /**
     * Sets the time-step size used by source-term updates.
     *
     * @param i_dt time-step size.
     **/
    void setDt( t_real i_dt ); 

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
      return m_nCells+2;
    }

    /**
     * Gets the cells' water heights.
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
     * Gets the cells' momenta in y-direction.
     *
     * @return nullptr because a one-dimensional patch has no y-momentum.
     **/
    t_real const * getMomentumY(){
      return nullptr;
    }

    /**
     * Gets the cells' bathymetry.
     * 
     * @return bathymetry.
     **/
    t_real const * getBathymetry() {
      return m_bathymetry+1;
    }

    /**
     * Sets the height of the cell to the given value.
     *
     * @param i_ix index of the cell in x-direction.
     * @param i_h water height.
     **/
    void setHeight( t_idx  i_ix,
                    t_idx,
                    t_real i_h ) {
      m_h[m_step][i_ix + 1] = i_h;
    }

    /**
     * Sets the momentum in x-direction to the given value.
     *
     * @param i_ix index of the cell in x-direction.
     * @param i_hu momentum in x-direction.
     **/
    void setMomentumX( t_idx  i_ix,
                       t_idx,
                       t_real i_hu ) {
      m_hu[m_step][i_ix+1] = i_hu;
    }

    /**
     * Ignores a y-momentum value because a one-dimensional patch has no y-momentum.
     **/
    void setMomentumY( t_idx,
                       t_idx,
                       t_real ) {};
    
    /**
     * Sets the cells' bathymetry.
     *
     * @param i_ix index of the cell in x-direction.
     * @param i_height bathymetry value.
     **/
    void setBathymetry( t_idx i_ix, 
                        t_idx, 
                        t_real i_height);
  /**
   * Check water mass
   */
    t_real getMass();
};

#endif
