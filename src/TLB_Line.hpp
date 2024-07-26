#ifndef TLB_LINE_HPP
#define TLB_LINE_HPP

#include <systemc>
#include "dflipflop.hpp"
#include "global_constants.hpp"
using namespace sc_core;
using namespace std;

SC_MODULE(TLB_LINE)
{
    sc_in<bool> clk;
    sc_in<bool> write_signal;
    sc_vector<sc_in<bool>> d_signals;

    sc_vector<sc_signal<bool>> q_bar_signals;
    sc_vector<D_FLIP_FLOP> dflipflops;
    sc_signal<bool> tlb_line_clk;

    sc_vector<sc_out<bool>> q_signals;

    SC_CTOR(TLB_LINE) : d_signals("d_signals", GC::tlb_line_length),
                        q_signals("q_signals", GC::tlb_line_length),
                        q_bar_signals("q_bar_signals", GC::tlb_line_length),
                        dflipflops("flipflops", GC::tlb_line_length)
    {

         for (int i = 0; i < GC::tlb_line_length; ++i)

        {
            dflipflops[i].write_enable(write_signal);
            dflipflops[i].clk(clk);
            dflipflops[i].d(d_signals[i]);
            dflipflops[i].q(q_signals[i]);
            dflipflops[i].q_bar(q_bar_signals[i]);
        }
    }
};

#endif