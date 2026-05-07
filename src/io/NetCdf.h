/**
 * @author Marvin Doering
 *
 * @section DESCRIPTION
 * IO-routines for writing a snapshot as netcdf file.
 **/
#ifndef TSUNAMI_LAB_IO_NETCDF
#define TSUNAMI_LAB_IO_NETCDF

#include "../constants.h"
#include <cstring>
#include <iostream>
#include <fstream>
#include <netcdf>

namespace tsunami_lab {
  namespace io {
    class NetCdf;
  }
}

class tsunami_lab::io::NetCdf {
  public:
    /**
     * Writes the data to a .nc file with netcdf.
     *
     * @param i_dxy cell width in x- and y-direction.
     * @param i_nx number of cells in x-direction.
     * @param i_ny number of cells in y-direction.
     * @param i_stride stride of the data arrays in y-direction (x is assumed to be stride-1).
     * @param i_h water height of the cells; optional: use nullptr if not required.
     * @param i_hu momentum in x-direction of the cells; optional: use nullptr if not required.
     * @param i_hv momentum in y-direction of the cells; optional: use nullptr if not required.
     * @param i_bathymetry bathymetry data of the cells; optional: use nullptr if not required.
     * @param i_filePath path to file in which the NetCdf-data is written.
     **/
    static void write( t_real               i_dxy,
                       t_idx                i_nx,
                       t_idx                i_ny,
                       t_idx                i_stride,
                       t_real       const * i_h,
                       t_real       const * i_hu,
                       t_real       const * i_hv,
                       t_real       const * i_bathymetry,
                       std::ostream       & io_stream );
    
    /**
     * Reads bathymetry data.
     * 
     * @param i_cellX number of cells to read in x direction.
     * @param i_cellY number of cells to read in y direction.
     * @param i_filePath path to file.
     * @param o_bathymetry bathymetry data.
     */
    static void readBathymetry( t_idx             i_cellX,
                                t_idx             i_cellY,
                                std::string       i_filePath,
                                t_real       *    o_bathymetry);
    
    /**
     * Reads displacement data.
     * 
     * @param i_cellX number of cells to read in x direction.
     * @param i_cellY number of cells to read in y direction.
     * @param i_filePath path to file.
     * @param o_displacement displacement data.
     */
    static void readDisplacement( t_idx         i_cellX,
                                  t_idx         i_cellY,
                                  std::string   i_filePath,
                                  t_real     *  o_displacement
                                    );
};

#endif