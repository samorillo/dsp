/*
from "The Art of VA Filter Design" by Vadim Zavalishin
*/

#ifndef DSP_SVFVA_H
#define DSP_SVFVA_H

#include "utilities.h"

namespace dsp {

struct SvfVA {
	explicit SvfVA() : s1{ 0.0 }, s2{ 0.0 } {}
	explicit SvfVA(double _dy, double _q = SQRT2_2) : SvfVA() {
		dy(_dy);
		q(_q);
	}
	SvfVA(const SvfVA& other) :
		dy_{other.dy_}, q_{other.q_}, g{other.g}, r{other.r},
		s1{other.s1}, s2{other.s2}
	{}
		
	double dy_;
	double q_;
	double g;
	double r;
	double s1, s2;
	double y_lp, y_hp, y_bpr, y_bp; //  bp{ 0.0 };

	SvfVA& operator()(double x) {
		double g1 = 2*r + g;
		double d = 1 / (1 + 2*r*g + g*g);
		y_hp = (x - g1*s1 - s2) * d;
		double v1 = g*y_hp;
		y_bpr = v1 + s1;
		y_bp = y_bpr / (2.0*r);
		s1 = y_bpr + v1;
		double v2 = g*y_bpr;
		y_lp = v2 + s2;
		s2 = y_lp + v2;
		//bp = 2*R*bpr;
		return *this;
	}
	void dy(double value) {
		dy_ = value;
		g = dytog(dy_);
	}
	double dy() { return dy_; }
	void q(double value) {
		q_ = value;
		r = qtor(q_);
	}
	double q() { return q_; }
	void reset() {
		s1 = 0.;
		s2 = 0.;
	}
};

} // // namespace

#endif // // include guard