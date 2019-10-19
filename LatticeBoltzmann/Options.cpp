#include "stdafx.h"
#include "Options.h"

#include "LatticeBoltzmann.h"


Options::Options()
	: minSpeed(0), maxSpeed(0.25), minDensity(0.93), maxDensity(1.08), minVorticity(-0.1), maxVorticity(0.1), chartColors(ChartColors::ThreeColors), zoom(2),
	resultsType(0), obstaclesColor(RGB(0, 0, 0)),
	numThreads(8), refreshSteps(10), boundaryConditions(1), 
	tau(0.6), accelX(0.015), useAccelX(0),
	inletOption(0), outletOption(0),
	inletDensity(1.05), outletDensity(1.),
	inletSpeed(0.051), outletSpeed(0.05)
{
}


Options::~Options()
{
}


double Options::GetDouble(LPCTSTR param, double defval)
{
	double val = defval;

	UINT sz = 0;
	LPBYTE buf = NULL;

	if (theApp.GetProfileBinary(L"options", param, &buf, &sz))
	{
		if (sizeof(double) == sz) val = *((double*)buf);
		delete[] buf;
	}

	return val;
}

COLORREF Options::GetColor(LPCTSTR param, COLORREF defval)
{
	UINT sz = 0;
	LPBYTE buf = NULL;

	if (theApp.GetProfileBinary(L"options", param, &buf, &sz))
	{
		if (sizeof(COLORREF) == sz)	return *((COLORREF*)buf);
		delete[] buf;
	}

	return defval;
}

void Options::Load()
{
	minSpeed = GetDouble(L"minSpeed", 0.);
	maxSpeed = GetDouble(L"maxSpeed", 0.3);
	minDensity = GetDouble(L"minDensity", 0.91);
	maxDensity = GetDouble(L"maxDensity", 1.06);
	minVorticity = GetDouble(L"minVorticity", -0.05);
	maxVorticity = GetDouble(L"maxVorticity", 0.05);
	
	chartColors = static_cast<ChartColors>(theApp.GetProfileInt(L"options", L"ChartColors", 1));
	zoom = theApp.GetProfileInt(L"options", L"zoom", 2);
	resultsType = theApp.GetProfileInt(L"options", L"resultsType", 1);

	obstaclesColor = GetColor(L"obstaclesColor", RGB(0, 0, 0));

	numThreads = theApp.GetProfileInt(L"options", L"numThreads", 4);
	refreshSteps = theApp.GetProfileInt(L"options", L"refreshSteps", 10);
	boundaryConditions = theApp.GetProfileInt(L"options", L"boundaryConditions", 1);

	tau = GetDouble(L"tau", 0.6);
	accelX = GetDouble(L"accelX", 0.015);

	useAccelX = theApp.GetProfileInt(L"options", L"useAccelX", 0);
	inletOption = theApp.GetProfileInt(L"options", L"inletOption", 0);
	outletOption = theApp.GetProfileInt(L"options", L"outletOption", 0);

	inletDensity = GetDouble(L"inletDensity", 1.03);
	outletDensity = GetDouble(L"outletDensity", 0.97);
	inletSpeed = GetDouble(L"inletSpeed", 0.03);
	outletSpeed = GetDouble(L"outletSpeed", 0.029);
}

void Options::Save()
{
	theApp.WriteProfileBinary(L"options", L"minSpeed", (LPBYTE)&minSpeed, sizeof(double));
	theApp.WriteProfileBinary(L"options", L"maxSpeed", (LPBYTE)&maxSpeed, sizeof(double));
	theApp.WriteProfileBinary(L"options", L"minDensity", (LPBYTE)&minDensity, sizeof(double));
	theApp.WriteProfileBinary(L"options", L"maxDensity", (LPBYTE)&maxDensity, sizeof(double));
	theApp.WriteProfileBinary(L"options", L"minVorticity", (LPBYTE)&minVorticity, sizeof(double));
	theApp.WriteProfileBinary(L"options", L"maxVorticity", (LPBYTE)&maxVorticity, sizeof(double));

	theApp.WriteProfileInt(L"options", L"ChartColors", static_cast<int>(chartColors));
	theApp.WriteProfileInt(L"options", L"zoom", zoom);
	theApp.WriteProfileInt(L"options", L"resultsType", resultsType);

	theApp.WriteProfileBinary(L"options", L"obstaclesColor", (LPBYTE)&obstaclesColor, sizeof(COLORREF));

	theApp.WriteProfileInt(L"options", L"numThreads", numThreads);
	theApp.WriteProfileInt(L"options", L"refreshSteps", refreshSteps);
	theApp.WriteProfileInt(L"options", L"boundaryConditions", boundaryConditions);

	theApp.WriteProfileBinary(L"options", L"tau", (LPBYTE)&tau, sizeof(double));
	theApp.WriteProfileBinary(L"options", L"accelX", (LPBYTE)&accelX, sizeof(double));


	theApp.WriteProfileInt(L"options", L"useAccelX", useAccelX);
	theApp.WriteProfileInt(L"options", L"inletOption", inletOption);
	theApp.WriteProfileInt(L"options", L"outletOption", outletOption);

	theApp.WriteProfileBinary(L"options", L"inletDensity", (LPBYTE)&inletDensity, sizeof(double));
	theApp.WriteProfileBinary(L"options", L"outletDensity", (LPBYTE)&outletDensity, sizeof(double));
	theApp.WriteProfileBinary(L"options", L"inletSpeed", (LPBYTE)&inletSpeed, sizeof(double));
	theApp.WriteProfileBinary(L"options", L"outletSpeed", (LPBYTE)&outletSpeed, sizeof(double));
}