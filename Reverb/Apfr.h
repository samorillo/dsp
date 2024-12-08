// // original by Scott Nordlund

#ifndef DSP_APFR_H
#define DSP_APFR_H

namespace dsp {

struct Apfr {
    Apfr(
        int _SR,
        int _maxDelTime = -1,
        double _maxAbsG = -1,
        int _minApfDelTime = -1,
        int _maxApfDelTime = -1,
        int _minRampTime = -1,
        int _maxRampTime = -1
    ) :
        SR{_SR},
        maxDelTime{_maxDelTime >= 0 ? _maxDelTime : (int)(SR * 0.1)},
        maxAbsG{_maxAbsG >= 0 ? _maxAbsG : 0.5},
        minApfDelTime{_minApfDelTime >= 0 ? _minApfDelTime : (int)(SR * 0.0015)},
        maxApfDelTime{_maxApfDelTime >= 0 ? _minApfDelTime : (int)(SR * 0.010)},
        minRampTime{_minRampTime >= 0 ? _minRampTime : (int)(SR * 0.240)},
        maxRampTime{_maxRampTime >= 0 ? _maxRampTime : (int)(SR * 0.375)}
    {
        if (maxApfDelTime < minApfDelTime) {
            int tmp = minApfDelTime;
            minApfDelTime = maxApfDelTime;
            maxApfDelTime = tmp;
        }
        if (maxDelTime < minApfDelTime) {
            minApfDelTime = 1;
            maxApfDelTime = maxDelTime-1;
        }
        else if (maxDelTime < maxApfDelTime) {
            maxApfDelTime = maxDelTime;
        }
        for (int i = 0; i < 2; ++i) {
            randomize(i);
        }
    }
    
    const int SR;
    
    const int maxDelTime;
    double maxAbsG;
    int minApfDelTime;
    int maxApfDelTime;
    int minRampTime;
    int maxRampTime;
    
    double y = 0;
    Apfr& operator()(double x) {
        ramp();
        if (ramp.n >= ramp.N) {
            state = (state+1) % 4;
            ramp.init(state % 2 ? rr(minRampTime, maxRampTime) : maxDelTime*2);
            gates[0] = gates[1] = 1;
            ramp(); // //
        }
        switch (state) {
            case 0: gs[0] = 0.0;        break;
            case 1: gs[0] = ramp.y;     break;
            case 2: gs[0] = 1.0;        break;
            case 3: gs[0] = 1.0-ramp.y; break;
        }
        gs[1] = 1.0 - gs[0];
        for (int i = 0; i < 2; ++i) gs[i] *= polarities[i] * maxAbsG;
        
        if (state%2 == 0 && ramp.n == ramp.N/2) {
            int i = state/2;
            gates[i] = 0;
            randomize(i);
        }
        
        y = x;
        for (int i = 0; i < 2; ++i) {
            delread = apfDel[i].y_prev(apfDelTimes[i]);
            w = y + gs[i]*delread;
            apfDel[i](w);
            outDel[i](delread - gs[i]*w);
            y = gates[i] ? outDel[i].y(gate1DelTimes[i]) : apfDel[i].y();
        }

        return *this;
    }
    double w, delread; // // auxiliar
    
    int state = rr(0,3); // // 0-->0, 0-->1, 1-->1, 1-->0
    Ramp01 ramp;
    double gs[2];
    int gates[2];
    
    int polarities[2];
    
    Delay apfDel[2]{Delay{maxDelTime}, Delay{maxDelTime}};
    Delay outDel[2]{Delay{maxDelTime}, Delay{maxDelTime}};
    
    int apfDelTimes[2];
    // // int gate0DelTimes[2]{maxDelTime, maxDelTime};
    int gate1DelTimes[2];
    
    void randomize(int i) {
        polarities[i] = rsign();
        apfDelTimes[i] = rr(minApfDelTime, maxApfDelTime);
        gate1DelTimes[i] = maxDelTime - apfDelTimes[i];
    }
};

} // // namespace

#endif // // include guard

/*
Apfr(
    int _SR,
    double _maxDelTime    = 0.1,
    double _maxAbsG       = 0.5,
    double _minApfDelTime = 0.0015,
    double _maxApfDelTime = 0.010,
    double _minRampTime   = 0.240,
    double _maxRampTime   = 0.375
) :
    SR{_SR},
    maxDelTime   {(int)(SR * _maxDelTime)},
    maxAbsG      {           _maxAbsG},
    minApfDelTime{(int)(SR * _minApfDelTime)},
    maxApfDelTime{(int)(SR * _maxApfDelTime)},
    minRampTime  {(int)(SR * _minRampTime)},
    maxRampTime  {(int)(SR * _maxRampTime)}
{
    for (int i = 0; i < 2; ++i) {
        randomize(i);
    }
}

const int SR;

const int maxDelTime;
const double maxAbsG;
const int minApfDelTime;
const int maxApfDelTime;
const int minRampTime;
const int maxRampTime;
*/