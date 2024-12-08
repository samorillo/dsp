/*
original by Scott Nordlund
*/

#ifndef DSP_DIFFUSION16_H
#define DSP_DIFFUSION16_H

namespace dsp {

struct Diffusion16 {
	explicit Diffusion16(int _stages) :
		stages{ _stages }
	{
		if (_stages < 0 || _stages > 4) throw "err";
		pow2_stages = 1;
		for (int i = 0; i < _stages; ++i) pow2_stages *= 2;
		g = gainCompensation[_stages];
	}
	Diffusion16(const Diffusion16& other) :
		stages{other.stages}, pow2_stages{other.pow2_stages}, g{other.g}
	{}
		
	double xy[16]{};
	const int stages;
	int pow2_stages; // const?
	static constexpr double gainCompensation[5]{
		1.0, 0.707107, 0.5, 0.353553, 0.25
	};
	double g;
	
	void operator()() {
		plusminus();
		for (double& v : xy) v *= g;
	}
	void plusminus() {
		short n1, n2, B;
		for (short N = 1; N < pow2_stages; N *= 2) {
			n1 = 0; n2 = N;
			while (n2 < 16) {
				B = n2;
				while (n1 < B) {
					double aux = xy[n1];
					xy[n1] += xy[n2];
					xy[n2] = aux - xy[n2];
					++n1; ++n2;
				}
				n1 = n2; // IMPROVE: useless in the last iteration
				n2 += N;
			}
		}
	}
};

} // namespace

#endif // // include guard