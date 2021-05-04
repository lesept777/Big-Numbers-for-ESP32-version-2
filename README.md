# Big-Numbers-for-ESP32-version-2
New version, use this one

Explanation are here: https://www.instructables.com/Computing-With-really-Very-Big-Numbers/

First create a folder called BigNumbers and put inside the attached files.

To run the code, first open the BigNumbers.ino file with the arduino IDE, connect your ESP32 and upload the code. The serial terminal displays a welcome message and waits for your operation (choose 115200 baud as communication rate).

A few operations are available for now:
* addition, multiplication,
* tree exponentiation,
* factorial and multiple factorials,
* comparison.
To input an operation, just type it in the input zone. The result will be displayed in the display zone. Using the up arrow of your keyboard will recall the previous operation.

The code uses a simple syntax to recognize the operation. Here are the main features:
* Type h or ? to get some help,
* Use e or E for the exponent,
* Use ^or p or P (for 'power') for exponentiation,
* + and *
* Use <, > or = for comparison
Allowed characters are: digits, h ? e E ^ p P + * ! . < > = a A. All unknown characters are ignored.

Here are a few examples of big numbers that you can type:
* 12245
* 1e45 is 1 * 10^45
* 3e45e56 is 3 * 10 ^ (45 * 10^56)
* 3.45 E 4.56 e 6.78 E 7.89 is 3.45 * 10^( 4.56 * 10^(6.78 * 10^7.89) : in this case, the code will provide a simplification of the number
* 3^4^5^6 is 3^(4^(5^6)) as seen earlier (power tower)
* 3p4P5^6 is the same as above.

There are 2 ways of using it.
*1: Type the operations*
The simplest way is to type the operation you want, and the code will (try to) recognize it and provide the result. This is limited to simple calculations.

For example, you can enter:
* 12345 !!
* but not (123 !! + 456 ! ) !
Entering 12345!! will answer:
```> 12345!!
  1.234500000000 10^4 !! =
 10^(1.554806049671 10^45155)```
 
Entering 3^4^5^6 answers:
```> 3^4^5^6
10^(7.3450247455 10^9407)```
An addition: 2e3e4+3e3.00001e4 leads to
```> 2e3e4+3e3.00001e4
2.000000000050 10^30000 + 3.776776235369 10^30000 
= 5.776776235420 10^30000```
