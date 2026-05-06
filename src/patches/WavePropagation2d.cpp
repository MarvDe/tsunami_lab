/**
 * @author Alexander Breuer (alex.breuer AT uni-jena.de)
 *
 * @section DESCRIPTION
 * One-dimensional wave propagation patch.
 **/
#include "WavePropagation2d.h"
#include "../solvers/Roe.h"
#include "../solvers/F_wave.h"

tsunami_lab::patches::WavePropagation2d::WavePropagation2d( t_idx i_xCells, t_idx i_yCells, tsunami_lab::t_idx i_solverId ): m_solverId(i_solverId) {
  m_xCells = i_xCells;
  m_yCells = i_yCells;

  // allocate memory including a single ghost cell on each side
  for( unsigned short l_st = 0; l_st < 2; l_st++ ) {
    m_h[l_st] = new t_real[  (m_xCells + 2) * (m_yCells + 2) ];
    m_hu[l_st] = new t_real[ (m_xCells + 2) * (m_yCells + 2) ];
    m_hv[l_st] = new t_real[ (m_xCells + 2) * (m_yCells + 2) ];
  }
  m_bathymetry = new t_real[ (m_xCells + 2) * (m_yCells + 2) ];

  // init to zero
  for( unsigned short l_st = 0; l_st < 2; l_st++ ) {
    for( t_idx l_cey = 0; l_cey < m_yCells + 2; l_cey++ ) {
      for ( t_idx l_cex = 0; l_cex < m_xCells + 2; l_cex++ ){
        m_h[l_st][l_cex + l_cey * getStride()] = 0;
        m_hu[l_st][l_cex + l_cey * getStride()] = 0;
        m_hv[l_st][l_cex + l_cey * getStride()] = 0;
      }
    }
  }
  // init bathymetry to 0
  for( t_idx l_cey = 0; l_cey < m_yCells + 2; l_cey++ ) {
    for ( t_idx l_cex = 0; l_cex < m_xCells + 2; l_cex++ ){
      m_bathymetry[l_cex + l_cey * getStride()] = 0;
    }
  }

}

tsunami_lab::patches::WavePropagation2d::~WavePropagation2d() {
  delete[] m_bathymetry;
  for( unsigned short l_st = 0; l_st < 2; l_st++ ) {
    delete[] m_h[l_st];
    delete[] m_hu[l_st];
    delete[] m_hv[l_st];
  }
}

void tsunami_lab::patches::WavePropagation2d::timeStep( t_real i_scaling ) {
  // pointers to old and new data
  t_real * l_hOld = m_h[m_step];
  t_real * l_huOld = m_hu[m_step];
  t_real * l_hvOld = m_hv[m_step];

  m_step = (m_step+1) % 2;
  t_real * l_hNew =  m_h[m_step];
  t_real * l_huNew = m_hu[m_step];
  t_real * l_hvNew = m_hv[m_step];

  // init new cell quantities
  for( t_idx l_cey = 0; l_cey < m_yCells + 2; l_cey++ ) {
    for ( t_idx l_cex = 0; l_cex < m_xCells + 2; l_cex++ ){
      l_hNew[l_cex + l_cey * getStride()] = l_hOld[l_cex + l_cey * getStride()];
      l_huNew[l_cex + l_cey * getStride()] = l_huOld[l_cex + l_cey * getStride()];
      l_hvNew[l_cex + l_cey * getStride()] = l_hvOld[l_cex + l_cey * getStride()];
    }
  }
  
  // iterate over edges and update with Riemann solutions
  // X
  for (t_idx l_yed = 0; l_yed < m_yCells+2; l_yed++){
    for( t_idx l_xed = 0; l_xed < m_xCells+1; l_xed++ ) {
      // determine left and right cell-id
      t_idx l_ceL = l_xed + getStride() * l_yed;
      t_idx l_ceR = l_xed + 1 + getStride() * l_yed;
  
      // extract cell data
      t_real l_hL = l_hOld[l_ceL];
      t_real l_hR = l_hOld[l_ceR];
      t_real l_huL = l_huOld[l_ceL];
      t_real l_huR = l_huOld[l_ceR];
      t_real l_bL = m_bathymetry[l_ceL];
      t_real l_bR = m_bathymetry[l_ceR];
      
      // compute net-updates
      t_real l_netUpdatesX[2][2];

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
                                  l_netUpdatesX[0],
                                  l_netUpdatesX[1] );
      }
      // select fwave solver
      else{
        solvers::Fwave::netUpdates( l_hL,
                                    l_hR,
                                    l_huL,
                                    l_huR,
                                    l_bL,
                                    l_bR,
                                    l_netUpdatesX[0],
                                    l_netUpdatesX[1] );
      }
  
      // update the cells' quantities
      if (!l_dryL){
        l_hNew[l_ceL]  -= i_scaling * l_netUpdatesX[0][0];
        if (l_hNew[l_ceL] <= 1e-6f ){
          l_hNew[l_ceL] = 1e-6f;
          l_huNew[l_ceL] = 0;
        }
        l_huNew[l_ceL] -= i_scaling * l_netUpdatesX[0][1];
      }
      if (!l_dryR){
        l_hNew[l_ceR]  -= i_scaling * l_netUpdatesX[1][0];
        if (l_hNew[l_ceR] <= 1e-6f ){
          l_hNew[l_ceR] = 1e-6f;
          l_huNew[l_ceR] = 0;
        }
        l_huNew[l_ceR] -= i_scaling * l_netUpdatesX[1][1];
      }
    }
  }
  // Y
  for (t_idx l_yed = 0; l_yed < m_yCells+1; l_yed++){
    for( t_idx l_xed = 0; l_xed < m_xCells+2; l_xed++ ) {
      // determine left and right cell-id

      t_idx l_ceU = l_xed + getStride() * l_yed;
      t_idx l_ceB = l_xed + getStride() * (l_yed + 1);
  
      // extract cell data
      t_real l_hU = l_hOld[l_ceU];
      t_real l_hB = l_hOld[l_ceB];
      t_real l_hvU = l_hvOld[l_ceU];
      t_real l_hvB = l_hvOld[l_ceB];
      t_real l_bU = m_bathymetry[l_ceU];
      t_real l_bB = m_bathymetry[l_ceB];
      
      // compute net-updates
      t_real l_netUpdatesY[2][2];
      
      // check for dry cells
      bool l_dryU = false, l_dryB = false;

      if (l_hU <= 1e-6f && l_hB <= 1e-6f) { // both cells dry
        // skip evaluation
        continue;
      }
      else if (l_hU <= 1e-6f){               // left cell dry
        // set reflecting boundary conditions left
        l_dryU = true;
        l_hU = l_hB;
        l_hvU = -l_hvB;
        l_bU  = l_bB;
      }
      else if (l_hB <= 1e-6f){      // right cell dry
        // set reflecting boundary conditions right
        l_dryB = true;
        l_hB = l_hU;
        l_hvB = -l_hvU;
        l_bB  = l_bU;
      }
      // select roe solver 
      if (m_solverId == tsunami_lab::solvers::ROE){
        solvers::Roe::netUpdates( l_hU,
                                  l_hB,
                                  l_hvU,
                                  l_hvB,
                                  l_netUpdatesY[0],
                                  l_netUpdatesY[1] );
      
      }
      // select fwave solver
      else{
        solvers::Fwave::netUpdates( l_hU,
                                    l_hB,
                                    l_hvU,
                                    l_hvB,
                                    l_bU,
                                    l_bB,
                                    l_netUpdatesY[0],
                                    l_netUpdatesY[1] );
      }
  
      // update the cells' quantities
      if (!l_dryU){
        l_hNew[l_ceU]  -= i_scaling * l_netUpdatesY[0][0];
        if (l_hNew[l_ceU] <= 1e-6f ){
          l_hNew[l_ceU] = 1e-6f;
          l_huNew[l_ceU] = 0;
        }
        l_hvNew[l_ceU] -= i_scaling * l_netUpdatesY[0][1];
      }
      if (!l_dryB){
        l_hNew[l_ceB]  -= i_scaling * l_netUpdatesY[1][0];
        if (l_hNew[l_ceB] <= 1e-6f ){
          l_hNew[l_ceB] = 1e-6f;
          l_huNew[l_ceB] = 0;
        }
        l_hvNew[l_ceB] -= i_scaling * l_netUpdatesY[1][1];
      }
    }
  }
}

void tsunami_lab::patches::WavePropagation2d::setGhostOutflow() {
  t_real * l_h = m_h[m_step];
  t_real * l_hu = m_hu[m_step];
  t_real * l_hv = m_hv[m_step];

  if (m_ghost == 1){
    for (t_idx l_i = 1; l_i < m_xCells+1; l_i++){
      l_h[l_i] = l_h[l_i + getStride()];
      l_hv[l_i] = -l_hv[l_i + getStride()];

      l_h[l_i + getStride() * (m_yCells + 1)] = l_h[l_i + getStride() * m_yCells];
      l_hv[l_i + getStride() * (m_yCells + 1)] = -l_hv[l_i + getStride() * m_yCells];
    }
    for (t_idx l_i = 1; l_i < m_yCells+1; l_i++){
      l_h[l_i * getStride()] = l_h[l_i * getStride() + 1];
      l_hu[l_i * getStride()] = -l_hu[l_i * getStride() + 1];

      l_h[l_i * getStride() + getStride() - 1] = l_h[l_i * getStride() + getStride() - 2];
      l_hu[l_i * getStride() + getStride() - 1] = -l_hu[l_i * getStride() + getStride() - 2];
    }

  }
  else{
    for (t_idx l_i = 1; l_i < m_xCells+1; l_i++){
      l_h[l_i] = l_h[l_i + getStride()];
      l_hv[l_i] = l_hv[l_i + getStride()];

      l_h[l_i + getStride() * (m_yCells + 1)] = l_h[l_i + getStride() * m_yCells];
      l_hv[l_i + getStride() * (m_yCells + 1)] = l_hv[l_i + getStride() * m_yCells];
    }
    for (t_idx l_i = 1; l_i < m_yCells+1; l_i++){
      l_h[l_i * getStride()] = l_h[l_i * getStride() + 1];
      l_hu[l_i * getStride()] = l_hu[l_i * getStride() + 1];

      l_h[l_i * getStride() + getStride() - 1] = l_h[l_i * getStride() + getStride() - 2];
      l_hu[l_i * getStride() + getStride() - 1] = l_hu[l_i * getStride() + getStride() - 2];
    }
  }


}

void tsunami_lab::patches::WavePropagation2d::setBathymetry( t_idx i_ix, 
                                                             t_idx i_iy, 
                                                             t_real i_height){ 
  m_bathymetry[i_ix + 1 + (i_iy + 1) * getStride()] = i_height;
  
  if (i_ix == 0){
    m_bathymetry[i_ix + (i_iy + 1) * getStride()]  = i_height;
  }
  if (i_ix == m_xCells - 1){
    m_bathymetry[i_ix + 2 + (i_iy + 1) * getStride()] = i_height;
  }
  if (i_iy == 0){
    m_bathymetry[i_ix + 1 + i_iy * getStride()]  = i_height;
  }
  if (i_iy == m_yCells - 1){
    m_bathymetry[i_ix + 1 + (i_iy + 2) * getStride()] = i_height;
  }
}
