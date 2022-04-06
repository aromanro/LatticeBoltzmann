#pragma once

#include <array>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <eigen\eigen>

#include "Cell.h"



namespace LatticeBoltzmann {


	class Lattice
	{
	public:
		Lattice();
		~Lattice();


		enum class BoundaryConditions
		{
			Periodic = 0,
			BounceBack,
			Slippery
		};

		enum class ResultsType
		{
			Density = 0,
			Speed,
			Vorticity
		};

		ResultsType resultsType;

		_declspec(align(16)) BoundaryConditions boundaryConditions;

		std::atomic<bool> simulate;

		unsigned int refreshSteps;
		unsigned int numThreads;

		static const Eigen::StorageOptions DataOrder = Eigen::ColMajor;

		typedef Eigen::Matrix<LatticeBoltzmann::Cell, Eigen::Dynamic, Eigen::Dynamic, DataOrder> CellLattice;

		_declspec(align(16)) double tau;
		_declspec(align(16)) double accelX; // only applied to the left side to 'push' the fluid through the 'pipe'
		//double accelY; // can be used to add gravity, not actually used - set to zero


		_declspec(align(16)) int useAccelX;

		// inlet/outlet

		_declspec(align(16)) int inletOption; // 0 - use inlet density, 1 - use inlet speed
		_declspec(align(16)) int outletOption;

		_declspec(align(16)) double inletDensity;
		_declspec(align(16)) double inletSpeed;

		_declspec(align(16)) double outletDensity;
		_declspec(align(16)) double outletSpeed;


		_declspec(align(16)) CellLattice lattice;


		_declspec(align(16)) Eigen::Matrix<bool, Eigen::Dynamic, Eigen::Dynamic, DataOrder> latticeObstacles;
		_declspec(align(16)) Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, DataOrder> results;
		std::mutex resMutex; // to protect the above and results type if changed during run


		void Init();
		void Simulate();
	private:
		std::mutex mw;
		std::mutex mp;

		std::condition_variable cvp;
		std::condition_variable cvw;

		int processed;
		std::vector<bool> wakeup;

		void WakeUp();
		void WaitForWork(int tid);

		void WaitForData();
		void SignalMoreData();


		// this and Equilibrium take up most of the cpu time so they would benefit a lot from optimizations
		inline void CollideAndStreamCellSlow(int x, int y, bool ShouldCollide, bool xInsideBoundaryOrUseAccel, bool useAccelXp, int LatticeRowsMinusOne, int LatticeRows, int LatticeColsMinusOne, int LatticeCols, double accelXtau, /*double accelYtau,*/ double taup, CellLattice& latticeWork)
		{
			// collision
			if (ShouldCollide && !latticeObstacles(y, x) && xInsideBoundaryOrUseAccel)
				lattice(y, x).Collision(x == 0 && useAccelXp ? accelXtau : 0, /*accelYtau,*/ taup);

			// stream

			// as a note, this is highly inefficient
			// for example
			// checking nine times for each cell for a boundary condition that is fixed before running the simulation
			// is overkill
			// this could be solved by moving the ifs outside the for loops
			// it could be for example solved with templates with the proper class instantiation depending on the settings
			// I did not want to complicate the code so much so for now I'll have it this way even if it's not efficient
			// hopefully the compiler is able to do some optimizations :)

			for (unsigned char dir = 0; dir < NumDir; ++dir)
			{
				Cell::Direction direction = static_cast<Cell::Direction>(dir);
				auto pos = Cell::GetNextPosition(direction, x, y);

				// ***************************************************************************************************************
				// left & right 

				ApplyBoundaryConditions(pos, direction, useAccelXp, LatticeRowsMinusOne, LatticeRows, LatticeColsMinusOne, LatticeCols);


				// ***************************************************************************************************************

				if (pos.first >= 0 && pos.first < LatticeCols && pos.second >= 0 && pos.second < LatticeRows)
				{
				    // bounce back for regular obstacles
					if (latticeObstacles(pos.second, pos.first)) direction = Cell::Reverse(direction);

					// x, y = old position, pos = new position, dir - original direction, direction - new direction
					latticeWork(pos.second, pos.first).density[static_cast<size_t>(direction)] = lattice(y, x).density[dir];
				}
			}
		}


		inline void ApplyBoundaryConditions(std::pair<int, int>& pos, Cell::Direction& direction, bool useAccelXp, int LatticeRowsMinusOne, int LatticeRows, int LatticeColsMinusOne, int LatticeCols)
		{
			if (useAccelXp) //periodic boundary with usage of an accelerating force
			{
				if (pos.first < 0) pos.first = LatticeColsMinusOne;
				else if (pos.first >= LatticeCols) pos.first = 0;
			}
			else
			{
				// bounce them back
				if ((pos.first == 0 || pos.first == LatticeColsMinusOne) && !(pos.second == 0 || pos.second == LatticeRowsMinusOne))
					direction = Cell::Reverse(direction);
			}

			// ***************************************************************************************************************
			// top & bottom, depends on boundaryConditions

			ApplyBoundaryConditionsTopBottom(pos, direction, LatticeRowsMinusOne, LatticeRows);
		}

		inline void ApplyBoundaryConditionsTopBottom(std::pair<int, int>& pos, Cell::Direction& direction, int LatticeRowsMinusOne, int LatticeRows)
		{
			if (BoundaryConditions::Periodic == boundaryConditions)
			{
				if (pos.second < 0) pos.second = LatticeRowsMinusOne;
				else if (pos.second >= LatticeRows) pos.second = 0;
			}
			else if (pos.second <= 0 || pos.second >= LatticeRowsMinusOne)
			{
				if (BoundaryConditions::BounceBack == boundaryConditions) direction = Cell::Reverse(direction);
				else direction = Cell::ReflectVert(direction);
			}
		}

		void CollideAndStreamCellFast(int x, int y, double taup, CellLattice& latticeWork)
		{
			_declspec(align(16)) Cell::Direction direction;			
			// collision
			if (!latticeObstacles(y, x))
				lattice(y, x).Collision(0, /*accelYtau,*/ taup);

			for (unsigned char dir = 0; dir < NumDir; ++dir)
			{
				direction = static_cast<Cell::Direction>(dir);
				auto pos = Cell::GetNextPosition(direction, x, y);

				// bounce back for regular obstacles
				if (latticeObstacles(pos.second, pos.first)) direction = Cell::Reverse(direction);

				// x, y = old position, pos = new position, dir - original direction, direction - new direction
				latticeWork(pos.second, pos.first).density[static_cast<size_t>(direction)] = lattice(y, x).density[dir];
			}
		}


		void CollideAndStream(int tid, CellLattice* latticeW, int startCol, int endCol);

		void GetResults();


		void DealWithInletOutlet(CellLattice& latticeWork, int startCol, int endCol, int LatticeRows, int LatticeCols, int LatticeRowsMinusOne, int LatticeColsMinusOne)
		{
			_declspec(align(16)) int lastCol;
			_declspec(align(16)) double density;
			_declspec(align(16)) double diff;
			_declspec(align(16)) double speed;
			// special treatment for inlet/outlet boundary conditions
			if (!useAccelX)
			{
				// except for periodic boundary conditions one would need to treat the corners in a special manner, but I got bored... 
				if (0 == startCol) // inlet
				{
					if (inletOption) // speed specified
					{
						for (int y = (BoundaryConditions::Periodic == boundaryConditions ? 0 : 1); y < (BoundaryConditions::Periodic == boundaryConditions ? LatticeRows : LatticeRowsMinusOne); ++y)
						{
							density = latticeWork(y, 1).Density();
							diff = 1. / 2 * (latticeWork(y, 1).density[static_cast<size_t>(Cell::Direction::N)] - latticeWork(y, 1).density[static_cast<size_t>(Cell::Direction::S)]);

							latticeWork(y, 1).density[static_cast<size_t>(Cell::Direction::E)] = latticeWork(y, 1).density[static_cast<size_t>(Cell::Direction::W)] + 2. / 3 * density * inletSpeed;
							latticeWork(y, 1).density[static_cast<size_t>(Cell::Direction::NE)] = latticeWork(y, 1).density[static_cast<size_t>(Cell::Direction::SW)] - diff + 1. / 6 * density * inletSpeed;
							latticeWork(y, 1).density[static_cast<size_t>(Cell::Direction::SE)] = latticeWork(y, 1).density[static_cast<size_t>(Cell::Direction::NW)] + diff + 1. / 6 * density * inletSpeed;
						}
					}
					else // density specified
					{
						for (int y = (BoundaryConditions::Periodic == boundaryConditions ? 0 : 1); y < (BoundaryConditions::Periodic == boundaryConditions ? LatticeRows : LatticeRowsMinusOne); ++y)
						{
							speed = 1. - (latticeWork(y, 1).density[static_cast<size_t>(Cell::Direction::none)] + latticeWork(y, 1).density[static_cast<size_t>(Cell::Direction::N)] 
									+ latticeWork(y, 1).density[static_cast<size_t>(Cell::Direction::S)] + 2.* (latticeWork(y, 1).density[static_cast<size_t>(Cell::Direction::NW)] 
									+ latticeWork(y, 1).density[static_cast<size_t>(Cell::Direction::W)] + latticeWork(y, 1).density[static_cast<size_t>(Cell::Direction::SW)])) / inletDensity;
							diff = 1. / 2. * (latticeWork(y, 1).density[static_cast<size_t>(Cell::Direction::N)] - latticeWork(y, 1).density[static_cast<size_t>(Cell::Direction::S)]);

							latticeWork(y, 1).density[static_cast<size_t>(Cell::Direction::E)] = latticeWork(y, 1).density[static_cast<size_t>(Cell::Direction::W)] + 2. / 3. * inletDensity * speed;
							latticeWork(y, 1).density[static_cast<size_t>(Cell::Direction::NE)] = latticeWork(y, 1).density[static_cast<size_t>(Cell::Direction::SW)] - diff + 1. / 6. * inletDensity * speed;
							latticeWork(y, 1).density[static_cast<size_t>(Cell::Direction::SE)] = latticeWork(y, 1).density[static_cast<size_t>(Cell::Direction::NW)] + diff + 1. / 6. * inletDensity * speed;
						}
					}
				}
				else if (LatticeCols == endCol) // outlet
				{
					lastCol = LatticeColsMinusOne - 1;
					if (outletOption) // speed specified
					{
						for (int y = (BoundaryConditions::Periodic == boundaryConditions ? 0 : 1); y < (BoundaryConditions::Periodic == boundaryConditions ? LatticeRows : LatticeRowsMinusOne); ++y)
						{
							density = latticeWork(y, lastCol).Density();
							diff = 1. / 2 * (latticeWork(y, lastCol).density[static_cast<size_t>(Cell::Direction::N)] - latticeWork(y, lastCol).density[static_cast<size_t>(Cell::Direction::S)]);

							latticeWork(y, lastCol).density[static_cast<size_t>(Cell::Direction::W)] = latticeWork(y, lastCol).density[static_cast<size_t>(Cell::Direction::E)] - 2. / 3. * density * outletSpeed;
							latticeWork(y, lastCol).density[static_cast<size_t>(Cell::Direction::NW)] = latticeWork(y, lastCol).density[static_cast<size_t>(Cell::Direction::SE)] - diff - 1. / 6. * density * outletSpeed;
							latticeWork(y, lastCol).density[static_cast<size_t>(Cell::Direction::SW)] = latticeWork(y, lastCol).density[static_cast<size_t>(Cell::Direction::NE)] + diff - 1. / 6. * density * outletSpeed;
						}
					}
					else // density specified
					{
						for (int y = (BoundaryConditions::Periodic == boundaryConditions ? 0 : 1); y < (BoundaryConditions::Periodic == boundaryConditions ? LatticeRows : LatticeRowsMinusOne); ++y)
						{
							speed = 1. - (latticeWork(y, lastCol).density[static_cast<size_t>(Cell::Direction::none)] + latticeWork(y, lastCol).density[static_cast<size_t>(Cell::Direction::N)] 
									+ latticeWork(y, lastCol).density[static_cast<size_t>(Cell::Direction::S)] + 2.* (latticeWork(y, lastCol).density[static_cast<size_t>(Cell::Direction::NE)] 
									+ latticeWork(y, lastCol).density[static_cast<size_t>(Cell::Direction::E)] + latticeWork(y, lastCol).density[static_cast<size_t>(Cell::Direction::SE)])) / outletDensity;

							diff = 1. / 2. * (latticeWork(y, lastCol).density[static_cast<size_t>(Cell::Direction::N)] - latticeWork(y, lastCol).density[static_cast<size_t>(Cell::Direction::S)]);

							latticeWork(y, lastCol).density[static_cast<size_t>(Cell::Direction::W)] = latticeWork(y, lastCol).density[static_cast<size_t>(Cell::Direction::E)] + 2. / 3 * outletDensity * speed;
							latticeWork(y, lastCol).density[static_cast<size_t>(Cell::Direction::NW)] = latticeWork(y, lastCol).density[static_cast<size_t>(Cell::Direction::SE)] - diff + 1. / 6. * outletDensity * speed;
							latticeWork(y, lastCol).density[static_cast<size_t>(Cell::Direction::SW)] = latticeWork(y, lastCol).density[static_cast<size_t>(Cell::Direction::NE)] + diff + 1. / 6. * outletDensity * speed;
						}
					}
				}
			}
		}
	};

}
