#ifndef NOT_GATE_HPP
#define NOT_GATE_HPP
using namespace sc_core;
#include <systemc>

SC_MODULE(NOT_GATE)
{
    sc_in<bool> input;
    sc_out<bool> output;

    SC_CTOR(NOT_GATE)
    {
        SC_METHOD(to_do);
        sensitive << input;
    }
    void to_do()
    {

        output = (!input.read());

    }
};
#endif