// // original by Scott Nordlund 

#ifndef DSP_REVERB727_H
#define DSP_REVERB727_H

#include "../Onepole.h"
#include "../Biquad.h"
#include "../Double2.h"

#include "Multitap.h"
#include "Diffusion16.h"
#include "Density.h"
#include "Apfr.h"
#include "Chorus3.h"

namespace dsp {

struct Reverb727 {
    struct Params {
        double apfMaxSeconds = 0.1;
        int nApfStages = 2;
        // chorus depth
        int nDiffusionStages = 2;
        int lateQuality = 6;//6;
        double lateMaxSeconds = 0.4;
        
        double fbkFactor = 0.37; //0.374
        double ls_freq = 100;
        double ls_db = -2;
        double hs_freq = 10000;
        double hs_db = -2;
    };
    
	explicit Reverb727(int SR, Params _params) :
		SR{ SR }, DT{ 1./SR }, params{_params}
	{}
	Reverb727(const Reverb727& other) : 
		Reverb727(other.SR, other.params)
	{}
	
	const int SR; 
	const double DT;
    Params params;

	Double2 x{ 0.0, 0.0 };
	Double2 y{ 0.0, 0.0 };
	
	void operator()() {
		early[0](x[0], feedback.y[0] * params.fbkFactor);
		early[1](x[1], feedback.y[0] * params.fbkFactor);
        
        for (int i = 0; i < 2; ++i) {
			int i8 = 8 * i;
			diffusion.xy[i8 + 0] = early[i].y[0][0];
			diffusion.xy[i8 + 1] = early[i].y[0][1];
			diffusion.xy[i8 + 2] = early[i].y[1][0];
			diffusion.xy[i8 + 3] = early[i].y[1][1];

			diffusion.xy[i8 + 4] = early[i].y[0][2];
			diffusion.xy[i8 + 5] = early[i].y[0][3];
			diffusion.xy[i8 + 6] = early[i].y[1][2];
			diffusion.xy[i8 + 7] = early[i].y[1][3];
		}
		diffusion();
        
        late[0](diffusion.xy);
		late[1](diffusion.xy + 8);
        
        for (int c = 0; c < 2; ++c) {
			y[c] = 0.25 * (late[c].y[0] + late[c].y[1] + late[1 - c].y_export);
		}

        feedback(late[0].y[0] + late[0].y[2], late[1].y[0] + late[1].y[2]);
        
        x.null();
	}

	struct Early {
		Early(int SR, int c, int _nApfStages, double _maxApfDel) :
			SR{ SR }, DT{ 1./SR },
			chan{ c }, nApfStages{_nApfStages}, maxApfDel{_maxApfDel} 
	 	{}
		const int SR; const double DT;
        const int chan;
        const int nApfStages;
		const double maxApfDel;
		void operator()(double x, double fb) {
            temp[1-chan] = 0.;
			temp[chan] = x;
			density(temp);
			//chorus[0](density.y[0] + fb);
			//chorus[1](density.y[1] + fb);
            chorus[0](apfStages[0](density.y[0] + fb).y);
			chorus[1](apfStages[1](density.y[1] + fb).y);
		}
        double temp[2];
		struct ApfStages {
			explicit ApfStages(int SR, int _nStages, double _maxApfDel) : 
				SR{ SR }, nStages{_nStages}, maxApfDel{_maxApfDel}
			{
				stages = new Apfr*[nStages];
				int d = (int)std::round(maxApfDel / (double)nStages);
				if (d == 0) {
					throw "err";
				}
				for (int i = 0; i < nStages; ++i) {
					stages[i] = new Apfr{
						SR,
						d
						//int(0.1 * SR),
						//int(0.24 * SR), int(0.375 * SR),
						//(int)std::round(0.0015 * SR), (int)std::round(0.01 * SR)
					};
				}
			}
			~ApfStages() {
				for (int i = 0; i < nStages; ++i) {
					delete stages[i];
				}
				delete[] stages;
			}
			const int SR;
			const int nStages;
			const double maxApfDel;
			Apfr** stages;

            double y;
			ApfStages& operator()(double x) {
				for (int i = 0; i < nStages; ++i) {
					x = stages[i]->operator()(x).y;
				}
				y = x;
                return *this;
			}
		};

        Density density{ 15, SR * 0.3 };
		ApfStages apfStages[2]{ ApfStages(SR, nApfStages, maxApfDel), ApfStages(SR, 2, maxApfDel) };
		Chorus3 chorus[2]{
			Chorus3(SR, 2, 0.08, int(0.05 * SR)),
			Chorus3(SR, 2, 0.08, int(0.05 * SR))
		};
		double* y[2]{ chorus[0].y, chorus[1].y };
	}; // // class Early
    
    struct Late {
		Late(int SR, int quality, int maxDel) :
			SR{ SR }, DT{ 1./SR }
		{
			for (int i = 0; i < 8; ++i) {
				feedbackTaps[i] = new Multitap{
					//quality, int(0.05 * SR), maxDel, int(0.8 * SR), int(1.25 * SR)
                    quality, 2, maxDel, int(0.8 * SR), int(1.25 * SR)
				};
			}
		}
		~Late() {
			for (int i = 0; i < nFeedbackTaps; ++i) {
				delete feedbackTaps[i];
			}
		}
		const int SR; const double DT;
		inline static constexpr int nFeedbackTaps = 8;
		Multitap* feedbackTaps[nFeedbackTaps];

		double y[3]{ 0.0, 0.0, 0.0 };
		double y_export{};

		void operator()(double* x) {
			y[0] = 0;
			for (int i = 0; i < 4; ++i) {
				y[0] += feedbackTaps[i]->operator()(x[i]).y;
			}

			static double temp[4];
			for (int i = 4; i < 8; ++i) {
				temp[i - 4] = feedbackTaps[i]->operator()(x[i]).y;
			}
			y[1] = temp[0] + temp[1] + temp[2];
			y_export = temp[3];
			y[2] = y[1] + y_export;
		}
	}; // // class Late
    
    struct Feedback {
		Feedback(int _SR, double ls_dy, double ls_db, double hs_dy, double hs_db) :
            SR{ _SR }, DT{ 1./_SR }
        {
            for (int c = 0; c < 2; ++c) {
                hs[c].dy_q_db(hs_dy, SQRT2_2, hs_db);
                ls[c].dy_q_db(ls_dy, SQRT2_2, ls_db);
            }
        }
		const int SR; const double DT;
		double y[2]{ 0.0, 0.0 };

		void operator()(double x0, double x1) { // x0 problem
			y[0] = dcblock[0]( hs[0](ls[0](x0).y).y ).y;
			y[1] = dcblock[1]( hs[1](ls[1](x1).y).y ).y;
		}

		//Onepole_hp dcblock[2]{ 10 * DT, 10 * DT };
		Onepole_hp dcblock[2]{ Onepole_hp(10 * DT), Onepole_hp(10 * DT) };
        Biquad_hs hs[2];//{Biquad_hs{hs_dy, SQRT2_2, hs_db}, Biquad_hs{hs_dy, SQRT2_2, hs_db}};
        Biquad_ls ls[2];//{BIquad_ls{ls_dy, SQRT2_2, ls_db}, Biquad_ls{ls_dy, SQRT2_2, ls_db}};
	}; // // class Feedback
    
    double maxApfDel = SR * params.apfMaxSeconds;
	Early early[2]{ {SR, 0, params.nApfStages, maxApfDel}, {SR, 1, params.nApfStages, maxApfDel} };
    Diffusion16 diffusion{ params.nDiffusionStages };
    int lateMaxDel = (int)std::round(SR * params.lateMaxSeconds);
    Late late[2]{ {SR, params.lateQuality, lateMaxDel}, {SR, params.lateQuality, lateMaxDel} };
    Feedback feedback{ SR, DT*params.ls_freq, params.ls_db, DT*params.hs_freq, params.hs_db };
};

} // // namespace

#endif // // include guard