/**
 * @author Marvin Doering
 *
 * @section DESCRIPTION
 * IO-routines handling netcdf files.
 **/
#include "NetCdf.h"
#include <vector>
#include <cstring>
using namespace tsunami_lab;

io::NetCdf::NetCdf( t_idx i_nx, t_idx i_ny, t_real i_dxy, t_real i_dt, t_real i_left, t_real i_upper, t_idx i_outRes, const std::string & i_filePath ){

    m_dxy = i_dxy * i_outRes;
    m_dt = i_dt;
    m_outRes = i_outRes;

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
    errorChecking( nc_create(i_filePath.c_str(), NC_NETCDF4 | NC_CLOBBER | NC_SHUFFLE, &m_fileId) );

    // creating dimensions
    errorChecking( nc_def_dim(m_fileId, "x", i_nx / i_outRes, &m_xDimId) );
    errorChecking( nc_def_dim(m_fileId, "y", i_ny / i_outRes, &m_yDimId) );
    errorChecking( nc_def_dim(m_fileId, "time", NC_UNLIMITED, &m_tDimId) );

    // creating variables
    int l_dimIds[3] = {m_tDimId, m_yDimId, m_xDimId};
    int l_dimIdsB[2] = {m_yDimId, m_xDimId};

    errorChecking( nc_def_var(m_fileId, "x", NC_FLOAT, 1, &m_xDimId, &m_xVarId) );
    errorChecking( nc_put_att_text(m_fileId, m_xVarId, "units", 1, "m") );

    errorChecking( nc_def_var(m_fileId, "y", NC_FLOAT, 1, &m_yDimId, &m_yVarId) );
    errorChecking( nc_put_att_text(m_fileId, m_yVarId, "units", 1, "m") );

    errorChecking( nc_def_var(m_fileId, "time", NC_FLOAT, 1, &m_tDimId, &m_tVarId) );
    const std::string l_timeUnit = "seconds since start";
    errorChecking( nc_put_att_text(m_fileId, m_tVarId, "units", l_timeUnit.size(), l_timeUnit.c_str()) );
    
    int l_compressionLevel = 3;
    int l_useZlibCompression = 1;
    int l_useShuffle = 1;

    errorChecking( nc_def_var(m_fileId, "h", NC_FLOAT, 3, l_dimIds, &m_hVarId) );
    // turn on compression
    errorChecking( nc_def_var_deflate(m_fileId, m_hVarId, l_useShuffle, l_useZlibCompression, l_compressionLevel) );
    errorChecking( nc_put_att_text(m_fileId, m_hVarId, "units", 1, "m") );

    errorChecking( nc_def_var(m_fileId, "hu", NC_FLOAT, 3, l_dimIds, &m_huVarId) );
    errorChecking( nc_def_var_deflate(m_fileId, m_huVarId, l_useShuffle, l_useZlibCompression, l_compressionLevel) );
    errorChecking( nc_put_att_text(m_fileId, m_huVarId, "units", 6, "kg*m/s") );
    
    errorChecking( nc_def_var(m_fileId, "hv", NC_FLOAT, 3, l_dimIds, &m_hvVarId) );
    errorChecking( nc_def_var_deflate(m_fileId, m_hvVarId, l_useShuffle, l_useZlibCompression, l_compressionLevel) );
    errorChecking( nc_put_att_text(m_fileId, m_hvVarId, "units", 6, "kg*m/s") );
    
    errorChecking( nc_def_var(m_fileId, "b", NC_FLOAT, 2, l_dimIdsB, &m_bVarId) );
    errorChecking( nc_def_var_deflate(m_fileId, m_bVarId, l_useShuffle, l_useZlibCompression, l_compressionLevel) );
    errorChecking( nc_put_att_text(m_fileId, m_bVarId, "units", 1, "m") );
    
    // register COARDS convention
    const char *l_coardsStr = "COARDS";
    nc_put_att_text( m_fileId, NC_GLOBAL, "Conventions", 6, l_coardsStr);

    nc_enddef(m_fileId);

    for (tsunami_lab::t_idx l_cx = 0; l_cx < i_nx / i_outRes; l_cx++){
        tsunami_lab::t_real l_cxFloat = l_cx * m_dxy + i_left;
        errorChecking( nc_put_var1_float(m_fileId, m_xVarId, &l_cx, &l_cxFloat) );
    }

    for (tsunami_lab::t_idx l_cy = 0; l_cy < i_ny / i_outRes; l_cy++){
        tsunami_lab::t_real l_cyFloat = l_cy * m_dxy + i_upper;
        errorChecking( nc_put_var1_float(m_fileId, m_yVarId, &l_cy, &l_cyFloat) );
    }
}

io::NetCdf::~NetCdf(){
    errorChecking( nc_close(m_fileId) );
}

int io::NetCdf::errorChecking(int i_errId, bool i_printErr){
    if (i_errId != NC_NOERR){
        if (i_printErr){
            std::cerr << "NC Error: " << nc_strerror(i_errId) << std::endl;
        }
        return -1;
    }
    return 0;
}

void io::NetCdf::write( t_idx                i_nx,
                        t_idx                i_ny,
                        t_idx                i_timeIndex,
                        t_real               i_simTime,
                        t_idx                i_stride,
                        t_real       const * i_h,
                        t_real       const * i_hu,
                        t_real       const * i_hv,
                        t_real       const * i_bathymetry ){

    if (i_nx + 2 == i_stride ){ // if ghost cells are passed
        errorChecking( nc_put_var1_float(m_fileId, m_tVarId, &i_timeIndex, &i_simTime));
        size_t start[3] = {i_timeIndex, 0, 0};
        size_t count[3] = {1, (i_ny / m_outRes), (i_nx / m_outRes)};
        size_t startB[2] = {0,0};
        size_t countB[2] = {(i_ny / m_outRes), (i_nx / m_outRes)};
        std::vector<float> buffer((i_nx / m_outRes) * (i_ny / m_outRes));

        // write the hight, momentum_x and momentum_y
        const t_real * l_buffers[3] = {i_h, i_hu, i_hv};
        int l_varIds[3] = {m_hVarId, m_huVarId, m_hvVarId};
        for (t_idx l_bufferIndex = 0; l_bufferIndex < 3; l_bufferIndex++){ // iterate over height, momentum_x and momentum_y
            if (l_buffers[l_bufferIndex] != nullptr){
                for (size_t y = 0; y < i_ny / m_outRes; y++) {
                    // if output resolution is equal to 1 -> copy whole array
                    if (m_outRes == 1){
                        std::memcpy(
                            &buffer[y * i_nx],
                            &l_buffers[l_bufferIndex][y * i_stride],
                            i_nx * sizeof(float)
                        );
                    }
                    // merge cells to reduce output size
                    else{
                        for (size_t x = 0; x < i_nx / m_outRes; x++){
                            t_real l_value = 0;
                            for (size_t a = 0; a < m_outRes; a++){
                                for (size_t b = 0; b < m_outRes; b++){
                                    l_value += l_buffers[l_bufferIndex][(y * m_outRes + a) * i_stride + x * m_outRes + b];
                                }
                            }
                            l_value /= m_outRes * m_outRes;
                            buffer[y * (i_nx / m_outRes) + x] = l_value;
                        }
                    }
                }
            errorChecking( nc_put_vara_float(m_fileId, l_varIds[l_bufferIndex], start, count, buffer.data()));
            }
        }
        // write the bathymetry only in the first time step
        if (i_bathymetry != nullptr && i_timeIndex == 0){
            for (size_t y = 0; y < i_ny / m_outRes; y++) {
                if (m_outRes == 1){
                    std::memcpy(
                        &buffer[y * i_nx],
                        &i_bathymetry[y * i_stride],
                        i_nx * sizeof(float)
                    );
                }
                else{
                    for (size_t x = 0; x < i_nx / m_outRes; x++){
                        t_real l_value = 0;
                        for (size_t a = 0; a < m_outRes; a++){
                            for (size_t b = 0; b < m_outRes; b++){
                                l_value += i_bathymetry[(y * m_outRes + a) * i_stride + x * m_outRes + b];
                            }
                        }
                        l_value /= m_outRes * m_outRes;
                        buffer[y * (i_nx / m_outRes) + x] = l_value; 
                    }
                }
            }
            errorChecking( nc_put_vara_float(m_fileId, m_bVarId, startB, countB, buffer.data()));
        }
    }
    else {
        // write data to file
        errorChecking( nc_put_var1_float(m_fileId, m_tVarId, &i_timeIndex, &i_simTime) );
        for (t_idx l_cy = 0; l_cy < i_ny; l_cy++){
            for (t_idx l_cx = 0; l_cx < i_nx; l_cx++){
                t_idx l_index[3] = {i_timeIndex, l_cy, l_cx};
                t_idx l_indexB[2] = {l_cy, l_cx};
                if (i_h != nullptr){
                    errorChecking( nc_put_var1_float( m_fileId, m_hVarId, l_index, &i_h[l_cy * i_stride + l_cx] ) );
                }
                if (i_hu != nullptr){
                    errorChecking( nc_put_var1_float( m_fileId, m_huVarId, l_index, &i_hu[l_cy * i_stride + l_cx] ) );
                }
                if (i_hv != nullptr){
                    errorChecking( nc_put_var1_float( m_fileId, m_hvVarId, l_index, &i_hv[l_cy * i_stride + l_cx] ) );
                }
                if (i_bathymetry != nullptr && i_timeIndex == 0){
                    errorChecking( nc_put_var1_float( m_fileId, m_bVarId, l_indexB, &i_bathymetry[l_cy * i_stride + l_cx] ) );
                }
            }
        }
    }
}

int io::NetCdf::read( const std::string  & i_filePath,
                      t_idx      &  o_cellX,
                      t_idx      &  o_cellY,
                      t_real     &  o_dxy,
                      t_real     &  o_left,
                      t_real     &  o_upper,
                      t_real    **  o_data,
                      bool printErr ){

    int l_ncid;
    int l_varidZ;
    int l_varIdX;
    int l_varIdY;
    int l_status = 0;
    int ndims;
    int dimids[NC_MAX_VAR_DIMS];
    size_t dim_sizes[NC_MAX_VAR_DIMS];
    
    
    l_status = errorChecking( nc_open(i_filePath.c_str(), NC_NOWRITE, &l_ncid), printErr );
    if (l_status) return -1;
    
    l_status = errorChecking( nc_inq_varid(l_ncid, "x", &l_varIdX), printErr );
    if (l_status) return -1;

    l_status = errorChecking( nc_inq_varid(l_ncid, "y", &l_varIdY), printErr );
    if (l_status) return -1;
    
    l_status = errorChecking( nc_inq_varid(l_ncid, "z", &l_varidZ), printErr );
    if (l_status) return -1;
    l_status = errorChecking( nc_inq_var(l_ncid, l_varidZ, nullptr, nullptr, &ndims, dimids, nullptr), printErr );
    if (l_status) return -1;
    l_status = errorChecking( nc_inq_dimlen(l_ncid, dimids[0], &dim_sizes[0]), printErr );
    if (l_status) return -1;
    l_status = errorChecking( nc_inq_dimlen(l_ncid, dimids[1], &dim_sizes[1]), printErr );
    if (l_status) return -1;

    o_cellX = dim_sizes[1];
    o_cellY = dim_sizes[0];
    *o_data = new t_real[o_cellX * o_cellY];
    
    float l_nextToLeft;
    size_t l_coord[] = {0};
    size_t l_coord1[] = {1};
    l_status = errorChecking( nc_get_var1_float(l_ncid, l_varIdX, l_coord, &o_left), printErr );
    if (l_status) return -1;
    l_status = errorChecking ( nc_get_var1_float(l_ncid, l_varIdX, l_coord1, &l_nextToLeft), printErr );
    if (l_status) return -1;
    l_status = errorChecking( nc_get_var1_float(l_ncid, l_varIdY, l_coord, &o_upper), printErr );
    if (l_status) return -1;
    o_dxy = l_nextToLeft - o_left;

    l_status = errorChecking( nc_get_var_float(l_ncid, l_varidZ, *o_data), printErr );
    if (l_status) return -1;
    l_status = errorChecking( nc_close(l_ncid), printErr );
    return l_status;
}  