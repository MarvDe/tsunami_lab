#include "NetCdf.h"

using namespace tsunami_lab;

void io::NetCdf::readDisplacement(  t_idx,
                                    t_idx,
                                    std::string i_filePath,
                                    t_real * o_displacement){
    int l_ncid;
    int l_status;
                                    
    l_status = nc_open(i_filePath.c_str(), NC_NOWRITE, &l_ncid);

    if (l_status != NC_NOERR){
        printf("Fehler beim Öffnen: %s\n", nc_strerror(l_status));
        return;
    }

    int l_varidZ;

    l_status = nc_inq_varid(l_ncid, "z", &l_varidZ);
    if (l_status != NC_NOERR){
        printf("Variable nicht gefunden: %s\n", nc_strerror(l_status));
        return;
    }


    l_status = nc_get_var_float(l_ncid, l_varidZ, o_displacement);
    if (l_status != NC_NOERR){
        printf("Read error %s\n", nc_strerror(l_status));
        return;
    }

    nc_close(l_ncid);
}