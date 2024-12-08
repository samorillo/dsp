#ifndef DSP_FREQSHIFT
#define DSP_FREQSHIFT

#include "Phasor.h"
#include "Hilbert.h"
#include "constants.h"

namespace dsp {

struct Freqshift {
	explicit Freqshift() : hilbert_filter{} {}
	explicit Freqshift(double _dy) { dy(_dy); }
	Freqshift(const Freqshift& other) : 
		shifter{other.shifter}, hilbert_filter{other.hilbert_filter}
	{}
	
	Phasor shifter;
	Hilbert hilbert_filter;
	double y[2];
	
	Freqshift& operator()(double x) {
		hilbert_filter(x);
		shifter();
		double aux0 = hilbert_filter.y[0] * cos(TWOPI * shifter.y);
		double aux1 = hilbert_filter.y[1] * cos(TWOPI * (shifter.y - 0.25));
		y[0] = aux0 - aux1; // // up
		y[1] = aux0 + aux1; // // down
		return *this;
	}
	void dy(double value) {
        shifter.dy(value);
    }
    double dy() const { return shifter.dy(); }
};

} // // namespace

#endif // // include guard
