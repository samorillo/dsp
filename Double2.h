#ifndef DSP_DOUBLE2_H
#define DSP_DOUBLE2_H

namespace dsp {
    
struct Double2 {
    explicit Double2(double _y0, double _y1) : y{_y0, _y1} {}
    explicit Double2() : y{0.,0.} {}
    Double2(const Double2& other) : y{other.y[0], other.y[1]} {}
    
    double y[2];
    
    void null() {
        y[0] = 0.;
        y[1] = 0.;
    }
    Double2& operator+=(const Double2& x) {
        y[0] += x[0];
        y[1] += x[1];
        return *this;
    }
    Double2& operator+=(double x) {
        y[0] += x;
        y[1] += x;
        return *this;
    }
    friend Double2 operator+(Double2 a, Double2 b) {
        return a+=b;
    }
    friend Double2 operator+(Double2 a, double b) {
        return a+=b;
    }
    friend Double2 operator+(double a, Double2 b) {
        return b+=a;
    }
    Double2& operator*=(const Double2& x) {
        y[0] *= x[0];
        y[1] *= x[1];
        return *this;
    }
    Double2& operator*=(double x) {
        y[0] *= x;
        y[1] *= x;
        return *this;
    }
    friend Double2 operator*(Double2 a, Double2 b) {
        return a*=b;
    }
    friend Double2 operator*(Double2 a, double b) {
        return a*=b;
    }
    friend Double2 operator*(double a, Double2 b) {
        return b*=a;
    }
    double& operator[](int i) {
        return y[i];
    }
    const double& operator[](int i) const {
        return y[i];
    }
    
};

} // // namespace

#endif // // include guard