/**
 * @author Marvin Doering
 *
 * @section DESCRIPTION
 * IO-routines handling netcdf files.
 **/
#include "NetCdf.h"


void tsunami_lab::io::NetCdf::errorChecking(int i_errId){
    if (i_errId != NC_NOERR){
        std::cerr << nc_strerror(i_errId) << std::endl;
    }
}


void tsunami_lab::io::NetCdf::create(t_idx i_nx, t_idx i_ny, const std::string & i_filePath, NetCdfFile * o_file){
    o_file = nullptr;
    // check if file path ends with '.nc'
    if (i_filePath.length() < 3 ||
        i_filePath[i_filePath.length() - 3] != '.' ||
        i_filePath[i_filePath.length() - 2] != 'n' ||
        i_filePath[i_filePath.length() - 1] != 'c'){
            std::cerr << "netcdf file should end with '.nc'" << std::endl;
            return;
    }

    int l_fileId;    
    errorChecking( nc_create(i_filePath.c_str(), NC_CLOBBER, &l_fileId) );
    // creating dimensions
    int l_xDimId, l_yDimId, l_tDimId; 
    errorChecking( nc_def_dim(l_fileId, "x", i_nx, &l_xDimId) );
    errorChecking( nc_def_dim(l_fileId, "y", i_nx, &l_yDimId) );
    errorChecking( nc_def_dim(l_fileId, "t", NC_UNLIMITED, &l_tDimId) );

    // creating variables
    int l_hVarId, l_huVarId, l_hvVarId, l_bVarId;
    int l_dimIds[3] = {l_tDimId, l_yDimId, l_xDimId};
    
    errorChecking( nc_def_var(l_fileId, "h", NC_FLOAT, 3, l_dimIds, &l_hVarId) );
    errorChecking( nc_put_att_schar(l_fileId, l_hVarId, "units", NC_UBYTE, 1, "m") );

    errorChecking( nc_def_var(l_fileId, "hu", NC_FLOAT, 3, l_dimIds, &l_huVarId) );
    errorChecking( nc_put_att_schar(l_fileId, l_hVarId, "units", NC_UBYTE, 6, "kg*m/s") );
    
    errorChecking( nc_def_var(l_fileId, "hv", NC_FLOAT, 3, l_dimIds, &l_hvVarId) );
    errorChecking( nc_put_att_schar(l_fileId, l_hVarId, "units", NC_UBYTE, 6, "kg*m/s") );
    
    errorChecking( nc_def_var(l_fileId, "b", NC_FLOAT, 3, l_dimIds, &l_bVarId) );
    errorChecking( nc_put_att_schar(l_fileId, l_hVarId, "units", NC_UBYTE, 1, "s") );
    
    // register COARDS convention
    const char *l_coardsStr = "COARDS";
    nc_put_att_text( l_fileId, NC_GLOBAL, "Conventions", 6, l_coardsStr);

    nc_enddef(l_fileId);
    *o_file = {l_fileId, l_xDimId, l_yDimId, l_tDimId, l_hVarId, l_huVarId, l_hvVarId, l_bVarId};
}


void tsunami_lab::io::NetCdf::write(t_real               i_dxy,
                                    t_idx                i_nx,
                                    t_idx                i_ny,
                                    t_idx                i_timeStep,
                                    t_idx                i_stride,
                                    t_real       const * i_h,
                                    t_real       const * i_hu,
                                    t_real       const * i_hv,
                                    t_real       const * i_bathymetry,
                                    const NetCdfFile   & i_Ncfile){
    
    int l_fileId = i_Ncfile.m_fileId;
    int l_hVarId = i_Ncfile.m_hVarId;
    int l_huVarId = i_Ncfile.m_huVarId;
    int l_hvVarId = i_Ncfile.m_hvVarId;
    int l_bVarId = i_Ncfile.m_bVarId;

    // write data to file
    for (t_idx l_cy = 0; l_cy < i_ny; l_cy++){
        for (t_idx l_cx = 0; l_cx < i_nx; l_cx++){
            t_idx l_index[3] = {i_timeStep, l_cy, l_cx};
            if (i_h != nullptr){
                errorChecking( nc_put_var1_float( l_fileId, l_hVarId, l_index, &i_h[l_cy * i_stride + l_cx] ) );
            }
            if (i_hu != nullptr){
                errorChecking( nc_put_var1_float( l_fileId, l_huVarId, l_index, &i_hu[l_cy * i_stride + l_cx] ) );
            }
            if (i_hv != nullptr){
                errorChecking( nc_put_var1_float( l_fileId, l_hvVarId, l_index, &i_hv[l_cy * i_stride + l_cx] ) );
            }
            if (i_bathymetry != nullptr){
                errorChecking( nc_put_var1_float( l_fileId, l_bVarId, l_index, &i_bathymetry[l_cy * i_stride + l_cx] ) );
            }
        }
    }
}


void tsunami_lab::io::NetCdf::readBathymetry( tsunami_lab::t_idx     i_cellX,
                                              tsunami_lab::t_idx     i_cellY,
                                              const std::string    & i_filePath,
                                              tsunami_lab::t_real  * o_bathymetry){
    
}


void tsunami_lab::io::NetCdf::readDisplacement( tsunami_lab::t_idx     i_cellX,
                                                tsunami_lab::t_idx     i_cellY,
                                                const std::string    & i_filePath,
                                                tsunami_lab::t_real  * o_displacement){

}
