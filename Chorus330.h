/* original by Scott Nordlund */

#ifndef DSP_CHORUS330_H
#define DSP_CHORUS330_H

#include "Phasor.h"
#include "Delay.h"
#include "constants.h"
#include "Double2.h"

#include <cmath>

namespace dsp {

struct Chorus330 {
	explicit Chorus330(int _SR) :
		SR{ _SR }, SRm{ _SR * 0.001 },
		fb{ 0 },
		frate{ 1.0 / SR }, fdepth{ 0.46 },
		srate{1.0 / SR }, sdepth{ 2.56 },
		vrate{ 4.9 / SR }, vdepth{ 0 },
		mode{ VP }
	{}
	explicit Chorus330(
		int _SR,
		double _fb, 
		double _frate, double _fdepth, 
		double _srate, double _sdepth, 
		double _vrate, double _vdepth, 
		int _mode
	) :
		SR{ _SR }, SRm{ _SR * 0.001 },
		fb{ _fb },
		frate{ _frate }, fdepth{ _fdepth },
		srate{ _srate }, sdepth{ _sdepth },
		vrate{ _vrate }, vdepth{ _vdepth },
		mode{ _mode }
	{}
	Chorus330(const Chorus330& other) : 
		SR{other.SR}, SRm{other.SRm},
		fb{other.fb}, 
		frate{other.frate}, fdepth{other.fdepth},
		srate{other.srate}, sdepth{other.sdepth},
		vrate{other.vrate}, vdepth{other.vdepth},
		mode{other.mode}
	{}
		
	const int SR;
	const double SRm;
		
	double fb;
	double frate; // // [-5, 5]
	double fdepth;
	double srate; // // [-5, 5]
	double sdepth;
	double vrate; // // (-5, 5)
	double vdepth;
	int mode;
	
	enum preset { VP = 0, RS };
	// // adimensional constants:
	static constexpr double fr[2][2]{
		{ 6.67, 4.7619 },
		{ 5.71, 5.882 }
	};
	static constexpr double sr[2][2]{
		{ 0.2564, 0.1493 },
		{ 0.2174, 0.1818 }
	};
	//double fdp[2]{ 0.46, 0.23997 };
	//double sdp[2]{ 2.56, 3.789 };
	class Aux {
	private:
		Chorus330& parent;
		int i;
		Phasor p_tri{};
		Phasor p_sin{};
	public:
		Aux(Chorus330& _parent, int _i) :
			parent{ _parent }, i{ _i } 
		{}
		double operator()() {
			static double tri, sin, t_tri, t_mix; // // temporary variables
			p_tri.dy(parent.srate * parent.sr[i][parent.mode]);
			p_tri();
			tri = p_tri.y <= 0.5 ? 4.0 * p_tri.y - 1.0 : 3.0 - 4.0 * p_tri.y;
			p_sin.dy(parent.frate * parent.fr[i][parent.mode]);
			p_sin();
			sin = std::sin(TWOPI * p_sin.y);
			t_tri = parent.sdepth * tri;
			t_mix = parent.fdepth * sin - t_tri;
			return parent.ens[i].y_prev((t_tri + 6.0) * parent.SRm) + parent.ens[i].y_prev((t_mix + 6.0) * parent.SRm);
		}
	}; // // class Aux
	Delay ens[2]{ Delay(int(12 * SRm)), Delay(int(12 * SRm)) };
	Aux aux[2]{ { *this, 0 }, { *this, 1 } };

	Delay vib{ int(10 * SRm) };
	Phasor p_vmod{ vrate / SR };

	Double2 y;
	Chorus330& operator()(double x) {
		static double vmod, v, fb0, fb1; // // temporary variables
		p_vmod.dy(vrate);
		p_vmod();
		vmod = std::sin(p_vmod.y * vdepth);
		v = vib.y_prev(5.0 + vmod);
		fb0 = aux[0]();
		fb1 = aux[1]();
		vib(x);
		ens[0](x + v + fb * fb0); ens[1](x + v + fb * fb1);
		y[0] = fb0; y[1] = fb1;
		y *= 0.25;
		return *this;
	}
};

} // // namespace

#endif // // include guard
