/*
original by Scott Nordlund
*/

#ifndef DSP_EARLYREF_H
#define DSP_EARLYREF_H

#include "../Biquad.h"
#include "../Double2.h"
#include "Density.h"
#include "Multitap.h"

namespace dsp {

struct Earlyref {
	explicit Earlyref(int _SR) : SR{_SR} {}
	Earlyref(const Earlyref& other) : 
		SR{other.SR},
		dens{ other.dens[0], other.dens[1] },
		units{ other.units[0], other.units[1] }
	{}
	
	const int SR;
	Density2 dens[2]{ Density2{8, 10, SR * 0.1}, Density2{8, 10, SR * 0.1} };
	
	struct Unit {
		explicit Unit(int _nTaps, int mind, int maxd, int mint, int maxt) :
			nTaps{ _nTaps },
			minDel{ mind }, maxDel{ maxd },
			minTime{ mind }, maxTime{ maxd },
			taps{ new Multitap*[4] }
		{
			for (int i = 0; i < 4; ++i) {
				taps[i] = new Multitap(nTaps, minDel, maxDel, minTime, maxTime);
			}
		}
		Unit(const Unit& other) :
			nTaps{ other.nTaps },
			minDel{ other.minDel }, maxDel{ other.maxDel },
			minTime{ other.minTime }, maxTime{ other.maxTime },
			taps{ new Multitap*[4] }
		{
			for (int i = 0; i < 4; ++i) {
				taps[i] = new Multitap(*other.taps[i]);
			}
		}
		~Unit() {
			for (int i = 0; i < 4; ++i) {
				delete taps[i];
			}
			delete[] taps;
		}
		
		int nTaps, minDel, maxDel, minTime, maxTime;
		Multitap** taps;
		
		double y[2];
		
		void operator()(double x0, double x1, double x2, double x3) {
			y[0] = (**(taps+0))(x0).y + (**(taps+1))(x1).y;
			y[1] = (**(taps+2))(x2).y + (**(taps+3))(x3).y;
		}
	};
	Unit units[2]{
		Unit(4, 100, int(0.05 * SR), int(1.6 * SR), int(2.5 * SR)),
		Unit(3, 100, int(0.10 * SR), int(1.6 * SR), int(2.5 * SR))
	};
	
	Double2 x;
	Double2 y;
	
	void operator()() {
		for (int i = 0; i < 2; ++i) {
			double temp[2] = { 0.0, 0.0 };
			temp[1 - i] = x[i];
			dens[i](temp);
		}
		for (int o = 0, i = 1; o < 2; ++o, --i) {
			units[o](
				dens[o].y[o][o] + x[o], dens[o].y[o][i] + x[o],
				dens[i].y[o][o] + x[i], dens[i].y[o][i] + x[i]
			);
		}
		y[0] = 0.707107 * (units[0].y[0] + units[1].y[0]);
		y[1] = 0.707107 * (units[0].y[1] + units[1].y[1]) * 0.7;
		x[0] = x[1] = 0;
	}
};

} // // namespace

#endif // // include guard
