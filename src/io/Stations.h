/**
 * @file
 * @brief Sampling and output of simulation data at fixed stations.
 */
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

/**
 * Describes a sampling station and its output stream.
 */
struct tsunami_lab::io::Station{
    const std::string m_name; //!< Station name.
    t_real m_posX;            //!< Station x-coordinate.
    t_real m_posY;            //!< Station y-coordinate.
    std::ostream *m_stream;   //!< Output stream receiving sampled data.
    bool m_ownsStream;        //!< Whether the station owns the output stream.
};

/**
 * Manages fixed sampling stations on a Cartesian simulation grid.
 */
class tsunami_lab::io::Stations{
    private:
        //! array of stations
        std::vector<tsunami_lab::io::Station> m_stations;

        //! number of cells
        t_idx m_cellX = 0;
        t_idx m_cellY = 0;

        //! cell size
        t_real m_dxy = 0;

        //! global coord of left upper cell
        t_real m_leftUpperX = 0;
        t_real m_leftUpperY = 0;
    

    public:
        /**
         * Constructs the stations class.
         *
         * @param i_cellX number of cells in x-direction.
         * @param i_cellY number of cells in y-direction.
         * @param i_dxy cell size.
         * @param i_leftUpperX x-coordinate of the upper-left grid cell.
         * @param i_leftUpperY y-coordinate of the upper-left grid cell.
         **/
        Stations( t_idx i_cellX, t_idx i_cellY, t_real i_dxy, t_real i_leftUpperX, t_real i_leftUpperY);


        /**
         * Destructor.
         */
        ~Stations();


        /**
         * Adds a station for which data can be tracked.
         *
         * @param i_name station name.
         * @param i_posX station x-coordinate.
         * @param i_posY station y-coordinate.
         * @param io_stream output stream receiving the sampled data.
         * @param i_ownsStream whether this object owns and deletes @p io_stream.
         **/
        void addStation(const std::string &i_name, t_real i_posX, t_real i_posY, std::ostream *io_stream, bool i_ownsStream = false);


        /**
         * Writes data to the output stream.
         *
         * @param i_simTime current simulation time.
         * @param i_height water height.
         * @param i_momentumX water momentum in x-direction.
         * @param i_momentumY water momentum in y-direction.
         * @param i_bathymetry bathymetry data.
         * @param i_stride stride of data arrays.
         **/
        void write( t_real i_simTime, const t_real * i_height, const t_real * i_momentumX, const t_real * i_momentumY, const t_real * i_bathymetry, t_idx i_stride ) const;

        
        /**
         * Reads station definitions from a YAML file.
         * 
         * @param i_filePath path to the station YAML file.
         **/
        void readFile(std::string i_filePath);
};


#endif
