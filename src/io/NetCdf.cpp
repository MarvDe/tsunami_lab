#include "NetCdf.h"

using namespace tsunami_lab;

int io::NetCdf::readDisplacement(  t_idx i_cellX,
                                    t_idx i_cellY,
                                    std::string i_filePath,
                                    t_real * o_displacement,
                                    t_idx printErr){
    int l_ncid;
    int l_status;
    int l_varidZ;
    int ndims;
    int dimids[NC_MAX_VAR_DIMS];
    size_t dim_sizes[NC_MAX_VAR_DIMS];
                                    
    l_status = nc_open(i_filePath.c_str(), NC_NOWRITE, &l_ncid);

    if (l_status != NC_NOERR){
        printf("Fehler beim Öffnen: %s\n", nc_strerror(l_status));
        return -1;
    }


    l_status = nc_inq_varid(l_ncid, "z", &l_varidZ);
    if (l_status != NC_NOERR){
        if (printErr){
            printf("Variable nicht gefunden: %s\n", nc_strerror(l_status));
        }
        return -1;
    }


    l_status = nc_inq_var(l_ncid, l_varidZ, nullptr, nullptr, &ndims, dimids, nullptr);
    if (l_status != NC_NOERR) {
        if (printErr){
            printf("Var info error: %s\n", nc_strerror(l_status));
        }
        return -1;
    }

    nc_inq_dimlen(l_ncid, dimids[0], &dim_sizes[0]);
    nc_inq_dimlen(l_ncid, dimids[1], &dim_sizes[1]);

    size_t nx = dim_sizes[1];
    size_t ny = dim_sizes[0];

    if (dim_sizes[2] != 0 || nx != i_cellX || ny != i_cellY){
        if (printErr){
            printf("Error: NetCdf dimension mismatch.\n");
        }
        return -1;
    }

    l_status = nc_get_var_float(l_ncid, l_varidZ, o_displacement);
    if (l_status != NC_NOERR){
        if (printErr){   
            printf("Read error %s\n", nc_strerror(l_status));
        }
        return -1;
    }

    nc_close(l_ncid);
    return 0;
}

int io::NetCdf::readBathymetry(t_idx i_cellX,
                                t_idx i_cellY,
                                std::string i_filePath,
                                t_real * o_bathymetry,
                                t_idx printErr){
    int l_ncid;
    int l_status;
    int l_varidZ;
    int ndims;
    int dimids[NC_MAX_VAR_DIMS];
    size_t dim_sizes[NC_MAX_VAR_DIMS];
                                    
    l_status = nc_open(i_filePath.c_str(), NC_NOWRITE, &l_ncid);

    if (l_status != NC_NOERR){
        if(printErr){
            printf("Fehler beim Öffnen: %s\n", nc_strerror(l_status));
        }
        return -1;
    }


    l_status = nc_inq_varid(l_ncid, "z", &l_varidZ);
    if (l_status != NC_NOERR){
        if(printErr){
            printf("Variable nicht gefunden: %s\n", nc_strerror(l_status));
        }
        return -1;
    }

    l_status = nc_inq_var(l_ncid, l_varidZ, nullptr, nullptr, &ndims, dimids, nullptr);
    if (l_status != NC_NOERR) {
        if(printErr){
            printf("Var info error: %s\n", nc_strerror(l_status));
        }
        return -1;
    }

    nc_inq_dimlen(l_ncid, dimids[0], &dim_sizes[0]);
    nc_inq_dimlen(l_ncid, dimids[1], &dim_sizes[1]);

    size_t nx = dim_sizes[1];
    size_t ny = dim_sizes[0];

    if (dim_sizes[2] != 0 || nx != i_cellX || ny != i_cellY){
        if(printErr){
            printf("Error: NetCdf dimension mismatch.\n");
        }
        return -1;
    }


    l_status = nc_get_var_float(l_ncid, l_varidZ, o_bathymetry);
    if (l_status != NC_NOERR){
        if(printErr){
            printf("Read error %s\n", nc_strerror(l_status));
        }
        return -1;
    }

    nc_close(l_ncid);
    return 0;
}