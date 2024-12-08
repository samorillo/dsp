#ifndef DSP_BUTTERVA_H
#define DSP_BUTTERVA_H

#include "SvfVA.h"
#include "utilities.h"
#include "constants.h"

#include <cmath>

namespace dsp {
	
/////// improve: use arithmetic optimizations
inline void calculate_butterworth_r(double* result, int N, double r) {
	double N_rec = 1.0 / (double)N;
	double halfpi_N = HALFPI * N_rec;
	if (r > 1) {
		double a1 = r + std::sqrt(r * r - 1), a2 = 1 / a1;
		double r1 = std::pow(a1, N_rec);
		double r2 = std::pow(a2, N_rec);
		
		for (int i = 1; i < N; i += 2) {
			double cos_theta = std::cos(i * halfpi_N);
			result[i - 1] = r1 * cos_theta;
			result[i] = r2 * cos_theta;
		}

		if (N % 2) { result[N - 1] = (r1 + r2) * 0.5; }
	}
	else {
		double alpha_inc = std::acos(r) * N_rec;
		for (int i = 1; i < N; i += 2) {
			double theta = i * halfpi_N;
			result[i - 1] = std::cos(theta + alpha_inc); ///// cos(t+a) = cos(t)cos(a) - sin(t)sin(a);
			result[i] = std::cos(theta - alpha_inc);
		}

		if (N % 2) { result[N - 1] = std::cos(alpha_inc); }
	}
}

struct ButterVA {
	explicit ButterVA(const double SvfVA::* _type, int _order) :
		type{ _type }, order{ _order },
		filters{ new SvfVA[_order] },
		rs{ new double[_order] }
	{}
	explicit ButterVA(const double SvfVA::* _type, int _order, double _dy, double _q) :
		ButterVA(_type, _order)
	{
		dy(_dy);
		q(_q);
	}
	ButterVA(const ButterVA& other) :
		ButterVA(other.type, other.order)
	{
		dy_ = other.dy_;
		q_ = other.q_;
		for (int i = 0; i < order; ++i) {
			rs[i] = other.rs[i]; // // copy, don't recalculate
			filters[i] = other.filters[i];
		}
	}
	~ButterVA() {
		delete[] rs;
		delete[] filters;
	}
	
	const double SvfVA::* type;
	const int order;
	SvfVA* filters;
	double* rs;
	double dy_;
	double q_;
	double y;
		
	ButterVA& operator()(double x) {
		y = filters[0](x).*type;
		for (int i = 1; i < order; ++i) {
			y = filters[i](y).*type;
		}
		return *this;
	}
	void dy(double _dy) {
		dy_ = _dy;
		for (int i = 0; i < order; ++i) {
			filters[i].dy(_dy);
		}
	}
	double dy() const {
		return dy_;
	}
	void q(double _q) {
		q_ = _q;
		calculate_rs(qtor(_q));
	}
	double q() const {
		return q_;
	}
	void calculate_rs(double _R) {
		calculate_butterworth_r(rs, order, _R);
		for (int i = 0; i < order; ++i) {
			filters[i].r = rs[i];
		}
	}
	void reset() {
		for (int i = 0; i < order; ++i) {
			filters[i].reset();
		}
	}
};

struct ButterVA_lp : public ButterVA {
	explicit ButterVA_lp(int order, double _dy = 0, double _q = SQRT2_2) :
		ButterVA(&SvfVA::y_lp, order, _dy, _q) 
	{}
	ButterVA_lp(const ButterVA_lp& other) : ButterVA(other) {}
};
struct ButterVA_hp : public ButterVA {
	explicit ButterVA_hp(int order, double _dy = 0, double _q = SQRT2_2) :
		ButterVA(&SvfVA::y_hp, order, _dy, _q)
	{}
	ButterVA_hp(const ButterVA_hp& other) : ButterVA(other) {}
};
struct ButterVA_bpr : public ButterVA {
	explicit ButterVA_bpr(int order, double _dy = 0, double _q = SQRT2_2) :
		ButterVA(&SvfVA::y_bpr, order, _dy, _q)
	{}
	ButterVA_bpr(const ButterVA_bpr& other) : ButterVA(other) {}
};
struct ButterVA_bp : public ButterVA {
	explicit ButterVA_bp(int order, double _dy = 0, double _q = SQRT2_2) :
		ButterVA(&SvfVA::y_bp, order, _dy, _q)
	{}
	ButterVA_bp(const ButterVA_bp& other) : ButterVA(other) {}
};

} // // namespace

#endif // // include guard
