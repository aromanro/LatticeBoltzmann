#include "stdafx.h"
#include "Lattice.h"

namespace LatticeBoltzmann {

	Lattice::Lattice()
		: resultsType(ResultsType::Density), boundaryConditions(BoundaryConditions::BounceBack), simulate(true), refreshSteps(10),
		accelX(0.015), 
//		accelY(0),
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
			results = Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, DataOrder>::Zero(latticeObstacles.rows(), latticeObstacles.cols());
		}

		lattice = CellLattice(latticeObstacles.rows(), latticeObstacles.cols());

		for (int j = 0; j < lattice.cols(); ++j)
			for (int i = 0; i < lattice.rows(); ++i)
				if (latticeObstacles(i, j) ||
					(BoundaryConditions::Periodic != boundaryConditions && (i == 0 || i == lattice.rows() - 1)))
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

		const int LatticeRows = static_cast<int>(lattice.rows());
		const int LatticeRowsMinusOne = LatticeRows - 1;
		const int LatticeRowsMinusTwo = LatticeRows - 2;
		const int LatticeCols = static_cast<int>(lattice.cols());
		const int LatticeColsMinusOne = LatticeCols - 1;
		const int LatticeColsMinusTwo = LatticeCols - 2;

		const double accelXtau = accelX * tau;
		//const double accelYtau = accelY * tau;

		const bool ShouldCollideAtUpDownBoundary = (BoundaryConditions::Periodic == boundaryConditions);

		for (;;)
		{
			WaitForWork(tid);
			if (!simulate)
			{
				SignalMoreData();
				break;
			}



			/*

			// Before:

			for (int x = startCol; x < endCol; ++x)
			{
				const bool xInsideBoundaryOrUseAccel = useAccelX || (x > 0 && x < LatticeColsMinusOne);

				CollideAndStreamCell(x, 0, ShouldCollideAtUpDownBoundary, xInsideBoundaryOrUseAccel, useAccelX, LatticeRowsMinusOne, LatticeRows, LatticeColsMinusOne, LatticeCols, accelXtau, tau, latticeWork);

				for (int y = 1; y < LatticeRowsMinusOne; ++y)
					CollideAndStreamCell(x, y, true, xInsideBoundaryOrUseAccel, useAccelX, LatticeRowsMinusOne, LatticeRows, LatticeColsMinusOne, LatticeCols, accelXtau, tau, latticeWork);

				CollideAndStreamCell(x, LatticeRowsMinusOne, ShouldCollideAtUpDownBoundary, xInsideBoundaryOrUseAccel, useAccelX, LatticeRowsMinusOne, LatticeRows, LatticeColsMinusOne, LatticeCols, accelXtau, tau, latticeWork);
			}
			*/



			// everything that needs the complex checks is called with the 'slow' variant - that is, for cells close to the boundaries
			// everything else use the fast variant

			if (startCol < 2)
			{
				// column 0
				CollideAndStreamCellSlow(0, 0, ShouldCollideAtUpDownBoundary, useAccelX, useAccelX, LatticeRowsMinusOne, LatticeRows, LatticeColsMinusOne, LatticeCols, accelXtau, tau, latticeWork);
				for (int y = 1; y < LatticeRowsMinusOne; ++y)
					CollideAndStreamCellSlow(0, y, true, useAccelX, useAccelX, LatticeRowsMinusOne, LatticeRows, LatticeColsMinusOne, LatticeCols, accelXtau, tau, latticeWork);
				CollideAndStreamCellSlow(0, LatticeRowsMinusOne, ShouldCollideAtUpDownBoundary, useAccelX, useAccelX, LatticeRowsMinusOne, LatticeRows, LatticeColsMinusOne, LatticeCols, accelXtau, tau, latticeWork);

				// column 1
				CollideAndStreamCellSlow(1, 0, ShouldCollideAtUpDownBoundary, true, useAccelX, LatticeRowsMinusOne, LatticeRows, LatticeColsMinusOne, LatticeCols, accelXtau, tau, latticeWork);
				for (int y = 1; y < LatticeRowsMinusOne; ++y)
					CollideAndStreamCellSlow(1, y, true, true, useAccelX, LatticeRowsMinusOne, LatticeRows, LatticeColsMinusOne, LatticeCols, accelXtau, tau, latticeWork);
				CollideAndStreamCellSlow(1, LatticeRowsMinusOne, ShouldCollideAtUpDownBoundary, true, useAccelX, LatticeRowsMinusOne, LatticeRows, LatticeColsMinusOne, LatticeCols, accelXtau, tau, latticeWork);
			}

			for (int x = ((startCol < 2) ? 2 : startCol); x < ((endCol >= LatticeColsMinusTwo) ? LatticeColsMinusTwo : endCol); ++x)
			{
				// first couple of rows
				CollideAndStreamCellSlow(x, 0, ShouldCollideAtUpDownBoundary, true, useAccelX, LatticeRowsMinusOne, LatticeRows, LatticeColsMinusOne, LatticeCols, accelXtau, tau, latticeWork);
				CollideAndStreamCellSlow(x, 1, true, true, useAccelX, LatticeRowsMinusOne, LatticeRows, LatticeColsMinusOne, LatticeCols, accelXtau, tau, latticeWork);

				for (int y = 2; y < LatticeRowsMinusTwo; ++y)
					CollideAndStreamCellFast(x, y, tau, latticeWork);

				// last couple of rows
				CollideAndStreamCellSlow(x, LatticeRowsMinusTwo, true, true, useAccelX, LatticeRowsMinusOne, LatticeRows, LatticeColsMinusOne, LatticeCols, accelXtau, tau, latticeWork);
				CollideAndStreamCellSlow(x, LatticeRowsMinusOne, ShouldCollideAtUpDownBoundary, true, useAccelX, LatticeRowsMinusOne, LatticeRows, LatticeColsMinusOne, LatticeCols, accelXtau, tau, latticeWork);
			}

			if (endCol >= LatticeColsMinusTwo)
			{
				// LatticeColsMinusTwo column
				CollideAndStreamCellSlow(LatticeColsMinusTwo, 0, ShouldCollideAtUpDownBoundary, true, useAccelX, LatticeRowsMinusOne, LatticeRows, LatticeColsMinusOne, LatticeCols, accelXtau, tau, latticeWork);
				for (int y = 1; y < LatticeRowsMinusOne; ++y)
					CollideAndStreamCellSlow(LatticeColsMinusTwo, y, true, true, useAccelX, LatticeRowsMinusOne, LatticeRows, LatticeColsMinusOne, LatticeCols, accelXtau, tau, latticeWork);
				CollideAndStreamCellSlow(LatticeColsMinusTwo, LatticeRowsMinusOne, ShouldCollideAtUpDownBoundary, true, useAccelX, LatticeRowsMinusOne, LatticeRows, LatticeColsMinusOne, LatticeCols, accelXtau, tau, latticeWork);

				// last column
				CollideAndStreamCellSlow(LatticeColsMinusOne, 0, ShouldCollideAtUpDownBoundary, useAccelX, useAccelX, LatticeRowsMinusOne, LatticeRows, LatticeColsMinusOne, LatticeCols, accelXtau, tau, latticeWork);
				for (int y = 1; y < LatticeRowsMinusOne; ++y)
					CollideAndStreamCellSlow(LatticeColsMinusOne, y, true, useAccelX, useAccelX, LatticeRowsMinusOne, LatticeRows, LatticeColsMinusOne, LatticeCols, accelXtau, tau, latticeWork);
				CollideAndStreamCellSlow(LatticeColsMinusOne, LatticeRowsMinusOne, ShouldCollideAtUpDownBoundary, useAccelX, useAccelX, LatticeRowsMinusOne, LatticeRows, LatticeColsMinusOne, LatticeCols, accelXtau, tau, latticeWork);
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

		const int workStride = static_cast<int>(ceil(static_cast<double>(lattice.cols()) / numThreads));
		for (int t = 0, strideStart = 0; t < (int)numThreads; ++t)
		{
			int endStride = strideStart + workStride;
			if (endStride > lattice.cols()) endStride = static_cast<int>(lattice.cols());

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
		case ResultsType::Density:
			for (int j = 0; j < lattice.cols(); ++j)
				for (int i = 0; i < lattice.rows(); ++i)
					results(i, j) = lattice(i, j).Density();
			break;
		case ResultsType::Speed:
			for (int j = 0; j < lattice.cols(); ++j)
				for (int i = 0; i < lattice.rows(); ++i)
				{
					auto res = lattice(i, j).Velocity();
					results(i, j) = sqrt(res.first * res.first + res.second * res.second);
				}
			break;
		case ResultsType::Vorticity:
			for (int j = 0; j < lattice.cols(); ++j)
				for (int i = 0; i < lattice.rows(); ++i)
				{
					auto v = lattice(i, j).Velocity();

					auto vx = i < lattice.rows() - 1LL ? lattice(i + 1LL, j).Velocity() : lattice(0, j).Velocity();
					auto vy = j > 0 ? lattice(i, j - 1LL).Velocity() : (boundaryConditions == BoundaryConditions::Periodic ? lattice(i, lattice.cols() - 1LL).Velocity() : std::make_pair(0., 0.));

					results(i, j) = (vy.second - v.second) - (vx.first - v.first);
				}
			break;
		}
	}


}

