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
     * Reads a NetCdf file and stores values into vars.
     * @param i_inputFile path to input NetCdf File.
     * @param o_lastSimTime last fully filled time in the simulation
     * @param o_lastTimeStep last fully filled timestep in the simulation
     * @param o_nx cells in x direction in simulation
     * @param o_ny cells in y direction in simulation
     * @param o_dxy cell size
     * @param o_globalX X value for global coord (top left corner)
     * @param o_globalY Y value for global coord (top left corner.
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
     * Constructor
     * @param i_inputFile path to input NetCdf File.
     * @param o_lastSimTime last fully filled time in the simulation
     * @param o_lastTimeStep last fully filled time step in the simulation
     * @param o_nx cells in x direction in simulation
     * @param o_ny cells in y direction in simulation
     * @param o_dxy cell size
     * @param o_globalX X value for global coord (top left corner)
     * @param o_globalY Y value for global coord (top left corner)
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
     * Destructor
     */
    ~CheckPoint(){
        delete[] m_bathymetry;
        delete[] m_height;
        delete[] m_momentumX;
        delete[] m_momentumY;
    }
    
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
     * gets the bathymetry.
     *
     * @param i_x x-coordinate of the queried point.
     * @param i_y y-coordinate of the queried point.
     * @return Bathymetry.
     **/
    t_real getBathymetry( t_real i_x,
                                  t_real i_y ) const;
};

#endif