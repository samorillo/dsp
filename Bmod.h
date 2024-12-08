/* original by Scott Nordlund */

#ifndef DSP_BMOD_H
#define DSP_BMOD_H

#include "Phasor.h"
#include "Pois.h"
#include "randomness.h"

namespace dsp {

struct Bmod {
	explicit Bmod(double mu, double min_dy, double max_dy) :
		pois{ mu }, min_dy{ min_dy }, max_dy{ max_dy }
	{
		p.reset(rr01());
	}
	explicit Bmod(double mu, double max_dy) :
		Bmod(mu, 0, max_dy)
	{}
	explicit Bmod() {}
	Bmod(const Bmod& other) : 
		pois{ other.pois }, p{ other.p }, 
		min_dy{ other.min_dy }, max_dy{ other.max_dy }
	{
		p.reset(rr01());
	}

	Pois pois;
	Phasor p;
	double min_dy, max_dy;
	double y;
	
	void operator()() {
		pois();
		if (pois.b) {
			p.dy(rr(min_dy, max_dy));
		}
		p();
		y = p.y;
	}
};

} // // namespace

#endif // // include guard
