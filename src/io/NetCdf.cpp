/**
 * @author Marvin Doering
 *
 * @section DESCRIPTION
 * IO-routines handling netcdf files.
 **/
#include "NetCdf.h"

tsunami_lab::io::NetCdf::NetCdf( t_idx i_nx, t_idx i_ny, t_real i_dxy, t_real i_dt, const std::string & i_filePath ){
    m_dxy = i_dxy;
    m_dt = i_dt;
    // Opening new netcdf file
    // check if file path ends with '.nc'
    if (i_filePath.length() < 3 ||
        i_filePath[i_filePath.length() - 3] != '.' ||
        i_filePath[i_filePath.length() - 2] != 'n' ||
        i_filePath[i_filePath.length() - 1] != 'c'){
            std::cerr << "netcdf file should end with '.nc'" << std::endl;
            return;
    }
    // creating file
    errorChecking( nc_create(i_filePath.c_str(), NC_CLOBBER, &m_fileId) );

    // creating dimensions
    errorChecking( nc_def_dim(m_fileId, "x", i_nx, &m_xDimId) );
    errorChecking( nc_def_dim(m_fileId, "y", i_ny, &m_yDimId) );
    errorChecking( nc_def_dim(m_fileId, "t", NC_UNLIMITED, &m_tDimId) );

    // creating variables
    int l_dimIds[3] = {m_tDimId, m_yDimId, m_xDimId};

    errorChecking( nc_def_var(m_fileId, "x", NC_FLOAT, 1, &m_xDimId, &m_xVarId) );
    errorChecking( nc_put_att_text(m_fileId, m_xVarId, "units", 1, "m") );

    errorChecking( nc_def_var(m_fileId, "y", NC_FLOAT, 1, &m_yDimId, &m_yVarId) );
    errorChecking( nc_put_att_text(m_fileId, m_yVarId, "units", 1, "m") );

    errorChecking( nc_def_var(m_fileId, "t", NC_FLOAT, 1, &m_tDimId, &m_tVarId) );
    errorChecking( nc_put_att_text(m_fileId, m_tVarId, "units", 1, "s") );
    
    errorChecking( nc_def_var(m_fileId, "h", NC_FLOAT, 3, l_dimIds, &m_hVarId) );
    errorChecking( nc_put_att_text(m_fileId, m_hVarId, "units", 1, "m") );

    errorChecking( nc_def_var(m_fileId, "hu", NC_FLOAT, 3, l_dimIds, &m_huVarId) );
    errorChecking( nc_put_att_text(m_fileId, m_huVarId, "units", 6, "kg*m/s") );
    
    errorChecking( nc_def_var(m_fileId, "hv", NC_FLOAT, 3, l_dimIds, &m_hvVarId) );
    errorChecking( nc_put_att_text(m_fileId, m_hvVarId, "units", 6, "kg*m/s") );
    
    errorChecking( nc_def_var(m_fileId, "b", NC_FLOAT, 3, l_dimIds, &m_bVarId) );
    errorChecking( nc_put_att_text(m_fileId, m_bVarId, "units", 1, "m") );
    
    // register COARDS convention
    const char *l_coardsStr = "COARDS";
    nc_put_att_text( m_fileId, NC_GLOBAL, "Conventions", 6, l_coardsStr);

    nc_enddef(m_fileId);

    for (tsunami_lab::t_idx l_cx = 0; l_cx < i_nx; l_cx++){
        tsunami_lab::t_real l_cxFloat = l_cx * m_dxy;
        errorChecking( nc_put_var1_float(m_fileId, m_xVarId, &l_cx, &l_cxFloat) );
    }

    for (tsunami_lab::t_idx l_cy = 0; l_cy < i_ny; l_cy++){
        tsunami_lab::t_real l_cyFloat = l_cy * m_dxy;
        errorChecking( nc_put_var1_float(m_fileId, m_yVarId, &l_cy, &l_cyFloat) );
    }
}

tsunami_lab::io::NetCdf::~NetCdf(){
    errorChecking( nc_close(m_fileId) );
}

void tsunami_lab::io::NetCdf::errorChecking(int i_errId){
    if (i_errId != NC_NOERR){
        std::cerr << nc_strerror(i_errId) << std::endl;
    }
}

void tsunami_lab::io::NetCdf::write(t_idx                i_nx,
                                    t_idx                i_ny,
                                    t_idx                i_timeStep,
                                    t_idx                i_stride,
                                    t_real       const * i_h,
                                    t_real       const * i_hu,
                                    t_real       const * i_hv,
                                    t_real       const * i_bathymetry ){
    
    // write data to file
    tsunami_lab::t_real l_timeStep = i_timeStep;
    errorChecking( nc_put_var1_float(m_fileId, m_tVarId, &i_timeStep, &l_timeStep) );
    for (t_idx l_cy = 0; l_cy < i_ny; l_cy++){
        for (t_idx l_cx = 0; l_cx < i_nx; l_cx++){
            t_idx l_index[3] = {i_timeStep * m_dt, l_cy * m_dxy, l_cx * m_dxy};
            if (i_h != nullptr){
                errorChecking( nc_put_var1_float( m_fileId, m_hVarId, l_index, &i_h[l_cy * i_stride + l_cx] ) );
            }
            if (i_hu != nullptr){
                errorChecking( nc_put_var1_float( m_fileId, m_huVarId, l_index, &i_hu[l_cy * i_stride + l_cx] ) );
            }
            if (i_hv != nullptr){
                errorChecking( nc_put_var1_float( m_fileId, m_hvVarId, l_index, &i_hv[l_cy * i_stride + l_cx] ) );
            }
            if (i_bathymetry != nullptr){
                errorChecking( nc_put_var1_float( m_fileId, m_bVarId, l_index, &i_bathymetry[l_cy * i_stride + l_cx] ) );
            }
        }
    }
}

