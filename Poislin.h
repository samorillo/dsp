/* original by Scott Nordlund */

#ifndef DSP_POISLIN_H
#define DSP_POISLIN_H

#include "randomness.h"

namespace dsp {

struct Poislin {
	explicit Poislin(int _min, int _max) :
		min{ _min }, max{ _max },
		//N{ rr(min, max) }, // // avoids triggering all instances at time 0
		N{ 1 },
		n{0}, dy{ 1./N }, y{ 0. }
	{}
	explicit Poislin() :
		min{ 0 }, max{ 0 },
		N{ 1 },
		n{0}, dy{ 1./N }, y{ 0. }
	{}
	Poislin(const Poislin& other) : Poislin(other.min, other.max) {}
		
	int min, max;	
	int N;
	int n;
	double dy;
	double y;
	bool b;

	Poislin& operator()() {
		++n;
		if (n >= N) {
			n = 0;
			N = rr(min, max);
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
