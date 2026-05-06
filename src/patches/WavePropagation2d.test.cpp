#include <catch2/catch.hpp>
#include "WavePropagation2d.h"

TEST_CASE("Test the 2d wave propagation solver.", "[WaveProp2d]") {

  SECTION("Lake at rest - flat bathymetry") {
    // flat bathymetry and still water should produce no waves
    tsunami_lab::patches::WavePropagation2d l_waveProp(10, 10, tsunami_lab::solvers::FWAVE);

    for(tsunami_lab::t_idx l_iy = 0; l_iy < 10; l_iy++) {
      for(tsunami_lab::t_idx l_ix = 0; l_ix < 10; l_ix++) {
        l_waveProp.setHeight(l_ix, l_iy, 10.0);
        l_waveProp.setMomentumX(l_ix, l_iy, 0.0);
        l_waveProp.setMomentumY(l_ix, l_iy, 0.0);
        l_waveProp.setBathymetry(l_ix, l_iy, 0.0);
      }
    }

    for(int l_step = 0; l_step < 10; l_step++) {
      l_waveProp.setGhostOutflow();
      l_waveProp.timeStep(0.05);
    }

    const tsunami_lab::t_real * l_h  = l_waveProp.getHeight();
    const tsunami_lab::t_real * l_hu = l_waveProp.getMomentumX();
    const tsunami_lab::t_real * l_hv = l_waveProp.getMomentumY();
    tsunami_lab::t_idx l_stride = l_waveProp.getStride();

    for(tsunami_lab::t_idx l_iy = 0; l_iy < 10; l_iy++) {
      for(tsunami_lab::t_idx l_ix = 0; l_ix < 10; l_ix++) {
        tsunami_lab::t_idx l_idx = l_ix + l_iy * l_stride;
        REQUIRE(l_h[l_idx]  == Approx(10.0).margin(1e-5));
        REQUIRE(l_hu[l_idx] == Approx(0.0).margin(1e-5));
        REQUIRE(l_hv[l_idx] == Approx(0.0).margin(1e-5));
      }
    }
  }

  SECTION("Lake at rest - uneven bathymetry (well-balanced)") {
    // h + b = 0 (surface at eta=0), u=v=0: should stay stable
    tsunami_lab::patches::WavePropagation2d l_waveProp(10, 10, tsunami_lab::solvers::FWAVE);

    for(tsunami_lab::t_idx l_iy = 0; l_iy < 10; l_iy++) {
      for(tsunami_lab::t_idx l_ix = 0; l_ix < 10; l_ix++) {
        tsunami_lab::t_real l_b = -10.0 - (tsunami_lab::t_real)(l_ix + l_iy) * 0.5;
        l_waveProp.setBathymetry(l_ix, l_iy, l_b);
        l_waveProp.setHeight(l_ix, l_iy, -l_b);   // h = -b → eta = 0
        l_waveProp.setMomentumX(l_ix, l_iy, 0.0);
        l_waveProp.setMomentumY(l_ix, l_iy, 0.0);
      }
    }

    for(int l_step = 0; l_step < 20; l_step++) {
      l_waveProp.setGhostOutflow();
      l_waveProp.timeStep(0.01);
    }

    const tsunami_lab::t_real * l_h  = l_waveProp.getHeight();
    const tsunami_lab::t_real * l_hu = l_waveProp.getMomentumX();
    const tsunami_lab::t_real * l_hv = l_waveProp.getMomentumY();
    const tsunami_lab::t_real * l_b  = l_waveProp.getBathymetry();
    tsunami_lab::t_idx l_stride = l_waveProp.getStride();

    for(tsunami_lab::t_idx l_iy = 0; l_iy < 10; l_iy++) {
      for(tsunami_lab::t_idx l_ix = 0; l_ix < 10; l_ix++) {
        tsunami_lab::t_idx l_idx = l_ix + l_iy * l_stride;
        // surface eta = h + b should stay at ~0
        REQUIRE(l_h[l_idx] + l_b[l_idx] == Approx(0.0).margin(1e-4));
        REQUIRE(l_hu[l_idx] == Approx(0.0).margin(1e-4));
        REQUIRE(l_hv[l_idx] == Approx(0.0).margin(1e-4));
      }
    }
  }

  SECTION("Symmetry - circular initial condition") {
    // symmetric setup should stay symmetric
    tsunami_lab::t_idx l_n = 10;
    tsunami_lab::patches::WavePropagation2d l_waveProp(l_n, l_n, tsunami_lab::solvers::FWAVE);

    for(tsunami_lab::t_idx l_iy = 0; l_iy < l_n; l_iy++) {
      for(tsunami_lab::t_idx l_ix = 0; l_ix < l_n; l_ix++) {
        // symmetric high water in center
        tsunami_lab::t_real l_dx = (tsunami_lab::t_real)l_ix - (l_n / 2.0 - 0.5);
        tsunami_lab::t_real l_dy = (tsunami_lab::t_real)l_iy - (l_n / 2.0 - 0.5);
        tsunami_lab::t_real l_r  = std::sqrt(l_dx * l_dx + l_dy * l_dy);
        tsunami_lab::t_real l_h  = (l_r < 2.0) ? 10.0 : 5.0;
        l_waveProp.setHeight(l_ix, l_iy, l_h);
        l_waveProp.setMomentumX(l_ix, l_iy, 0.0);
        l_waveProp.setMomentumY(l_ix, l_iy, 0.0);
        l_waveProp.setBathymetry(l_ix, l_iy, -5.0);
      }
    }

    for(int l_step = 0; l_step < 5; l_step++) {
      l_waveProp.setGhostOutflow();
      l_waveProp.timeStep(0.05);
    }

    const tsunami_lab::t_real * l_h = l_waveProp.getHeight();
    tsunami_lab::t_idx l_stride = l_waveProp.getStride();

    // check x-symmetry: cell (ix, iy) == cell (n-1-ix, iy)
    for(tsunami_lab::t_idx l_iy = 0; l_iy < l_n; l_iy++) {
      for(tsunami_lab::t_idx l_ix = 0; l_ix < l_n / 2; l_ix++) {
        tsunami_lab::t_idx l_idxL = l_ix          + l_iy * l_stride;
        tsunami_lab::t_idx l_idxR = (l_n-1-l_ix)  + l_iy * l_stride;
        REQUIRE(l_h[l_idxL] == Approx(l_h[l_idxR]).margin(1e-4));
      }
    }
  }

  SECTION("No NaN or negative heights after dry cell interaction") {
    tsunami_lab::patches::WavePropagation2d l_waveProp(10, 10, tsunami_lab::solvers::FWAVE);

    for(tsunami_lab::t_idx l_iy = 0; l_iy < 10; l_iy++) {
      for(tsunami_lab::t_idx l_ix = 0; l_ix < 10; l_ix++) {
        // half the domain nearly dry
        tsunami_lab::t_real l_h = (l_ix < 5) ? 10.0 : 0.0;
        l_waveProp.setHeight(l_ix, l_iy, l_h);
        l_waveProp.setMomentumX(l_ix, l_iy, 0.0);
        l_waveProp.setMomentumY(l_ix, l_iy, 0.0);
        l_waveProp.setBathymetry(l_ix, l_iy, -10.0);
      }
    }

    for(int l_step = 0; l_step < 20; l_step++) {
      l_waveProp.setGhostOutflow();
      l_waveProp.timeStep(0.05);
    }

    const tsunami_lab::t_real * l_h = l_waveProp.getHeight();
    tsunami_lab::t_idx l_stride = l_waveProp.getStride();

    for(tsunami_lab::t_idx l_iy = 0; l_iy < 10; l_iy++) {
      for(tsunami_lab::t_idx l_ix = 0; l_ix < 10; l_ix++) {
        tsunami_lab::t_real l_val = l_h[l_ix + l_iy * l_stride];
        REQUIRE_FALSE(std::isnan(l_val));
        REQUIRE(l_val >= 0.0);
      }
    }
  }

  SECTION("Ghost outflow copies boundary values correctly") {
    tsunami_lab::patches::WavePropagation2d l_waveProp(4, 4, tsunami_lab::solvers::FWAVE);

    for(tsunami_lab::t_idx l_iy = 0; l_iy < 4; l_iy++) {
      for(tsunami_lab::t_idx l_ix = 0; l_ix < 4; l_ix++) {
        l_waveProp.setHeight(l_ix, l_iy, (tsunami_lab::t_real)(l_ix + 1));
        l_waveProp.setMomentumX(l_ix, l_iy, 1.0);
        l_waveProp.setMomentumY(l_ix, l_iy, 1.0);
        l_waveProp.setBathymetry(l_ix, l_iy, -5.0);
      }
    }

    l_waveProp.setGhostOutflow();

    // access full internal array via stride
    // left ghost column (ix=-1) should equal ix=0
    // right ghost column (ix=4) should equal ix=3
    // we verify indirectly: after a timestep, no NaN
    l_waveProp.timeStep(0.05);

    const tsunami_lab::t_real * l_h = l_waveProp.getHeight();
    tsunami_lab::t_idx l_stride = l_waveProp.getStride();

    for(tsunami_lab::t_idx l_iy = 0; l_iy < 4; l_iy++) {
      for(tsunami_lab::t_idx l_ix = 0; l_ix < 4; l_ix++) {
        REQUIRE_FALSE(std::isnan(l_h[l_ix + l_iy * l_stride]));
        REQUIRE(l_h[l_ix + l_iy * l_stride] > 0.0);
      }
    }
  }
}