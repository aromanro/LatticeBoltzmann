#pragma once

#include <array>


namespace LatticeBoltzmann {

	constexpr int NumDir = 9;

	class Cell
	{
	public:
		Cell();

		static std::array<signed char, NumDir> ex;
		static std::array<signed char, NumDir> ey;

		static std::array<double, NumDir> coeff;

		std::array<double, NumDir> density;

		enum class Direction : unsigned char
		{
			none = 0,
			N,
			NE,
			E,
			SE,
			S,
			SW,
			W,
			NW
		};

		void Init();

		inline static std::pair<int, int> GetNextPosition(Direction direction, int x, int y)
		{
			// - for the y is because y coordinate is reversed, 0 is top
			// alternatively, one could just change the signs in the ey array
			return std::make_pair(x + ex[static_cast<size_t>(direction)], y - ey[static_cast<size_t>(direction)]);
		}

		inline static Direction Reverse(Direction dir)
		{
			switch (dir)
			{
			case Direction::N:
				return Direction::S;
			case Direction::S:
				return Direction::N;
			case Direction::W:
				return Direction::E;
			case Direction::E:
				return Direction::W;
			case Direction::NE:
				return Direction::SW;
			case Direction::SE:
				return Direction::NW;
			case Direction::NW:
				return Direction::SE;
			case Direction::SW:
				return Direction::NE;
			}

			return Direction::none;
		}

		inline static Direction ReflectVert(Direction dir)
		{
			switch (dir)
			{
			case Direction::N:
				return Direction::S;
			case Direction::S:
				return Direction::N;
			case Direction::W:
				return Direction::W;
			case Direction::E:
				return Direction::E;
			case Direction::NE:
				return Direction::SE;
			case Direction::SE:
				return Direction::NE;
			case Direction::NW:
				return Direction::SW;
			case Direction::SW:
				return Direction::NW;
			}

			return Direction::none;
		}


		inline double Density() const
		{
			/*
			double tDensity = 0;
			for (unsigned char i = 0; i < NumDir; ++i)
				tDensity += density[i];
			return tDensity;
			*/
			// unrolled
			return density[0] + density[1] + density[2] + density[3] + density[4] + density[5] + density[6] + density[7] + density[8];
		}

		inline std::pair<double, double> Velocity() const
		{
			/*
			double tDensity = 0;
			double vx = 0;
			double vy = 0;

			for (unsigned char i = 0; i < 9; ++i)
			{
				tDensity += density[i];
				vx += ex[i] * density[i];
				vy += ey[i] * density[i];
			}
			*/
			// unrolled
			const double tDensity = density[0] + density[1] + density[2] + density[3] + density[4] + density[5] + density[6] + density[7] + density[8];
			const double vx = ex[0] * density[0] + ex[1] * density[1] + ex[2] * density[2] + ex[3] * density[3] + ex[4] * density[4] + ex[5] * density[5] + ex[6] * density[6] + ex[7] * density[7] + ex[8] * density[8];
			const double vy = ey[0] * density[0] + ey[1] * density[1] + ey[2] * density[2] + ey[3] * density[3] + ey[4] * density[4] + ey[5] * density[5] + ey[6] * density[6] + ey[7] * density[7] + ey[8] * density[8];

			if (tDensity < 1E-14) return std::make_pair(0., 0.);

			const double invDens = 1. / tDensity;

			return std::make_pair(vx * invDens, vy * invDens);
		}

		// this can be optimized, I won't do that to have the code easy to understand
		// accelX, accelY are here to let you add a 'force' (as for example gravity, or some force to move the fluid at an inlet)
		inline std::array<double, NumDir> Equilibrium(double accelXtau/*, double accelYtau*/) const
		{
			std::array<double, NumDir> result;

			/*
			double totalDensity = density[0];
			double vx = 0;
			double vy = 0;
			
			for (unsigned char i = 1; i < NumDir; ++i)
			{
				totalDensity += density[i];
				vx += ex[i] * density[i];
				vy += ey[i] * density[i];
			}
			*/

			const double totalDensity = density[0] + density[1] + density[2] + density[3] + density[4] + density[5] + density[6] + density[7] + density[8];
			double vx = ex[0] * density[0] + ex[1] * density[1] + ex[2] * density[2] + ex[3] * density[3] + ex[4] * density[4] + ex[5] * density[5] + ex[6] * density[6] + ex[7] * density[7] + ex[8] * density[8];
			double vy = ey[0] * density[0] + ey[1] * density[1] + ey[2] * density[2] + ey[3] * density[3] + ey[4] * density[4] + ey[5] * density[5] + ey[6] * density[6] + ey[7] * density[7] + ey[8] * density[8];
			const double invDens = 1. / totalDensity;

			vx *= invDens;
			vy *= invDens;

			vx += accelXtau;
			//vy += accelYtau;
			
			const double vx2 = vx * vx;
			const double vy2 = vy * vy;
			const double v2 = vx2 + vy2;

			static const double coeff1 = 3.;
			static const double coeff2 = 9. / 2.;
			static const double coeff3 = -3. / 2.;

			// this is the original code described on the blog
			// what follows is loop unrolling optimized somewhat, by avoiding recalculating some terms

			/*
			for (unsigned char i = 0; i < NumDir; ++i)
			{
				const double term = ex[i] * vx + ey[i] * vy;

				result[i] = coeff[i] * totalDensity * (1. + coeff1 * term + coeff2 * term * term + coeff3 * v2);
			}
			*/


			const double coeff1vx = coeff1 * vx;
			const double coeff2vx2 = coeff2 * vx2;
			const double coeff1vy = coeff1 * vy;
			const double coeff2vy2 = coeff2 * vy2;

			const double onePluscoeff3v2 = 1. + coeff3 * v2; 

			result[0] = coeff[0] * totalDensity * onePluscoeff3v2; // 0, 0 - vx, vy

			const double coeff2vy2PlusonePluscoeff3v2 = coeff2vy2 + onePluscoeff3v2;

			result[1] = coeff[1] * totalDensity * (coeff1vy + coeff2vy2PlusonePluscoeff3v2); // 0, 1 - vx, vy

			const double term2 = vx + vy;
			const double coeff1term2 = coeff1 * term2;
			const double coeff2term22 = coeff2 * term2 * term2;

			const double coeff2term22PlusonePluscoeff3v2 = coeff2term22 + onePluscoeff3v2;

			result[2] = coeff[2] * totalDensity * (coeff1term2 + coeff2term22PlusonePluscoeff3v2); // 1, 1 - vx, vy


			const double coeff2vx2PlusonePluscoeff3v2 = coeff2vx2 + onePluscoeff3v2;
			result[3] = coeff[3] * totalDensity * (coeff1vx + coeff2vx2PlusonePluscoeff3v2); // 1, 0 - vx, vy

			const double term4 = vx - vy;
			const double coeff1term4 = coeff1 * term4;
			const double coeff2term42 = coeff2 * term4 * term4;

			const double coeff2term42PlusonePluscoeff3v2 = coeff2term42 + onePluscoeff3v2;
			
			result[4] = coeff[4] * totalDensity * (coeff1term4 + coeff2term42PlusonePluscoeff3v2); // 1, -1 - vx, vy
			result[5] = coeff[5] * totalDensity * (-coeff1vy + coeff2vy2PlusonePluscoeff3v2); // 0, -1 - vx, vy
			result[6] = coeff[6] * totalDensity * (-coeff1term2 + coeff2term22PlusonePluscoeff3v2); // -1, -1 - vx, vy
			result[7] = coeff[7] * totalDensity * (-coeff1vx + coeff2vx2PlusonePluscoeff3v2); // -1, 0 - vx, vy
			result[8] = coeff[8] * totalDensity * (-coeff1term4 + coeff2term42PlusonePluscoeff3v2); // -1, 1 - vx, vy

			return std::move(result);
		}


		inline void Collision(double accelXtau, /*double accelYtau,*/ double tau)
		{
			const std::array<double, NumDir> equilibriumDistribution = Equilibrium(accelXtau/*, accelYtau*/);

			/*
			for (unsigned char i = 0; i < NumDir; ++i)
				density[i] -= (density[i] - equilibriumDistribution[i]) / tau;
			*/
			// unrolling
			const double invTau = 1. / tau;

			density[0] -= (density[0] - equilibriumDistribution[0]) * invTau;
			density[1] -= (density[1] - equilibriumDistribution[1]) * invTau;
			density[2] -= (density[2] - equilibriumDistribution[2]) * invTau;
			density[3] -= (density[3] - equilibriumDistribution[3]) * invTau;
			density[4] -= (density[4] - equilibriumDistribution[4]) * invTau;
			density[5] -= (density[5] - equilibriumDistribution[5]) * invTau;
			density[6] -= (density[6] - equilibriumDistribution[6]) * invTau;
			density[7] -= (density[7] - equilibriumDistribution[7]) * invTau;
			density[8] -= (density[8] - equilibriumDistribution[8]) * invTau;
		}
	};

}