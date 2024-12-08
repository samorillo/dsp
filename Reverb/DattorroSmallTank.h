// // Jon Dattorro, Effect Design Part 1: Reverberator and other filters

#ifndef DSP_DATTORROSMALLTANK_H
#define DSP_DATTORROSMALLTANK_H

namespace dsp {

struct DattorroSmallTank {
    DattorroSmallTank(
        int _SR,
        double _sizeFactor = 1.0,
        double _decay = 0.50,
        double _hpFreq = 10,
        double _lpFreq = 15000,
        double _decayDiffusion1 = 0.70,
        double _decayDiffusion2 = 0.50
    ) :
        SR{_SR}, DT{1./_SR},
        sizeFactor{_sizeFactor},
        decay{_decay},
        decayDiffusion1{_decayDiffusion1},
        decayDiffusion2{_decayDiffusion2}
    {
        lfos[0].dy(DT * 0.10);
        lfos[1].dy(DT * 0.12);
        
        lfos[0].reset(0.);
        lfos[1].reset(0.5);
        
        for (int c = 0; c < 2; ++c) {
            lps[c].dy(DT * _lpFreq);
            hps[c].dy(DT * _hpFreq);
        }
    }
    const int SR;
    const double DT;
    
    //////////////////////////////////////
    const double sizeFactor = 1;
    const double decay = 0.50;
    const double decayDiffusion1 = 0.70;
    const double decayDiffusion2 = 0.50;
    //////////////////////////////////////
    
    double lfo1Freq = 0.10;
    double lfo2Freq = 0.150;
    double lfo3Freq = 0.120;
    double lfo4Freq = 0.180;
    
    const int maxExcursion = dd(16);
    
    inline static const int inSR = 29761;
    inline static const double inDT = 1./inSR;
    int dd(int inSamples) { // // nSamples
        return std::max(1, (int)std::round(sizeFactor * (inSamples * inDT) * SR));
    }
    
    struct Ap {
        Ap(const int deltime, double _C) : del{deltime}, C{_C} {}
        Delay del;
        double C;
        double y;
        Ap& operator()(double x, double d) {
            v = del.y_prev(d);
            w = x - C*v;
            y = v + C*w;
            del(w);
            return *this;
        }
        double v, w;
        void reset() {
            del.reset();
        }
    };

    Ap diffusionLR1[2]{
        {dd(672)+maxExcursion+1, decayDiffusion1},
        {dd(908)+maxExcursion+1, decayDiffusion2}
    };
    Delay delayLR1[2]{Delay{dd(4453)}, Delay{dd(4217)}};
    Onepole_lp lps[2];
    Onepole_hp hps[2];
    Ap diffusionLR2[2]{
        {dd(1800), decayDiffusion1},
        {dd(2656), decayDiffusion2}
    };
    Delay delayLR2[2]{Delay{dd(3720)}, Delay{dd(4217)}};
    
    Phasor lfos[4];
    
    Double2 x;
    Double2 y;
    void operator()() {
        y.null();
        
        y += x;
        for (int c = 0; c < 2; ++c) {
            y[c] += delayLR2[1-c].y_prev() * decay;
        }
        for (int c = 0; c < 2; ++c) {
            lfos[c]();
            double d = dd(c == 0 ? 672 : 908) + maxExcursion * tri(lfos[c].y);
            
            diffusionLR1[c](y[c],d).y;
            delayLR1[c](diffusionLR1[c].y);
            hps[c](delayLR1[c].y());
            lps[c](hps[c].y);
            diffusionLR2[c](decay * lps[c].y, dd(c==0?1800:2656));
            delayLR2[c](diffusionLR2[c].y);
        }
        
        y[0] = 
            + 0.6 * delayLR1[1].y(dd(266))
            + 0.6 * delayLR1[1].y(dd(2974))
            - 0.6 * diffusionLR2[1].del.y(dd(1913))
            + 0.6 * delayLR2[1].y(dd(1996))
            - 0.6 * delayLR1[0].y(dd(1990))
            - 0.6 * diffusionLR2[0].del.y(dd(187))
            - 0.6 * delayLR2[0].y(dd(1066));
            
        y[1] = 
            + 0.6 * delayLR1[0].y(dd(353))
            + 0.6 * delayLR1[0].y(dd(3627))
            - 0.6 * diffusionLR2[0].del.y(dd(1228))
            + 0.6 * delayLR2[0].y(dd(2673))
            - 0.6 * delayLR1[1].y(dd(2111))
            - 0.6 * diffusionLR2[1].del.y(dd(335))
            - 0.6 * delayLR2[1].y(dd(121));
            
        x.null();
    }
    
    void reset() {
        for (int i = 0; i < 2; ++i) {
            diffusionLR1[i].reset();
            delayLR1[i].reset();
            lps[i].reset();
            hps[i].reset();
            diffusionLR2[i].reset();
            delayLR2[i].reset();
        }
    }
};

} // // namespace

#endif // // include guard