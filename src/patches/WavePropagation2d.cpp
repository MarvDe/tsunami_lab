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
#include <vector>
#include <cstdlib>

tsunami_lab::patches::WavePropagation2d::WavePropagation2d( t_idx i_xCells, t_idx i_yCells, tsunami_lab::t_idx i_solverId, tsunami_lab::t_idx i_ghost ): m_solverId(i_solverId) {
  m_xCells = i_xCells;
  m_yCells = i_yCells;
  m_ghost = i_ghost;

  const t_idx l_stride = getStride();
  const t_idx n = (m_yCells + 2) * l_stride;

  const size_t alignment = 64;
  const size_t bytes = n * sizeof(t_real);

  // vielfaches von alignment
  const size_t aligned_size = ((bytes + alignment - 1) / alignment) * alignment;

  // allocate memory including a single ghost cell on each side
  for( unsigned short l_st = 0; l_st < 2; l_st++ ) {
    m_h[l_st] = static_cast<t_real*>(std::aligned_alloc(alignment, aligned_size));
    m_hu[l_st] = static_cast<t_real*>(std::aligned_alloc(alignment, aligned_size));;
    m_hv[l_st] = static_cast<t_real*>(std::aligned_alloc(alignment, aligned_size));;
  }
  m_bathymetry = static_cast<t_real*>(std::aligned_alloc(alignment, aligned_size));;




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
  free(m_bathymetry);
  for( unsigned short l_st = 0; l_st < 2; l_st++ ) {
    free(m_h[l_st]);
    free(m_hu[l_st]);
    free(m_hv[l_st]);
  }
}

void tsunami_lab::patches::WavePropagation2d::timeStep( t_real i_scaling ) {
  const t_idx l_stride = getStride();

  t_real * __restrict__ l_hOld  = m_h[m_step];
  t_real * __restrict__ l_huOld = m_hu[m_step];
  t_real * __restrict__ l_hvOld = m_hv[m_step];

  m_step = (m_step + 1) % 2;

  t_real * __restrict__ l_hNew  = m_h[m_step];
  t_real * __restrict__ l_huNew = m_hu[m_step];
  t_real * __restrict__ l_hvNew = m_hv[m_step];
  const t_real * __restrict__ l_bath = m_bathymetry;

  #pragma omp parallel
  {
    const int tid = omp_get_thread_num();
    const int num_threads = omp_get_num_threads();

    // 1. Zuweisung der Zeilenbereiche (exakte Partitionierung)
    // Wir teilen das Gitter in horizontale Streifen
    const t_idx rows_total = m_yCells + 2;
    const t_idx rows_per_thread = (rows_total + num_threads - 1) / num_threads;
    const t_idx row_start = std::min<t_idx>(tid * rows_per_thread, rows_total);
    const t_idx row_end   = std::min<t_idx>(row_start + rows_per_thread, rows_total);

    // Initialisierung des neuen Zeitschritts (nur den eigenen Bereich)
    for (t_idx i = row_start * l_stride; i < row_end * l_stride; i++) {
        l_hNew[i]  = l_hOld[i];
        l_huNew[i] = l_huOld[i];
        l_hvNew[i] = l_hvOld[i];
    }
    #pragma omp barrier

    // 2. X-SWEEP: Compute Fluxe & Apply
    // Hier können wir sicher arbeiten, da jeder Thread an seinen eigenen Zeilen schreibt
    // X-Sweep beeinflusst l_hNew[x] und l_hNew[x+1]. 
    // Solange Threads keine Zeilen teilen, ist das sicher.
    for (t_idx l_yed = row_start; l_yed < row_end; l_yed++) {
        if (l_yed == 0 || l_yed > m_yCells) continue; // Ghost Rows überspringen

        for (t_idx l_xed = 0; l_xed < m_xCells + 1; l_xed++) {
            const t_idx l_ceL = l_xed + l_stride * l_yed;
            const t_idx l_ceR = l_xed + 1 + l_stride * l_yed;

            t_real l_netL[2] = {0,0};
            t_real l_netR[2] = {0,0};

            // F-Wave Solver Aufruf
            tsunami_lab::solvers::Fwave::netUpdates(
                l_hOld[l_ceL], l_hOld[l_ceR],
                l_huOld[l_ceL], l_huOld[l_ceR],
                l_bath[l_ceL], l_bath[l_ceR],
                l_netL, l_netR
            );

            // Update direkt (sicher, da Threads an verschiedenen Zeilen arbeiten)
            l_hNew[l_ceL]  -= i_scaling * l_netL[0];
            l_huNew[l_ceL] -= i_scaling * l_netL[1];
            l_hNew[l_ceR]  -= i_scaling * l_netR[0];
            l_huNew[l_ceR] -= i_scaling * l_netR[1];
        }
    }
    #pragma omp barrier

    // 3. Y-SWEEP (Der gefährliche Teil)
    // Wir nutzen hier explizit ATOMIC, da Y-Sweep-Fluxe Zell-Grenzen zwischen Threads überschreiten können.
    // Das ist die einzige mathematisch wasserdichte Lösung für 2D-Gitter-Updates.
    for (t_idx l_yed = row_start; l_yed < row_end; l_yed++) {
        if (l_yed >= m_yCells + 1) continue; // Boundary

        for (t_idx l_xed = 1; l_xed < m_xCells + 1; l_xed++) {
            const t_idx l_ceU = l_xed + l_stride * l_yed;
            const t_idx l_ceB = l_xed + l_stride * (l_yed + 1);

            t_real l_netU[2] = {0,0};
            t_real l_netB[2] = {0,0};

            tsunami_lab::solvers::Fwave::netUpdates(
                l_hOld[l_ceU], l_hOld[l_ceB],
                l_hvOld[l_ceU], l_hvOld[l_ceB],
                l_bath[l_ceU], l_bath[l_ceB],
                l_netU, l_netB
            );

            // ATOMIC ist hier zwingend, um Race Conditions bei den Zeilengrenzen zu verhindern
            #pragma omp atomic
            l_hNew[l_ceU] -= i_scaling * l_netU[0];
            #pragma omp atomic
            l_hvNew[l_ceU] -= i_scaling * l_netU[1];
            
            #pragma omp atomic
            l_hNew[l_ceB] -= i_scaling * l_netB[0];
            #pragma omp atomic
            l_hvNew[l_ceB] -= i_scaling * l_netB[1];
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
