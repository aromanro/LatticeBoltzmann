#include "stdafx.h"

#include "Cell.h"


namespace LatticeBoltzmann {


	const double c0 = 4. / 9.;
	const double c1 = 1. / 9;
	const double c2 = 1. / 36.;
	
	                                                                // 0, N, NE,E, SE,  S, SW,  W, NW
	std::array<signed char, 9> Cell::ex = std::array<signed char, 9>{ {0, 0, 1, 1,  1,  0, -1, -1, -1} };
	std::array<signed char, 9> Cell::ey = std::array<signed char, 9>{ {0, 1, 1, 0, -1, -1, -1,  0,  1} };

	std::array<double, 9> Cell::coeff = std::array<double, 9>{ { c0, c1, c2, c1, c2, c1, c2, c1, c2 } };

	Cell::Cell()
		: density{ {0, 0, 0, 0, 0, 0, 0, 0, 0} }
	{
	}




	void Cell::Init()
	{
		density = coeff;
	}
}

