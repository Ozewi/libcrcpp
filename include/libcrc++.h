/**
 * @package   libcrc++: C++ library for universal CRC calculation.
 * @brief     Template class definitions
 * @author    José Luis Sánchez Arroyo
 * @section   License
 * Copyright (c) 2017 - 2025 José Luis Sánchez Arroyo
 * This software is distributed under the terms of the BSD 3-Clause License and comes WITHOUT ANY WARRANTY.
 * Please read the file LICENSE for further details.
 */

#ifndef _LIBCRCPP_H_
#define _LIBCRCPP_H_

#include <stdint.h>                                     // uint8_t
#include <stdlib.h>                                     // size_t
#include <type_traits>                                  // std::enable_if_t, std::is_unsigned

namespace libcrc {

/**
 * @brief     Bit shift directions
 */
enum ShiftDir
{
    shiftLeft,
    shiftRight
};

/** ----------------------------------------------------
 * @brief     Auxiliary functions
 * ------ */
/**
 * @brief     Reverse order of bits of a word.
 */
template <typename T>
T                                                       /** @return Value with the bits in reverse order */
reverse(
    T word                                              /** @param word  Value to reverse */
)
{
    T result = 0;
    for (unsigned bit = 0; bit < sizeof(T) * 8; ++bit)
        if (word & (1 << bit))
            result |= 1 << ((sizeof(T) * 8) - bit - 1);
    return result;
}

/** ----------------------------------------------------
 * @brief     Struct Shifter: Function to shift bits.
 * @desc      The template contains a single function to shift bits left or right, according the instantiation parameter.
 * ------ */
template <typename T, ShiftDir dir>
struct Shifter
{
    /**
     * @brief     Shift a value the specified amount of bits.
     */
    T                                                   /** @return Value shifted */
    shift(
        T data,                                         /** @param data Value to shift */
        int bits                                        /** @param bits Number of bits to shift */
    ) const;
};

/**
 * @brief     Partial template specialization for left bit shifting.
 */
template <typename T>
struct Shifter<T, shiftLeft>
{
    T                                                   /** @return Value shifted */
    shift(
        T data,                                         /** @param data Value to shift */
        int bits                                        /** @param bits Number of bits to shift */
    ) const
    {
        return data << bits;
    };
};

/**
 * @brief     Partial template specialization for right bit shifting.
 */
template <typename T>
struct Shifter<T, shiftRight>
{
    T                                                   /** @return Value shifted */
    shift(
        T data,                                         /** @param data Value to shift */
        int bits                                        /** @param bits Number of bits to shift */
    ) const
    {
        return data >> bits;
    };
};

/** ----------------------------------------------------
 * @brief     Class CrcCalc: CRC calculator without table.
 * ------ */
template <typename T, ShiftDir dir, typename = std::enable_if_t<std::is_unsigned<T>::value>>
class CrcCalc
{
public:
    /**
     * @brief     Constructor. Initializes to zero.
     */
    CrcCalc(
      T poly                                            /** @param poly Polynomial to use for the computation */
    ) : shifter_(),
        polynomial_(dir == shiftLeft? poly : libcrc::reverse(poly)),
        mask_(dir == shiftLeft? (1 << ((sizeof(T) * 8) - 1)) : 1),
        pack_(dir == shiftLeft? ((sizeof(T) - 1) * 8) : 0)
    { };

    /**
     * @brief     Computes the CRC of a data block.
     */
    T                                                   /** @return Computed CRC */
    compute (
        const uint8_t* data,                            /** @param data    Pointer to the data block to compute CRC */
        size_t length,                                  /** @param length  Data length */
        T seed = 0                                      /** @param seed    Seed or computed CRC from the previous block */
    ) const
    {
        T result = seed;
        while (length--)
        {
            result ^= (*data++) << pack_;
            for (int bit = 8; bit > 0; --bit)
                if (result & mask_)
                  result = shifter_.shift(result, 1) ^ polynomial_;
              else
                  result = shifter_.shift(result, 1);
        }
        return result;
    };

private:
    Shifter<T, dir> shifter_;                           //!< Bit shifter
    T polynomial_;                                      //!< Polynomial used in the CRC computation
    const T   mask_;                                    //!< Bit mask with the next bit to be processed according to the processing direction set to 1
    const int pack_;                                    //!< Number of bits to shift to pack a byte inside a word
};

/** ----------------------------------------------------
 * @brief     Class CrcFastCalc: CRC calculador with lookup table.
 * ------ */
template <typename T, ShiftDir dir, typename = std::enable_if_t<std::is_unsigned<T>::value>>
class CrcFastCalc
{
public:
    /**
     * @brief     Constructor. Initializes the precalculated table.
     */
    CrcFastCalc(
      T poly                                            /** @param poly Polynomial to use for the computation */
    ) : shifter_(),
        polynomial_(dir == shiftLeft? poly : libcrc::reverse(poly)),
        mask_(dir == shiftLeft? (1 << ((sizeof(T) * 8) - 1)) : 1),
        pack_(dir == shiftLeft? ((sizeof(T) - 1) * 8) : 0)
    {
        for (unsigned idx = 0; idx < 256; ++idx)
        {
            T result = idx << pack_;
            for (int bit = 8; bit > 0; --bit)
                if (result & mask_)
                    result = shifter_.shift(result, 1) ^ polynomial_;
                else
                    result = shifter_.shift(result, 1);
            lookup_table_[idx] = result;
        }
    };

    /**
     * @brief     Provides a pointer to the lookup table.
     */
    const T*                                            /** @return Precalculated lookup table */
    getLookupTable() const
    {
        return lookup_table_;
    };

    /**
     * @brief     Computes the CRC of a data block.
     */
    T                                                   /** @return Computed CRC */
    compute (
      const uint8_t* data,                              /** @param data    Pointer to the data block to compute CRC */
      size_t length,                                    /** @param length  Data length */
      T seed = 0                                        /** @param seed    Seed or computed CRC from the previous block */
    ) const
    {
        T result = seed;
        while (length--)
            result = shifter_.shift(result, 8) ^ lookup_table_[((result >> pack_) ^ (*data++)) & 0xff];
        return result;
    };

private:
    Shifter<T, dir> shifter_;                           //!< Bit shifter
    T polynomial_;                                      //!< Polynomial used in the CRC computation
    const T   mask_;                                    //!< Bit mask with the next bit to be processed according to the processing direction set to 1
    const int pack_;                                    //!< Number of bits to shift to pack a byte inside a word
    T lookup_table_[256];                               //!< Precalculated lookup table
};

} // namespace

#endif  // _LIBCRCPP_H_
