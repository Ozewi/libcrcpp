# libcrc++

This is a one-header library for CRC calculation. It includes two templates: a template for direct CRC calculation
and a template for fast CRC calculation using lookup tables.

## Usage

There's only a single header file, so you can just copy it into your project and you're done.

Some examples are available in the `test` directory.

### Hands on

There are two different templates: CrcCalc and CrcFastCalc. The first template implements the plain algorithm,
the second one uses lookup tables.

### Instantiation

The class template has two parameters: base type and shift direction. The base type represents the register size and
must be an unsigned integer. Standard integer types are available in the header file **cstdint**, f.i. uint8_t, uint32_t,
etc. The implementation of the class template requires that the type size be a multiple of a byte.

The shift direction is an enumerated type defined in the header. It can take one of two values: libcrc::shiftLeft or
libcrc::shiftRight to select shifts to the left or to the right, respectively.

The class constructor requires a parameter with the polynomial to be used in the calculation. This parameter is an
integer of the same type specified in the class template instantiation.

### Calculation

The function **compute** does the CRC calculation and returns the calculated value. It has three parameters:
- A pointer to the data block to be computed.
- The length, in bytes, of the data block.
- Previous CRC value. In the first iteration, use the 'seed'.

Large CRC calculations can be done by segments, so the output of a segment is the "seed" for the next one. F.i.,
the following code computes the CRC of a file reading it by blocks of 1024 bytes:

```
    static constexpr int buffer_length = 1024;
    libcrc::CrcCalc<uint16_t, libcrc::shiftRight> crc_calculator(0x1021);
    uint8_t buffer[buffer_length];
    uint16_t crc = initial_seed;

    while (!feof(hf))
    {
        size_t lesen = fread(buffer, 1, buffer_length, hf);
        if (lesen <= 0)
            break;
        crc = crc_calculator.compute(buffer, lesen, crc);
    }

```

### Getting the lookup table

Sometimes it is interesting to get the calculated lookup table. The function **getLookupTable** provides a pointer
to the internal table.

The table itself is an array of 256 elements of the instantiation type.


## What is CRC?

CRC stands for Cyclic Redundancy Check and is a commom way of error detection in a dataset. The calculation consists
of considering the dataset as a stream of bits, being each bit the coefficient of a binary polynomial. The resulting
polynomial is then divided by other polynomial; the remainder of the division is the CRC code of the dataset.

The algorithm is quite simple, but it has some customizable items. Because of this, a number of variations have been
made over time.
These elements are:

- Length of the divider polynomial. Common values are 9, 17, 33 and 65 bits, from which CRC8, CRC16, CRC32 and CRC64
calculations are performed. These are the most common values, but there are applications and protocols using polynomial
lenghts ranging from 3 to 82 bits.
- Data input direction of the bits. Bits can be added from the left or the right side of the register; that is,
the bits received first are the most or least significant.
- Initial value, or "seed". Most usual values are 0 and the maximum value of the register, but there are other variants.
- Partial or total bit reversion after the calculation. Several specifications apply a XOR mask to the result of the
computation either to the whole register or to part of it.

Also, a common technique to speed up the process is to use a precalculated lookup table. Some programs use it, others
don't. The result is the same.

### Lookup tables

Lookup tables contain the result of dividing a byte by the chosen polynomial. In other words: a table consists of
an array of 256 elements, each the size of the polynomial. Using a table can reduce the computation cost in a factor
of 8 or more, at the cost of generating it and keeping it in memory. Grosso modo, the cost of filling the lookup
table is equivalent to the cost of calculating the CRC for a 256-byte message, so using lookup tables is usually a good
choice except in cases of severe memory constraints or very occasional use for small messages.

## Common CRC polynomials

CRC calculation has been used in numerous systems, applications and protocols, each with its own configuration
parameters. The following table shows the parameters for some common variants.

**Legend**
- **Name**: Most common name for the variant.
- **Type**: Data type of the CRC register. Used as the first parameter of the template instantiation.
- **Direction**: Bit shift direction. Used as the second parameter of the template instantiation.
- **Polynomial**: Polynomial used, in hexadecimal. Used as the constructor parameter.
- **Seed**: Initial value of the calculation. Used as the initial CRC value.
- **Final XOR value**: In some variants, a final XOR is applied to the calculated value.
- **Used in**: Some contexts in which the variant is used.

| Name          | Type     | Direction | Polynomial | Seed       | Final XOR  | Used in |
| :------------ | :------- | :-------: | :--------: | :--------: | :--------: | :------ |
| CRC8/ITU      | uint8_t  | Left      | `07`       | `00`       | `55`       | I.432.1, ATM HEC, ISDN HEC, SMBus |
| CRC8/DVB-S2   | uint8_t  | Left      | `D5`       | `00`       | -          | DVB-S2 (digital TV) |
| CRC8/AUTOSAR  | uint8_t  | Left      | `2F`       | `FF`       | `FF`       | AUTOSAR, OpenSafety |
| CRC16/GSM     | uint16_t | Left      | `1021`     | `0000`     | `FFFF`     | GSM |
| CRC16/XMODEM  | uint16_t | Left      | `1021`     | `0000`     | -          | XMODEM, ZMODEM, CRC16/LTE |
| CRC16/CCITT   | uint16_t | Right     | `1021`     | `0000`     | -          | Kermit, V.41, Bluetooth, ... |
| CRC16/X.25    | uint16_t | Right     | `1021`     | `FFFF`     | `FFFF`     | X.25, CRC16/IBM-SDLC |
| CRC16/IBM     | uint16_t | Right     | `8005`     | `0000`     | -          | ARC, LHA |
| USB           | uint16_t | Right     | `8005`     | `FFFF`     | `FFFF`     | USB |
| MODBUS        | uint16_t | Right     | `8005`     | `FFFF`     | -          | MODBUS |
| CRC32         | uint32_t | Right     | `04C11DB7` | `FFFFFFFF` | `FFFFFFFF` | HDLC, Ethernet, SATA, PkZIP, PNG, Gzip, V.42... |
| CRC32/AUTOSAR | uint32_t | Right     | `F4ACFB13` | `FFFFFFFF` | `FFFFFFFF` | AUTOSAR |
| CRC32/BZIP2   | uint32_t | Left      | `04C11DB7` | `FFFFFFFF` | `FFFFFFFF` | Bzip2 |
| CRC32C        | uint32_t | Right     | `1EDC6F41` | `FFFFFFFF` | `FFFFFFFF` | CRC32 Castagnoli (RFC 7143). ISCSI, Interlaken |
||||||||

You can find a more complete listing here:
http://reveng.sourceforge.net/crc-catalogue/all.htm


## Appendix: Shift direction and endianness

Sometimes, the bit shift direction is expressed as the bit order inside the CRC register. The equivalences are summarized below:

| Bit shift direction | Endianness |
| :-----------------: | :--------: |
| Left                | Big endian |
| Right               | Little endian |
|||
