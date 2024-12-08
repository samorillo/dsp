/*
hp = x - lp;
ap = lp - hp = 2*lp - x
*/

#ifndef DSP_ONEPOLE_H
#define DSP_ONEPOLE_H

#include "constants.h"
#include "utilities.h"

#include <cmath>

namespace dsp {
	
struct Onepole {
	explicit Onepole() {}
	explicit Onepole(double _a0, double _a1, double _b0, double _b1) :
		a0{_a0}, a1{_a1}, b0{_b0}, b1{_b1}
	{}
	double a0, a1, b0, b1;
	double y;
	Onepole& operator()(double x) {
		y = a0 * x + a1 * x_1 + b1 * y_1;
		y_1 *= 1. / b1;
		x_1 = x;
		y_1 = y;
		return *this;
	}
	double x_1 = 0, y_1 = 0;
	double phaseResponse(double dy) const {
		double w = TWOPI * dy;
		double cosw = std::cos(w);
		double sinw = std::sin(w);
		double c0 = a0 + a1 * cosw;
		double c1 =      a1 * sinw;
		double d0 = b0 + b1 * cosw;
		double d1 =      b1 * sinw;
		double f = (c1*d0 + c0*d1) / (c0*d0 * c1*d1);
		return std::atan(f) + (f < 0 ? PI : 0);
	}
};

struct Onepole_lp {
	explicit Onepole_lp() : y_1{0.} {}
	explicit Onepole_lp(double _dy) : Onepole_lp() { dy(_dy); }
	Onepole_lp(const Onepole_lp& other) :
		dy_{other.dy_}, a0{other.a0}, b1{other.b1}, y_1{other.y}
	{}
	
	double dy_;
	double a0, b1;
	double y_1;
	double y;
	
	Onepole_lp& operator()(double x) {
		y = a0 * x + b1 * y_1;
		y_1 = y;
		return *this;
	}
	void dy(double value) { 
		dy_ = value;
		a0 = 1.0 - std::exp(-TWOPI*dy_);
		b1 = 1.0 - a0;
	}
	double dy() const { return dy_; }
	void reset(double y_1_ = 0) {
		y_1 = y_1_;
		y = 0;
	}
	
	// // https://ccrma.stanford.edu/~jos/filters/One_Pole.html
	virtual double phaseResponse(double dy) const {
		double wT = TWOPI * dy;
		double tmp = -std::atan((b1*std::sin(wT))/(1-b1*std::cos(wT)));
		return a0>0 ? tmp : PI + tmp;
	}
};

struct Onepole_hp : public Onepole_lp {
	using Onepole_lp::Onepole_lp;
	Onepole_hp& operator()(double x) {
		y_1 = a0 * x + b1 * y_1;
		y = x - y_1;
		return *this;
	}
	double phaseResponse(double dy) const override {
		// // y = x - a0*x / (1 + b1*z) ===> (1-a0 + b1*z)*x / (1 + b1*z)
		return dsp::phaseResponse(dy, 1.-a0, b1, 1., b1);
	}
};

struct Onepole_ap : public Onepole_lp {
	using Onepole_lp::Onepole_lp;
	Onepole_ap& operator()(double x) {
		y_1 = a0 * x + b1 * y_1;
		y = 2*y_1 - x;
		return *this;
	}
	double phaseResponse(double dy) const override {
		// // y = 2*a0*x / (1 + b1*z) - x ===> (2*a0-1 + b1*z)*x / (1 + b1*z)
		return dsp::phaseResponse(dy, 2.*a0-1., -b1, 1., b1);
	}
};

struct OnepoleRaw {
	explicit OnepoleRaw() {}
	explicit OnepoleRaw(double _ramp_time) {
		ramp_time(_ramp_time);
	}
	OnepoleRaw(const OnepoleRaw& other) :
		ramp_time_{other.ramp_time_}, k{other.k}, mkp1{other.mkp1}
	{}
	
	double ramp_time_;
	double k;
	double mkp1;
	double y;
	
	OnepoleRaw& operator()(double x) {
		y = k * x + mkp1 * y;
		return *this;
	}
	void ramp_time(double val) {
		ramp_time_ = val;
		k = 1 - std::pow(1. - 0.9, 1. / (ramp_time_ - 1.));
		mkp1 = 1. - k;
	}
	double ramp_time() const { return ramp_time_; };
};

} // // namespace

#endif // // include guard