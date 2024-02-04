#pragma once

#include "Lattice.h"

#if ( _MSC_VER < 1900 )
#define noexcept
#endif

class MemoryBitmap
{
public:
	MemoryBitmap();

	MemoryBitmap(const MemoryBitmap& other); // copy constructor
	MemoryBitmap(MemoryBitmap&& other) noexcept; // move constructor
	MemoryBitmap& operator=(const MemoryBitmap& other); //copy assignment operator
	MemoryBitmap& operator=(MemoryBitmap&& other) noexcept; // move assignment operator

	~MemoryBitmap() noexcept;
protected:
	_declspec(align(16)) int m_width;
	_declspec(align(16)) int m_height;

	unsigned char* data;

	inline int GetStrideLength() const {
		return 4 * ((m_width * 3 + 3) / 4);
	}

	inline COLORREF ConvertToColor(double value, int colorType, double minVal, double maxVal) const
	{
		COLORREF color = 0;

		if (value < minVal) value = minVal;
		else if (value > maxVal) value = maxVal;

		if (const double interval = maxVal - minVal; 0 == colorType) // two colors
		{
			const int B = static_cast<int>((value - minVal) / interval * 255.);
			const int R = 255 - B;

			color = RGB(R, 0, B);
		}
		else
		{
			if (int v = static_cast<int>((value - minVal) / interval * 255. * 2.); v > 0xff)
			{
				v -= 0xff;

				const int B = v;
				const int G = 255 - v;

				color = RGB(0, G, B);
			}
			else
			{
				const int G = v;
				const int R = 255 - v;

				color = RGB(R, G, 0);
			}
		}

		return color;
	}
public:
	void SetSize(int width, int height);

	void SetMatrix(const Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, LatticeBoltzmann::Lattice::DataOrder>& results, int resultsType, const Eigen::Matrix<bool, Eigen::Dynamic, Eigen::Dynamic, LatticeBoltzmann::Lattice::DataOrder>& latticeObstacles);

	void Draw(CDC* pDC) const;
	void Draw(CDC* pDC, CRect& rect, int origWidth = 0, int origHeight = 0) const;
};

