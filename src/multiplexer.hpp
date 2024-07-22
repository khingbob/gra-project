#ifndef MULTIPLEXER_HPP
#define MULTIPLEXER_HPP

#include <systemc>
#include <systemc.h>
using namespace sc_core;

SC_MODULE(MULTIPLEXER)
{
    sc_vector<sc_vector<sc_in<bool>>> inputs;
    sc_in<sc_bv<32>> choose;

    int choose_temp;
    sc_event choose_written_event;

    sc_vector<sc_out<bool>> out;
    SC_CTOR(MULTIPLEXER) : inputs("inputs", GC::tlbSize),
                           out("out_mux", GC::tlb_line_length)

    {
        for (int i = 0; i < GC::tlbSize; i++)
        {
            inputs[i].init(GC::tlb_line_length);
        }

        SC_THREAD(propagate_signals);

        sensitive << choose_written_event;
        dont_initialize();
    }

    void propagate_signals()
    {
        while (true)
        {
            choose_temp = choose.read().to_uint();
            for (int i = 0; i < GC::tlb_line_length; ++i)
            {
                bool in_value = inputs[choose_temp][i].read();
                out[i].write(in_value);
            }
            wait();
        }
    }
}

;
#endif