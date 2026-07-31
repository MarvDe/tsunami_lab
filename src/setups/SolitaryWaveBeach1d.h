/**
 * @file
 * @brief One-dimensional solitary-wave run-up on a plane beach.
 **/
#ifndef TSUNAMI_LAB_SETUPS_SOLITARY_WAVE_BEACH_1D_H
#define TSUNAMI_LAB_SETUPS_SOLITARY_WAVE_BEACH_1D_H

#include "../io/SetupConfig.h"
#include "Setup.h"

namespace tsunami_lab {
  namespace setups {
    class SolitaryWaveBeach1d;
  }
}

/**
 * solitary wave on a plane beach.
 **/
class tsunami_lab::setups::SolitaryWaveBeach1d: public Setup {
  private:
    t_real m_depth = 1;
    t_real m_waveHeight = 0.019f;
    t_real m_slopeInv = 19.85f;
    t_real m_shorelineX = 50;
    t_real m_waveCenterX = 95;
    t_real m_velocitySign = -1;

    /**
     * Computes the solitary-wave surface elevation.
     *
     * @param i_x x-coordinate of the queried point.
     * @return surface elevation at the given point.
     */
    t_real getSurfaceElevation( t_real i_x ) const;

  public:
    /**
     * Constructs a solitary wave on a plane beach.
     *
     * @param i_depth offshore still-water depth.
     * @param i_waveHeight solitary-wave height.
     * @param i_slopeInv inverse beach slope.
     * @param i_shorelineX x-coordinate of the shoreline.
     * @param i_waveCenterX initial x-coordinate of the wave center.
     * @param i_velocitySign propagation direction, normally -1 or 1.
     */
    SolitaryWaveBeach1d( t_real i_depth,
                         t_real i_waveHeight,
                         t_real i_slopeInv,
                         t_real i_shorelineX,
                         t_real i_waveCenterX,
                         t_real i_velocitySign );

    /**
     * Gets the YAML argument definition of the setup.
     *
     * @return setup argument definition.
     */
    static tsunami_lab::io::SetupDef getSetupDef();

    /**
     * Gets the water height at a given point.
     *
     * @param i_x x-coordinate of the queried point.
     * @return water height at the given point.
     */
    t_real getHeight( t_real i_x,
                      t_real      ) const;

    /**
     * Gets the momentum in x-direction.
     *
     * @param i_x x-coordinate of the queried point.
     * @return momentum in x-direction.
     */
    t_real getMomentumX( t_real i_x,
                         t_real      ) const;

    /**
     * Gets the momentum in y-direction, which is zero initially.
     *
     * @return zero.
     */
    t_real getMomentumY( t_real,
                         t_real ) const;

    /**
     * Gets the bathymetry at a given point.
     *
     * @param i_x x-coordinate of the queried point.
     * @return bathymetry at the given point.
     */
    t_real getBathymetry( t_real i_x,
                          t_real      ) const;
};

#endif
