#ifndef DSP_RANDOMNESS_H
#define DSP_RANDOMNESS_H

#include <time.h>
#include <cmath>

namespace dsp {

// // fast real time safe random generator
// // preferred over the default generators in std::random because those have 'amortized' constant complexity
// // it can be used as a generator for std::random classes
struct Xorshift {
	using result_type = unsigned long;
	result_type min() const {
		return 0;
	}
	result_type max() const {
		return ULONG_MAX;
	}
	// // https://en.wikipedia.org/wiki/Xorshift
	result_type operator()() {
		state ^= state << 13;
		state ^= state >> 7;
		state ^= state << 17;
		return state;
	}
	void seed(result_type value) {
		state = value;
	}
private:
	result_type state;
};

struct Random { // // singleton
	using EngineType = Xorshift;
	
	Random(const Random&) = delete;
	explicit Random() {
		seed_ = time(NULL);
		engine_.seed(seed_);
		invMaxPlus1_ = 1. / (double(engine_.max()) + 1.);
	}
		
	EngineType engine_;
	EngineType::result_type seed_;
	double invMaxPlus1_;

	static Random& get() {
		static Random s_instance;
		return s_instance;
	}
	static EngineType& engine() {
		return get().engine_;
	}
	static unsigned long seed() {
		return get().seed_;
	}
	static double invMaxPlus1() {
		return get().invMaxPlus1_;
	}
};

inline double rr01() {
	return double(Random::engine()()) * Random::invMaxPlus1();
}
inline double rr11() {
	return 2. * rr01() - 1.;
}
inline double rr(double min, double max) {
	return min + (max - min) * rr01();
}
inline int rr(int min, int max) {
	return (int)std::floor(rr((double)min, double(max + 1)));
}
inline int rsign() {
	return 2 * rr(0,1) - 1;
}
inline bool rcoin(double p = 0.5) {
	return rr01() < p;
}

inline double exponentialDistribution(double mean) {
	return -mean * std::log(1. - rr01());
}

} // // namespace

#endif // // include guard
