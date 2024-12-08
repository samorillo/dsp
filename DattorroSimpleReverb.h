#ifndef DSP_DATTORRO_REVERB_H
#define DSP_DATTORRO_REVERB_H

namespace dsp {

struct DattorroSimpleReverb {
    DattorroSimpleReverb(const int _SR) : SRfactor{_SR/48000} {
        if (_SR != 48000*SRfactor) throw "err";
        diffusionStages[0].C = 0.75;
        diffusionStages[1].C = 0.75;
        diffusionStages[2].C = 0.625;
        diffusionStages[3].C = 0.625;
        
        tankDampLR[0].damping = 0.0005;
        tankDampLR[1].damping = 0.0005;
        
        tankDiffusionLR1[0].C = tankDiffusionLR1[1].C = -0.7;
        tankDiffusionLR2[0].C = tankDiffusionLR2[1].C = +0.5;
        
        decay = 0.5;
    }
    struct Ap {
        Ap(const int deltime) : del{deltime} {}
        Delay del;
        double C;
        double y;
        Ap& operator()(double x) {
            v = del.y_prev();
            w = x - C*v;
            y = v + C*w;
            del(w);
            return *this;
        }
        double v, w;
    };
    struct DampFilter {
        double damping;
        double y_prev = 0;
        double y;
        DampFilter& operator()(double x) {
            y = damping*y_prev + (1-damping)*x;
            y_prev = 0;
            return *this;
        }
    };
    
    Ap diffusionStages[4]{142, 107, 379, 277};
    
    Ap tankDiffusionLR1[2]{672, 908};
    Delay tankDelayLR1[2]{Delay{4453}, Delay{4217}};
    DampFilter tankDampLR[2];
    Ap tankDiffusionLR2[2]{1800, 2656};
    Delay tankDelayLR2[2]{Delay{3720}, Delay{4217}};
    double decay;
    
    const int SRfactor;
    ButterVA_lp downsamplerLp{4, 1./(48000.*SRfactor) * 20000., SQRT2_2};
    int downsampleCounter = SRfactor-1;
    
    double x;
    Double2 y;
    Double2 yInternal;
    void operator()() {
        y.null();
        x = downsamplerLp(x).y;
        ++downsampleCounter;
        if (downsampleCounter == SRfactor) {
            downsampleCounter = 0;
            
            // // process at lower sample rate
            for (int i = 0; i < 4; ++i) x = diffusionStages[i](x).y;
            y += x;
            for (int c = 0; c < 2; ++c) {
                y[c] += tankDelayLR2[1-c].y_prev() * decay;
            }
            for (int c = 0; c < 2; ++c) {
                tankDiffusionLR2[c](decay * tankDampLR[c](tankDelayLR1[c].y_prev()).y);
                tankDelayLR2[c](tankDiffusionLR2[c].y);
                tankDelayLR1[c](tankDiffusionLR1[c](y[c]).y);
            }
            
            y[0] = 
                + 0.6 * tankDelayLR1[1].y(266)
                + 0.6 * tankDelayLR1[1].y(2974)
                - 0.6 * tankDiffusionLR2[1].del.y(1913)
                + 0.6 * tankDelayLR2[1].y(1996)
                - 0.6 * tankDelayLR1[0].y(1990)
                - 0.6 * tankDiffusionLR2[0].del.y(187)
                - 0.6 * tankDelayLR2[0].y(1066);
                
            y[1] = 
                + 0.6 * tankDelayLR1[0].y(353)
                + 0.6 * tankDelayLR1[0].y(3627)
                - 0.6 * tankDiffusionLR2[0].del.y(1228)
                + 0.6 * tankDelayLR2[0].y(2673)
                - 0.6 * tankDelayLR1[1].y(2111)
                - 0.6 * tankDiffusionLR2[1].del.y(335)
                - 0.6 * tankDelayLR2[1].y(121);
                
            yInternal = y;
            // // //
            
        }
        x = 0;
        y = yInternal;
    }
};
        
} // // namespace

/*
struct DattorroSimpleReverb {
    DattorroSimpleReverb(const int _SR) : SRfactor{_SR/48000} {
        if (_SR != 48000*SRfactor) throw "err";
        diffusionStages[0].C = 0.7;
        diffusionStages[1].C = 0.7;
        diffusionStages[2].C = 0.5;
        diffusionStages[3].C = 0.5;
    }
    struct Ap {
        Ap(const int deltime) : del{deltime} {}
        Delay del;
        double C;
        double y;
        Ap& operator()(double x) {
            v = del.y_prev();
            w = x - C*v;
            y = v + C*w;
            del(w);
            return *this;
        }
        double v, w;
    };
    
    const int SRfactor;
    Ap diffusionStages[4]{142, 107, 379, 277};
    
    ButterVA_lp downsamplerLp{4, 1./(48000.*SRfactor) * 20000., SQRT2_2};
    int downsampleCounter = SRfactor-1;
    
    double x;
    Double2 y;
    void operator()() {
        y.null();
        x = downsamplerLp(x).y;
        ++downsampleCounter;
        if (downsampleCounter == SRfactor) {
            downsampleCounter = 0;
            
            // // process at lower sample rate
            for (int i = 0; i < 4; ++i) x = diffusionStages[i](x).y;
            y += x;
            // //
            
        }
        x = 0;
        //y += x; x = 0;
    }
};
*/

/*
DattorroSimpleReverb(const int _SR) : SRfactor{_SR/48000} {
    if (_SR != 48000*SRfactor) throw "err";
    diffusionStages[0].C = 0.75;
    diffusionStages[1].C = 0.75;
    diffusionStages[2].C = 0.625;
    diffusionStages[3].C = 0.625;
    
    tankDampLR[0].damping = 0.0005;
    tankDampLR[1].damping = 0.0005;
    
    tankDiffusionLR1[0].C = tankDiffusionLR1[1].C = -0.7;
    tankDiffusionLR2[0].C = tankDiffusionLR2[1].C = +0.5;
    
    decay = 0.5;
}
struct Ap {
    Ap(const int deltime) : del{deltime} {}
    Delay del;
    double C;
    double y;
    Ap& operator()(double x) {
        v = del.y_prev();
        w = x - C*v;
        y = v + C*w;
        del(w);
        return *this;
    }
    double v, w;
};
struct DampFilter {
    double damping;
    double y_prev = 0;
    double y;
    DampFilter& operator()(double x) {
        y = damping*y_prev + (1-damping)*x;
        y_prev = 0;
        return *this;
    }
};

const int SRfactor;
Ap diffusionStages[4]{142*SRfactor, 107*SRfactor, 379*SRfactor, 277*SRfactor};

Ap tankDiffusionLR1[2]{672*SRfactor, 908*SRfactor};
Delay tankDelayLR1[2]{Delay{4453*SRfactor}, Delay{4217*SRfactor}};
DampFilter tankDampLR[2];
Ap tankDiffusionLR2[2]{1800*SRfactor, 2656*SRfactor};
Delay tankDelayLR2[2]{Delay{3720*SRfactor}, Delay{4217*SRfactor}};
double decay;

double x;
Double2 y;
void operator()() {
    y.null();
    for (int i = 0; i < 4; ++i) x = diffusionStages[i](x).y;
    y += x;
    for (int c = 0; c < 2; ++c) {
        y[c] += tankDelayLR2[1-c].y_prev() * decay;
    }
    for (int c = 0; c < 2; ++c) {
        tankDiffusionLR2[c](decay * tankDampLR[c](tankDelayLR1[c].y_prev()).y);
        tankDelayLR2[c](tankDiffusionLR2[c].y);
        tankDelayLR1[c](tankDiffusionLR1[c](y[c]).y);
    }
    x = 0;
    
    y[0] = 
        + 0.6 * tankDelayLR1[1].y(266*SRfactor)
        + 0.6 * tankDelayLR1[1].y(2974*SRfactor)
        - 0.6 * tankDiffusionLR2[1].del.y(1913*SRfactor)
        + 0.6 * tankDelayLR2[1].y(1996*SRfactor)
        - 0.6 * tankDelayLR1[0].y(1990*SRfactor)
        - 0.6 * tankDiffusionLR2[0].del.y(187*SRfactor)
        - 0.6 * tankDelayLR2[0].y(1066*SRfactor);
        
    y[1] = 
        + 0.6 * tankDelayLR1[0].y(353*SRfactor)
        + 0.6 * tankDelayLR1[0].y(3627*SRfactor)
        - 0.6 * tankDiffusionLR2[0].del.y(1228*SRfactor)
        + 0.6 * tankDelayLR2[0].y(2673*SRfactor)
        - 0.6 * tankDelayLR1[1].y(2111*SRfactor)
        - 0.6 * tankDiffusionLR2[1].del.y(335*SRfactor)
        - 0.6 * tankDelayLR2[1].y(121*SRfactor);
}
};

*/

#endif // // include guard