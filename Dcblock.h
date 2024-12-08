#ifndef DSP_DCBLOCK_H
#define DSP_DCBLOCK_H

namespace dsp {

struct Dcblock {
	explicit Dcblock() : b1{0.995}, x_1{0.}, y_1{0.} {}
	Dcblock(const Dcblock& other) : Dcblock() {}

	const double b1; // // 0.995 or 0.9997
	double x_1;
	double y_1;
	double y;
	
	Dcblock& operator()(double x) {
		y = x - x_1 + b1*y_1;
		x_1 = x;
		y_1 = y;
		return *this;
	}
	void reset(double value = 0.) {
		x_1 = y_1 = value;
	}
	double phaseResponse(double dy) const {
		// // Y/X = (1-z) / (1-b1*z)
		return dsp::phaseResponse(dy, 1, -1, 1, -b1);
	}
};

} // // namespace

#endif // // include guard
