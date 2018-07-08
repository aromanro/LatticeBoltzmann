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
	int m_width;
	int m_height;

	unsigned char* data;

	inline int GetStrideLength() {
		return 4 * ((m_width * 3 + 3) / 4);
	}

	inline COLORREF ConvertToColor(double value, int colorType, double minVal, double maxVal)
	{
		COLORREF color = RGB(0, 0, 0);

		if (value < minVal) value = minVal;
		else if (value > maxVal) value = maxVal;

		const double interval = maxVal - minVal;
		
		if (0 == colorType) // two colors
		{
			const double unit = interval / 255.;

			int B = static_cast<int>((value - minVal) / unit);
			int R = 255 - B;

			color = RGB(R, 0, B);
		}
		else
		{
			const double unit = interval / (255. * 2.);

			int v = static_cast<int>((value - minVal) / unit);

			if (v > 0xff)
			{
				v -= 0xff;


				int B = v;
				int G = 255 - v;

				color = RGB(0, G, B);
			}
			else
			{
				int G = v;
				int R = 255 - v;

				color = RGB(R, G, 0);
			}
		}

		return color;
	}
public:
	void SetSize(int width, int height);

	void SetMatrix(const Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, LatticeBoltzmann::Lattice::DataOrder>& results, int resultsType, const Eigen::Matrix<bool, Eigen::Dynamic, Eigen::Dynamic, LatticeBoltzmann::Lattice::DataOrder>& latticeObstacles);

	void Draw(CDC* pDC);
	void Draw(CDC* pDC, CRect& rect, int origWidth = 0, int origHeight = 0);
};

