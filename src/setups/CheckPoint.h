/**
 * @file
 * @brief Setup for resuming a simulation from a NetCDF checkpoint.
 */
#ifndef TSUNAMI_LAB_SETUP_CHECK_POINT
#define TSUNAMI_LAB_SETUP_CHECK_POINT

#include <netcdf.h>
#include <string>
#include "Setup.h"

namespace tsunami_lab {
    namespace setups{
        class CheckPoint;
    }
}

/** Restores a simulation state from a NetCDF checkpoint file. */
class tsunami_lab::setups::CheckPoint : public Setup {
private:
    //! bathymetry
    t_real* m_bathymetry = nullptr;

    //! height of water
    t_real* m_height = nullptr;

    //! momentum in x direction
    t_real* m_momentumX = nullptr;

    //! momentum in y direction
    t_real* m_momentumY = nullptr;

    //! stride
    t_idx m_stride;

    /**
     * Reads the state and grid metadata from a NetCDF checkpoint file.
     *
     * @param i_inputFile path to the input NetCDF file.
     * @param o_lastSimTime latest complete simulation time.
     * @param o_lastTimeStep latest complete simulation time step.
     * @param o_nx number of simulation cells in x-direction.
     * @param o_ny number of simulation cells in y-direction.
     * @param o_dxy cell size.
     * @param o_globalX x-coordinate of the upper-left grid cell.
     * @param o_globalY y-coordinate of the upper-left grid cell.
     */
    void readNetCdfFile(std::string i_inputFile,
                        t_real &o_lastSimTime, 
                        t_idx &o_lastTimeStep,
                        t_idx &o_nx,
                        t_idx &o_ny,
                        t_real &o_dxy,
                        t_real &o_globalX,
                        t_real &o_globalY
                    );

public:
    /**
     * Constructs the setup from a NetCDF checkpoint file.
     *
     * @param i_inputFile path to the input NetCDF file.
     * @param o_lastSimTime latest complete simulation time.
     * @param o_lastTimeStep latest complete simulation time step.
     * @param o_nx number of simulation cells in x-direction.
     * @param o_ny number of simulation cells in y-direction.
     * @param o_dxy cell size.
     * @param o_globalX x-coordinate of the upper-left grid cell.
     * @param o_globalY y-coordinate of the upper-left grid cell.
     * 
     */
    CheckPoint( std::string i_inputFile,
                t_real &o_lastSimTime,
                t_idx &o_lastTimeStep,
                t_idx &o_nx,
                t_idx &o_ny,
                t_real &o_dxy,
                t_real &o_globalX,
                t_real &o_globalY
    ){
        readNetCdfFile( i_inputFile,
                        o_lastSimTime,
                        o_lastTimeStep,
                        o_nx,
                        o_ny,
                        o_dxy,
                        o_globalX,
                        o_globalY
                    );
    };

    /**
     * Frees the restored state arrays.
     */
    ~CheckPoint(){
        delete[] m_bathymetry;
        delete[] m_height;
        delete[] m_momentumX;
        delete[] m_momentumY;
    }

    /**
     * Gets the YAML argument definition of the setup.
     *
     * @return setup argument definition.
     **/
    static tsunami_lab::io::SetupDef getSetupDef();
    
    /**
     * Gets the water height at a given point.
     *
     * @param i_x x-coordinate of the queried point.
     * @param i_y y-coordinate of the queried point.
     * @return water height at the given point.
     **/
    t_real getHeight( t_real i_x,
                              t_real i_y ) const;

    /**
     * Gets the momentum in x-direction.
     *
     * @param i_x x-coordinate of the queried point.
     * @param i_y y-coordinate of the queried point.
     * @return momentum in x-direction.
     **/
    t_real getMomentumX( t_real i_x,
                                 t_real i_y ) const;

    /**
     * Gets the momentum in y-direction.
     *
     * @param i_x x-coordinate of the queried point.
     * @param i_y y-coordinate of the queried point.
     * @return momentum in y-direction.
     **/
    t_real getMomentumY( t_real i_x,
                                 t_real i_y ) const;
    
    /**
     * Gets the bathymetry.
     *
     * @param i_x x-coordinate of the queried point.
     * @param i_y y-coordinate of the queried point.
     * @return bathymetry at the given point.
     **/
    t_real getBathymetry( t_real i_x,
                                  t_real i_y ) const;
};

#endif
