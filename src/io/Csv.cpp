/**
 * @author Alexander Breuer (alex.breuer AT uni-jena.de)
 *
 * @section DESCRIPTION
 * IO-routines for writing a snapshot as Comma Separated Values (CSV).
 **/
#include "Csv.h"

void tsunami_lab::io::Csv::write( t_real               i_dxy,
                                  t_idx                i_nx,
                                  t_idx                i_ny,
                                  t_idx                i_stride,
                                  t_real       const * i_h,
                                  t_real       const * i_hu,
                                  t_real       const * i_hv,
                                  t_real       const * i_bathymetry,
                                  std::ostream       & io_stream ) {
  // write the CSV header
  io_stream << "x,y";
  if( i_h  != nullptr ) io_stream << ",height";
  if( i_hu != nullptr ) io_stream << ",momentum_x";
  if( i_hv != nullptr ) io_stream << ",momentum_y";
  if( i_bathymetry != nullptr ) io_stream << ",bathymetry";
  io_stream << "\n";

  // iterate over all cells
  for( t_idx l_iy = 0; l_iy < i_ny; l_iy++ ) {
    for( t_idx l_ix = 0; l_ix < i_nx; l_ix++ ) {
      // derive coordinates of cell center
      t_real l_posX = (l_ix + 0.5) * i_dxy;
      t_real l_posY = (l_iy + 0.5) * i_dxy;

      t_idx l_id = l_iy * i_stride + l_ix;

      // write data
      io_stream << l_posX << "," << l_posY;
      if( i_h  != nullptr ) io_stream << "," << i_h[l_id];
      if( i_hu != nullptr ) io_stream << "," << i_hu[l_id];
      if( i_hv != nullptr ) io_stream << "," << i_hv[l_id];
      if( i_bathymetry != nullptr ) io_stream << "," << i_bathymetry[l_id];
      io_stream << "\n";
    }
  }
  io_stream << std::flush;
}

void tsunami_lab::io::Csv::readBathymetry( t_idx             i_cellx,
                                           std::istream &    i_stream,
                                           t_real       *    o_bathymetry){
  
  std::string l_entry;
  
  for ( t_idx l_i = 0; l_i < i_cellx; l_i++){
    // read line of file
    if (getline(i_stream, l_entry)){
      // find last comma in line
      t_idx l_pos = l_entry.find_last_of(',');
      // convert height from string to float
      t_real l_height = atof(l_entry.substr(l_pos + 1).c_str());
      // set height in array
      o_bathymetry[l_i] = l_height;
    } 
    else return;  
  }
}