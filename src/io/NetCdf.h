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

    /**
     * Constructs a NetCdf object and creates a new .nc file. With calls to the write function this nc file can be filled with data.
     *
     * @param i_nx number of cells in x-direction.
     * @param i_ny number of cells in y-direction.
     * @param i_dxy size of cell.
     * @param i_left coordinates of left most cell.
     * @param i_upper coordinates of upper most cell.
     * @param i_filePath path of the new nc file.
     **/
    NetCdf( t_idx i_nx, t_idx i_ny, t_real i_dxy, t_real i_dt, t_real i_left, t_real i_upper, const std::string & i_filePath );
    

    /**
     * Destructor.
     */
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
     * Reads data.
     * 
     * @param i_filePath path to file.
     * @param o_cellX number of cells in x direction.
     * @param o_cellY number of cells in y direction.
     * @param o_dxy   size of cell (calculated through the difference between the first and second element in the x variable of the nc file)
     * @param o_left  x-coordinate of the left most cell in the nc file
     * @param o_upper y-coordinate of the upper most cell in the nc file
     * @param o_data data.
     *
     * @return success.
     */
    static int read(  const std::string  & i_filePath,
                      t_idx      &  o_cellX,
                      t_idx      &  o_cellY,
                      t_real     &  o_dxy,
                      t_real     &  o_left,
                      t_real     &  o_upper,
                      t_real     ** o_data,
                      bool printErr = true );                                

};

#endif