#include <catch2/catch.hpp>
#include "CircularDamBreak2d.h"
#include <cmath>

TEST_CASE("Test the 2d CircularDamBreak setup.", "[DamBreak2d]") {

  // helper: flat bathymetry array
  constexpr tsunami_lab::t_idx l_cellX = 10;
  constexpr tsunami_lab::t_idx l_cellY = 10;
  constexpr tsunami_lab::t_real l_dxy  = 1.0;
  constexpr tsunami_lab::t_real l_b    = -5.0;  // flat, all below sea level

  tsunami_lab::t_real l_bathymetry[l_cellX * l_cellY];
  for(tsunami_lab::t_idx l_i = 0; l_i < l_cellX * l_cellY; l_i++) {
    l_bathymetry[l_i] = l_b;
  }

  SECTION("Momentum is always zero") {
    tsunami_lab::setups::CircularDamBreak2d l_setup(10.0, l_bathymetry, 3.0,
                                                    l_cellX, l_cellY, l_dxy);
    // inside circle
    REQUIRE(l_setup.getMomentumX(5.5, 5.5) == Approx(0.0));
    REQUIRE(l_setup.getMomentumY(5.5, 5.5) == Approx(0.0));
    // outside circle
    REQUIRE(l_setup.getMomentumX(0.5, 0.5) == Approx(0.0));
    REQUIRE(l_setup.getMomentumY(0.5, 0.5) == Approx(0.0));
  }

  SECTION("Flat bathymetry - height inside circle") {
    // h = heightCircle - b
    constexpr tsunami_lab::t_real l_hCircle = 10.0;
    tsunami_lab::setups::CircularDamBreak2d l_setup(l_hCircle, l_bathymetry, 3.0,
                                                    l_cellX, l_cellY, l_dxy);

    // center of domain = (5,5), points within radius 3 are inside
    REQUIRE(l_setup.getHeight(5.5, 5.5) == Approx(l_hCircle - l_b));  // r ≈ 0
    REQUIRE(l_setup.getHeight(6.5, 5.5) == Approx(l_hCircle - l_b));  // r ≈ 1.5
    REQUIRE(l_setup.getHeight(5.5, 7.0) == Approx(l_hCircle - l_b));  // r ≈ 2.0
  }

  SECTION("Flat bathymetry - height outside circle") {
    // h = -b → surface at eta = 0
    constexpr tsunami_lab::t_real l_hCircle = 10.0;
    tsunami_lab::setups::CircularDamBreak2d l_setup(l_hCircle, l_bathymetry, 3.0,
                                                    l_cellX, l_cellY, l_dxy);

    // corners are far from center, clearly outside
    REQUIRE(l_setup.getHeight(0.5, 0.5) == Approx(-l_b));
    REQUIRE(l_setup.getHeight(9.5, 9.5) == Approx(-l_b));
    REQUIRE(l_setup.getHeight(0.5, 9.5) == Approx(-l_b));
    REQUIRE(l_setup.getHeight(9.5, 0.5) == Approx(-l_b));
  }

  SECTION("Water surface is flat outside circle (eta = h + b = 0)") {
    tsunami_lab::setups::CircularDamBreak2d l_setup(10.0, l_bathymetry, 3.0,
                                                    l_cellX, l_cellY, l_dxy);

    // outside circle: h + b should equal 0
    for(tsunami_lab::t_idx l_iy = 0; l_iy < l_cellY; l_iy++) {
      for(tsunami_lab::t_idx l_ix = 0; l_ix < l_cellX; l_ix++) {
        tsunami_lab::t_real l_x  = l_ix * l_dxy + 0.5;
        tsunami_lab::t_real l_y  = l_iy * l_dxy + 0.5;
        tsunami_lab::t_real l_cx = l_dxy * l_cellX / 2 - l_x;
        tsunami_lab::t_real l_cy = l_dxy * l_cellY / 2 - l_y;
        tsunami_lab::t_real l_r  = std::sqrt(l_cx * l_cx + l_cy * l_cy);

        if(l_r > 3.0) {
          tsunami_lab::t_real l_h    = l_setup.getHeight(l_x, l_y);
          tsunami_lab::t_real l_bath = l_setup.getBathymetry(l_x, l_y);
          REQUIRE(l_h + l_bath == Approx(0.0).margin(1e-6));
        }
      }
    }
  }

  SECTION("Water surface inside circle equals heightCircle (eta = heightCircle)") {
    constexpr tsunami_lab::t_real l_hCircle = 10.0;
    tsunami_lab::setups::CircularDamBreak2d l_setup(l_hCircle, l_bathymetry, 3.0,
                                                    l_cellX, l_cellY, l_dxy);

    for(tsunami_lab::t_idx l_iy = 0; l_iy < l_cellY; l_iy++) {
      for(tsunami_lab::t_idx l_ix = 0; l_ix < l_cellX; l_ix++) {
        tsunami_lab::t_real l_x  = l_ix * l_dxy + 0.5;
        tsunami_lab::t_real l_y  = l_iy * l_dxy + 0.5;
        tsunami_lab::t_real l_cx = l_dxy * l_cellX / 2 - l_x;
        tsunami_lab::t_real l_cy = l_dxy * l_cellY / 2 - l_y;
        tsunami_lab::t_real l_r  = std::sqrt(l_cx * l_cx + l_cy * l_cy);

        if(l_r <= 3.0) {
          tsunami_lab::t_real l_h    = l_setup.getHeight(l_x, l_y);
          tsunami_lab::t_real l_bath = l_setup.getBathymetry(l_x, l_y);
          REQUIRE(l_h + l_bath == Approx(l_hCircle).margin(1e-6));
        }
      }
    }
  }

  SECTION("Uneven bathymetry - surface still correct") {
    // parabolic bathymetry, always negative
    tsunami_lab::t_real l_unevenBath[l_cellX * l_cellY];
    for(tsunami_lab::t_idx l_iy = 0; l_iy < l_cellY; l_iy++) {
      for(tsunami_lab::t_idx l_ix = 0; l_ix < l_cellX; l_ix++) {
        tsunami_lab::t_real l_dx = (tsunami_lab::t_real)l_ix - l_cellX / 2.0;
        tsunami_lab::t_real l_dy = (tsunami_lab::t_real)l_iy - l_cellY / 2.0;
        l_unevenBath[l_ix + l_iy * l_cellX] = -10.0 - (l_dx*l_dx + l_dy*l_dy) * 0.05;
      }
    }

    constexpr tsunami_lab::t_real l_hCircle = 15.0;
    tsunami_lab::setups::CircularDamBreak2d l_setup(l_hCircle, l_unevenBath, 3.0,
                                                    l_cellX, l_cellY, l_dxy);

    for(tsunami_lab::t_idx l_iy = 0; l_iy < l_cellY; l_iy++) {
      for(tsunami_lab::t_idx l_ix = 0; l_ix < l_cellX; l_ix++) {
        tsunami_lab::t_real l_x    = l_ix * l_dxy + 0.5;
        tsunami_lab::t_real l_y    = l_iy * l_dxy + 0.5;
        tsunami_lab::t_real l_h    = l_setup.getHeight(l_x, l_y);
        tsunami_lab::t_real l_bath = l_setup.getBathymetry(l_x, l_y);

        // height must be positive
        REQUIRE(l_h > 0.0);

        // surface = h + b must equal either heightCircle or 0
        tsunami_lab::t_real l_cx = l_dxy * l_cellX / 2 - l_x;
        tsunami_lab::t_real l_cy = l_dxy * l_cellY / 2 - l_y;
        tsunami_lab::t_real l_r  = std::sqrt(l_cx * l_cx + l_cy * l_cy);

        if(l_r <= 3.0) {
          REQUIRE(l_h + l_bath == Approx(l_hCircle).margin(1e-6));
        } else {
          REQUIRE(l_h + l_bath == Approx(0.0).margin(1e-6));
        }
      }
    }
  }

  SECTION("getBathymetry returns correct values") {
    tsunami_lab::t_real l_customBath[l_cellX * l_cellY];
    for(tsunami_lab::t_idx l_iy = 0; l_iy < l_cellY; l_iy++) {
      for(tsunami_lab::t_idx l_ix = 0; l_ix < l_cellX; l_ix++) {
        l_customBath[l_ix + l_iy * l_cellX] = -(tsunami_lab::t_real)(l_ix + l_iy) - 1.0;
      }
    }

    tsunami_lab::setups::CircularDamBreak2d l_setup(10.0, l_customBath, 3.0,
                                                    l_cellX, l_cellY, l_dxy);

    for(tsunami_lab::t_idx l_iy = 0; l_iy < l_cellY; l_iy++) {
      for(tsunami_lab::t_idx l_ix = 0; l_ix < l_cellX; l_ix++) {
        tsunami_lab::t_real l_x        = l_ix * l_dxy + 0.5;
        tsunami_lab::t_real l_y        = l_iy * l_dxy + 0.5;
        tsunami_lab::t_real l_expected = -(tsunami_lab::t_real)(l_ix + l_iy) - 1.0;
        REQUIRE(l_setup.getBathymetry(l_x, l_y) == Approx(l_expected));
      }
    }
  }
}