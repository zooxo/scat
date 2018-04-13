# ScArY - Scientific RPN calculator on a Digispark
Welcome to ScAt (Scientific RPN Calculator on a Digispark)
Version 1.0 ... (c) 2018 by deetee/zooxo

ScAt is free software. You can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation.

## PREAMBLE
It is amazing how much calculator power a simple board like the Disgispark is able to perform. Based on an ATTINY the Digispark offers flashing and power supply via USB interface. Unfortunately the according bootloader consumes more than 2 kilobytes of the valuable flash memory. Nevertheless an optimized code can provide a wide range of commands and operations.

In general, functionality and functions outweighted comfort and error handling which can be seen, for instance, looking at the permanent scientific display format or a "non interpretable" display after dividing by zero. Thus the user has to follow the calculation process more strictly than on conventional RPN calculators.

Enjoy! deetee

## INTRODUCTION
ScAt is a scientific calculator based on an Digispark (ATTINY85) microcontroller and a QYF-TM1638-board (8 digit LED display with 16 buttons controlled with 3 pins). ScAt is capable of basic arithmetical functions and familiar operations of RPN calculators (ie stack operations) as well as a wide range of scientific functions as well as business and probability operations.

## THE CALCULATOR
### 8-DIGITS SCIENTIFIC DISPLAY (permanent SCI notation):
```
    mantissa  exponent
    |         |
  - m.m m m - e e
  |         |
  sign      EE-sign
```
### KEYBOARD LAYOUT:
```
  (f)(zZZ)        7      [SQRT]    8       [POW]        9(/) [1/X]
  [g]             4(GAUSS)[EXP]    5(ANNU)  [LN]        6(*)  [PI]
  CHS(ROT)[EE]    1(RCL)  [SIN]    2(STO)  [COS]        3(-) [TAN]
  C(BRIGHTNWSS)   0(SHOW)[ASIN]    .(SWAP)[ACOS]    ENTER(+)[ATAN]
```
### ENTERING NUMBERS:
```
  1 Enter mantissa (with '.' if applicable)
    Press ENTER to push mantissa to stack
  2 To enter power of ten:
    Enter exponent and (if applicable) change sign of exponent with CHS
  3 Press EE to link the mantissa with the exponent  
  4 If applicable: Toggle sign of number with CHS
```
## OPERATIONS and KEYS

### BASIC KEYS:
```
  0 1 2 3 4 5 6 7 8 9 . ... Digits and decimal point
  CHS                   ... Change sign
  ENTER                 ... Enter number (push stack)
  C                     ... Clear X, clear entry or escape/stop
  f g                   ... Function or shift to choose shifted keys
  FUNCTION-KEYS:
  + - * /    ... Basic operations
  STO RCL    ... Store number to respectively recall number from memory
  SHOW       ... Show full mantissa of number (7 digits without decimal point)
  SWAP       ... Swap X and Y register of stack (X<->Y)
  ROT        ... Rotate stack (X=Y Y=Z Z=T T=X)
  zZZ        ... Toggle screensaver      BRIGHTNESS ... Set brightness of display (0...7)
```
### MENU-FUNCTIONS:
```
  EXP LN SQRT POWER INV            ... Basic scientific operations
  GAUSS ANNU                       ... Probability(PDF/CDF), Annuity
  SIN  COS  TAN  ASIN  ACOS  ATAN  ... Trigonometric
  
  Note: Some functions (EE, COS, TAN, ACOS, GAUSS, GAMMA) may affect the whole
  stack as they are calculated indirectly with basic operations.
```
## SPECIALITIES
### ANNUITY (PRESENT VALUE):
```
  Example to calculate the present value of a $1 5 year return with an
  interest rate of 8%:
  1 .08 ENTER 5 ANNU ... annuity factor for 5 years and 8%
  2 X=3.99 ... 5 years $1 each are equal to ~$4 invested with 8%
```
### GAUSS:
```
                 y
                 ^
                 |
               1 ------------------------
                 |       +++++++ CDF (Cumulative Distribution Function)
                 |   +
                 | +                     (x)         .
                 |+              CDF = integral(PDF) = 1/(1+exp(-0.07*x^3-1.6*x))
                 +                      (-inf)
               **+**
            **  +|   **
              +  |     *         PDF = 1/sqrt(2*PI)*exp(-x*x/2)
         ** +    |      **
  +*+*+*+        |         ***** PDF (Probability Density Function)
  ---------------+------------------------> x
  
  Example to calculate PDF and CDF at x=0:
  1 0 GAUSS
  2 PDF=0.3989=1/sqrt(2*PI)
  3 SWAP
  4 CDF=0.5
```
## APPENDIX
### HARDWARE:
```
                            __________________
  VCC=5V                   | 8.8.8.8.8.8.8.8. |
       ---------       GND-|GND               |
  VCC-| VCC  P1 |----------|DIO   *   *   *   |
      |      P2 |----------|CLK   *   *   *   |
  GND-| GND  P3 |----------|STB   *   *   *   |
       ---------       VCC-|VCC   *   *   *   |
       DIGISPARK            ------------------
                                QYF-TM1638
```
