#include "TsunamiEvent2d.h"

#include <cmath>
#include <algorithm>

using namespace tsunami_lab;

setups::TsunamiEvent2d::TsunamiEvent2d( t_idx i_cellsX, t_idx i_cellsY, t_real i_dxy, t_real i_left, t_real i_upper, t_idx i_bCellsX, t_idx i_bCellsY, t_real i_dxyBat, t_real i_leftBat, t_real i_upperBat,
                                        t_idx i_dCellsX, t_idx i_dCellsY, t_real i_dxyDis, t_real i_leftDis, t_real i_upperDis, const t_real * i_bathymetry, const t_real * i_displacement ){
    m_cellsX = i_cellsX;
    m_cellsY = i_cellsY;
    m_dxy = i_dxy;
    m_left = i_left;
    m_upper = i_upper;

    m_cellsXBat = i_bCellsX;
    m_cellsYBat = i_bCellsY;

    m_cellsXDis = i_dCellsX;
    m_cellsYDis = i_dCellsY;

    m_dxyBat = i_dxyBat;
    m_dxyDis = i_dxyDis;

    m_leftBat = i_leftBat;
    m_upperBat = i_upperBat;

    m_leftDis = i_leftDis;
    m_upperDis = i_upperDis;

    m_bathymetry = new t_real[m_cellsXBat * m_cellsYBat];
    m_displacement = new t_real[m_cellsXDis * m_cellsYDis];

    for (t_idx l_iy = 0; l_iy < m_cellsYBat; l_iy++){
        for (t_idx l_ix = 0; l_ix < m_cellsXBat; l_ix++){
            m_bathymetry[l_iy * m_cellsXBat + l_ix] = i_bathymetry[l_iy * m_cellsXBat + l_ix];
        }
    }

    for (t_idx l_iy = 0; l_iy < m_cellsYDis; l_iy++){
        for (t_idx l_ix = 0; l_ix < m_cellsXDis; l_ix++){
            m_displacement[l_iy * m_cellsXDis + l_ix] = i_displacement[l_iy * m_cellsXDis + l_ix];
        }
    }
}

setups::TsunamiEvent2d::~TsunamiEvent2d(){
    delete[] m_bathymetry;
    delete[] m_displacement;
}

t_real setups::TsunamiEvent2d::getHeight( t_real i_x, t_real i_y ) const {
    
    t_idx l_bIx = (t_idx) std::clamp<t_real>( std::round((i_x + m_left - m_leftBat) / m_dxyBat), 0, m_cellsXBat -1 );
    t_idx l_bIy = (t_idx) std::clamp<t_real>( std::round((i_y + m_upper - m_upperBat) / m_dxyBat), 0, m_cellsYBat -1 );
    
    if (m_bathymetry[l_bIy * m_cellsXBat + l_bIx] < 0){
        return std::max(-m_bathymetry[l_bIy * m_cellsXBat + l_bIx], m_delta);
    }
    return 0;
}

t_real setups::TsunamiEvent2d::getBathymetry( t_real i_x, t_real i_y ) const {

    t_idx l_bIx = (t_idx) std::clamp<t_real>( std::round((i_x + m_left - m_leftBat) / m_dxyBat), 0, m_cellsXBat -1 );
    t_idx l_bIy = (t_idx) std::clamp<t_real>( std::round((i_y + m_upper - m_upperBat) / m_dxyBat), 0, m_cellsYBat -1 );

    t_idx l_dIx = (t_idx) std::clamp<t_real>( std::round((i_x + m_left - m_leftDis) / m_dxyDis), 0, m_cellsXDis -1 );
    t_idx l_dIy = (t_idx) std::clamp<t_real>( std::round((i_y + m_upper - m_upperDis) / m_dxyDis), 0, m_cellsYDis -1 );

    if (m_bathymetry[l_bIy * m_cellsXBat + l_bIx] < 0){
        return std::min(m_bathymetry[l_bIy * m_cellsXBat + l_bIx], -m_delta) + m_displacement[l_dIy * m_cellsXDis + l_dIx];
    }
    return std::max(m_bathymetry[l_bIy * m_cellsXBat + l_bIx], m_delta) + m_displacement[l_dIy * m_cellsXDis + l_dIx];
}