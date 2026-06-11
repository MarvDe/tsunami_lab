/**
 * @author Alexander Breuer (alex.breuer AT uni-jena.de)
 *
 * @section DESCRIPTION
 * Two-dimensional wave propagation patch.
 **/
#include "WavePropagation2d.h"
#include "../solvers/Roe.h"
#include "../solvers/F_wave.h"
#include <iostream>
#include <algorithm>
#include <omp.h>

tsunami_lab::patches::WavePropagation2d::WavePropagation2d( t_idx i_xCells, t_idx i_yCells, tsunami_lab::t_idx i_solverId, tsunami_lab::t_idx i_ghost ): m_solverId(i_solverId) {
  const t_idx l_stride = getStride();
  m_xCells = i_xCells;
  m_yCells = i_yCells;
  m_ghost = i_ghost;

  // allocate memory including a single ghost cell on each side
  for( unsigned short l_st = 0; l_st < 2; l_st++ ) {
    m_h[l_st] = new t_real[  (m_xCells + 2) * (m_yCells + 2) ];
    m_hu[l_st] = new t_real[ (m_xCells + 2) * (m_yCells + 2) ];
    m_hv[l_st] = new t_real[ (m_xCells + 2) * (m_yCells + 2) ];
  }
  m_bathymetry = new t_real[ (m_xCells + 2) * (m_yCells + 2) ];

  // init to zero
  const t_idx n = (m_yCells + 2) * l_stride;
  for( unsigned short l_st = 0; l_st < 2; ++l_st ) {
      std::fill_n(m_h[l_st],  n, t_real(0));
      std::fill_n(m_hu[l_st], n, t_real(0));
      std::fill_n(m_hv[l_st], n, t_real(0));
  }

  // init bathymetry to 0
  std::fill_n(m_bathymetry, n, t_real(0));

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
  const t_idx l_stride = getStride();
  // pointers to old and new data
  t_real * l_hOld = m_h[m_step];
  t_real * l_huOld = m_hu[m_step];
  t_real * l_hvOld = m_hv[m_step];

  m_step = (m_step+1) % 2;
  t_real * l_hNew =  m_h[m_step];
  t_real * l_huNew = m_hu[m_step];
  t_real * l_hvNew = m_hv[m_step];

  // init new cell quantities
  const t_idx n = (m_yCells + 2) * l_stride;
  std::copy_n(l_hOld, n, l_hNew);
  std::copy_n(l_huOld, n, l_huNew);
  std::copy_n(l_hvOld, n, l_hvNew);

  
  // iterate over edges and update with Riemann solutions
  // X
  if (m_solverId == tsunami_lab::solvers::ROE){
    
    for (t_idx l_yed = 1; l_yed < m_yCells+1; l_yed++){
      for( t_idx l_xed = 0; l_xed < m_xCells+1; l_xed++ ) {
        // determine left and right cell-id
        t_idx l_ceL = l_xed + l_stride * l_yed;
        t_idx l_ceR = l_xed + 1 + l_stride * l_yed;
    
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
        if (l_hL <= 0 && l_hR <= 0) { // both cells dry
          // skip evaluation
          continue;
        }
        else if (l_hL <= 0){               // left cell dry
          // set reflecting boundary conditions left
          l_dryL = true;
          l_hL = l_hR;
          l_huL = -l_huR;
          l_bL  = l_bR;
        }
        else if (l_hR <= 0){      // right cell dry
          // set reflecting boundary conditions right
          l_dryR = true;
          l_hR = l_hL;
          l_huR = -l_huL;
          l_bR  = l_bL;
        }
        
        solvers::Roe::netUpdates( l_hL,
                                  l_hR,
                                  l_huL,
                                  l_huR,
                                  l_netUpdatesX[0],
                                  l_netUpdatesX[1] );
    
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
      for( t_idx l_xed = 1; l_xed < m_xCells+1; l_xed++ ) {
        // determine left and right cell-id
  
        t_idx l_ceU = l_xed + l_stride * l_yed;
        t_idx l_ceB = l_xed + l_stride * (l_yed + 1);
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
  
        if (l_hU <= 0 && l_hB <= 0) { // both cells dry
          // skip evaluation
          continue;
        }
        else if (l_hU <= 0){               // left cell dry
          // set reflecting boundary conditions left
          l_dryU = true;
          l_hU = l_hB;
          l_hvU = -l_hvB;
          l_bU  = l_bB;
  
        }
        else if (l_hB <= 0){      // right cell dry
          // set reflecting boundary conditions right
          l_dryB = true;
          l_hB = l_hU;
          l_hvB = -l_hvU;
          l_bB  = l_bU;
        } 
        solvers::Roe::netUpdates( l_hU,
                                  l_hB,
                                  l_hvU,
                                  l_hvB,
                                  l_netUpdatesY[0],
                                  l_netUpdatesY[1] );
    
        // update the cells' quantities
        if (!l_dryU){
          l_hNew[l_ceU]  -= i_scaling * l_netUpdatesY[0][0];
          if (l_hNew[l_ceU] <= 1e-6f ){
            l_hNew[l_ceU] = 1e-6f;
            l_hvNew[l_ceU] = 0;
          }
          l_hvNew[l_ceU] -= i_scaling * l_netUpdatesY[0][1];
        }
        if (!l_dryB){
          l_hNew[l_ceB]  -= i_scaling * l_netUpdatesY[1][0];
          if (l_hNew[l_ceB] <= 1e-6f ){
            l_hNew[l_ceB] = 1e-6f;
            l_hvNew[l_ceB] = 0;
          }
          l_hvNew[l_ceB] -= i_scaling * l_netUpdatesY[1][1];
        }
      }
    }
      
  } else {
    #pragma omp parallel for collapse(2)
    for (t_idx l_yed = 1; l_yed < m_yCells+1; l_yed++){
      for( t_idx l_xed = 0; l_xed < m_xCells+1; l_xed++ ) {
        // determine left and right cell-id
        t_idx l_ceL = l_xed + l_stride * l_yed;
        t_idx l_ceR = l_xed + 1 + l_stride * l_yed;
    
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
        if (l_hL <= 0 && l_hR <= 0) { // both cells dry
          // skip evaluation
          continue;
        }
        else if (l_hL <= 0){               // left cell dry
          // set reflecting boundary conditions left
          l_dryL = true;
          l_hL = l_hR;
          l_huL = -l_huR;
          l_bL  = l_bR;
        }
        else if (l_hR <= 0){      // right cell dry
          // set reflecting boundary conditions right
          l_dryR = true;
          l_hR = l_hL;
          l_huR = -l_huL;
          l_bR  = l_bL;
        }
        
        // select fwave solver
        solvers::Fwave::netUpdates( l_hL,
                                    l_hR,
                                    l_huL,
                                    l_huR,
                                    l_bL,
                                    l_bR,
                                    l_netUpdatesX[0],
                                    l_netUpdatesX[1] );
    
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
      for( t_idx l_xed = 1; l_xed < m_xCells+1; l_xed++ ) {
        // determine left and right cell-id
  
        t_idx l_ceU = l_xed + l_stride * l_yed;
        t_idx l_ceB = l_xed + l_stride * (l_yed + 1);
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
  
        if (l_hU <= 0 && l_hB <= 0) { // both cells dry
          // skip evaluation
          continue;
        }
        else if (l_hU <= 0){               // left cell dry
          // set reflecting boundary conditions left
          l_dryU = true;
          l_hU = l_hB;
          l_hvU = -l_hvB;
          l_bU  = l_bB;
  
        }
        else if (l_hB <= 0){      // right cell dry
          // set reflecting boundary conditions right
          l_dryB = true;
          l_hB = l_hU;
          l_hvB = -l_hvU;
          l_bB  = l_bU;
        }
        // select Fwave solver 
        solvers::Fwave::netUpdates( l_hU,
                                    l_hB,
                                    l_hvU,
                                    l_hvB,
                                    l_bU,
                                    l_bB,
                                    l_netUpdatesY[0],
                                    l_netUpdatesY[1] );
    
        // update the cells' quantities
        if (!l_dryU){
          l_hNew[l_ceU]  -= i_scaling * l_netUpdatesY[0][0];
          if (l_hNew[l_ceU] <= 1e-6f ){
            l_hNew[l_ceU] = 1e-6f;
            l_hvNew[l_ceU] = 0;
          }
          l_hvNew[l_ceU] -= i_scaling * l_netUpdatesY[0][1];
        }
        if (!l_dryB){
          l_hNew[l_ceB]  -= i_scaling * l_netUpdatesY[1][0];
          if (l_hNew[l_ceB] <= 1e-6f ){
            l_hNew[l_ceB] = 1e-6f;
            l_hvNew[l_ceB] = 0;
          }
          l_hvNew[l_ceB] -= i_scaling * l_netUpdatesY[1][1];
        }
      }
    }
  }
}

void tsunami_lab::patches::WavePropagation2d::setGhostOutflow() {
  const t_idx l_stride = getStride();
  t_real * __restrict__ l_h = m_h[m_step];
  t_real * __restrict__ l_hu = m_hu[m_step];
  t_real * __restrict__ l_hv = m_hv[m_step];

  const bool l_reflect = (m_ghost == 1);

  t_real* l_h_bot = l_h + 1;
  t_real* l_h_bot1 = l_h + l_stride + 1;
  t_real* l_hv_bot = l_hv + 1;
  t_real* l_hv_bot1 = l_hv + l_stride + 1;

  t_real* l_h_top = l_h + l_stride * m_yCells + 1;
  t_real* l_h_top1 = l_h + l_stride * (m_yCells + 1) + 1;
  t_real* l_hv_top = l_hv + l_stride * m_yCells + 1;
  t_real* l_hv_top1 = l_hv + l_stride* (m_yCells + 1) + 1;

  std::copy_n(l_h_bot1, m_xCells, l_h_bot);
  std::copy_n(l_h_top, m_xCells, l_h_top1);

  if (l_reflect) {
    std::transform(l_hv_bot1, l_hv_bot1 + m_xCells, l_hv_bot, std::negate<t_real>{});
    std::transform(l_hv_top, l_hv_top + m_xCells, l_hv_top1, std::negate<t_real>{});
  } else {
    std::copy_n(l_hv_bot1, m_xCells, l_hv_bot);
    std::copy_n(l_hv_top, m_xCells, l_hv_top1);
  }

  const t_real l_sign = l_reflect ? t_real(-1) : t_real(1);

  #if defined(__clang__)
    __builtin_assume(l_stride >= 2);
  #elif defined(__GNUG__) 
    #if __GNUG__ >= 13
    __attribute__((__assume__(l_stride >= 2)));
    #endif
  #endif

  t_real* __restrict__ const l_h_lw  = l_h  + l_stride;
  t_real* __restrict__ const l_h_lr  = l_h  + l_stride + 1;
  t_real* __restrict__ const l_hu_lw = l_hu + l_stride;
  t_real* __restrict__ const l_hu_lr = l_hu + l_stride + 1;
  #ifdef __clang__
    #pragma clang loop vectorize(disable)
  #endif
  for (t_idx l_i = 0; l_i < m_yCells; ++l_i) {
    const t_idx l_off = l_i * l_stride;
    l_h_lw [l_off] = l_h_lr [l_off];
    l_hu_lw[l_off] = l_sign * l_hu_lr[l_off];
  }

  t_real* __restrict__ const l_h_rw  = l_h  + 2*l_stride - 1;
  t_real* __restrict__ const l_h_rr  = l_h  + 2*l_stride - 2;
  t_real* __restrict__ const l_hu_rw = l_hu + 2*l_stride - 1;
  t_real* __restrict__ const l_hu_rr = l_hu + 2*l_stride - 2;
  #ifdef __clang__
    #pragma clang loop vectorize(assume_safety)
  #endif
  for (t_idx l_i = 0; l_i < m_yCells; ++l_i) {
    const t_idx l_off = l_i * l_stride;
    l_h_rw [l_off] = l_h_rr [l_off];
    l_hu_rw[l_off] = l_sign * l_hu_rr[l_off];
  }
}

void tsunami_lab::patches::WavePropagation2d::setBathymetry( t_idx i_ix, 
                                                             t_idx i_iy, 
                                                             t_real i_height){ 
  const t_idx l_stride = getStride();
  m_bathymetry[i_ix + 1 + (i_iy + 1) * l_stride] = i_height;
  // setting ghost edges
  if (i_ix == 0){
    m_bathymetry[i_ix + (i_iy + 1) * l_stride]  = i_height;
  }
  if (i_ix == m_xCells - 1){
    m_bathymetry[i_ix + 2 + (i_iy + 1) * l_stride] = i_height;
  }
  if (i_iy == 0){
    m_bathymetry[i_ix + 1 + i_iy * l_stride]  = i_height;
  }
  if (i_iy == m_yCells - 1){
    m_bathymetry[i_ix + 1 + (i_iy + 2) * l_stride] = i_height;
  }
  // setting ghost corners
  if (i_ix == 0 && i_iy == 0){
    m_bathymetry[0] = i_height;
  }
  if (i_ix == m_xCells - 1 && i_iy == 0){
    m_bathymetry[m_xCells + 1] = i_height;
  }
  if (i_ix == 0 && i_iy == m_yCells - 1){
    m_bathymetry[m_yCells * l_stride] = i_height;
  }
  if (i_ix == m_xCells - 1 && i_iy == m_yCells - 1){
    m_bathymetry[m_xCells + 1 + m_yCells * l_stride] = i_height;
  }
}
