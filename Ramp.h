#ifndef AUDIO_RAMP_H
#define AUDIO_RAMP_H

namespace dsp {

struct Ramp {
	explicit Ramp(int _N) : N{ _N }, n{ _N } {}
	explicit Ramp() : N{ 0 }, n{ 0 } {}
	Ramp(const Ramp& other) : N{ other.N }, n{ other.n } {}
	
	int N;
	int n;

	void operator()() {
		if (n >= N) {
			n = N;
		}
		else {
			++n;
		}
	}
	void init() {
		n = -1;
	}
	void init(int time) {
		N = time;
		init();
	}
	void reset() {
		N = 0;
		n = 0;
	}
};

struct Ramp01 {
	explicit Ramp01(int _N) : N{ _N }, n{ _N }, dy{ 1./N } {}
	explicit Ramp01() : N{ 0 }, n{ 0 }, dy{ 0. } {}
	Ramp01(const Ramp01& other) : 
		N{ other.N }, n{ other.n }, dy{ other.dy }, y{ other.y }
	{}

	int N;
	int n;
	double dy;
	double y;

	void operator()() {
		if (n >= N) {
			n = N;
			y = 1.;
		}
		else {
			++n;
			y += dy;
			if (y > 1) y = 1;
		}
	}
	void init() {
		n = -1;
		y = -dy;
	}
	void init(int time) {
		N = time;
		dy = 1. / N;
		init();
	}
	void reset() {
		N = 0;
		n = 0;
		dy = 0.;
	}
};

} // // namespace

#endif // // include guard
