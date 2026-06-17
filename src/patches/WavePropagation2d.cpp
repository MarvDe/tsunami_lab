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
  m_xCells = i_xCells;
  m_yCells = i_yCells;
  m_ghost = i_ghost;

  const t_idx l_stride = getStride();
  const t_idx n = (m_yCells + 2) * l_stride;

  // allocate memory including a single ghost cell on each side
  for( unsigned short l_st = 0; l_st < 2; l_st++ ) {
    m_h[l_st] = new t_real[  n ];
    m_hu[l_st] = new t_real[ n ];
    m_hv[l_st] = new t_real[ n ];
  }
  m_bathymetry = new t_real[ n ];

  // init to zero
  // NUMA fist touch
  #pragma omp parallel for schedule(static)
  for ( t_idx i = 0; i < n; i++ ){
    m_h[0][i] = 0;
    m_h[1][i] = 0;

    m_hu[0][i] = 0;
    m_hu[1][i] = 0;

    m_hv[0][i] = 0;
    m_hv[1][i] = 0;

    m_bathymetry[i] = 0;
  }


  // for( unsigned short l_st = 0; l_st < 2; ++l_st ) {
  //     std::fill_n(m_h[l_st],  n, t_real(0));
  //     std::fill_n(m_hu[l_st], n, t_real(0));
  //     std::fill_n(m_hv[l_st], n, t_real(0));
  // }

  // // init bathymetry to 0
  // std::fill_n(m_bathymetry, n, t_real(0));

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
  const t_idx n = (m_yCells + 2) * l_stride;

  // pointers to old and new data
  t_real * __restrict__ l_hOld = m_h[m_step];
  t_real * __restrict__ l_huOld = m_hu[m_step];
  t_real * __restrict__ l_hvOld = m_hv[m_step];

  m_step = (m_step+1) % 2;

  t_real * __restrict__ l_hNew =  m_h[m_step];
  t_real * __restrict__ l_huNew = m_hu[m_step];
  t_real * __restrict__ l_hvNew = m_hv[m_step];


  const t_real * __restrict__ l_bath = m_bathymetry;

  // const auto solver = //(m_solverId == tsunami_lab::solvers::ROE) ? tsunami_lab::solvers::Roe::netUpdates : tsunami_lab::solvers::Fwave::netUpdates; // Roe solver netupdates needs dummy input 
  //               tsunami_lab::solvers::Fwave::netUpdates;
                
  #pragma omp parallel
  {
    // init new cell quantities
    #pragma omp for schedule(static)
    for (t_idx i = 0; i < n; i++) {
      l_hNew[i] = l_hOld[i];
      l_huNew[i] = l_huOld[i];
      l_hvNew[i] = l_hvOld[i];
    }


    #pragma omp for schedule(static)
    // X
    for (t_idx l_yed = 1; l_yed < m_yCells+1; l_yed++){
      #pragma omp simd
      for( t_idx l_xed = 0; l_xed < m_xCells+1; l_xed++ ) {
        // determine left and right cell-id
        const t_idx l_ceL = l_xed + l_stride * l_yed;
        const t_idx l_ceR = l_xed + 1 + l_stride * l_yed;

        // extract cell data
        const t_real l_hL = l_hOld[l_ceL];
        const t_real l_hR = l_hOld[l_ceR];

        if (l_hL <= 0 && l_hR <= 0) continue; // langsamer wenn man das weglässt, trotz SIMD

        const t_real l_huL = l_huOld[l_ceL];
        const t_real l_bL = l_bath[l_ceL];
        const t_real l_huR = l_huOld[l_ceR];
        const t_real l_bR = l_bath[l_ceR];
        
        // compute net-updates
        
        // check for dry cells
        const bool l_isDryL = (l_hL <= 0);
        const bool l_isDryR = (l_hR <= 0);
        


        const t_real l_hL_mod = l_isDryL ? l_hR : l_hL;
        const t_real l_huL_mod = l_isDryL ? -l_huR : l_huL;
        const t_real l_bL_mod = l_isDryL ? l_bR : l_bL;
        
        const t_real l_hR_mod = l_isDryR ? l_hL : l_hR;
        const t_real l_huR_mod = l_isDryR ? -l_huL : l_huR;
        const t_real l_bR_mod = l_isDryR ? l_bL : l_bR;


        // // left cell dry
        // if (l_hL <= 0){               
        //   // set reflecting boundary conditions left
        //   l_dryL = true;
        //   l_hL = l_hR;
        //   l_huL = -l_huR;
        //   l_bL  = l_bR;
        // }
        // else if (l_hR <= 0){      // right cell dry
        //   // set reflecting boundary conditions right
        //   l_dryR = true;
        //   l_hR = l_hL;
        //   l_huR = -l_huL;
        //   l_bR  = l_bL;
        // }
        
        t_real l_netUpdatesX[2][2];
        // select fwave solver (for now, because of inlining ) 
        tsunami_lab::solvers::Fwave::netUpdates(
                                    l_hL_mod,
                                    l_hR_mod,
                                    l_huL_mod,
                                    l_huR_mod,
                                    l_bL_mod,
                                    l_bR_mod,
                                    l_netUpdatesX[0],
                                    l_netUpdatesX[1] );

                                    
        // update the cells' quantities
        const t_real l_hNewL_next = l_hNew[l_ceL] - i_scaling * l_netUpdatesX[0][0];
        const t_real l_huNewL_next = l_huNew[l_ceL] - i_scaling * l_netUpdatesX[0][1];

        const t_real l_hNewR_next = l_hNew[l_ceR] - i_scaling * l_netUpdatesX[1][0];
        const t_real l_huNewR_next = l_huNew[l_ceR] - i_scaling * l_netUpdatesX[1][1];
        

        const bool l_clampL = (l_hNewL_next <= 1e-6f);
        const bool l_clampR = (l_hNewR_next <= 1e-6f);

        l_hNew[l_ceL] = l_isDryL ? l_hNew[l_ceL] : (l_clampL ? 1e-6f : l_hNewL_next);
        l_huNew[l_ceL] = l_isDryL ? l_huNew[l_ceL] : (l_clampL ? 0.0f : l_huNewL_next);

        l_hNew[l_ceR] = l_isDryR ? l_hNew[l_ceR] : (l_clampR ? 1e-6f : l_hNewR_next);
        l_huNew[l_ceR] = l_isDryR ? l_huNew[l_ceR] : (l_clampR ? 0.0f : l_huNewR_next);


        // if (!l_dryL){
        //   l_hNew[l_ceL]  -= i_scaling * l_netUpdatesX[0][0];
        //   if (l_hNew[l_ceL] <= 1e-6f ){
        //     l_hNew[l_ceL] = 1e-6f;
        //     l_huNew[l_ceL] = 0;
        //   } else {
        //     l_huNew[l_ceL] -= i_scaling * l_netUpdatesX[0][1];
        //   }
        // }
        // if (!l_dryR){
        //   l_hNew[l_ceR]  -= i_scaling * l_netUpdatesX[1][0];
        //   if (l_hNew[l_ceR] <= 1e-6f ){
        //     l_hNew[l_ceR] = 1e-6f;
        //     l_huNew[l_ceR] = 0;
        //   } else {
        //     l_huNew[l_ceR] -= i_scaling * l_netUpdatesX[1][1];
        //   }
        // }
      }
    }

    const int tid = omp_get_thread_num();
    const int num_threads = omp_get_num_threads();

    const t_idx chunk_size = (m_xCells + num_threads - 1) / num_threads;
    const t_idx x_start =  tid * chunk_size;
    const t_idx x_end = std::min<t_idx>(x_start + chunk_size, m_xCells + 1);

    // Y
    // #pragma omp for schedule(static) nowait
    // #pragma omp simd
    for( t_idx l_yed = 0; l_yed < m_yCells+1; l_yed++ ) {
      #pragma omp simd
      for (t_idx l_xed = x_start; l_xed < x_end; l_xed++){
        // determine left and right cell-id

        const t_idx l_ceU = l_xed + l_stride * l_yed;
        const t_idx l_ceB = l_xed + l_stride * (l_yed + 1);
        // extract cell data
        const t_real l_hU = l_hOld[l_ceU];
        const t_real l_hB = l_hOld[l_ceB];
        
        if (l_hU <= 0 && l_hB <= 0) continue; // langsamer wenn man das weglässt, trotz SIMD
        
        const t_real l_hvU = l_hvOld[l_ceU];
        const t_real l_hvB = l_hvOld[l_ceB];
        const t_real l_bU = l_bath[l_ceU];
        const t_real l_bB = l_bath[l_ceB];
        
        // compute net-updates
        t_real l_netUpdatesY[2][2];
        
        // check for dry cells

        const bool l_isDryL = (l_hU <= 0);
        const bool l_isDryR = (l_hB <= 0);

        const t_real l_hU_mod = l_isDryL ? l_hB : l_hU;
        const t_real l_hvU_mod = l_isDryL ? -l_hvB : l_hvU;
        const t_real l_bU_mod = l_isDryL ? l_bB : l_bU;
        
        const t_real l_hB_mod = l_isDryR ? l_hU : l_hB;
        const t_real l_hvB_mod = l_isDryR ? -l_hvU : l_hvB;
        const t_real l_bB_mod = l_isDryR ? l_bU : l_bB;


        // bool l_dryU = false, l_dryB = false;

        // if (l_hU <= 0 && l_hB <= 0) { // both cells dry
        //   // skip evaluation
        //   continue;
        // }
        // else if (l_hU <= 0){               // left cell dry
        //   // set reflecting boundary conditions left
        //   l_dryU = true;
        //   l_hU = l_hB;
        //   l_hvU = -l_hvB;
        //   l_bU  = l_bB;

        // }
        // else if (l_hB <= 0){      // right cell dry
        //   // set reflecting boundary conditions right
        //   l_dryB = true;
        //   l_hB = l_hU;
        //   l_hvB = -l_hvU;
        //   l_bB  = l_bU;
        // }
        // select Fwave solver for now
        tsunami_lab::solvers::Fwave::netUpdates(                     
                                    l_hU_mod,
                                    l_hB_mod,
                                    l_hvU_mod,
                                    l_hvB_mod,
                                    l_bU_mod,
                                    l_bB_mod,
                                    l_netUpdatesY[0],
                                    l_netUpdatesY[1] );
          



        // update the cells' quantities
        const t_real l_hNewU_next = l_hNew[l_ceU] - i_scaling * l_netUpdatesY[0][0];
        const t_real l_hvNewU_next = l_hvNew[l_ceU] - i_scaling * l_netUpdatesY[0][1];

        const t_real l_hNewB_next = l_hNew[l_ceB] - i_scaling * l_netUpdatesY[1][0];
        const t_real l_hvNewB_next = l_hvNew[l_ceB] - i_scaling * l_netUpdatesY[1][1];
        

        const bool l_clampU = (l_hNewU_next <= 1e-6f);
        const bool l_clampB = (l_hNewB_next <= 1e-6f);

        l_hNew[l_ceU] = l_isDryL ? l_hNew[l_ceU] : (l_clampU ? 1e-6f : l_hNewU_next);
        l_hvNew[l_ceU] = l_isDryL ? l_hvNew[l_ceU] : (l_clampU ? 0.0f : l_hvNewU_next);

        l_hNew[l_ceB] = l_isDryR ? l_hNew[l_ceB] : (l_clampB ? 1e-6f : l_hNewB_next);
        l_hvNew[l_ceB] = l_isDryR ? l_hvNew[l_ceB] : (l_clampB ? 0.0f : l_hvNewB_next);


        // // update the cells' quantities
        // if (!l_dryU){
        //   l_hNew[l_ceU]  -= i_scaling * l_netUpdatesY[0][0];
        //   if (l_hNew[l_ceU] <= 1e-6f ){
        //     l_hNew[l_ceU] = 1e-6f;
        //     l_hvNew[l_ceU] = 0;
        //   } else {
        //     l_hvNew[l_ceU] -= i_scaling * l_netUpdatesY[0][1];
        //   }
        // }
        // if (!l_dryB){
        //   l_hNew[l_ceB]  -= i_scaling * l_netUpdatesY[1][0];
        //   if (l_hNew[l_ceB] <= 1e-6f ){
        //     l_hNew[l_ceB] = 1e-6f;
        //     l_hvNew[l_ceB] = 0;
        //   } else {
        //     l_hvNew[l_ceB] -= i_scaling * l_netUpdatesY[1][1];
        //   }
        // }
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
