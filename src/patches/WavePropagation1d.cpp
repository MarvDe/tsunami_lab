/**
 * @author Alexander Breuer (alex.breuer AT uni-jena.de)
 *
 * @section DESCRIPTION
 * One-dimensional wave propagation patch.
 **/
#include "WavePropagation1d.h"
#include "../solvers/Roe.h"
#include "../solvers/F_wave.h"
#include "../solvers/hlle.h"
#include <cmath>
#include <iostream>

tsunami_lab::patches::WavePropagation1d::WavePropagation1d( t_idx i_nCells, tsunami_lab::t_idx i_solverId ): m_solverId(i_solverId) {
  m_nCells = i_nCells;

  // allocate memory including a single ghost cell on each side
  for( unsigned short l_st = 0; l_st < 2; l_st++ ) {
    m_h[l_st] = new t_real[  m_nCells + 2 ];
    m_hu[l_st] = new t_real[ m_nCells + 2 ];
  }
  m_bathymetry = new t_real[ m_nCells + 2 ];

  // init to zero
  for( unsigned short l_st = 0; l_st < 2; l_st++ ) {
    for( t_idx l_ce = 0; l_ce < m_nCells; l_ce++ ) {
      m_h[l_st][l_ce] = 0;
      m_hu[l_st][l_ce] = 0;
    }

  }
  // init bathymetry to 0
  for ( t_idx l_ce = 0; l_ce < m_nCells + 2; l_ce++ ) {
    m_bathymetry[l_ce] = 0;
  }
}

tsunami_lab::patches::WavePropagation1d::WavePropagation1d( t_idx i_nCells, tsunami_lab::t_idx i_solverId, tsunami_lab::t_idx i_ghostL, tsunami_lab::t_idx i_ghostR ): m_solverId(i_solverId), m_ghostL(i_ghostL), m_ghostR(i_ghostR) {
  m_nCells = i_nCells;

  // allocate memory including a single ghost cell on each side
  for( unsigned short l_st = 0; l_st < 2; l_st++ ) {
    m_h[l_st] = new t_real[  m_nCells + 2 ];
    m_hu[l_st] = new t_real[ m_nCells + 2 ];
  }
  m_bathymetry = new t_real[ m_nCells + 2 ];

  // init to zero
  for( unsigned short l_st = 0; l_st < 2; l_st++ ) {
    for( t_idx l_ce = 0; l_ce < m_nCells; l_ce++ ) {
      m_h[l_st][l_ce] = 0;
      m_hu[l_st][l_ce] = 0;

    }

  }
  // init bathymetry to 0
  for ( t_idx l_ce = 0; l_ce < m_nCells + 2; l_ce++ ) {
    m_bathymetry[l_ce] = 0;
  }
}



tsunami_lab::patches::WavePropagation1d::~WavePropagation1d() {
  delete[] m_bathymetry;
  for( unsigned short l_st = 0; l_st < 2; l_st++ ) {
    delete[] m_h[l_st];
    delete[] m_hu[l_st];
  }
}

void tsunami_lab::patches::WavePropagation1d::timeStep( t_real i_scaling ) {
  // pointers to old and new data
  t_real * l_hOld = m_h[m_step];
  t_real * l_huOld = m_hu[m_step];

  m_step = (m_step+1) % 2;
  t_real * l_hNew =  m_h[m_step];
  t_real * l_huNew = m_hu[m_step];

  // init new cell quantities
  for( t_idx l_ce = 1; l_ce < m_nCells+1; l_ce++ ) {
    l_hNew[l_ce] = l_hOld[l_ce];
    l_huNew[l_ce] = l_huOld[l_ce];
  }

  // iterate over edges and update with Riemann solutions
  for( t_idx l_ed = 0; l_ed < m_nCells+1; l_ed++ ) {
    // determine left and right cell-id
    t_idx l_ceL = l_ed;
    t_idx l_ceR = l_ed+1;

    // extract cell data
    t_real l_hL = l_hOld[l_ceL];
    t_real l_hR = l_hOld[l_ceR];
    t_real l_huL = l_huOld[l_ceL];
    t_real l_huR = l_huOld[l_ceR];
    t_real l_bL = m_bathymetry[l_ceL];
    t_real l_bR = m_bathymetry[l_ceR];
    
    // compute net-updates
    t_real l_netUpdates[2][2];

    bool l_dryL = false, l_dryR = false;
    if (m_solverId != tsunami_lab::solvers::FWAVE_HYDROSTATIC_RECONSTRUCTION){
      // check for dry cells
      if (l_hL <= 1e-6f && l_hR <= 1e-6f) { // both cells dry
        // skip evaluation
        continue;
      }
      else if (l_hL <= 1e-6f){               // left cell dry
        // set reflecting boundary conditions left
        l_dryL = true;
        l_hL = l_hR;
        l_huL = -l_huR;
        l_bL  = l_bR;
      }
      else if (l_hR <= 1e-6f){      // right cell dry
        // set reflecting boundary conditions right
        l_dryR = true;
        l_hR = l_hL;
        l_huR = -l_huL;
        l_bR  = l_bL;
      }
    }

 
    // select roe solver 
    if (m_solverId == tsunami_lab::solvers::ROE){
      solvers::Roe::netUpdates( l_hL,
                                l_hR,
                                l_huL,
                                l_huR,
                                l_netUpdates[0],
                                l_netUpdates[1] );
    
    }
    // select fwave solver
    else if (m_solverId == tsunami_lab::solvers::FWAVE){
      solvers::Fwave::netUpdates( l_hL,
                                  l_hR,
                                  l_huL,
                                  l_huR,
                                  l_bL,
                                  l_bR,
                                  l_netUpdates[0],
                                  l_netUpdates[1] );
    }
    // Hydrostatic reconstruction
    else{

      // 1. reconstruction first, using original cell-centre values
      t_real l_bHalf = std::max( l_bL, l_bR );
      t_real l_hL2   = std::max( t_real(0), l_hL + l_bL - l_bHalf );
      t_real l_hR2   = std::max( t_real(0), l_hR + l_bR - l_bHalf );
      t_real l_huL2  = ( l_hL > 0 ) ? l_hL2 * ( l_huL / l_hL ) : t_real(0);
      t_real l_huR2  = ( l_hR > 0 ) ? l_hR2 * ( l_huR / l_hR ) : t_real(0);

      // 2. source corrections (signs now correct)
      t_real l_sourceL = t_real(0.5) * 9.81 * ( l_hL2 * l_hL2 - l_hL * l_hL );
      t_real l_sourceR = t_real(0.5) * 9.81 * ( l_hR * l_hR - l_hR2 * l_hR2 );

      // 3. skip only if both reconstructed heights are negligible
      if( l_hL2 <= 0 && l_hR2 <= 0 ) continue;

      // 4. call solver
      tsunami_lab::solvers::Fwave::netUpdates(  l_hL2, l_hR2,
                                                l_huL2, l_huR2,
                                                t_real(0), t_real(0),
                                                l_netUpdates[0],
                                                l_netUpdates[1] );

      l_netUpdates[0][1] += l_sourceL;
      l_netUpdates[1][1] += l_sourceR;

      // if (l_ed == 50){
      //   std::cout << "hL " << l_hL << ";  hR " << l_hR << std::endl;
      //   std::cout << "hL2 " << l_hL2 << "; hR2 " << l_hR2 << std::endl;
      //   std::cout << "netUpLeft " << l_netUpdates[0][0] << "; " << l_netUpdates[0][1] << std::endl;
      //   std::cout << "netUpRight " << l_netUpdates[1][0] << "; " << l_netUpdates[1][1] << std::endl;  
      // }
    }


    // update the cells' quantities
    if (!l_dryL){
      
      l_hNew[l_ceL]  -= i_scaling * l_netUpdates[0][0];
      l_huNew[l_ceL] -= i_scaling * l_netUpdates[0][1];
      
    }
    if (!l_dryR){
      
      l_hNew[l_ceR]  -= i_scaling * l_netUpdates[1][0];
      l_huNew[l_ceR] -= i_scaling * l_netUpdates[1][1];
      
      }
      
  }

  t_real l_dt = 0.1;
  const t_real mann = 0.02;
  for (t_idx i = 1; i <= m_nCells; i++) {
      if (l_hNew[i] > 1e-6) {
          t_real vel = l_huNew[i] / l_hNew[i];
          t_real denom = 1.0 + 9.81 * l_dt * mann*mann
                        * std::abs(vel) / std::pow(l_hNew[i], 4.0/3.0);
          l_huNew[i] /= denom;   // semi-implicit: always stable
      }
  }
}

void tsunami_lab::patches::WavePropagation1d::setGhostOutflow() {
  t_real * l_h = m_h[m_step];
  t_real * l_hu = m_hu[m_step];

  // set left boundary
  if (m_ghostL == 1){
    l_h[0] = l_h[1];
    l_hu[0] = -l_hu[1];
  }
  else{
    l_h[0] = l_h[1];
    l_hu[0] = l_hu[1];
  }

  // set right boundary
  if (m_ghostR == 1){
    l_h[m_nCells+1] = l_h[m_nCells];
    l_hu[m_nCells+1] = -l_hu[m_nCells];
  }
  else{
    l_h[m_nCells+1] = l_h[m_nCells];
    l_hu[m_nCells+1] = l_hu[m_nCells];
  }  

}

void tsunami_lab::patches::WavePropagation1d::setBathymetry( t_idx i_ix, 
                                                             t_idx, 
                                                             t_real i_height){ 
  m_bathymetry[i_ix + 1] = i_height;
  if (i_ix == 0){
    m_bathymetry[0] = i_height;
  }
  if (i_ix == m_nCells - 1){
    m_bathymetry[m_nCells + 1] = i_height;
  }
}

void tsunami_lab::patches::WavePropagation1d::HydrostaticReconstruction(
    t_real i_hL,  t_real i_hR,
    t_real i_huL, t_real i_huR,
    t_real i_bL,  t_real i_bR,
    t_real o_netUpdateL[2],
    t_real o_netUpdateR[2] )
{
    // 1. upwind bed level at the interface
    t_real l_bHalf = std::max( i_bL, i_bR );

    // 2. reconstructed water heights (dry-cell clipping)
    t_real l_hL = std::max( t_real(0), i_hL + i_bL - l_bHalf );
    t_real l_hR = std::max( t_real(0), i_hR + i_bR - l_bHalf );

    // 3. reconstructed momenta (velocity stays, height changes)
    t_real l_huL = ( i_hL > 0 ) ? l_hL * ( i_huL / i_hL ) : t_real(0);
    t_real l_huR = ( i_hR > 0 ) ? l_hR * ( i_huR / i_hR ) : t_real(0);

    // 4. call your existing solver with reconstructed states
    //    bed is flat at z_half on both sides, so pass 0/0 or equal values
    tsunami_lab::solvers::Fwave::netUpdates( l_hL, l_hR,
                                             l_huL, l_huR,
                                             t_real(0), t_real(0),
                                             o_netUpdateL,
                                             o_netUpdateR );

    // 5. source term correction (distribute bathymetry jump to each side)
    t_real l_sourceL = t_real(0.5) * 9.81 * ( l_hL * l_hL - i_hL * i_hL );
    t_real l_sourceR = t_real(0.5) * 9.81 * ( i_hR * i_hR - l_hR * l_hR );

    o_netUpdateL[1] += l_sourceL;
    o_netUpdateR[1] -= l_sourceR;
}
