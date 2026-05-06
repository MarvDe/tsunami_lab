#include "Stations.h"

tsunami_lab::io::Stations::Stations( t_idx i_cellX, t_idx i_cellY, t_real i_dxy) : 
    m_cellX(i_cellX), m_cellY(i_cellY), m_dxy(i_dxy){
}

tsunami_lab::io::Stations::~Stations(){

}

void tsunami_lab::io::Stations::addStation(const std::string &i_name, t_real i_posX, t_real i_posY, std::ostream &io_stream) {
   m_stations.push_back(Station{i_name, i_posX, i_posY, io_stream});
}

void tsunami_lab::io::Stations::write( t_real i_simTime, const t_real * i_height, const t_real * i_momentumX, const t_real * i_momentumY, const t_real * i_bathymetry, t_idx i_stride ) const{
    

    for (const auto &l_station: m_stations){

        std::ostream &l_stream = l_station.m_stream;

        // write CSV header
        if (i_simTime == 0){
            l_stream << "sim_time,name,x,y";
            if( i_height  != nullptr ) l_stream << ",height";
            if( i_momentumX != nullptr ) l_stream << ",momentum_x";
            if( i_momentumY != nullptr ) l_stream << ",momentum_y";
            if( i_bathymetry != nullptr ) l_stream << ",bathymetry";
            l_stream << "\n";
        }

        t_idx l_idxPosX = (t_idx) l_station.m_posX / m_dxy;
        t_idx l_idxPosY = (t_idx) l_station.m_posY / m_dxy;
        
        t_idx l_id = l_idxPosX + l_idxPosY * i_stride;
        l_stream << i_simTime << "," << l_station.m_name << ",";
        l_stream << l_station.m_posX << "," << l_station.m_posY;
        if (l_id < m_cellX + (m_cellY - 1) * i_stride){
            // write data
            if( i_height  != nullptr ) l_stream << "," << i_height[l_id];
            if( i_momentumX != nullptr ) l_stream << "," << i_momentumX[l_id];
            if( i_momentumY != nullptr ) l_stream << "," << i_momentumY[l_id];
            if( i_bathymetry != nullptr ) l_stream << "," << i_bathymetry[l_id];
        }
        l_stream << std::endl;
    }

}