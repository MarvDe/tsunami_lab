/**
 * @author Marvin Doering
 *
 * @section DESCRIPTION
 * IO-routines handling netcdf files.
 **/
#include "NetCdf.h"
using namespace tsunami_lab;

io::NetCdf::NetCdf( t_idx i_nx, t_idx i_ny, t_real i_dxy, t_real i_dt, const std::string & i_filePath ){
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
    errorChecking( nc_def_dim(m_fileId, "time", NC_UNLIMITED, &m_tDimId) );

    // creating variables
    int l_dimIds[3] = {m_tDimId, m_yDimId, m_xDimId};

    errorChecking( nc_def_var(m_fileId, "x", NC_FLOAT, 1, &m_xDimId, &m_xVarId) );
    errorChecking( nc_put_att_text(m_fileId, m_xVarId, "units", 1, "m") );

    errorChecking( nc_def_var(m_fileId, "y", NC_FLOAT, 1, &m_yDimId, &m_yVarId) );
    errorChecking( nc_put_att_text(m_fileId, m_yVarId, "units", 1, "m") );

    errorChecking( nc_def_var(m_fileId, "time", NC_FLOAT, 1, &m_tDimId, &m_tVarId) );
    const std::string l_timeUnit = "seconds since start";
    errorChecking( nc_put_att_text(m_fileId, m_tVarId, "units", l_timeUnit.size(), l_timeUnit.c_str()) );
    
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

io::NetCdf::~NetCdf(){
    errorChecking( nc_close(m_fileId) );
}

int io::NetCdf::errorChecking(int i_errId, bool i_printErr){
    if (i_errId != NC_NOERR){
        if (i_printErr){
            std::cerr << nc_strerror(i_errId) << std::endl;
        }
        return -1;
    }
    return 0;
}

void io::NetCdf::write( t_idx                i_nx,
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
            t_idx l_index[3] = {i_timeStep, l_cy, l_cx};
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

int io::NetCdf::readDisplacement(  t_idx i_cellX,
                                    t_idx i_cellY,
                                    std::string i_filePath,
                                    t_real * o_displacement,
                                    bool printErr){
    int l_ncid;
    int l_varidZ;
    int l_status = 0;
    int ndims;
    int dimids[NC_MAX_VAR_DIMS];
    size_t dim_sizes[NC_MAX_VAR_DIMS];
                                    
    l_status = errorChecking( nc_open(i_filePath.c_str(), NC_NOWRITE, &l_ncid) );
    if (l_status) return -1;
    l_status = errorChecking( nc_inq_varid(l_ncid, "z", &l_varidZ) );
    if (l_status) return -1;
    l_status = errorChecking( nc_inq_var(l_ncid, l_varidZ, nullptr, nullptr, &ndims, dimids, nullptr) );
    if (l_status) return -1;
    l_status = errorChecking( nc_inq_dimlen(l_ncid, dimids[0], &dim_sizes[0]) );
    if (l_status) return -1;
    l_status = errorChecking( nc_inq_dimlen(l_ncid, dimids[1], &dim_sizes[1]) );
    if (l_status) return -1;
    size_t nx = dim_sizes[1];
    size_t ny = dim_sizes[0];

    if (dim_sizes[2] != 0 || nx != i_cellX || ny != i_cellY){
        if (printErr){
            printf("Error: NetCdf dimension mismatch.\n");
        }
        return -1;
    }

    l_status = errorChecking( nc_get_var_float(l_ncid, l_varidZ, o_displacement) );
    if (l_status) return -1;
    l_status = errorChecking( nc_close(l_ncid) );
    return l_status;
}

int io::NetCdf::readBathymetry(t_idx i_cellX,
                                t_idx i_cellY,
                                std::string i_filePath,
                                t_real * o_bathymetry,
                                bool printErr){
    int l_ncid;
    int l_varidZ;
    int l_status = 0;
    int ndims;
    int dimids[NC_MAX_VAR_DIMS];
    size_t dim_sizes[NC_MAX_VAR_DIMS];
                                    
    l_status = errorChecking( nc_open(i_filePath.c_str(), NC_NOWRITE, &l_ncid) );
    if (l_status) return -1;
    l_status = errorChecking( nc_inq_varid(l_ncid, "z", &l_varidZ) );
    if (l_status) return -1;
    l_status = errorChecking( nc_inq_var(l_ncid, l_varidZ, nullptr, nullptr, &ndims, dimids, nullptr) );
    if (l_status) return -1;
    l_status = errorChecking( nc_inq_dimlen(l_ncid, dimids[0], &dim_sizes[0]) );
    if (l_status) return -1;
    l_status = errorChecking( nc_inq_dimlen(l_ncid, dimids[1], &dim_sizes[1]) );
    if (l_status) return -1;
    size_t nx = dim_sizes[1];
    size_t ny = dim_sizes[0];

    if (dim_sizes[2] != 0 || nx != i_cellX || ny != i_cellY){
        if(printErr){
            printf("Error: NetCdf dimension mismatch.\n");
        }
        return -1;
    }

    l_status = errorChecking( nc_get_var_float(l_ncid, l_varidZ, o_bathymetry) );
    if (l_status) return -1;
    l_status = errorChecking( nc_close(l_ncid) );
    return l_status;
}
