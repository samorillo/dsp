#ifndef DSP_COUNTER
#define DSP_COUNTER

namespace dsp {

struct Counter {
	explicit Counter(int _N, int _n) : N{ _N }, n{_n-1} {}
	explicit Counter(int _N) : N{ _N }, n{-1} {}
	explicit Counter() : Counter(1) {}
	Counter(const Counter& other) : N{ other.N }, n{ other.n } {}

	int N;
	int n;
	
	void operator()() {
		n += 1; 
		n *= (n != N);
	}
	void init(int _N) {
		N = _N;
		n = -1;
	}
};

struct Counter01 {
	explicit Counter01(int _N, int _n) :
		N{ _N }, n{ _n-1 } , dy{ 1.0 / N }, y{ -dy }
	{}
	explicit Counter01(int _N) :
		N{ _N }, n{ -1 } , dy{ 1.0 / N }, y{ -dy }
	{}
	explicit Counter01() : Counter01(1) {}
	Counter01(const Counter01& other) : 
	 	N{ other.N }, n{ other.n } , dy{ other.dy }, y{ other.dy }
	{}
	
	int N;
	int n;
	double dy;
	double y;

	void operator()() {
		++n;
		if (n == N) {
			n = 0;
			y = 0;
		}
		else {
			y += dy;
		}
	}
	void init(int _N) {
		N = _N;
		dy = 1.0 / N;
		n = -1;
		y = -dy;
	}
};

} // // namespace

#endif // // include guard
