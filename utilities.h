#ifndef DSP_UTILITIES_H
#define DSP_UTILITIES_H

#include "constants.h"
#include "Double2.h"

#include <cmath>

// // ln_rt12_2 = log( pow(2, 1/12) )
static constexpr double ln_rt12_2 = 0.0577622650466621091181026767881813806729583445300212711767233341244494684974745596338219439163682239618334567516360546265;

// // aux_ftom = -12 * log2(C0HZ)
static constexpr double aux_ftom = -36.376316562295915248836189714582197614816755024476184200410696498507184755943722822715572511755247726019106863;

namespace dsp {

// // f = C0HZ * std::pow(2, m / 12.0);
inline double mtof(double m) {
	return C0HZ * std::exp(ln_rt12_2 * m);
}
inline double ftom(double f) {
	return 12.0 * std::log2(f) + aux_ftom;
}

inline double stfactor(double st) {
	return std::exp(ln_rt12_2 * st);
}
inline double factorst(double factor) {
	return 12.0 * std::log2(factor);
}

inline double dytog(double dy) {
	return std::tan(dy * PI);
}
inline double gtody(double g) {
	return std::atan(g) * ONE_OVER_PI;
}

// // r = 1 / (2 * q)
inline double qtor(double q) {
	return 0.5 / q;
}
inline double rtoq(double r) {
	return 0.5 / r;
}

inline double dbtoa(double db) {
	static constexpr double aux = 2.302585092994*0.05;
	return std::exp(aux*db);
}
inline double atodb(double a) {
	static constexpr double aux = 1./(2.302585092994*0.05);
	return std::log(a) * aux;
}

inline double scale(double x, double min0, double max0, double min1, double max1) {
	// if (max0 == min0) return ???
	return ((x - min0) * (max1 - min1) / (max0 - min0)) + min1;
}
inline double scale01(double x, double min, double max) {
	return (x *= (max - min)) += min; // better ???
}
inline double scale11(double x, double min, double max) {
	return scale01((x += 1.0) *= 0.5, min, max); // better ???
}
inline double scale0111(double x) {
	return scale01(x, -1, 1.);
}
inline double scale1101(double x) {
	return scale11(x, 0., 1.);
}

inline double scale01_exp(double x, double min, double max) {
    return std::exp(scale01(x, std::log(min), std::log(max)));
}

inline double clip(double x, double min, double max) {
	return x <= min ? min : (x >= max ? max : x);
}
inline double clip01(double x) {
	return x <= 0 ? 0 : (x >= 1 ? 1 : x);
}
inline double clip11(double x) {
	return x <= -1 ? -1 : (x >= 1 ? 1 : x);
}

/* ------------------------------ */
// // interpolation functions from: http://paulbourke.net/miscellaneous/interpolation/
inline double cubic_interpolate(double mu, double y0, double y1, double y2, double y3) {
	double a0, a1, a2, a3, mu2;

	mu2 = mu * mu;
	a3 = y3 - y2 - y0 + y1;
	a2 = y0 - y1 - a3;
	a1 = y2 - y0;
	a0 = y1;

	return(a3*mu*mu2 + a2*mu2 + a1*mu + a0);
}

inline double spline_interpolate(double mu, double y0, double y1, double y2, double y3) {
	double a0, a1, a2, a3, mu2;

	mu2 = mu * mu;
	a3 = -0.5*y0 + 1.5*y1 - 1.5*y2 + 0.5*y3;
	a2 = y0 - 2.5*y1 + 2*y2 - 0.5*y3;
	a1 = -0.5*y0 + 0.5*y2;
	a0 = y1;

	return(a3*mu*mu2 + a2*mu2 + a1*mu + a0);
}
/* ------------------------------ */

inline double fade(double a, double b, double m) {
	return m*a + (1-m)*b;
}

inline double fade_sqrt(double a, double b, double m) {
	return std::sqrt(m)*a + std::sqrt(1 - m)*b;
}

/*
DAFX book
*/
inline double overdrive(double x) {
	double x2 = x * x;
	if (x2 <= 0.111111) return x *= 2;
	if (x2 < 0.4444444) return 4*x + (x >= 0? - 3*x2 - 0.333333 : + 3*x2 + 0.333333);
	return x >= 0 ? 1.0 : -1.0;
}

int sgn(int val) {
	return (0 < val) - (val < 0);
}
int sgn(double val) {
	return (0 < val) - (val < 0);
}

inline double tri(double x, double vertex = 0.5) {
	if (vertex <= 0) return 1. - x;
	if (vertex >= 1) return x;
	if (x <= vertex) {
		return x / vertex;
	}
	else {
		return 1 - (x - vertex) / (1 - vertex);
	}
}
inline double triq(double x, double vertex) {
	if (vertex <= 0) {
		x = 1.-x;
		x *= x;
		x *= x;
		return x;
	}
	if (vertex >= 1) return x;
	if (x < vertex) {
		return x / vertex;
	}
	else {
		x = 1.0 - (x - vertex)/(1.0 - vertex);
		x *= x;
		x *= x;
		return x; // return x^4
	}
}
inline double pclip(double x, double c) {
	return x < c ? x / c : 1.0;
}
inline double trapezoid(double x, double atk, double dec) {
	if (x < atk) return x / atk;
	if (x > 1.-dec) return 1. - (x - (1.-dec)) / dec;
	return 1.;
}

inline Double2 pancos(double pan01) {
    double ph = pan01 * 0.25;
    return Double2{ std::cos(TWOPI * ph), std::cos(TWOPI * (ph - 0.25)) };
}

inline double phaseResponse(double dy, double a0, double a1, double b0, double b1) {
	double w = TWOPI * dy;
	double cosw = std::cos(w);
	double sinw = std::sin(w);
	double c0 = a0 + a1 * cosw;
	//double c1 =      a1 * sinw;
	double c1 =    - a1 * sinw;
	double d0 = b0 + b1 * cosw;
	//double d1 =      b1 * sinw;
	double d1 =    - b1 * sinw;
	double f = (c1*d0 - c0*d1) / (c0*d0 + c1*d1);
	//return std::atan(f) + (f <= 0 ? PI : 0);
	return std::atan(f);// + (f >= 0 ? -PI : 0);
}

inline double phaseResponse2phaseDelay(double dy, double phaseResponse) {
	return -phaseResponse / (TWOPI * dy);
}

} // // namespace

#endif // // include guard
