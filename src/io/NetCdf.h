/**
 * @author Marvin Doering
 *
 * @file
 * @brief Input/output routines for NetCDF files.
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


/**
 * Reads gridded input data and writes simulation results in NetCDF format.
 **/
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
    tsunami_lab::t_real m_dxy;
    //! time step size
    tsunami_lab::t_real m_dt;
    //! output resolution
    t_idx m_outRes;

    /**
     * Handles a return code from the NetCDF C API.
     *
     * @param i_errId NetCDF status code.
     * @param i_printErr whether to report the error and throw an exception.
     * @return zero on success and -1 for a suppressed error.
     * @throws std::runtime_error if an error occurs and @p i_printErr is true.
     **/
    static int errorChecking(int i_errId, bool i_printErr = true);

  public:

    /**
     * Creates a NetCDF output file or opens an existing one for appending.
     *
     * @param i_nx number of cells in x-direction.
     * @param i_ny number of cells in y-direction.
     * @param i_dxy cell size.
     * @param i_dt time-step size.
     * @param i_left x-coordinate of the leftmost cell.
     * @param i_upper y-coordinate of the uppermost cell.
     * @param i_outRes output coarsening factor; 1 preserves the input resolution.
     * @param i_compressionLevel NetCDF compression level; 0 disables compression.
     * @param i_filePath path to the NetCDF file.
     * @param i_existingFile whether to open an existing file instead of creating one.
     **/
    NetCdf( t_idx i_nx, t_idx i_ny, t_real i_dxy, t_real i_dt, t_real i_left, t_real i_upper, t_idx i_outRes, t_idx i_compressionLevel, const std::string & i_filePath , bool i_existingFile = false);
    
    /**
     * Destructor.
     */
    ~NetCdf();

    /**
     * Writes one simulation state to the NetCDF file.
     *
     * @param i_nx number of cells in x-direction.
     * @param i_ny number of cells in y-direction.
     * @param i_timeIndex current time step of simulation.
     * @param i_simTime current time of simulation.
     * @param i_stride stride of the data arrays in y-direction (x is assumed to be stride-1).
     * @param i_h water height of the cells; optional: use nullptr if not required.
     * @param i_hu momentum in x-direction of the cells; optional: use nullptr if not required.
     * @param i_hv momentum in y-direction of the cells; optional: use nullptr if not required.
     * @param i_bathymetry bathymetry data of the cells; optional: use nullptr if not required.
     * @param i_writeCheckpoint whether to flush the file to durable storage after writing.
     **/
    void write( t_idx                i_nx,
                t_idx                i_ny,
                t_idx                i_timeIndex,
                t_real               i_simTime,
                t_idx                i_stride,
                t_real       const * i_h,
                t_real       const * i_hu,
                t_real       const * i_hv,
                t_real       const * i_bathymetry,
                bool                 i_writeCheckpoint );

    /**
     * Reads the two-dimensional variable `z` and its grid coordinates.
     * 
     * @param i_filePath path to file.
     * @param o_cellX number of cells in x-direction.
     * @param o_cellY number of cells in y-direction.
     * @param o_dxy cell size derived from the first two x-coordinates.
     * @param o_left x-coordinate of the leftmost cell.
     * @param o_upper y-coordinate of the uppermost cell.
     * @param o_data newly allocated array containing the gridded data.
     * @param printErr whether NetCDF errors are reported by throwing an exception.
     *
     * @return zero on success and -1 on a suppressed NetCDF error.
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
