#include "TsunamiEvent2d.h"

#include <cmath>

using namespace tsunami_lab;

setups::TsunamiEvent2d::TsunamiEvent2d( t_idx i_cellsX, t_idx i_cellsY, t_real i_dxy, t_idx i_bCellsX, t_idx i_bCellsY, t_idx i_dCellsX, t_idx i_dCellsY, const t_real * i_bathymetry, const t_real * i_displacement ){
    m_cellsX = i_cellsX;
    m_cellsY = i_cellsY;
    m_dxy = i_dxy;

    m_bCellsX = i_bCellsX;
    m_bCellsY = i_bCellsY;

    m_dCellsX = i_dCellsX;
    m_dCellsY = i_dCellsY;

    m_bathymetry = new t_real[m_bCellsX * m_bCellsY];
    m_displacement = new t_real[m_dCellsX * m_dCellsY];

    for (t_idx l_iy = 0; l_iy < m_bCellsY; l_iy++){
        for (t_idx l_ix = 0; l_ix < m_bCellsX; l_ix++){
            m_bathymetry[l_iy * m_bCellsX + l_ix] = i_bathymetry[l_iy * m_bCellsX + l_ix];
        }
    }

    for (t_idx l_iy = 0; l_iy < m_dCellsY; l_iy++){
        for (t_idx l_ix = 0; l_ix < m_dCellsX; l_ix++){
            m_displacement[l_iy * m_dCellsX + l_ix] = i_displacement[l_iy * m_dCellsX + l_ix];
        }
    }
}

setups::TsunamiEvent2d::~TsunamiEvent2d(){
    delete[] m_bathymetry;
    delete[] m_displacement;
}

t_real setups::TsunamiEvent2d::getHeight( t_real i_x, t_real i_y ) const {
    t_real rel_x = i_x / ( m_dxy * (t_real) m_cellsX );
    t_real rel_y = i_y / ( m_dxy * (t_real) m_cellsY );

<<<<<<< HEAD
    t_idx i_bIx = (t_idx) (rel_x * m_bCellsX);
    t_idx i_bIy = (t_idx) (rel_y * m_bCellsY);
=======
    t_idx i_bIx = (t_idx) std::round(rel_x * m_bCellsX);
    t_idx i_bIy = (t_idx) std::round(rel_y * m_bCellsY);
>>>>>>> Marvin

    if (i_bIx >= m_bCellsX) i_bIx = m_bCellsX - 1;
    if (i_bIy >= m_bCellsY) i_bIy = m_bCellsY - 1;
    


    if (m_bathymetry[i_bIy * m_bCellsX + i_bIx] < 0){
        return std::max(-m_bathymetry[i_bIy * m_bCellsX + i_bIx], m_delta);
    }
    return 0;
}

t_real setups::TsunamiEvent2d::getBathymetry( t_real i_x, t_real i_y ) const {
    
    t_real rel_x = i_x / ( m_dxy * (t_real) m_cellsX );
    t_real rel_y = i_y / ( m_dxy * (t_real) m_cellsY );

<<<<<<< HEAD
    t_idx i_bIx = (t_idx) (rel_x * m_bCellsX);
    t_idx i_bIy = (t_idx) (rel_y * m_bCellsY);
=======
    t_idx i_bIx = (t_idx) std::round(rel_x * m_bCellsX);
    t_idx i_bIy = (t_idx) std::round(rel_y * m_bCellsY);
>>>>>>> Marvin

    if (i_bIx >= m_bCellsX) i_bIx = m_bCellsX - 1;
    if (i_bIy >= m_bCellsY) i_bIy = m_bCellsY - 1;


<<<<<<< HEAD
    t_idx i_dIx = (t_idx) (rel_x * m_dCellsX);
    t_idx i_dIy = (t_idx) (rel_y * m_dCellsY);
=======
    t_idx i_dIx = (t_idx) std::round(rel_x * m_dCellsX);
    t_idx i_dIy = (t_idx) std::round(rel_y * m_dCellsY);
>>>>>>> Marvin

    if (i_dIx >= m_dCellsX) i_dIx = m_dCellsX - 1;
    if (i_dIy >= m_dCellsY) i_dIy = m_dCellsY - 1;

    if (m_bathymetry[i_bIy * m_bCellsX + i_bIx] < 0){
        return std::min(m_bathymetry[i_bIy * m_bCellsX + i_bIx], -m_delta) + m_displacement[i_dIy * m_dCellsX + i_dIx];
    }
    return std::max(m_bathymetry[i_bIy * m_bCellsX + i_bIx], m_delta) + m_displacement[i_dIy * m_dCellsX + i_dIx];
}