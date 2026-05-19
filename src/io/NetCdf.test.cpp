#include <catch2/catch.hpp>
#include <sstream>
#include <string>
#define private public
#include "NetCdf.h"
#undef public

using namespace tsunami_lab;

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

//TBD: implement tests for NetCDF with ghost cells (l_nx + 2 == l_stride)

TEST_CASE( "test the NetCdf displacement reader.", "[NetCdf]"){
    t_real l_dis[100*100];
    for (int l_y = 0; l_y < 100; l_y++){
        for (int l_x = 0; l_x < 100; l_x++){
            l_dis[l_x + l_y * 100] = 0;
        }
    }

    io::NetCdf::readDisplacement(100,100,"utilities/artificialtsunami_displ_1000.nc", l_dis, 0);
    for (int l_y = 20; l_y < 100; l_y+=50){
        for (int l_x = 0; l_x < 100; l_x+=50){
            REQUIRE( l_dis[l_y * 100 + l_x] != 0);
        }
    }

    SECTION( "Test dimension error." ){
        int ret = io::NetCdf::readDisplacement(100,99,"utilities/artificialtsunami_displ_1000.nc", l_dis, 0);
        REQUIRE(ret == -1);

        ret = io::NetCdf::readDisplacement(99,100,"utilities/artificialtsunami_displ_1000.nc", l_dis, 0);
        REQUIRE(ret == -1);

        ret = io::NetCdf::readDisplacement(101,100,"utilities/artificialtsunami_displ_1000.nc", l_dis, 0);
        REQUIRE(ret == -1);

        ret = io::NetCdf::readDisplacement(100,101,"utilities/artificialtsunami_displ_1000.nc", l_dis, 0);
        REQUIRE(ret == -1);
    }
}

TEST_CASE( "test the NetCdf bathymetry reader.", "[NetCd2f]"){
    t_real l_bathy[1000*1000];
    for (int l_y = 0; l_y < 1000; l_y++){
        for (int l_x = 0; l_x < 1000; l_x++){
            l_bathy[l_x + l_y * 1000] = 0;
        }
    }

    io::NetCdf::readBathymetry(1000,1000,"utilities/artificialtsunami_bathymetry_1000.nc", l_bathy, 0);
    for (int l_y = 20; l_y < 1000; l_y+=500){
        for (int l_x = 0; l_x < 1000; l_x+=500){
            REQUIRE( l_bathy[l_y * 1000 + l_x] != 0);
        }
    }

    SECTION( "Test dimension error." ){
        int ret = io::NetCdf::readBathymetry(1000,999,"utilities/artificialtsunami_bathymetry_1000.nc", l_bathy, 0);
        REQUIRE(ret == -1);

        ret = io::NetCdf::readBathymetry(999,1000,"utilities/artificialtsunami_bathymetry_1000.nc", l_bathy, 0);
        REQUIRE(ret == -1);

        ret = io::NetCdf::readBathymetry(1001,1000,"utilities/artificialtsunami_bathymetry_1000.nc", l_bathy, 0);
        REQUIRE(ret == -1);

        ret = io::NetCdf::readBathymetry(1000,1001,"utilities/artificialtsunami_bathymetry_1000.nc", l_bathy, 0);
        REQUIRE(ret == -1);
    }
}

TEST_CASE( "test the dynamic read method", "[NetCdf]"){
    t_real * l_data = nullptr;

    t_idx l_x = 0;
    t_idx l_y = 0;

    io::NetCdf::read("utilities/artificialtsunami_bathymetry_1000.nc", l_x, l_y, &l_data);

    REQUIRE( l_x == 1000 );
    REQUIRE( l_y == 1000 );

    REQUIRE( l_data[0] == -100 );
    REQUIRE( l_data[l_x * l_y - 1] == -100 );

    delete[] l_data;
}