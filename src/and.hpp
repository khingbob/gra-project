#ifndef AND_GATE_HPP
#define AND_GATE_HPP
using namespace sc_core;
#include <systemc>

SC_MODULE(AND_GATE)
{
    sc_in<bool> input1;
    sc_in<bool> input2;

    sc_out<bool> output;

    SC_CTOR(AND_GATE)
    {
        SC_THREAD(to_do);
        sensitive << input1 << input2;
    }
    void to_do()
    {
        while (true)
        {
            output = (input1.read() && input2.read());
            wait();
        }
    }
};
#endif