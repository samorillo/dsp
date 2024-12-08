/*
adapted from the "yafr2" abstraction in Max-MSP
*/

#ifndef DSP_YAFR2
#define DSP_YAFR2	

#include "Delay.h"
#include "Biquad.h"
#include "randomness.h"

namespace dsp {
	
struct Allpass {
	explicit Allpass(int maxDel, double _gain) : 
		d1{ maxDel }, d2{ maxDel }, gain{ _gain }
	{}
	Allpass(const Allpass& other) :
		d1{other.d1}, d2{other.d2}, gain{other.gain}
	{}
	
	Delay d1;
	Delay d2;
	double gain;
	
	double y{};
	Allpass& operator()(double x) {		
		y = -gain*x + d1.y_prev() + gain*d2.y_prev();			
		d1(x); d2(y);
		return *this;
	}
	
	void reset() {
		d1.reset();
		d2.reset();
	}
};

struct Yafr2 {
	explicit Yafr2(int _SR, double _size, double _decay) : // // _size, _decay from 0..1
		SR{ _SR }, decay{ _decay },
		noise_radius{0.02 * SR}, noise_diameter{2.0 * noise_radius}, factor{0.10606601717798}
	{
		_size = _size * 1.59 + 0.01;
		_decay = _decay * 0.58 + 0.05;
		for (int c = 0; c < 2; ++c) {
			for (int i = 0; i < 4; ++i) er[c][i] = new Allpass{ int(SR * (0.001 * er_ms[i])), er_fb[i] };
			fb0[c] = new Allpass{ int(SR * (0.001 * fb_ms[c][0] * _size)), -0.7 };
			fb1[c] = new Delay{ SR }; ////
			fb1_t[c] = SR * (0.001 * fb_ms[c][1] * _size);
			fb2[c] = new Allpass{ int(SR * (0.001 * fb_ms[c][2] * _size)), 0.5 };
			fb3[c] = new Delay{ int(SR * (0.001 * fb_ms[c][3] * _size)) };
			for (int i = 0; i < 7; ++i) co[c][i] = new Delay{ int(SR * (0.001 * co_ms[c][i])) };
			lp[c] = Biquad_lp{ 4.35 / SR, 0.5 };
			
			for (int i = 0; i < 4; ++i) f[c][i] = 0.;
		}
		
	}
	~Yafr2() {
		for (int c = 0; c < 2; ++c) {
			for (int i = 0; i < 4; ++i) delete[] er[c][i];

			delete fb0[c];
			delete fb1[c];
			delete fb2[c];
			delete fb3[c];

			for (int i = 0; i < 7; ++i) delete[] co[c][i];
		}
	}
	Yafr2(const Yafr2& other) :
		SR{other.SR}, decay{other.decay},
		noise_radius{other.noise_radius}, noise_diameter{other.noise_diameter}, factor{other.factor}
	{
		for (int c = 0; c < 2; ++c) {
			for (int i = 0; i < 4; ++i) er[c][i] = new Allpass{ *other.er[c][i] };
			fb0[c] = new Allpass{ *other.fb0[c] };
			fb1[c] = new Delay{ *other.fb1[c] };
			fb1_t[c] = other.fb1_t[c];
			fb2[c] = new Allpass{ *other.fb2[c] };
			fb3[c] = new Delay{ *other.fb3[c] };
			for (int i = 0; i < 7; ++i) co[c][i] = new Delay{ *other.co[c][i] };
			lp[c] = Biquad_lp{ 4.35 / SR, 0.5 };
			
			for (int i = 0; i < 4; ++i) f[c][i] = 0.;
		}
	}
	
	static constexpr double er_ms[4]{ 4.76, 3.58, 12.73, 9.3 };
	static constexpr double er_fb[4]{ 0.75, 0.75, 0.625, 0.625 };
	static constexpr double fb_ms[2][4]{
		{ 22.58, 149.6, 60.48, 125.0 },
		{ 30.51, 141.7, 89.24, 106.28 }
	};
	static constexpr double co_ms[2][7]{
		{ 110, 160, 25, 50, 69, 80, 75 },
		{ 110, 135, 3, 55, 35, 27, 52 }
	};
	
	const int SR;
	double decay;
	
	const double noise_radius;
	const double noise_diameter;
	const double factor;
	
	double fb1_t[2];
	Allpass* er[2][4];
	Delay* co[2][7];
	Allpass* fb0[2];
	Delay* fb1[2];
	Allpass* fb2[2];
	Delay* fb3[2];
	Biquad_lp lp[2];
	double f[2][4];
	
	void reset() {
		for (int i = 0; i < 2; ++i) {
			for (int j = 0; j < 4; ++j) {
				er[i][j]->reset();
			}
		}
		for (int i = 0; i < 2; ++i) {
			for (int j = 0; j < 7; ++j) {
				co[i][j]->reset();
			}
		}
		for (int i = 0; i < 2; ++i) {
			fb1[i]->reset();
			fb2[i]->reset();
			fb3[i]->reset();
			lp[i].reset();
		}
	}

	Double2 x{ 0.,0. };
	Double2 y{ 0.,0. };
	
	void operator()() {
		// // earlyref
		for (int c = 0; c < 2; ++c) {
			for (int i = 0; i < 4; ++i) {
				x[c] = (*er[c][i])(x[c]).y;
			}
		}
		// // fbk loop	
		for (int c = 0; c < 2; ++c) {

			f[c][0] = (*fb0[c])(x[c] + f[c][3]).y;
			f[c][1] = (*fb1[c]).y_prev(fb1_t[c] + SR * 0.02 * lp[c](rr11()).y);
			(*fb1[c])(f[c][0]);
			f[c][2] = decay * (*fb2[c])(f[c][1]).y;
			f[c][3] = (*fb3[c]).y_prev();
			(*fb3[c])(f[c][2]);
		}
		// // combine
		for (int c = 0; c < 2; ++c) {
			x[c] = 0;
			int i = 0;
			for (; i < 3; ++i) {
				x[c] += (*co[c][i]).y_prev();
			}
			for (; i < 7; ++i) {
				x[c] -= (*co[c][i]).y_prev();
			}

			x[c] *= factor;

			(*co[c][0])(f[c][0]); (*co[c][1])(f[c][0]);
			(*co[c][2])(f[c][2]);
			(*co[c][3])(f[c][3]);
			(*co[c][4])(f[1 - c][1]);
			(*co[c][5])(f[1 - c][2]);
			(*co[c][6])(f[1 - c][3]);
		}

		y = x;
		x.null();
	}
};

} // // namespace

#endif // // include guard
