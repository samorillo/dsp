/* !!!!! BECOMES UNSTABLE WITH FAST MODULATION OF "dy" */

#ifndef DSP_OSCGS_H
#define DSP_OSCGS_H

#include "constants.h"

#include <cmath>

namespace dsp {

struct OscGS { // // Gordon-Smith oscillator
	explicit OscGS(double _dy) {
		dy(_dy);
		xn = 1.;
		y = 0.5 * eps;
	}
	explicit OscGS() :
		xn{ 1. }, y{ 0. }
	{}
	OscGS(const OscGS& other) : 
		xn{other.xn}, y{other.y}, eps{other.eps}, dy_{other.dy_}
	{}
		
	double xn, y;
	double eps;
	double dy_;
		
	void operator()() {
		xn -= eps * y;
		y += eps * xn;
	}
	void dy(double value) {
		dy_ = value;
		eps = 2 * std::sin(PI * value);
	}
	double dy() const {
		return dy_;
	}
};

} // // namespace

#endif // // include guard
