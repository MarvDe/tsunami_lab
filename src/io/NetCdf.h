/**
 * @author Marvin Doering
 *
 * @section DESCRIPTION
 * IO-routines handling netcdf files.
 **/
#ifndef TSUNAMI_LAB_IO_NETCDF
#define TSUNAMI_LAB_IO_NETCDF

#include "../constants.h"
#include <iostream>
#include <fstream>
#include <netcdf.h>
#include <unordered_map>
#include <string>

namespace tsunami_lab {
  namespace io {
    class NetCdf;
  }
}


class tsunami_lab::io::NetCdf {
  private:

    //! File Id
    int m_fileId;
    //! Ids of dimensions 
    int m_xDimId;
    int m_yDimId;
    int m_tDimId;
    //! Ids of variables
    int m_xVarId;
    int m_yVarId;
    int m_tVarId;
    int m_hVarId;
    int m_huVarId;
    int m_hvVarId;
    int m_bVarId;
    //! cell size
    tsunami_lab::t_idx m_dxy;
    //! time step size
    tsunami_lab::t_idx m_dt;

    static int errorChecking(int i_errId, bool i_printErr = true);

  public:

    NetCdf( t_idx i_nx, t_idx i_ny, t_real i_dxy, t_real i_dt, const std::string & i_filePath );
    
    ~NetCdf();

    /**
     * Writes the data to a .nc file using the netcdf library.
     *
     * @param i_nx number of cells in x-direction.
     * @param i_ny number of cells in y-direction.
     * @param i_timeStep current time step of simulation.
     * @param i_stride stride of the data arrays in y-direction (x is assumed to be stride-1).
     * @param i_h water height of the cells; optional: use nullptr if not required.
     * @param i_hu momentum in x-direction of the cells; optional: use nullptr if not required.
     * @param i_hv momentum in y-direction of the cells; optional: use nullptr if not required.
     * @param i_bathymetry bathymetry data of the cells; optional: use nullptr if not required.
     **/
    void write( t_idx                i_nx,
                t_idx                i_ny,
                t_idx                i_timeStep,
                t_idx                i_stride,
                t_real       const * i_h,
                t_real       const * i_hu,
                t_real       const * i_hv,
                t_real       const * i_bathymetry );
    
    /**
     * Reads bathymetry data.
     * 
     * @param i_cellX number of cells to read in x direction.
     * @param i_cellY number of cells to read in y direction.
     * @param i_filePath path to file.
     * @param o_bathymetry bathymetry data.
     */
    static int readBathymetry( t_idx             i_cellX,
                                t_idx             i_cellY,
                                std::string       i_filePath,
                                t_real       *    o_bathymetry,
                                bool printErr = true);
    
    /**
     * Reads displacement data.
     * 
     * @param i_cellX number of cells to read in x direction.
     * @param i_cellY number of cells to read in y direction.
     * @param i_filePath path to file.
     * @param o_displacement displacement data.
     *
     * @return success.
     */
    static int readDisplacement( t_idx         i_cellX,
                                  t_idx         i_cellY,
                                  std::string   i_filePath,
                                  t_real     *  o_displacement,
                                  bool printErr = true
                                    );

};

#endif