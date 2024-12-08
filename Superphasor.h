#ifndef DSP_SUPERPHASOR_H
#define DSP_SUPERPHASOR_H
    
namespace dsp {
    
struct Superphasor {
    explicit Superphasor(int _N) : N_{_N}, invN_{1./_N} {}
    Superphasor(const Superphasor& other) :
        N_{other.N_}, invN_{other.invN_}, b{other.b}, n{other.n},
        y{other.y}, master_y_1{other.master_y_1}
    {}
    
    int N_;
    double invN_;
    bool b;
    int n = -1;
    double y;
    double master_y_1 = 10.; /////
    long long int lap = -1;
    
    void operator()(double master_y) {
        b = false;
        if (master_y < master_y_1 || master_y == 0) {
            ++n;
            if (n == N_ || n == 0) {
                b = true;
                ++lap;
                n = 0;
            }
        }
        y = (n + master_y) * invN_;
        master_y_1 = master_y;
    }
    void N_post(int _N) {
        N_ = _N;
        invN_ = 1./N_;
        n = (int)(y * _N);
        y = (n + master_y_1) * invN_;
    }
    int N() const { return N_; }
};

} // // namespace

#endif // // include guard