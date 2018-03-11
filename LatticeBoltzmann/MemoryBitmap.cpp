#include "stdafx.h"
#include "MemoryBitmap.h"

#include "LatticeBoltzmann.h"

#include <utility>
#include <cassert>

MemoryBitmap::MemoryBitmap()
	: m_width(0), m_height(0), data(nullptr)
{
}


MemoryBitmap::MemoryBitmap(const MemoryBitmap& other) // copy constructor
{
	if (other.data)
	{
		m_width = other.m_width;
		m_height = other.m_height;
		int size = GetStrideLength() * m_height;
		data = new unsigned char[size];
		memcpy(data, other.data, size);
	}
	else {
		data = nullptr;
		m_width = m_height = 0;
	}
}

MemoryBitmap::MemoryBitmap(MemoryBitmap&& other) noexcept // move constructor
	: data(other.data), m_width(other.m_width), m_height(other.m_height)
{
	other.data = nullptr;
	other.m_height = other.m_width = 0;
}

MemoryBitmap& MemoryBitmap::operator=(const MemoryBitmap& other) //copy assignment operator
{
	MemoryBitmap temp(other);

	*this = std::move(temp);

	return *this;
}

MemoryBitmap& MemoryBitmap::operator=(MemoryBitmap&& other) noexcept // move assignment operator
{
	delete[] data;

	m_width = other.m_width;
	m_height = other.m_height;

	data = other.data;

	other.m_height = other.m_width = 0;
	other.data = nullptr;

	return *this;
}

MemoryBitmap::~MemoryBitmap() noexcept
{
	delete[] data;
}


void MemoryBitmap::SetSize(int width, int height)
{
	assert(width != 0 && height != 0);

	if (m_width != width || m_height != height)
	{
		delete[] data;

		m_width = width;
		m_height = height;

		data = new unsigned char[GetStrideLength() * height];
	}
}

void MemoryBitmap::SetMatrix(const Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, LatticeBoltzmann::Lattice::DataOrder>& results, int resultsType, const Eigen::Matrix<bool, Eigen::Dynamic, Eigen::Dynamic, LatticeBoltzmann::Lattice::DataOrder>& latticeObstacles)
{
	if (results.cols() == 0 || results.rows() == 0) return;

	SetSize((int)results.cols(), (int)results.rows());

	int stride = GetStrideLength();

	double minVal, maxVal;

	if (1 == resultsType)
	{
		minVal = theApp.options.minSpeed;
		maxVal = theApp.options.maxSpeed;
	}
	else if (2 == resultsType)
	{
		minVal = theApp.options.minVorticity;
		maxVal = theApp.options.maxVorticity;
	}
	else
	{
		minVal = theApp.options.minDensity;
		maxVal = theApp.options.maxDensity;
	}

	for (int j = 0; j < results.cols(); ++j)
		for (int i = 0; i < results.rows(); ++i)
		{
			const int line = (m_height - i - 1) * stride;
			const int pos = line + 3 * j;

			const COLORREF color = latticeObstacles(i, j) ? theApp.options.obstaclesColor : ConvertToColor(results(i, j), theApp.options.chartColors, minVal, maxVal);

			data[pos] = GetBValue(color);
			data[pos + 1] = GetGValue(color);
			data[pos + 2] = GetRValue(color);
		}
}


void MemoryBitmap::Draw(CDC* pDC)
{
	BITMAPINFO bmi;
	ZeroMemory(&bmi, sizeof(BITMAPINFOHEADER));
	
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = m_width;
	bmi.bmiHeader.biHeight = m_height;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 24;
	bmi.bmiHeader.biCompression = BI_RGB;

	CBitmap bitmap;
	
	bitmap.CreateCompatibleBitmap(pDC, m_width, m_height);
	::SetDIBits(pDC->GetSafeHdc(), bitmap, 0, m_height, data, &bmi, DIB_RGB_COLORS);
	CDC dcMemory;
	dcMemory.CreateCompatibleDC(pDC);
	CBitmap * pOldBitmap = dcMemory.SelectObject(&bitmap);
	pDC->BitBlt(0, 0, m_width, m_height, &dcMemory, 0, 0, SRCCOPY);
	dcMemory.SelectObject(pOldBitmap);
}

void MemoryBitmap::Draw(CDC* pDC, CRect& rect, int origWidth, int origHeight)
{
	BITMAPINFO bmi;
	ZeroMemory(&bmi, sizeof(BITMAPINFOHEADER));

	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = m_width;
	bmi.bmiHeader.biHeight = m_height;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 24;
	bmi.bmiHeader.biCompression = BI_RGB;

	CBitmap bitmap;

	bitmap.CreateCompatibleBitmap(pDC, m_width, m_height);
	::SetDIBits(pDC->GetSafeHdc(), bitmap, 0, m_height, data, &bmi, DIB_RGB_COLORS);
	CDC dcMemory;
	dcMemory.CreateCompatibleDC(pDC);
	CBitmap * pOldBitmap = dcMemory.SelectObject(&bitmap);
	pDC->StretchBlt(rect.left, rect.top, rect.Width(), rect.Height(), &dcMemory, origWidth ? (m_width - origWidth)/2 : 0, origHeight ? (m_height - origHeight)/2 : 0, origWidth ? origWidth : m_width, origHeight ? origHeight : m_height, SRCCOPY);
	dcMemory.SelectObject(pOldBitmap);
}




