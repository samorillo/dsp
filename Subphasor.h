#ifndef DSP_SUBPHASOR_H
#define DSP_SUBPHASOR_H
    
namespace dsp {
    
struct Subphasor {
    explicit Subphasor() {}
    explicit Subphasor(int _N) : N_{_N} {}
    Subphasor(const Subphasor& other) :
        N_{other.N_}, b{other.b}, n{other.n},
        y{other.y}, master_y_1{other.master_y_1}
    {}
    
    int N_;
    bool b;
    int n = 0;
    double y;
    double master_y_1 = -1.;
    
    void operator()(double master_y) {
        y = master_y * N_;
        if (master_y < master_y_1 || master_y == 0) {
            b = true;
            n = 0;
        }
        else {
            y -= n;
            if (y >= 1.) {
                b = true;
                y -= 1.;
                ++n;
            }
            else {
                b = false;
            }
        }
        master_y_1 = master_y;
    }
    void N(int _N) {
        N_ = _N;
        n = -1;
    }
    void N_post(int _N) {
        N_ = _N;
        n = 0;
        y = master_y_1 * N_;
        while (y >= 1.) { // // thought to be used for relatively small N
            ++n;
            y -= 1.;
        }
    }
    int N() const { return N_; }
    void reset() {
        n = 0;
        master_y_1 = -1.;
    }
};

} // // namespace

#endif // // include guard