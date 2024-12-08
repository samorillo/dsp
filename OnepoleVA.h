// // from V. Zavalishin's "The Art of VA Filter Design"

#ifndef DSP_ONEPOLEVA_H
#define DSP_ONEPOLEVA_H

#include "utilities.h"

namespace dsp {

struct OnepoleVA_lp {
    explicit OnepoleVA_lp() : s{0.} {}
	explicit OnepoleVA_lp(double _dy) : OnepoleVA_lp() { dy(_dy); }
	OnepoleVA_lp(const OnepoleVA_lp& other) :
		G{other.G}, s{other.s}
	{}
	
	double G = 0;
	double s = 0;
	double y;
	
	OnepoleVA_lp& operator()(double x) {
        v = (x - s) * G;
		y = v + s;
        s = y + v;
		return *this;
	}
    double v; // // aux
	void dy(double value) { 
        g(dytog(value));
	}
    void g(double _g) {
        G = _g / (1.0 + _g);
    }
	void reset(double y_1_ = 0) {
		s = y_1_;
		y = 0;
	}
};

struct OnepoleVA_hp {
    explicit OnepoleVA_hp() : s{0.} {}
	explicit OnepoleVA_hp(double _dy) : OnepoleVA_hp() { dy(_dy); }
	OnepoleVA_hp(const OnepoleVA_hp& other) :
		Ghp{other.Ghp}, s{other.s}
	{}
	
	double Ghp = 0;
	double s = 0;
    double g2;
	double y;
	
	OnepoleVA_hp& operator()(double x) {
        xs = x - s;
        y = xs * Ghp;
        s = s + y * g2;
		return *this;
	}
    double xs; // // aux
	void dy(double value) { 
        g(dytog(value));
	}
    void g(double _g) {
        Ghp = 1.0 / (1.0 + _g);
        g2 = 2.0 * _g;
    }
	void reset(double y_1_ = 0) {
		s = y_1_;
		y = 0;
	}
};

struct OnepoleVA_ap {
    explicit OnepoleVA_ap() : s{0.} {}
	explicit OnepoleVA_ap(double _dy) : OnepoleVA_ap() { dy(_dy); }
	OnepoleVA_ap(const OnepoleVA_ap& other) :
		Glp2{other.Glp2}, s{other.s}
	{}
	
	double Glp2 = 0;
	double s = 0;
	double y;
	
	OnepoleVA_ap& operator()(double x) {
        xs = x - s;
        s = s + xs * Glp2;
        y = s - xs;
		return *this;
	}
    double xs; // // aux
	void dy(double value) { 
        g(dytog(value));
	}
    void g(double _g) {
        Glp2 = 2.0 * _g / (1.0 + _g);
    }
	void reset(double y_1_ = 0) {
		s = y_1_;
		y = 0;
	}
};

} // // namespace

#endif // // include guard