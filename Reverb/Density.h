/*
original by Scott Nordlund
modified gain compensation
*/

#ifndef DSP_DENSITY_H
#define DSP_DENSITY_H

#include "../Delay.h"

#include <cmath>

namespace dsp {

struct Density {
	explicit Density(int N, double rt60) :
		N{ N }
	{
		if (N <= 0 || 16 <= N) throw "err";
		delayLines = new Delay*[N];
		g = new double[N];
		for (int i = 0; i < N; ++i) {
			delayLines[i] = new Delay{ deltimes[i] };
		}

		double inv_rt60 = 1.0 / rt60;
		gainCompensation = 1;
		for (int i = N-1; i >= 0; --i) {
			g[i] = std::pow(10, -3.0 * deltimes[i] * inv_rt60);
			gainCompensation *= (1 + g[i]);
		}
		gainCompensation = 1./std::sqrt(gainCompensation);
	}
	Density(const Density& other) : 
		N{ other.N },
		gainCompensation{ other.gainCompensation },
		delayLines{ new Delay*[N] },
		g{ new double[N] }
	{
		for (int i = 0; i < N; ++i) {
			delayLines[i] = new Delay{ deltimes[i] };
		}
		for (int i = 0; i < N; ++i) {
			g[i] = other.g[i];
		}
	}
	~Density() {
		for (int i = 0; i < N; ++i) {
			delete delayLines[i];
		}
		delete[] delayLines;
		delete[] g;
	}
	
	const int N;
	static constexpr int deltimes[16]{ 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768 };
	Delay** delayLines;
	double* g;
	long double gainCompensation;

	double y[2];
	void operator()(double x[2]) {
		y[0] = x[0] + x[1];
		y[1] = x[0] - x[1];
		for (int i = 0; i < N; ++i) {
			double temp0 = y[0];
			double temp1 = g[i] * delayLines[i]->y_prev();
			delayLines[i]->operator()(y[1]);
			y[0] = temp0 + temp1;
			y[1] = temp0 - temp1;
		}
		y[0] *= gainCompensation;
		y[1] *= gainCompensation;
	}
};

struct Density2 {
	explicit Density2(int N0, int N1, double rt60) :
		N0{ N0 }, N1{ N1 },
		Nmin{ N0 >= N1 ? N1 : N0 },
		Nmax{ N0 >= N1 ? N0 : N1 },
		Nand{ Nmax == N1 }
	{
		if (Nmin <= 0 || 16 < Nmax) throw "err";
		delayLines = new Delay*[Nmax];
		g = new double[Nmax];
		for (int i = 0; i < Nmax; ++i) {
			delayLines[i] = new Delay{ deltimes[i] };
		}

		double inv_rt60 = 1.0 / rt60;
		gainCompensation[0] = gainCompensation[1] = 1.;
		for (int i = 0; i < Nmax; ++i) {
			g[i] = std::pow(10, -3.0 * deltimes[i] * inv_rt60);
			gainCompensation[Nand] *= (1 + g[i]);
			if (i == Nmin) {
				gainCompensation[1-Nand] = gainCompensation[Nand];
			}
		}
		for (int i = 0; i < 2; ++i) {
			gainCompensation[i] = 1.0 / std::sqrt(gainCompensation[i]);
		}
	}
	Density2(const Density2& other) :
		N0{ other.N0 }, N1{ other.N1 },
		Nmin{ other.Nmin },
		Nmax{ other.Nmax },
		Nand{ other.Nand },
		delayLines{ new Delay*[Nmax] },
		g{ new double[Nmax] }
	{
		for (int i = 0; i < Nmax; ++i) {
			delayLines[i] = new Delay{ deltimes[i] };
		}
		for (int i = 0; i < Nmax; ++i) {
			g[i] = other.g[i];
		}
		for (int i = 0; i < 2; ++i) {
			gainCompensation[i] = other.gainCompensation[i];
		}
	}
	~Density2() {
		for (int i = 0; i < Nmax; ++i) {
			delete delayLines[i];
		}
		delete[] delayLines;
		delete[] g;
	}
	
	const int N0;
	const int N1;
	const int Nmin;
	const int Nmax;
	const bool Nand;
	
	static constexpr int deltimes[16]{ 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768 };
	Delay** delayLines;
	double* g;
	long double gainCompensation[2];

	double y[2][2]{};

	void operator()(double x[2]) {
		y[Nand][0] = x[0] + x[1];
		y[Nand][1] = x[0] - x[1];
		for (int i = 0; i < Nmax; ++i) {
			double temp0 = y[Nand][0];
			double temp1 = g[i] * delayLines[i]->y();
			delayLines[i]->operator()(y[Nand][1]);
			y[Nand][0] = temp0 + temp1;
			y[Nand][1] = temp0 - temp1;
			if (i == Nmin) {
				y[1-Nand][0] = y[Nand][0];
				y[1-Nand][1] = y[Nand][1];
			}
		}
		y[0][0] *= gainCompensation[0];
		y[0][1] *= gainCompensation[0];
		y[1][0] *= gainCompensation[1];
		y[1][1] *= gainCompensation[1];
	}
};

} // // namespace

#endif // // include guard
