#pragma once

#include <array>
#include <atomic>
#include <mutex>

#include <eigen/eigen>

#include "Cell.h"



namespace LatticeBoltzmann {


	class Lattice
	{
	public:
		Lattice();
		~Lattice();


		enum BoundaryConditions
		{
			Periodic = 0,
			BounceBack,
			Slippery
		};

		enum ResultsType
		{
			Density = 0,
			Speed,
			Vorticity
		};

		ResultsType resultsType;

		BoundaryConditions boundaryConditions;

		std::atomic_bool simulate;

		unsigned int refreshSteps;
		unsigned int numThreads;

		static const Eigen::StorageOptions DataOrder = Eigen::ColMajor;

		typedef Eigen::Matrix<LatticeBoltzmann::Cell, Eigen::Dynamic, Eigen::Dynamic, DataOrder> CellLattice;

		double tau;
		double accelX; // only applied to the left side to 'push' the fluid through the 'pipe'
		//double accelY; // can be used to add gravity, not actually used - set to zero


		int useAccelX;

		// inlet/outlet

		int inletOption; // 0 - use inlet density, 1 - use inlet speed
		int outletOption;

		double inletDensity;
		double inletSpeed;

		double outletDensity;
		double outletSpeed;


		CellLattice lattice;


		Eigen::Matrix<bool, Eigen::Dynamic, Eigen::Dynamic, DataOrder> latticeObstacles;

		Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, DataOrder> results;
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
		inline void CollideAndStreamCell(int x, int y, bool ShouldCollide, bool xInsideBoundaryOrUseAccel, bool useAccelX, int LatticeRowsMinusOne, int LatticeRows, int LatticeColsMinusOne, int LatticeCols, double accelXtau, /*double accelYtau,*/ double tau, CellLattice& latticeWork)
		{
			// collision
			if (ShouldCollide && !latticeObstacles(y, x) && xInsideBoundaryOrUseAccel)
				lattice(y, x).Collision(x == 0 && useAccelX ? accelXtau : 0, /*accelYtau,*/ tau);

			// stream

			// as a note, this is highly inefficient
			// for example
			// checking nine times for each cell for a boundary condition that is fixed before running the simulation
			// is overkill
			// this could be solved by moving the ifs outside the for loops
			// it could be for example solved with templates with the proper class instantiation depending on the settings
			// I did not want to complicate the code so much so for now I'll have it this way even if it's not efficient
			// hopefully the compiler is able to do some optimizations :)

			for (unsigned char dir = 0; dir < 9; ++dir)
			{
				Cell::Direction direction = static_cast<Cell::Direction>(dir);
				auto pos = Cell::GetNextPosition(direction, x, y);

				// ***************************************************************************************************************
				// left & right 


				if (useAccelX) //periodic boundary with usage of an accelerating force
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

				if (Periodic == boundaryConditions)
				{
					if (pos.second < 0) pos.second = LatticeRowsMinusOne;
					else if (pos.second >= LatticeRows) pos.second = 0;
				}
				else if (pos.second <= 0 || pos.second >= LatticeRowsMinusOne)
				{
					if (BounceBack == boundaryConditions) direction = Cell::Reverse(direction);
					else direction = Cell::ReflectVert(direction);
				}

				// ***************************************************************************************************************

				// bounce back for regular obstacles
				const bool inside = (pos.first > 0 || pos.first < LatticeColsMinusOne) && (pos.second > 0 || pos.second < LatticeRowsMinusOne);
				if (inside && latticeObstacles(pos.second, pos.first)) direction = Cell::Reverse(direction);

				// x, y = old position, pos = new position, dir - original direction, direction - new direction
				if (pos.first >= 0 && pos.first < LatticeCols && pos.second >= 0 && pos.second < LatticeRows)
					latticeWork(pos.second, pos.first).density[direction] = lattice(y, x).density[dir];

			}
		}

		void CollideAndStream(int tid, CellLattice* latticeW, int startCol, int endCol);

		void GetResults();


		inline void DealWithInletOutlet(CellLattice& latticeWork, int startCol, int endCol, int LatticeRows, int LatticeCols, int LatticeRowsMinusOne, int LatticeColsMinusOne)
		{
			// special treatment for inlet/outlet boundary conditions
			if (!useAccelX)
			{
				// except for periodic boundary conditions one would need to treat the corners in a special manner, but I got bored... 
				if (0 == startCol) // inlet
				{
					if (inletOption) // speed specified
					{
						for (int y = (Periodic == boundaryConditions ? 0 : 1); y < (Periodic == boundaryConditions ? LatticeRows : LatticeRowsMinusOne); ++y)
						{
							double density = latticeWork(y, 1).Density();
							double diff = 1. / 2. * (latticeWork(y, 1).density[Cell::Direction::N] - latticeWork(y, 1).density[Cell::Direction::S]);

							latticeWork(y, 1).density[Cell::Direction::E] = latticeWork(y, 1).density[Cell::Direction::W] + 2. / 3. * density * inletSpeed;
							latticeWork(y, 1).density[Cell::Direction::NE] = latticeWork(y, 1).density[Cell::Direction::SW] - diff + 1. / 6. * density * inletSpeed;
							latticeWork(y, 1).density[Cell::Direction::SE] = latticeWork(y, 1).density[Cell::Direction::NW] + diff + 1. / 6. * density * inletSpeed;
						}
					}
					else // density specified
					{
						for (int y = (Periodic == boundaryConditions ? 0 : 1); y < (Periodic == boundaryConditions ? LatticeRows : LatticeRowsMinusOne); ++y)
						{
							double speed = 1. - (latticeWork(y, 1).density[Cell::Direction::none] + latticeWork(y, 1).density[Cell::Direction::N] + latticeWork(y, 1).density[Cell::Direction::S] + 2.* (latticeWork(y, 1).density[Cell::Direction::NW] + latticeWork(y, 1).density[Cell::Direction::W] + latticeWork(y, 1).density[Cell::Direction::SW])) / inletDensity;
							double diff = 1. / 2. * (latticeWork(y, 1).density[Cell::Direction::N] - latticeWork(y, 1).density[Cell::Direction::S]);

							latticeWork(y, 1).density[Cell::Direction::E] = latticeWork(y, 1).density[Cell::Direction::W] + 2. / 3. * inletDensity * speed;
							latticeWork(y, 1).density[Cell::Direction::NE] = latticeWork(y, 1).density[Cell::Direction::SW] - diff + 1. / 6. * inletDensity * speed;
							latticeWork(y, 1).density[Cell::Direction::SE] = latticeWork(y, 1).density[Cell::Direction::NW] + diff + 1. / 6. * inletDensity * speed;
						}
					}
				}
				else if (LatticeCols == endCol) // outlet
				{
					int lastCol = LatticeColsMinusOne - 1;
					if (outletOption) // speed specified
					{
						for (int y = (Periodic == boundaryConditions ? 0 : 1); y < (Periodic == boundaryConditions ? LatticeRows : LatticeRowsMinusOne); ++y)
						{
							double density = latticeWork(y, lastCol).Density();
							double diff = 1. / 2. * (latticeWork(y, lastCol).density[Cell::Direction::N] - latticeWork(y, lastCol).density[Cell::Direction::S]);

							latticeWork(y, lastCol).density[Cell::Direction::W] = latticeWork(y, lastCol).density[Cell::Direction::E] - 2. / 3. * density * outletSpeed;
							latticeWork(y, lastCol).density[Cell::Direction::NW] = latticeWork(y, lastCol).density[Cell::Direction::SE] - diff - 1. / 6. * density * outletSpeed;
							latticeWork(y, lastCol).density[Cell::Direction::SW] = latticeWork(y, lastCol).density[Cell::Direction::NE] + diff - 1. / 6. * density * outletSpeed;
						}
					}
					else // density specified
					{
						for (int y = (Periodic == boundaryConditions ? 0 : 1); y < (Periodic == boundaryConditions ? LatticeRows : LatticeRowsMinusOne); ++y)
						{
							double speed = 1. - (latticeWork(y, lastCol).density[Cell::Direction::none] + latticeWork(y, lastCol).density[Cell::Direction::N] + latticeWork(y, lastCol).density[Cell::Direction::S] + 2.* (latticeWork(y, lastCol).density[Cell::Direction::NE] + latticeWork(y, lastCol).density[Cell::Direction::E] + latticeWork(y, lastCol).density[Cell::Direction::SE])) / outletDensity;
							double diff = 1. / 2. * (latticeWork(y, lastCol).density[Cell::Direction::N] - latticeWork(y, lastCol).density[Cell::Direction::S]);

							latticeWork(y, lastCol).density[Cell::Direction::W] = latticeWork(y, lastCol).density[Cell::Direction::E] + 2. / 3. * outletDensity * speed;
							latticeWork(y, lastCol).density[Cell::Direction::NW] = latticeWork(y, lastCol).density[Cell::Direction::SE] - diff + 1. / 6. * outletDensity * speed;
							latticeWork(y, lastCol).density[Cell::Direction::SW] = latticeWork(y, lastCol).density[Cell::Direction::NE] + diff + 1. / 6. * outletDensity * speed;
						}
					}
				}
			}

		}

	};

}
