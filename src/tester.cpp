#include <systemc>
#include <systemc.h>
#include "global_constants.hpp"
using namespace sc_core;

#include "and.hpp"

int sc_main(int argc, char *argv[])
{

    sc_signal<bool> a, b;
    AND_GATE and_gate("and");
    and_gate.input1(a);
    and_gate.input2(b);
    and_gate.output(a);
    a = true;
    sc_start(10, SC_NS);

    return 0;
}