#pragma once

#include <array>


namespace LatticeBoltzmann {


	class Cell
	{
	public:
		Cell();
		~Cell();


		static std::array<signed char, 9> ex;
		static std::array<signed char, 9> ey;

		static std::array<double, 9> coeff;

		std::array<double, 9> density;

		enum Direction
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
			return std::make_pair<int, int>(x + ex[direction], y + ey[direction]);
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
			double tDensity = 0;

			for (unsigned char i = 0; i < 9; ++i)
				tDensity += density[i];

			return tDensity;
		}

		inline std::pair<double, double> Velocity() const
		{
			double tDensity = 0;
			double vx = 0;
			double vy = 0;


			for (unsigned char i = 0; i < 9; ++i)
			{
				tDensity += density[i];
				vx += ex[i] * density[i];
				vy += ey[i] * density[i];
			}

			if (tDensity < 1E-14) return std::make_pair<double, double>(0, 0);

			return std::make_pair<double, double>(vx / tDensity, vy / tDensity);
		}

		// this can be optimized, I won't do that to have the code easy to understand
		// accelX, accelY are here to let you add a 'force' (as for example gravity, or some force to move the fluid at an inlet)
		inline std::array<double, 9> Equilibrium(double accelXtau, double accelYtau) const
		{
			std::array<double, 9> result;

			double totalDensity = density[0];
			double vx = ex[0] * density[0];
			double vy = ey[0] * density[0];
			
			for (int i = 1; i < 9; ++i)
			{
				totalDensity += density[i];
				vx += ex[i] * density[i];
				vy += ey[i] * density[i];
			}


			vx /= totalDensity;
			vy /= totalDensity;

			vx += accelXtau;
			vy += accelYtau;
			
			const double v2 = vx * vx + vy * vy;

			static const double coeff1 = 3.;
			static const double coeff2 = 9. / 2.;
			static const double coeff3 = -3. / 2.;

			for (unsigned char i = 0; i < 9; ++i)
			{
				const double term = ex[i] * vx + ey[i] * vy;

				result[i] = coeff[i] * totalDensity * (1. + coeff1 * term + coeff2 * term * term + coeff3 * v2);
			}

			return std::move(result);
		}


		inline void Collision(double accelXtau, double accelYtau, double tau)
		{
			const std::array<double, 9> equilibriumDistribution = Equilibrium(accelXtau, accelYtau);

			for (unsigned char i = 0; i < 9; ++i)
				density[i] -= (density[i] - equilibriumDistribution[i]) / tau;
		}

	};

}