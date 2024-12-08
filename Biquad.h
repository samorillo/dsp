/*
from Cookbook
*/

#ifndef DSP_BIQUAD_H
#define DSP_BIQUAD_H

#include "constants.h"

#include <cmath>

// // ln_rt40_10 := log( pow(10., 1./40.) )
static constexpr double log_rt40_10 = 0.05756462732485114210044978636710910519002753721571932440083319752418931524193381200;

namespace dsp {

inline void biquadcoeff_lp(double c[5], double dy, double q) {
	double w = TWOPI * dy;
	double cosw = std::cos(w);
	double alpha = std::sin(w) / (2.0 * q);

	c[0] = (1.0 - cosw) * 0.5;
	c[1] = 1.0 - cosw;
	c[2] = (1.0 - cosw) * 0.5;
	c[3] = -2.0 * cosw;
	c[4] = 1.0 - alpha;
	double a0_inv = 1.0 / (1.0 + alpha);

	for (int i = 0; i < 5; ++i) c[i] *= a0_inv;
}

inline void biquadcoeff_hp(double c[5], double dy, double q) {
	double w = TWOPI * dy;
	double cosw = std::cos(w);
	double alpha = std::sin(w) / (2.0 * q);

	c[0] = (1.0 + cosw) * 0.5;
	c[1] = -(1.0 + cosw);
	c[2] = (1.0 + cosw) * 0.5;
	c[3] = -2.0 * cosw;
	c[4] = 1.0 - alpha;
	double a0_inv = 1.0 / (1.0 + alpha);

	for (int i = 0; i < 5; ++i) c[i] *= a0_inv;
}

inline void biquadcoeff_bpr(double c[5], double dy, double q) {
	double w = TWOPI * dy;
	double cosw = std::cos(w);
	double alpha = std::sin(w) / (2.0 * q);

	c[0] = q * alpha;
	c[1] = 0.0;
	c[2] = -q * alpha;
	c[3] = -2.0 * cosw;
	c[4] = 1.0 - alpha;
	double a0_inv = 1.0 / (1.0 + alpha);

	for (int i = 0; i < 5; ++i) c[i] *= a0_inv;
}

inline void biquadcoeff_bp(double c[5], double dy, double q) { ////////////
	double w = TWOPI * dy;
	double cosw = std::cos(w);
	double alpha = std::sin(w) / (2.0 * q);

	c[0] = alpha;
	c[1] = 0.0;
	c[2] = -alpha;
	c[3] = -2.0 * cosw;
	c[4] = 1.0 - alpha;
	double a0_inv = 1.0 / (1.0 + alpha);

	for (int i = 0; i < 5; ++i) c[i] *= a0_inv;
}

inline void biquadcoeff_notch(double c[5], double dy, double q) {
	double w = TWOPI * dy;
	double cosw = std::cos(w);
	double alpha = std::sin(w) / (2.0 * q);

	c[0] = 1.0;
	c[1] = -2.0 * cosw;
	c[2] = 1.0;
	c[3] = -2.0 * cosw;
	c[4] = 1.0 - alpha;
	double a0_inv = 1.0 / (1.0 + alpha);

	for (int i = 0; i < 5; ++i) c[i] *= a0_inv;
}

inline void biquadcoeff_ap(double c[5], double dy, double q) {
	double w = TWOPI * dy;
	double cosw = std::cos(w);
	double alpha = std::sin(w) / (2.0 * q);

	c[0] = 1.0 - alpha;
	c[1] = -2.0 * cosw;
	c[2] = 1.0 + alpha;
	c[3] = -2.0 * cosw;
	c[4] = 1.0 - alpha;
	double a0_inv = 1.0 / (1.0 + alpha);

	for (int i = 0; i < 5; ++i) c[i] *= a0_inv;
}

inline void biquadcoeff_peak(double c[5], double dy, double q, double dbGain) {
	double w = TWOPI * dy;
	double cosw = std::cos(w);
	double alpha = std::sin(w) / (2.0 * q);
	double A = std::exp(log_rt40_10 * dbGain);

	c[0] = 1.0 + alpha * A;
	c[1] = -2.0 * cosw;
	c[2] = 1.0 - alpha * A;
	c[3] = -2.0 * cosw;
	c[4] = 1.0 - alpha / A;
	double a0_inv = 1.0 / (1.0 + alpha / A);

	for (int i = 0; i < 5; ++i) c[i] *= a0_inv;
}

inline void biquadcoeff_ls(double c[5], double dy, double q, double dbGain) {
	double w = TWOPI * dy;
	double cosw = std::cos(w);
	double alpha = std::sin(w) / (2.0 * q);
	double A = std::exp(log_rt40_10 * dbGain);
	double two_sqrtA_alpha = 2 * std::sqrt(A) * alpha;

	c[0] = A * ((A + 1) - (A - 1) * cosw + two_sqrtA_alpha);
	c[1] = 2 * A * ((A - 1) - (A + 1) * cosw);
	c[2] = A * ((A + 1) - (A - 1) * cosw - two_sqrtA_alpha);
	c[3] = -2 * ((A - 1) + (A + 1) * cosw);
	c[4] = (A + 1) + (A - 1) * cosw - two_sqrtA_alpha;
	double a0_inv = 1.0 / ((A + 1) + (A - 1) * cosw + two_sqrtA_alpha);

	for (int i = 0; i < 5; ++i) c[i] *= a0_inv;
}

inline void biquadcoeff_hs(double c[5], double dy, double q, double dbGain) {
	double w = TWOPI * dy;
	double cosw = std::cos(w);
	double alpha = std::sin(w) / (2.0 * q);
	double A = std::exp(log_rt40_10 * dbGain);
	double two_sqrtA_alpha = 2 * std::sqrt(A) * alpha;

	c[0] = A * ((A + 1) + (A - 1) * cosw + two_sqrtA_alpha);
	c[1] = -2 * A * ((A - 1) + (A + 1) * cosw);
	c[2] = A * ((A + 1) + (A - 1) * cosw - two_sqrtA_alpha);
	c[3] = 2 * ((A - 1) - (A + 1) * cosw);
	c[4] = (A + 1) - (A - 1) * cosw - two_sqrtA_alpha;
	double a0_inv = 1.0 / ((A + 1) - (A - 1) * cosw + two_sqrtA_alpha);

	for (int i = 0; i < 5; ++i) c[i] *= a0_inv;
}

///////////////////////////////////

struct Biquad {
	explicit Biquad() : x_n{0.,0.}, y_n{0.,0.} {}
	explicit Biquad(double a0, double a1, double a2, double b1, double b2) :
		Biquad()
	{
		coeff[0] = a0; coeff[1] = a1; coeff[2] = a2;
		coeff[3] = b1; coeff[4] = b2;
	}
	explicit Biquad(double* _coeff) :
		Biquad()
	{
		for (int i = 0; i < 5; ++i) coeff[i] = _coeff[i];
	}
	Biquad(const Biquad& other)
	{
		for (int i = 0; i < 5; ++i) coeff[i] = other.coeff[i];
		for (int i = 0; i < 2; ++i) {
			x_n[i] = other.x_n[i];
			y_n[i] = other.y_n[i]; 
		}
	}
	
	double coeff[5];
	double x_n[2];
	double y_n[2];
	double y; // // not useless: serves as temporary value
	
	Biquad& operator()(double x) {
		y = coeff[0] * x + coeff[1] * x_n[0] + coeff[2] * x_n[1]
			- coeff[3] * y_n[0] - coeff[4] * y_n[1];
		x_n[1] = x_n[0]; x_n[0] = x;
		y_n[1] = y_n[0]; y_n[0] = y;
		return *this;
	}
	void reset(double value = 0.) {
		x_n[0] = x_n[1] = y_n[0] = y_n[1] = value;
	}
	void set(double ff0, double ff1, double ff2, double mfb1, double mfb2) {
		coeff[0] = ff0; coeff[1] = ff1; coeff[2] = ff2;
		coeff[3] = mfb1; coeff[4] = mfb2;
	}
};

struct Biquad_prototype1 : public Biquad {
	explicit Biquad_prototype1(
		void(*_calculate_coefficients)(double*, double, double)
	) :
		calculate_coefficients{ _calculate_coefficients }
	{}
	explicit Biquad_prototype1(
		void (*_calculate_coefficients)(double*, double, double),
		double _dy, double _q
	) :
		calculate_coefficients{ _calculate_coefficients }, dy_{ _dy }, q_{ _q }
	{
		calculate_coefficients(coeff, dy_, q_);
	}
	Biquad_prototype1(const Biquad_prototype1& other) :
		Biquad(other),
		calculate_coefficients{ other.calculate_coefficients }, 
		dy_{ other.dy_ }, q_{ other.q_ }
	{}
		
	void (*calculate_coefficients)(double*, double, double);
	double dy_;
	double q_;

	void dy_q(double value_dy, double value_q) {
		dy_ = value_dy;
		q_ = value_q;
		calculate_coefficients(coeff, dy_, q_);
	}
	void dy(double _dy) {
		dy_ = _dy;
		calculate_coefficients(coeff, dy_, q_);
	}
	double dy() const { return dy_; }
	void q(double value) {
		q_ = value;
		calculate_coefficients(coeff, dy_, q_);
	}
	double q() const { return q_; }
	Biquad_prototype1& operator()(double x) {
		this->Biquad::operator()(x);
		return *this;
	}
};

struct Biquad_prototype2 : public Biquad {
	explicit Biquad_prototype2(
		void(*_calculate_coefficients)(double*, double, double, double)
	) :
		calculate_coefficients{ _calculate_coefficients }
	{}
	explicit Biquad_prototype2(
		void (*_calculate_coefficients)(double*, double, double, double),
		double _dy, double _q, double _db
	) :
		calculate_coefficients{ _calculate_coefficients }, 
		dy_{ _dy }, q_{ _q }, db_{ _db }
	{
		calculate_coefficients(coeff, dy_, q_, db_);
	}
	Biquad_prototype2(const Biquad_prototype2& other) :
		Biquad(other),
		calculate_coefficients{ other.calculate_coefficients }, 
		dy_{ other.dy_ }, q_{ other.q_ }, db_{ other.db_ }
	{}
		
	void (*calculate_coefficients)(double*, double, double, double);
	double dy_;
	double q_;
	double db_;

	void dy_q_db(double value_dy, double value_q, double value_db) {
		dy_ = value_dy;
		q_ = value_q;
		db_ = value_db;
		calculate_coefficients(coeff, dy_, q_, db_);
	}
	void dy(double _dy) {
		dy_ = _dy;
		calculate_coefficients(coeff, dy_, q_, db_);
	}
	double dy() const { return dy_; }
	void q(double value) {
		q_ = value;
		calculate_coefficients(coeff, dy_, q_, db_);
	}
	double q() const { return q_; }
	void db(double value) {
		db_ = value;
		calculate_coefficients(coeff, dy_, q_, db_);
	}
	double db() const { return db_; }
	Biquad_prototype2& operator()(double x) {
		this->Biquad::operator()(x);
		return *this;
	}	
};

// // prototype1:
struct Biquad_lp : public Biquad_prototype1 {
	explicit Biquad_lp() : Biquad_prototype1(biquadcoeff_lp) {}
	explicit Biquad_lp(double _dy, double _q = SQRT2_2) :
		Biquad_prototype1(biquadcoeff_lp, _dy, _q) 
	{}
	Biquad_lp(const Biquad_lp& other) : Biquad_prototype1(other) {}
};

struct Biquad_hp : public Biquad_prototype1 {
	explicit Biquad_hp() : Biquad_prototype1(biquadcoeff_hp) {}
	explicit Biquad_hp(double _dy, double _q = SQRT2_2) :
		Biquad_prototype1(biquadcoeff_hp, _dy, _q)
	{}
	Biquad_hp(const Biquad_hp& other) : Biquad_prototype1(other) {}
};

struct Biquad_bpr : public Biquad_prototype1 {
	explicit Biquad_bpr() : Biquad_prototype1(biquadcoeff_bpr) {}
	explicit Biquad_bpr(double _dy, double _q = SQRT2_2) :
		Biquad_prototype1(biquadcoeff_bpr, _dy, _q)
	{}
	Biquad_bpr(const Biquad_bpr& other) : Biquad_prototype1(other) {}
};

struct Biquad_bp : public Biquad_prototype1 {
	explicit Biquad_bp() : Biquad_prototype1(biquadcoeff_bp) {}
	explicit Biquad_bp(double _dy, double _q = SQRT2_2) :
		Biquad_prototype1(biquadcoeff_bp, _dy, _q)
	{}
	Biquad_bp(const Biquad_bp& other) : Biquad_prototype1(other) {}
};

struct Biquad_notch : public Biquad_prototype1 {
	explicit Biquad_notch() : Biquad_prototype1(biquadcoeff_notch) {}
	explicit Biquad_notch(double _dy, double _q = SQRT2_2) :
		Biquad_prototype1(biquadcoeff_notch, _dy, _q)
	{}
	Biquad_notch(const Biquad_notch& other) : Biquad_prototype1(other) {}
};

struct Biquad_ap : public Biquad_prototype1 {
	explicit Biquad_ap() : Biquad_prototype1(biquadcoeff_ap) {}
	explicit Biquad_ap(double _dy, double _q = SQRT2_2) :
		Biquad_prototype1(biquadcoeff_ap, _dy, _q)
	{}
	Biquad_ap(const Biquad_ap& other) : Biquad_prototype1(other) {}
};

// // prototype2:
struct Biquad_peak : public Biquad_prototype2 {
	explicit Biquad_peak() : Biquad_prototype2(biquadcoeff_peak) {}
	explicit Biquad_peak(double _dy, double _q = SQRT2_2, double _dbGain = 0.) : 
		Biquad_prototype2(biquadcoeff_peak, _dy, _q, _dbGain) 
	{}
	Biquad_peak(const Biquad_peak& other) : Biquad_prototype2(other) {}
};

struct Biquad_ls : public Biquad_prototype2 {
	explicit Biquad_ls() : Biquad_prototype2(biquadcoeff_ls) {}
	explicit Biquad_ls(double _dy, double _q = SQRT2_2, double _dbGain = 0.) : 
	Biquad_prototype2(biquadcoeff_ls, _dy, _q, _dbGain) 
	{}
	Biquad_ls(const Biquad_ls& other) : Biquad_prototype2(other) {}
};

struct Biquad_hs : public Biquad_prototype2 {
	explicit Biquad_hs() : Biquad_prototype2(biquadcoeff_hs) {}
	explicit Biquad_hs(double _dy, double _q = SQRT2_2, double _dbGain = 0.) : 
		Biquad_prototype2(biquadcoeff_hs, _dy, _q, _dbGain) 
	{}
	Biquad_hs(const Biquad_hs& other) : Biquad_prototype2(other) {}
};

} // // namespace

#endif // // include guard
