/**
 * @section DESCRIPTION
 * One-dimensional solitary wave runup on a plane beach.
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
 * NTHMP/Synolakis-style solitary wave on a 1:slopeInv beach.
 **/
class tsunami_lab::setups::SolitaryWaveBeach1d: public Setup {
  private:
    t_real m_depth = 1;
    t_real m_waveHeight = 0.019f;
    t_real m_slopeInv = 19.85f;
    t_real m_shorelineX = 50;
    t_real m_waveCenterX = 95;
    t_real m_velocitySign = -1;

    t_real getSurfaceElevation( t_real i_x ) const;

  public:
    SolitaryWaveBeach1d( t_real i_depth,
                         t_real i_waveHeight,
                         t_real i_slopeInv,
                         t_real i_shorelineX,
                         t_real i_waveCenterX,
                         t_real i_velocitySign );

    static tsunami_lab::io::SetupDef getSetupDef();

    t_real getHeight( t_real i_x,
                      t_real      ) const;

    t_real getMomentumX( t_real i_x,
                         t_real      ) const;

    t_real getMomentumY( t_real,
                         t_real ) const;

    t_real getBathymetry( t_real i_x,
                          t_real      ) const;
};

#endif
