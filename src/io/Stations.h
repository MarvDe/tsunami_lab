#ifndef TSUNAMI_LAB_IO_STATIONS_H
#define TSUNAMI_LAB_IO_STATIONS_H

#include "../constants.h"
#include <cstring>
#include <vector>
#include <iostream>
#include <fstream>
#include <yaml-cpp/yaml.h>

namespace tsunami_lab {
  namespace io {
    class Stations;
    struct Station;
    }
}

struct tsunami_lab::io::Station{
    const std::string m_name;
    t_real m_posX;
    t_real m_posY;
    std::ostream *m_stream;
    bool m_ownsStream;
};

class tsunami_lab::io::Stations{
    private:
        //! array of stations
        std::vector<tsunami_lab::io::Station> m_stations;

        //! number of cells
        t_idx m_cellX = 0;
        t_idx m_cellY = 0;

        //! cell size
        t_real m_dxy = 0;
    

    public:
        /**
         * Constructs the stations class.
         *
         * @param i_cellX number of cells in x direction.
         * @param i_cellY number of cells in y direction.
         * @param i_dxy cell size
         **/
        Stations( t_idx i_cellX, t_idx i_cellY, t_real i_dxy);


        /**
         * Destructor.
         */
        ~Stations();


        /**
         * Adds a station for which data can be tracked.
         *
         * @param i_name name of stations
         * @param i_posX position in x direction
         * @param i_posY position in y direction
         * @param io_stream output stream for writing data
         * @param i_ownsStream flag for memory management if true memory io_stream gets deleted else it is assumed that stream is externally managed.
         **/
        void addStation(const std::string &i_name, t_real i_posX, t_real i_posY, std::ostream *io_stream, bool i_ownsStream = false);


        /**
         * Writes data to the output stream.
         *
         * @param i_simTime current time of simulaton.
         * @param i_height water height.
         * @param i_momentumX water momentum in x direction.
         * @param i_momentumY water momentum in y direction.
         * @param i_bathymetry bathymetry data.
         * @param i_stride stride of data arrays.
         **/
        void write( t_real i_simTime, const t_real * i_height, const t_real * i_momentumX, const t_real * i_momentumY, const t_real * i_bathymetry, t_idx i_stride ) const;

        
        /**
         * Reads station from yaml file
         * 
         * @param i_filePath path of the stations yaml file.
         **/
        void readFile(std::string i_filePath);
};


#endif