#include "CheckPoint.h"
#include <iostream>
#include <cmath>
#include <algorithm>

using namespace tsunami_lab;

int errorChecking(int i_errId, bool i_printErr){
    if (i_errId != NC_NOERR){
        if (i_printErr){
            throw std::runtime_error(std::string("NC Error: ") + nc_strerror(i_errId));
        }
        return -1;
    }
    return 0;
}

void setups::CheckPoint::readNetCdfFile(std::string i_inputFile,
                                        t_real &o_lastSimTime,
                                        t_idx &o_lastTimeStep,
                                        t_idx &o_nx,
                                        t_idx &o_ny,
                                        t_real &o_dxy,
                                        t_real &o_globalX,
                                        t_real &o_globalY
){
    int l_ncid;
    int l_varIdHeight;
    int l_varIdHu;
    int l_varIdHv;
    int l_varIdBathy;

    int l_varIdX;
    int l_varIdY;
    
    int l_varIdSimTime;
    int l_timeDimId;
    size_t l_ntimes;

    errorChecking( nc_open(i_inputFile.c_str(), NC_NOWRITE, &l_ncid), true);

    // get the last time index
    errorChecking( nc_inq_dimid(l_ncid, "time", &l_timeDimId), true);
    errorChecking( nc_inq_dimlen(l_ncid, l_timeDimId, &l_ntimes), true);
    if (l_ntimes == 0){
        std::cout << "Keine Zeitschritte Vorhanden!.\n";
        nc_close(l_ncid);
        return; 
    }
    o_lastTimeStep = l_ntimes - 1;

    // get the last simtime
    errorChecking( nc_inq_varid(l_ncid, "time", &l_varIdSimTime), true);

    size_t l_start[1] = { l_ntimes - 1 };
    //size_t l_count[1] = { 1 };
    
    float l_lastTime;
    errorChecking( nc_get_var1_float(l_ncid, l_varIdSimTime, l_start, &l_lastTime), true);
    o_lastSimTime = l_lastTime;

    // get the dims of the simulation
    int l_dimIdX;
    int l_dimIdY;
    errorChecking( nc_inq_dimid(l_ncid, "x", &l_dimIdX), true);
    errorChecking( nc_inq_dimid(l_ncid, "y", &l_dimIdY), true);

    size_t l_nx;
    size_t l_ny;
    errorChecking( nc_inq_dimlen(l_ncid, l_dimIdX, &l_nx), true);
    errorChecking( nc_inq_dimlen(l_ncid, l_dimIdY, &l_ny), true);

    o_nx = l_nx;
    o_ny = l_ny;

    m_stride = l_nx;

    // set up coords for vars
    size_t l_startVars[3] = { l_ntimes - 1, 0, 0};
    size_t l_countVars[3] = {1, l_ny, l_nx};

    size_t l_startBathy[2] = {0, 0};
    size_t l_countBathy[2] = {l_ny, l_nx};


    // collect vars and push to heap

    errorChecking( nc_inq_varid(l_ncid, "h", &l_varIdHeight), true);
    m_height = new t_real[l_nx * l_ny];
    errorChecking( nc_get_vara_float(l_ncid, l_varIdHeight, l_startVars, l_countVars, m_height), true);

    errorChecking( nc_inq_varid(l_ncid, "hu", &l_varIdHu), true);
    m_momentumX = new t_real[l_nx * l_ny];
    errorChecking( nc_get_vara_float(l_ncid, l_varIdHu, l_startVars, l_countVars, m_momentumX), true);

    errorChecking( nc_inq_varid(l_ncid, "hv", &l_varIdHv), true);
    m_momentumY = new t_real[l_nx * l_ny];
    errorChecking( nc_get_vara_float(l_ncid, l_varIdHv, l_startVars, l_countVars, m_momentumY), true);

    errorChecking( nc_inq_varid(l_ncid, "b", &l_varIdBathy), true);
    m_bathymetry = new t_real[l_nx * l_ny];
    errorChecking( nc_get_vara_float(l_ncid, l_varIdBathy, l_startBathy, l_countBathy, m_bathymetry), true);

    // calculate l_dxy with x
    errorChecking( nc_inq_varid(l_ncid, "x", &l_varIdX), true);

    t_real l_x_0_0;
    t_real l_x_0_1;

    size_t l_coord_0_0[1] = {0};
    size_t l_coord_0_1[1] = {1};


    errorChecking( nc_get_var1_float(l_ncid, l_varIdX, l_coord_0_0, &l_x_0_0), true);
    errorChecking( nc_get_var1_float(l_ncid, l_varIdX, l_coord_0_1, &l_x_0_1), true);

    t_real l_dxy = l_x_0_1 - l_x_0_0;
    o_dxy = l_dxy;

    // get global coords (x[0], y[0])
    errorChecking( nc_inq_varid(l_ncid, "y", &l_varIdY), true);

    t_real l_y_0_0;

    errorChecking( nc_get_var1_float(l_ncid, l_varIdY, l_coord_0_0, &l_y_0_0), true);

    o_globalX = l_x_0_0;
    o_globalY = l_y_0_0;

    nc_close(l_ncid);
}

t_real setups::CheckPoint::getHeight(t_real i_x, t_real i_y) const {
    t_idx l_x = (t_idx) (i_x + 0.5);
    t_idx l_y = (t_idx) (i_y + 0.5);
    
    return m_height[l_y * m_stride + l_x];
}

t_real setups::CheckPoint::getMomentumX(t_real i_x, t_real i_y) const {
    t_idx l_x = (t_idx) (i_x + 0.5);
    t_idx l_y = (t_idx) (i_y + 0.5);
    
    return m_momentumX[l_y * m_stride + l_x];
}

t_real setups::CheckPoint::getMomentumY(t_real i_x, t_real i_y) const {
    t_idx l_x = (t_idx) (i_x + 0.5);
    t_idx l_y = (t_idx) (i_y + 0.5);
    
    return m_momentumY[l_y * m_stride + l_x];
}

t_real setups::CheckPoint::getBathymetry(t_real i_x, t_real i_y) const {
    t_idx l_x = (t_idx) (i_x + 0.5);
    t_idx l_y = (t_idx) (i_y + 0.5);
    
    return m_bathymetry[l_y * m_stride + l_x];
}