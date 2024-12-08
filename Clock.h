#ifndef DSP_CLOCK_H
#define DSP_CLOCK_H

#include "Counter.h"

namespace dsp {

struct Clock {
    explicit Clock() {}
    explicit Clock(int _taktDur) {
        start(_taktDur);
    }
    Clock(const Clock& other) : 
        taktDur{other.taktDur}, takt{other.takt},
        n{other.n}, b{other.b}, sampleCount{other.sampleCount}
    {}
    
	int taktDur;
	Counter01 takt;
	int n = -1;
	bool b{ false };
	long long sampleCount = -1;

	void operator()() {
		++sampleCount;
		takt();
		if (takt.n == 0) {
			++n;
			b = true;
		}
		else {
			b = false;
		}
	}
    void start(int _taktDur) {
        taktDur = _taktDur;
		takt.init(taktDur);
        n = -1;
        sampleCount = -1;
	}
    void setTaktDur_post(int _taktDur) {
        taktDur = _taktDur;
		takt.init(taktDur);
        takt();
	}
    double y() {
        return takt.y;
    }
	double y(int ntakts) {
		return ((n % ntakts) + takt.y) / ntakts;
	}
};

} // // namespace

#endif // // include guard