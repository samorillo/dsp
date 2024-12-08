/* exact copy of hilbert~ from PureData */

#ifndef DSP_HILBERT
#define DSP_HILBERT

#include "Biquad.h"

namespace dsp {

struct Hilbert {
	explicit Hilbert() :
		b00{ -0.260502, 0.02569, 1.0, 0.02569, -0.260502 },
		b01{ 0.870686, -1.8685, 1.0, -1.8685, 0.870686 },
		b10{ 0.94657, -1.94632, 1.0, -1.94632, 0.94657 },
		b11{ 0.06338, -0.83774, 1.0, -0.83774, 0.06338 }
	{}
	Hilbert(const Hilbert& other) : Hilbert() {}
	
	Biquad b00;
	Biquad b01;
	Biquad b10;
	Biquad b11;
	double y[2];
	
	Hilbert& operator()(double in) {
		y[0] = b01( b00(in).y_n[0] ).y_n[0];
		y[1] = b11( b10(in).y_n[0] ).y_n[0];
		return *this;
	}
	void reset() {
		b00.reset(); b01.reset(); b10.reset(); b11.reset();
	}
};

} // // namespace

#endif // // include guard