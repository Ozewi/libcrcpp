/**
 * @package   libcrc++: C++ library for universal CRC calculation.
 * @brief     Library test / demo program: generic CRC calculator.
 * @author    José Luis Sánchez Arroyo
 * @section   License
 * Copyright (c) 2017 - 2025 José Luis Sánchez Arroyo
 * This software is distributed under the terms of the MIT license and comes WITHOUT ANY WARRANTY.
 * Please read the file LICENSE for further details.
 */

#include <iostream>
#include <iomanip>
#include "libcrc++.h"

/*--- Calculation parameters ---*/
static constexpr int polynomial = 0x1021;
static constexpr unsigned seed = 0;
static constexpr int buffer_size = 4096;

/** ----------------------------------------------------
 * @brief   Example: Calculate CRC16 of a file.
 * ------*/
int main(int argc, char* argv[])
{

    if (argc < 2)
    {
        std::cerr << "A filename is required.\n";
        exit(-1);
    }

    FILE* f = fopen(argv[1], "r");
    if (f == nullptr)
    {
        std::cerr << "Error opening file " << argv[1] << std::endl;
        exit(-1);
    }

    uint8_t buffer[buffer_size];
    libcrc::CrcCalc<uint16_t, libcrc::shiftRight> crc_calculator(polynomial);
    uint16_t crc = seed;

    while (!feof(f))
    {
        int lesen = fread(buffer, 1, buffer_size, f);
        if (lesen <= 0)
            break;
        crc = crc_calculator.compute(buffer, lesen, crc);       // The file is read in chunks. The CRC calculated for a chunk is used as seed for the next.
    }
    fclose(f);

    std::cout << "File      : " << argv[1] << std::endl
              << "Algorithm : CRC16" << std::endl
              << "Polynomial: " << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << polynomial << std::endl
              << "Seed      : " << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << seed << std::endl
              << "CRC       : " << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << crc << std::endl;
}
