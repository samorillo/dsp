#ifndef DSP_LINEQUE_H
#define DSP_LINEQUE_H

#include "Ramp.h"

#include <cmath>

namespace dsp {

struct LineQue {
    LineQue(const int _maxQueSize = 15) :
        maxQueSize{_maxQueSize}, que{new Data[maxQueSize]}
    {}
    ~LineQue() { delete[] que; }
    struct Data {
        double destination;
        int duration = 0;
        double slope = 0.;
    };
    
    const int maxQueSize;
    Ramp01 phase;
    Data* que;
    int queSize;
    Ramp counter;
    Ramp01 ramp;
    bool isActive = false;
    double range[2]{0., 0.};
    double y = 0.;
    
    void operator()() {
        if (!isActive) return;
        if (ramp.n >= ramp.N) {
            while (true) {
                counter();
                if (counter.n == counter.N) {
                    isActive = false;
                    return;
                }
                range[0] = range[1];
                range[1] = que[counter.n].destination;
                //if (que[counter.n].duration > 1) {
                //    ramp.init(que[counter.n].duration - 1);
                if (que[counter.n].duration > 0) {
                    ramp.init(que[counter.n].duration);
                    break;
                }
                else {
                    y = range[1];
                }
            }
        }
        ramp();
        double slope = que[counter.n].slope;
        double val = slopeCurve(ramp.y, slope);
        y = range[0] == range[1] ? range[0] : scale01(val, range[0], range[1]);
        if (ramp.n == 0) y = range[0];
        else if (ramp.n >= ramp.N-1) y = range[1];
        phase();
    }
    void init(const Data* _que, int _queSize) {
        queSize = _queSize;
        if (queSize > maxQueSize) {
            throw "err in LineQue";
        }
        //delete[] que;
        //que = new Data[queSize];
        for (int i = 0; i < queSize; ++i) que[i] = _que[i];
        counter.init(queSize);
        isActive = true;
        int durGlobal = 0;
        for (int i = 0; i < queSize; ++i) {
            durGlobal += que[i].duration;
        }
        //phase.init(durGlobal-1);
        phase.init(durGlobal);
        range[1] = y;//que[0].destination;
        ramp.init(0);
        ramp();
    }
    void set(double value) {
        y = value;
        range[0] = range[1] = value;
        isActive = false;
    }
    
    // // negative sign of slope means that more time is spent near 0.
    // // with positive sign, more time is spent near 1
    double slopeCurve(double x, double slope) {
        return slope == 0 ? x : (std::exp(-slope*ramp.y)-1)/(std::exp(-slope)-1);
    }
};

} // // namespace

#endif // // include guard