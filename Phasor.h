#ifndef DSP_PHASOR_H
#define DSP_PHASOR_H

namespace dsp {

struct Phasor {
	explicit Phasor(double _dy) : dy_{_dy}, y{-dy_} {}
	explicit Phasor() : dy_{0.}, y{0.} {}
	Phasor(const Phasor& other) : dy_{other.dy_}, y{other.y} {}
	
	double dy_;
	double y;
	bool b;
	
	void operator()() {
		y += dy_;
		if (y >= 1.) {
			do {
				y -= 1.;
			} while (y >= 1.);
			b = true;
		}
		else if (y < 0.) {
			do {
				y += 1.;
			} while (y < 0.);
			b = true;
		}
		else {
			b = false;
		}
	}
	void dy(double val) { dy_ = val; }
	double dy() const { return dy_; }
	void reset(double _y = 0.) {
		y = _y - dy_;
	}
};

} // // namespace

#endif // // include guard