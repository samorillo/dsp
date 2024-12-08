/* original by Scott Nordlund */

#ifndef DSP_POIS_H
#define DSP_POIS_H

#include "randomness.h"

namespace dsp {

struct Pois {
	explicit Pois(double _mu, int _min = 0, bool _init = false) : 
		mu{ _mu }, min{ _min },
		//N{ _init ? 1 : (int)exponentialDistribution(mu) }, // // avoids triggering all instances at time 0
		N{ 1 },
		n{ 0 }, dy{ 1./N }, y{ 0. }
	{}
	explicit Pois() : 
		mu{ 1. }, min{ 0 },
		N{ 1 },
		n{ 0 }, dy{ 1./N }, y{ 0. }
	{}
	Pois(const Pois& other) : Pois(other.mu, other.min) {}

	double mu;
	int min;
	int N;
	int n;
	double dy;
	double y;
	bool b;

	Pois& operator()() {
		++n;
		if (n >= N) {
			n = 0;
			N = (int)exponentialDistribution(mu);
			if (N < min) N = min;
			dy = 1./N;
			y = 0.;
			b = true;
		}
		else {
			y += dy;
			b = false;
		}
		return *this;
	}
};

} // // namespace

#endif // // include guard
