/* adaptation of PureData example J09 */

#ifndef DSP_SAWBL_H
#define DSP_SAWBL_H

#include "Phasor.h"

#include <cmath>

namespace dsp {

struct Sawbl {
    explicit Sawbl(double _dy = 0) {
        if (!table_filled) {
            table_filled = true;
            fill_table();
        }
        dy(_dy);
    }
    Sawbl(const Sawbl& other) :
        factor{other.factor}, p{other.p}
    {}
    
    inline static int table_size = 1002;
    inline static int table_size_minus_2 = table_size - 2;
    inline static double half_table_size = table_size/2;
    inline static double* table;
    inline static bool table_filled{ false };
    
    double factor;
    Phasor p;
    double y;
    
    static void fill_table() {
        table = new double[table_size];
        double inc = 1. / (table_size - 1);
        double x;
        for (int i0 = -1, i1 = 0; i0 < table_size; i0=i1, ++i1) {
            x = inc * i0;
            x = -std::cos(PI * x) + 1./3. * std::cos(3. * PI * x);
            x *= 0.75;
            table[i1] = x;
        }
    }
    static double read(double ind) {
        double fl = std::floor(ind);
        int int_fl = (int)fl;
        double t = spline_interpolate(ind - fl,
            table[int_fl - 1], table[int_fl], 
            table[int_fl + 1], table[int_fl + 2]
        );
        return t;
    }
    static double read_point5(double ph) { // // phase in [-0.5, 0.5]
        return read(ph * table_size_minus_2 + half_table_size);
    }
    void operator()() {
        p();
        double x;
        x = p.y - 0.5;
        double ind;
        ind = x * factor;//clip(x * factor, -0.5, 0.5);
        if (ind < -0.5) ind = -0.5;
        else if (ind > 0.5) ind = 0.5;
        ind = ind * table_size_minus_2 + half_table_size;
        y = x - read(ind);
    }
    void dy(double value) {
        p.dy(value);
        factor = 0.1 / value; // 0.2 / value;
    }
    double dy() const { return p.dy(); }
};

} // // namespace

#endif // // include guard
