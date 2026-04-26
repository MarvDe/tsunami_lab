/**
 * @author Alexander Breuer (alex.breuer AT uni-jena.de)
 *
 * @section DESCRIPTION
 * One-dimensional wave propagation patch.
 **/
#include "WavePropagation1d.h"
#include "../solvers/Roe.h"
#include "../solvers/F_wave.h"

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
    
    // check for dry cells
    bool l_dryL = false, l_dryR = false;
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
    else{
      solvers::Fwave::netUpdates( l_hL,
                                  l_hR,
                                  l_huL,
                                  l_huR,
                                  l_bL,
                                  l_bR,
                                  l_netUpdates[0],
                                  l_netUpdates[1] );
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
