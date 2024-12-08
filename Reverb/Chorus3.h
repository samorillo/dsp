/*
original by Scott Nordlund

"Modulation is triangular with lowpass smoothing. Detuning is asymmetric in pitch, symmetric in time/waveform. You can specify either positive shift or negative shift, and that specified amount will be accurate, while the other will be determined by the complementary slope. It makes less difference for small amounts of modulation. The modulation rate is reported in the control panel, and may be changed, though this will be overridden by other controls. Lag is provided to smooth the modulation, but this may reduce the overall depth if used excessively. Pairs of outputs have complementary modulation, which can be useful for stereo effects."
*/

#ifndef DSP_CHORUS3_H
#define DSP_CHORUS3_H

#include "../Delay.h"
#include "../Phasor.h"
#include "../Onepole.h"
#include "../randomness.h"
#include "../Pois.h"

namespace dsp {

struct Chorus3 {
	explicit Chorus3(int SR, int N, double _maxDetune, int _maxDel) :
		SR{ SR }, DT{ 1./SR }, N{ N },
		maxDetune{ _maxDetune }, maxDel{ _maxDel },
		depth{ _maxDel - 2 }, /////////////////
		dy_max{ (std::pow(2, (_maxDetune / 12.0)) - 1.0) / (2.0 * _maxDel) },
		del{ _maxDel },
		y{ new double[2*N] }, taps{ new Tap[N] }
			
	{
		for (int i = 0; i < N; ++i) {
			taps[i].setParent(this);
		}
	}
	Chorus3(const Chorus3& other) :
		SR{other.SR}, DT{other.DT}, N{other.N},
		maxDetune{other.maxDetune}, maxDel{other.maxDel},
		depth{other.depth},
		dy_max{other.dy_max},
		del{other.del},
		y{ new double[2*N] }, taps{ new Tap[N] }
	{}
	~Chorus3() {
		delete[] y;
		delete[] taps;
	}
	
	const int SR;
	const double DT;
	const int N;
	double maxDetune; // st
	int maxDel; // samples
	int depth;
	double dy_max;
	Delay del;
	double* y;
	
	double minDepth{ 0.75 }; // 0..1
	
	Chorus3& operator()(double x) {
		for (int i = 0, i2 = 0; i < N; ++i, i2 += 2) {
			taps[i](i2);
		}
		del(x);
		return *this;
	}

	struct Tap {
		Chorus3* parent;
		Onepole_lp lp0, lp1;
		double dy{};
		Phasor p;
		
		void setParent(Chorus3* val) {
			parent = val;
			randDy.setup(val, this);
			lp0.dy(1.59 * parent->DT);
			lp1.dy(1.59 * parent->DT);
			// // to avoid initial glide:
			lp0.y_1 = parent->depth;
			lp1.y_1 = 0;
		}

		void operator()(int i) {
			randDy();
			p.dy(dy);
			p();
			double mod0 = p.y - 0.5;
			if (mod0 < 0) mod0 = -mod0;
			double mod1 = 0.5 - mod0;

			*(parent->y + i+0) = parent->del.y_prev(2. + lp0(mod0 * 2. * parent->depth).y);
			*(parent->y + i+1) = parent->del.y_prev(2. + lp1(mod1 * 2. * parent->depth).y);
		}

		struct Rand {
			Chorus3* parent;
			Tap* parentTap;
			Pois pois;

			void operator()() {
				pois();
				if (pois.b) {
					double r = rr11();
					r = r * (1 - parent->minDepth) + ((r > 0.0) * 2.0 - 1.0) * parent->minDepth;
					parentTap->dy = parent->dy_max * r;
				}
			}
			void setup(Chorus3* _parent, Tap* _parentTap) {
				parent = _parent;
				parentTap = _parentTap;
				pois.mu = 0.3333 * parent->SR;
				pois.min =  int(0.1 * parent->SR);
			}
		};
		Rand randDy;
	};
	Tap* taps;
};

} // namespace

#endif // // include guard
