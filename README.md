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
2.10^(1.554806049671 10^45155)
```
 
Entering 3^4^5^6 answers:
```> 3^4^5^6
10^(7.3450247455 10^9407)
```

An addition: 2e3e4+3e3.00001e4 leads to
```> 2e3e4+3e3.00001e4
2.000000000050 10^30000 + 3.776776235369 10^30000 
= 5.776776235420 10^30000
```
And a multiplication:
```> 2e3e4*4e5e6
2.000000000050 10^30000 * 4.000000000000 10^5000000 
= 8.000000000000 10^5030000
```
For the comparison:
```> 2e3e4e5>3e4e5
2.000000000000 10^(3.000000000804 10^400000) > 3.000000000804 10^400000 
true
```

*2: Program your operations*
Another way is to use the API, i.e. the available functions, to deal directly in C with the big numbers and do more complex calculations.

To create an instance of a big number, first declare it:
`BigNumber N;`
Then you can affect a value to it using the set function. To set the value to 2 * 10^( 34 * 10^(1.2 * 10^58.6) it's as easy as
`N = set({2, 34, 1.2, 58.6});`
BigNumber are just a regular vectors of integers on 64 bits:
`using BigNumber = std::vector<int64_t>;`
so if you are familiar with them, you can also use the associated functions (such as `push_back`). I chose this kind of storage to keep the maximum possible accuracy.

To display the value of a big number, there are 2 possibilities:
```displayBigNumber(N);
displayStruct(N);
```
The first one displays the value of the big number using the scientific notation. The second shows the structure of the number, with all floors and mantissa / exponent.

For example:
```N = set({2, 34, 1.2, 58.6});
displayBigNumber(N); Serial.println();
```
displays on the monitor:
`2.0000000000 10^(3.4000000000 10^(4.7772860466 10^58))`
