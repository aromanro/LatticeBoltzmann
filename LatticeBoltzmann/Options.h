#pragma once
class Options
{
public:
	Options();
	~Options();

	// display

	double minSpeed;
	double maxSpeed;

	double minDensity;
	double maxDensity;

	double minVorticity;
	double maxVorticity;

	enum class ChartColors {
		TwoColors = 0,
		ThreeColors
	};

	ChartColors chartColors;
	int zoom;

	int resultsType; // 0 - density, 1 - speed, 2 - vorticity

	COLORREF obstaclesColor;

	// computation
	int numThreads;
	int refreshSteps;

	// simulation
	int boundaryConditions; // 0 - periodic, 1 - bounce back, 2 - slippery
	double tau;
	double accelX; // push force on the left side

	int useAccelX;

	// inlet/outlet

	int inletOption; // 0 - use inlet density, 1 - use inlet speed
	int outletOption;

	double inletDensity;
	double inletSpeed;

	double outletDensity;
	double outletSpeed;


	void Load();
	void Save();
protected:
	static double GetDouble(LPCTSTR param, double defval);
	static COLORREF GetColor(LPCTSTR param, COLORREF defval = RGB(0,0,0));
};

