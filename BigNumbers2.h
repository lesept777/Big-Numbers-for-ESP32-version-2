/*
    Big Numbers library for ESP32

    A big number is defined as follows:
    mantissa 10^( expo_0 10^(expo_1 10^...))
    where mantissa, expo_0, expo_1 ... expo_n are floating
    point numbers. n is the number of floors of the big number.

    Input 'e' and 'E' as short for '10^' , e.g. 2E4 is 2. 10^4 or 20000
    and 2eee4 is 2. 10^(10^(10^10^4))

    This library enables to apply some of the standard operations
    to the big numbers: addition, multiplication, exponentianation,
    factorial, comparison (< == > !=)

    The maximum number of floors is defined below as MAXFLOORS and
    can be changed

    (c) 2021 Lesept
    contact: lesept777@gmail.com

    Permission is hereby granted, free of charge, to any person obtaining
    a copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:
    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
    OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
    OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
    ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
    OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef Big_h
#define Big_h

#include <Arduino.h>
// #include "utils.h"
#define MAXPREC  16  // Maximum number of significant digits (<= 18)
#define MAXINPUT 255 // Maximum size of input or processing buffer
#define MAXFLOORS 100

/*
    For maximum precision, all the values are stored as uint64
    All values, except the last floor's exponent are normalized
    between 1 and 10. To keep them readable, the stored value is
    the real value multiplied by 1e16 (i.e. 10^MAXPREC)
    Example:
    10000000000000000 means 1
    12345600000000000 means 1.23456
    12345600000000010 means 1.234560000000001 (accuracy kept)
*/

using BigNumber = std::vector<int64_t>;

const uint64_t maxPower  = pow(10, MAXPREC);
const uint64_t maxNumber = 10 * maxPower - 1;

BigNumber set (std::vector<double> const &);

// operators
bool isEqual (BigNumber const &, BigNumber const &);
bool lessThan (BigNumber const &, BigNumber const &);
bool lessEqual (BigNumber const &, BigNumber const &);
bool greaterThan (BigNumber const &, BigNumber const &);
bool greaterEqual (BigNumber const &, BigNumber const &);

// binary operations
BigNumber bigAdd (BigNumber &, BigNumber &);
BigNumber bigMult (BigNumber &, BigNumber &);
BigNumber bigPower (BigNumber &, BigNumber &);

// unary operations
BigNumber bigStirling (BigNumber &);

void displayBigNumber (BigNumber const &, uint8_t = 10);
void displayStruct (BigNumber const &);
void XXdisp (int64_t, uint8_t, bool);

char* readSerial ();
bool parseOperator (char*, bool);
void parseInput(char*, int);
void cleanInput(char*);
void parsePowerTower (char*);
BigNumber parseKnuth (char*);
BigNumber parseNumber(char*);

void splash();
void help();

#endif