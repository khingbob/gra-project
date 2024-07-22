#ifndef DFLIPFLOP_HPP
#define DFLIPFLOP_HPP
#include <systemc>
using namespace sc_core;

// Data storing unit for each TLB line cell.
SC_MODULE(D_FLIP_FLOP)
{
  sc_in<bool> d;
  sc_in<bool> clk;
  sc_out<bool> q, q_bar;

  SC_CTOR(D_FLIP_FLOP)
  {
    SC_THREAD(behaviour);
    sensitive << clk.pos();
    dont_initialize();
  }
  void behaviour()
  {
      wait(7.5 , SC_NS); //the writing in the tlb line will happen in the last 3/4 of clock cycle to ensure signals propagation
      q = d.read();
      q_bar = !d.read();
  }
};
#endif