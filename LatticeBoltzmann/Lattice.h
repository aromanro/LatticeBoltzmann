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

		

		typedef Eigen::Matrix<LatticeBoltzmann::Cell, Eigen::Dynamic, Eigen::Dynamic> CellLattice;

		double tau;
		double accelX; // only applied to the left side to 'push' the fluid through the 'pipe'
		double accelY; // can be used to add gravity, not actually used - set to zero


		int useAccelX;

		// inlet/outlet

		int inletOption; // 0 - use inlet density, 1 - use inlet speed
		int outletOption;

		double inletDensity;
		double inletSpeed;

		double outletDensity;
		double outletSpeed;


		CellLattice lattice;		

		Eigen::Matrix<bool, Eigen::Dynamic, Eigen::Dynamic> latticeObstacles;

		Eigen::MatrixXd results;
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
