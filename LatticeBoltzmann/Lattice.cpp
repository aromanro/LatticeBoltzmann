#include "stdafx.h"
#include "Lattice.h"

namespace LatticeBoltzmann {

	Lattice::Lattice()
		: resultsType(Density), boundaryConditions(BounceBack), simulate(true), refreshSteps(10),
		accelX(0.015), accelY(0),
		useAccelX(0),
		inletOption(1), outletOption(1),
		inletDensity(1.05), outletDensity(1.),
		inletSpeed(0.5), outletSpeed(0.5),
		tau(0.6), numThreads(8),
		processed(0)
	{
	}


	Lattice::~Lattice()
	{
	}





	void Lattice::Init()
	{
		{
			std::lock_guard<std::mutex> lock(resMutex);
			results = Eigen::MatrixXd::Zero(latticeObstacles.rows(), latticeObstacles.cols());
		}

		lattice = CellLattice(latticeObstacles.rows(), latticeObstacles.cols());

		for (int i = 0; i < lattice.rows(); ++i)
			for (int j = 0; j < lattice.cols(); ++j)
				if (latticeObstacles(i, j) ||
					(Periodic != boundaryConditions && (i == 0 || i == lattice.rows() - 1)))
					lattice(i, j).density = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
				else lattice(i, j).Init();
	}

	void Lattice::WakeUp()
	{
		// signal the worker thread to wake up if it's waiting
		{
			std::lock_guard<std::mutex> lk(mw);
			for (unsigned int i = 0; i < wakeup.size(); ++i) wakeup[i] = true;
		}
		cvw.notify_all();
	}

	void Lattice::SignalMoreData()
	{
		std::unique_lock<std::mutex> lk(mp);
		++processed;
		lk.unlock();
		cvp.notify_one();
	}

	void Lattice::WaitForData()
	{
		//wait for the worker threads to finish some work
		std::unique_lock<std::mutex> lk(mp);
		cvp.wait(lk, [this] { return processed == static_cast<int>(numThreads); });
		processed = 0;
	}

	void Lattice::WaitForWork(int tid)
	{
		std::unique_lock<std::mutex> lk(mw);
		cvw.wait(lk, [this, tid] { return wakeup[tid]; });
		wakeup[tid] = false;
	}


	void Lattice::CollideAndStream(int tid, CellLattice* latticeW, int startCol, int endCol)
	{
		CellLattice& latticeWork = *latticeW;
		// stream (including bounce back) and collision combined

		int LatticeRows = static_cast<int>(lattice.rows());
		int LatticeRowsMinusOne = LatticeRows - 1;
		int LatticeCols = static_cast<int>(lattice.cols());
		int LatticeColsMinusOne = LatticeCols - 1;

		const double accelXtau = accelX * tau;
		const double accelYtau = accelY * tau;

		for (;;)
		{
			WaitForWork(tid);
			if (!simulate)
			{
				SignalMoreData();
				break;
			}

			for (int y = 0; y < LatticeRows; ++y)
			{
				const int LatticeRowsMinuOneMinusRow = LatticeRowsMinusOne - y;
				const bool ShouldCollide = (Periodic == boundaryConditions || (0 != y && y != LatticeRowsMinusOne));

				for (int x = startCol; x < endCol; ++x)
				{
					// collision
					if (!latticeObstacles(y, x) && ShouldCollide && (useAccelX || (x > 0 && x < LatticeColsMinusOne)))
						lattice(y, x).Collision(x == 0 && useAccelX ? accelXtau : 0, accelYtau, tau);

					// stream

					// as a note, this is highly inefficient
					// for example
					// checking nine times for each cell for a boundary condition that is fixed before running the simulation
					// is overkill
					// this could be solved by moving the ifs outside the for loops
					// it could be for example solved with templates with the proper class instantiation depending on the settings
					// I did not want to complicate the code so much so for now I'll have it this way even if it's not efficient
					// hopefully the compiler is able to do some optimizations :)

					for (int dir = 0; dir < 9; ++dir)
					{
						Cell::Direction direction = static_cast<Cell::Direction>(dir);

						auto pos = Cell::GetNextPosition(direction, x, LatticeRowsMinuOneMinusRow);
						pos.second = LatticeRowsMinusOne - pos.second;

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
						else if (pos.second == 0 || pos.second == LatticeRowsMinusOne)
						{
							if (BounceBack == boundaryConditions) direction = Cell::Reverse(direction);
							else direction = Cell::ReflectVert(direction);
						}

						// ***************************************************************************************************************

						// bounce back for regular obstacles
						if (latticeObstacles(pos.second, pos.first)) direction = Cell::Reverse(direction);

						// x, y = old position, pos = new position, dir - original direction, direction - new direction
						if (pos.first >= 0 && pos.first < LatticeCols && pos.second >= 0 && pos.second < LatticeRows)
							latticeWork(pos.second, pos.first).density[direction] = lattice(y, x).density[dir];
					}
				}
			}


			DealWithInletOutlet(latticeWork, startCol, endCol, LatticeRows, LatticeCols, LatticeRowsMinusOne, LatticeColsMinusOne);

			SignalMoreData();
		}
	}

	void Lattice::Simulate()
	{
		Init();

		CellLattice latticeWork = CellLattice(lattice.rows(), lattice.cols());
		std::vector<std::thread> theThreads(numThreads);

		processed = 0;
		wakeup.resize(numThreads);
		for (unsigned int i = 0; i < numThreads; ++i) wakeup[i] = false;

		const int workStride = static_cast<int>(lattice.cols() / numThreads);
		for (int t = 0, strideStart = 0; t < (int)numThreads; ++t)
		{
			const int endStride = strideStart + workStride;
			theThreads[t] = std::thread(&Lattice::CollideAndStream, this, t, &latticeWork, strideStart, t == static_cast<int>(numThreads - 1) ? static_cast<int>(lattice.cols()) : endStride);
			strideStart = endStride;
		}


		for (unsigned int step = 0; ; ++step)
		{
			WakeUp();
			WaitForData();
			if (!simulate) break;

			lattice.swap(latticeWork);

			// compute values to display, here I also use an arbitrary 'warmup' interval where results are not calculated
			if (step > 2000 && step % refreshSteps == 0)
				GetResults();
		}

		WakeUp();
		for (unsigned int t = 0; t < numThreads; ++t)
			if (theThreads[t].joinable()) theThreads[t].join();
	}


	void Lattice::GetResults()
	{
		std::lock_guard<std::mutex> lock(resMutex);

		switch (resultsType)
		{
		case Density:
			for (int i = 0; i < lattice.rows(); ++i)
				for (int j = 0; j < lattice.cols(); ++j)
					results(i, j) = lattice(i, j).Density();
			break;
		case Speed:
			for (int i = 0; i < lattice.rows(); ++i)
				for (int j = 0; j < lattice.cols(); ++j)
				{
					auto res = lattice(i, j).Velocity();
					results(i, j) = sqrt(res.first * res.first + res.second * res.second);
				}
			break;
		case Vorticity:
			for (int i = 0; i < lattice.rows(); ++i)
				for (int j = 0; j < lattice.cols(); ++j)
				{
					auto v = lattice(i, j).Velocity();

					auto vx = i < lattice.rows() - 1 ? lattice(i + 1, j).Velocity() : lattice(0, j).Velocity();
					auto vy = j > 0 ? lattice(i, j - 1).Velocity() : (boundaryConditions == Periodic ? lattice(i, lattice.cols() - 1).Velocity() : std::make_pair<double, double>(0, 0));

					results(i, j) = (vy.second - v.second) - (vx.first - v.first);
				}
			break;
		}
	}


}

