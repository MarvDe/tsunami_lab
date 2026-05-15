#include <catch2/catch.hpp>
#include <sstream>
#include <string>
#define private public
#include "NetCdf.h"
#undef public


TEST_CASE( "Test netcdf.", "[NetCdf]" ){
    /*
    * Testing writing of nc file.
    *   
    */

    const tsunami_lab::t_idx l_nx = 10;
    const tsunami_lab::t_idx l_ny = 10;
    std::string l_fileName = "nc_test.nc";

    auto *l_nc = new tsunami_lab::io::NetCdf(l_nx, l_ny, 1, 1, l_fileName);
    
    tsunami_lab::t_real l_h[l_nx * l_ny] = {1};
    tsunami_lab::t_real l_hu[l_nx * l_ny] = {2};
    tsunami_lab::t_real l_hv[l_nx * l_ny] = {3};
    tsunami_lab::t_real l_b[l_nx * l_ny] = {4};

    for (tsunami_lab::t_idx l_i = 0; l_i < 3; l_i++){
        l_nc->write(l_nx, l_ny, l_i, l_nx, l_h, l_hu, l_hv, l_b);
    }
    delete l_nc;

    int l_fileId;
    nc_open(l_fileName.c_str(), NC_NOCLOBBER, &l_fileId);
    int l_xDimId, l_yDimId, l_tDimId;
    int l_xVarId, l_yVarId, l_tVarId;
    int l_hVarId, l_huVarId, l_hvVarId, l_bVarId;

    nc_inq_dimid(l_fileId, "x", &l_xDimId);
    nc_inq_dimid(l_fileId, "y", &l_yDimId);
    nc_inq_dimid(l_fileId, "t", &l_tDimId);

    nc_inq_varid(l_fileId, "x", &l_xVarId);
    nc_inq_varid(l_fileId, "y", &l_yVarId);
    nc_inq_varid(l_fileId, "t", &l_tVarId);

    nc_inq_varid(l_fileId, "h", &l_hVarId);
    nc_inq_varid(l_fileId, "hu", &l_huVarId);
    nc_inq_varid(l_fileId, "hv", &l_hvVarId);
    nc_inq_varid(l_fileId, "b", &l_bVarId);

    for (tsunami_lab::t_idx l_ix = 0; l_ix < l_nx; l_ix++){
        tsunami_lab::t_real l_x;
        nc_get_var1_float(l_fileId, l_xDimId, &l_ix, &l_x);
        REQUIRE( l_x == l_ix );
    }

    for (tsunami_lab::t_idx l_iy = 0; l_iy < l_ny; l_iy++){
        tsunami_lab::t_real l_y;
        nc_get_var1_float(l_fileId, l_yDimId, &l_iy, &l_y);
        REQUIRE( l_y == l_iy );
    }

    for (tsunami_lab::t_idx l_it = 0; l_it < l_ny; l_it++){
        tsunami_lab::t_real l_t;
        nc_get_var1_float(l_fileId, l_yDimId, &l_it, &l_t);
        REQUIRE( l_t == l_it );
    }
    
    tsunami_lab::t_real l_h1, l_hu1, l_hv1, l_b1;
    tsunami_lab::t_idx l_i[3] = {0, 0, 0};

    nc_get_var1_float(l_fileId, l_hVarId, l_i, &l_h1);
    REQUIRE( l_h1 == l_h[0]);

    nc_get_var1_float(l_fileId, l_huVarId, l_i, &l_hu1);
    REQUIRE( l_hu1 == l_hu[0]);

    nc_get_var1_float(l_fileId, l_hvVarId, l_i, &l_hv1);
    REQUIRE( l_hv1 == l_hv[0]);

    nc_get_var1_float(l_fileId, l_bVarId, l_i, &l_b1);
    REQUIRE( l_b1 == l_b[0]);
    
    nc_close(l_fileId);
    remove(l_fileName.c_str());
}