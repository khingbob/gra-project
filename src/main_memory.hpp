#ifndef MAIN_MEMORY_HPP
#define MAIN_MEMORY_HPP

//
// Created by mohsen on 07/07/2024.
// template of interface for the main_memory.cpp
#include <systemc>
#include <map>
#include <array>
#include <iostream>
#include "global_constants.hpp"
#include "systemc.h"

using namespace sc_core;

// Main memory module that reads/writes data from/to memory
SC_MODULE(MAIN_MEMORY)
{
    sc_in<bool> clk;
    sc_in<int> write_enable;        // reading - 0, writing - 1
    sc_in<sc_bv<32>> address;       // address to read from or write into
    sc_in<sc_bv<32>> write_data;    // data to write
    sc_bv<32> addressout, valueout; // address and value (read or written)

    std::map<unsigned, unsigned> memory; // map data structure for the memory

    SC_CTOR(MAIN_MEMORY)
    {
        // At the rising edge of the clock, the address is not extracted from the tlb yet, thus memory acts on the falling edge.
        SC_THREAD(behaviour);
        sensitive << clk.pos();
        dont_initialize();
    };

    void behaviour()
    {
        while (true)
        {
            wait(8, SC_NS);
            const unsigned convertedAddress = address.read().to_uint();
            const unsigned convertedWriteData = write_data.read().to_uint();

            if (write_enable)
            {
                memory[convertedAddress] = convertedWriteData;
                addressout = address.read();
                valueout = write_data.read();
            }
            else
            {
                auto it = memory.find(convertedAddress);
                if (it != memory.end())
                {
                    addressout = it->first;
                    valueout = it->second;
                    GC::current_request->data = it->second;
                }
                else
                {
                    addressout = address.read();
                    valueout = 0;
                }
            }
//            addressPrinter(memory); // show what happened in the console
            wait();
        }
    }

//    void addressPrinter(std::map<unsigned, unsigned> memory)
//    {
//        std::cout << "-----------------------------" << std::endl;
//        for (auto i : memory)
//        {
//            std::cout << "||" << i.first << "|" << i.second << "||" << std::endl;
//        }
//        std::cout << "-----------------------------" << std::endl;
//    }
};

#endif
