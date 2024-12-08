/*
original by Scott Nordlund
"Time-variant stochastic multi-tap delay line for use as reverberator early reflections, etc. The distribution of delays can be adjusted using the del_dist parameter: 0 is a uniform distribution, 1 is an exponential-like distribution (biased toward shorter times) and -1 is an inverse exponential-like distribution (biased toward longer times). The envelope of the delays can also be varied using the RT60 parameter. The taps are scaled to decay exponentially (RT60 > 0), reverse-exponentially (RT60 < 0) or flat (RT60 = 0). The time_min and time_max controls determine the rate of time variation.
Time-variant stochastic multi-tap delay line for use as reverberator early reflections, etc. The distribution of delays can be adjusted using the del_dist parameter: 0 is a uniform distribution, 1 is an exponential-like distribution (biased toward shorter times) and -1 is an inverse exponential-like distribution (biased toward longer times). The envelope of the delays can also be varied using the RT60 parameter. The taps are scaled to decay exponentially (RT60 > 0), reverse-exponentially (RT60 < 0) or flat (RT60 = 0). The time_min and time_max controls determine the rate of time variation."

simplified: only uniform distribution
*/

#ifndef DSP_MULTITAP_H
#define DSP_MULTITAP_H

#include "../Delay.h"
#include "../randomness.h"
#include "../Poislin.h"

namespace dsp {

struct Multitap {
	explicit Multitap(int _nTaps, int minDel, int maxDel, int minTime, int maxTime) :
		nTaps{ _nTaps }, gainCorrection{ pow(nTaps - 1, -0.6) },
		delays{ new int[_nTaps] }, amps{ new double[_nTaps] },
		minDelay{ minDel }, maxDelay{ maxDel }, delayLine{ maxDel },
		pois{ minTime, maxTime },
		tap{ 0 }
	{
		randomizeDelAmp();
	}
	Multitap(const Multitap& other) : 
		nTaps{ other.nTaps }, gainCorrection{ other.gainCorrection },
		delays{ new int[other.nTaps] }, amps{ new double[other.nTaps] },
		minDelay{ other.minDelay }, maxDelay{ other.maxDelay }, 
		delayLine{ other.maxDelay },
		pois{ other.pois },
		tap{ 0 }
	{
		randomizeDelAmp();
	}
	~Multitap() {
		delete[] delays;
		delete[] amps;
	}

	const int nTaps;
	const double gainCorrection;
	int* delays;
	double* amps;
	const int maxDelay;
	int minDelay;
	Delay delayLine;
	Poislin pois;
	int new_delay; double new_amp;
	int tap;
	double y;
	
	Multitap& operator()(double input) {
		pois();
		
		if (pois.b) {
			delays[tap] = new_delay;
			amps[tap] = new_amp;
			randDelAmp(&new_delay, &new_amp); // improve?
			++tap; if (tap == nTaps) tap = 0;
		}
		double env = pois.y;

		double fadein = env * new_amp * delayLine.y_prev(new_delay);
		double fadeout = (1 - env) * amps[tap] * delayLine.y_prev(delays[tap]);

		y = fadein + fadeout;

		for (int i = 0; i < nTaps; ++i) {
			if (i == tap) continue;
			y += amps[i] * delayLine.y_prev(delays[i]);
		}

		delayLine(input);
		y *= gainCorrection;

		return *this;
	}
	void randDelAmp(int* del, double* amp) { /////////////////////////////////// rt60?
		*del = rr(minDelay, maxDelay);
		*amp = rr01();
	}
	void randomizeDelAmp() {
		for (int i = 0; i < nTaps; ++i) {
			randDelAmp(delays + i, amps + i);
		}
		new_delay = delays[0];
		new_amp = amps[0];
	}
};

} // // namespace

#endif // // include guard
