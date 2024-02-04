#include "stdafx.h"

#include "Cell.h"


namespace LatticeBoltzmann {

	const double c0 = 4. / NumDir;
	const double c1 = 1. / NumDir;
	const double c2 = 1. / 36.;
	
	                                                                // 0, N, NE,E, SE,  S, SW,  W, NW
	std::array<signed char, NumDir> Cell::ex = std::array<signed char, 9>{ {0, 0, 1, 1,  1,  0, -1, -1, -1} };
	std::array<signed char, NumDir> Cell::ey = std::array<signed char, 9>{ {0, 1, 1, 0, -1, -1, -1,  0,  1} };
															   // 0, N,  NE, E,  SE, S,  SW, W,  NW
	std::array<double, NumDir> Cell::coeff = std::array<double, NumDir>{ { c0, c1, c2, c1, c2, c1, c2, c1, c2 } };

	void Cell::Init()
	{
		density = coeff;
	}
}

