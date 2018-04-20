/*
  ____________________

  Welcome to ScAt (Scientific RPN Calculator on a Digispark)
  Version 1.0 ... (c) 2018 by deetee/zooxo

  ScAt is free software. You can redistribute it and/or modify it under the terms
  of the GNU General Public License as published by the Free Software Foundation.

  ____________________

        PREAMBLE
  ____________________

    It is amazing how much calculator power a simple board like the Disgispark
    is able to perform. Based on an ATTINY the Digispark offers flashing and
    power supply via USB interface. Unfortunately the according bootloader
    consumes more than 2 kilobytes of the valuable 8k flash memory. Nevertheless
    an optimized code can provide a wide range of commands and operations.

    In general, functionality and functions outweighted comfort and error handling
    which can be seen, for instance, looking at the permanent scientific display
    format or a "non interpretable" display after dividing by zero. Thus the
    user has to follow the calculation process more strictly than on conventional
    RPN calculators.

    Enjoy!
    deetee
  ____________________

     INTRODUCTION
  ____________________

    ScAt is a scientific calculator based on an Digispark (ATTINY85) microcontroller
    and a QYF-TM1638-board (8 digit LED display with 16 buttons controlled with 3 pins).

    ScAt is capable of basic arithmetical functions and familiar operations
    of RPN calculators (ie stack operations) as well as a wide range of
    scientific functions as well as business and probability operations.

  ____________________

    THE CALCULATOR
  ____________________

    8-DIGITS SCIENTIFIC DISPLAY (permanent SCI notation):

        mantissa  exponent
        |         |
      - m.m m m - e e
      |         |
      sign      EE-sign


    KEYBOARD LAYOUT:

      (f)(zZZ)        7(R->P)[SQRT]    8(P->R) [POW]        9(/) [1/X]
      [g]             4(GAUSS)[EXP]    5(ANNU)  [LN]        6(*)  [PI]
      CHS(ROT)[EE]    1(RCL)  [SIN]    2(STO)  [COS]        3(-) [TAN]
      C(BRIGHTNWSS)   0(SHOW)[ASIN]    .(SWAP)[ACOS]    ENTER(+)[ATAN]


  ENTERING NUMBERS:

    1 Enter mantissa (with '.' if applicable)
      Press ENTER to push mantissa to stack
    2 To enter power of ten:
      Enter exponent and (if applicable) change sign of exponent with CHS
    3 Press EE to link the mantissa with the exponent
    4 If applicable: Toggle sign of number with CHS

  ____________________

  OPERATIONS and KEYS
  ____________________

    BASIC KEYS:
      0 1 2 3 4 5 6 7 8 9 . ... Digits and decimal point
      CHS                   ... Change sign
      ENTER                 ... Enter number (push stack)
      C                     ... Clear X, clear entry or escape/stop
      f g                   ... Function or shift to choose shifted keys

    FUNCTION-KEYS:
      + - * /    ... Basic operations
      STO RCL    ... Store number to respectively recall number from memory
      SHOW       ... Show full mantissa of number (7 digits without decimal point)
      SWAP       ... Swap X and Y register of stack (X<->Y)
      ROT        ... Rotate stack (X=Y Y=Z Z=T T=X)
      zZZ        ... Toggle screensaver
      BRIGHTNESS ... Set brightness of display (0...7)

    MENU-FUNCTIONS:
      EXP LN SQRT POWER INV            ... Basic scientific operations
      R->P P->R                        ... Polar/Rectangular conversion
      GAUSS ANNU                       ... Probability(PDF/CDF), Annuity
      SIN  COS  TAN  ASIN  ACOS  ATAN  ... Trigonometric Functions

    Note: Some functions (EE, COS, TAN, ACOS, R->P, P->R,GAUSS, GAMMA)
          may affect the whole stack as they are calculated indirectly
          with basic operations.


  ____________________

     SPECIALITIES
  ____________________

    ANNUITY (PRESENT VALUE):

      Example to calculate the present value of a $1 5 year return with an
      interest rate of 8%:
        1 .08 ENTER 5 ANNU ... annuity factor for 5 years and 8%
        2 X=3.99 ... 5 years $1 each are equal to ~$4 invested with 8%


    GAUSS:
                     y
                     ^
                     |
                   1 ------------------------
                     |       +++++++ CDF (Cumulative Distribution Function)
                     |   +
                     | +                     (x)         .
                     |+              CDF = integral(PDF) = 1/(1+exp(-0.07*x^3-1.6*x))
                     +                      (-inf)
                   **+**
                **  +|   **
                  +  |     *         PDF = 1/sqrt(2*PI)*exp(-x*x/2)
             ** +    |      **
      +*+*+*+        |         ***** PDF (Probability Density Function)
      ---------------+------------------------> x

      Example to calculate PDF and CDF at x=0:
        1 0 GAUSS
        2 PDF=0.3989=1/sqrt(2*PI)
        3 SWAP
        4 CDF=0.5

  ____________________

       APPENDIX
  ____________________

    HARDWARE:
                                 __________________
       VCC=5V                   | 8.8.8.8.8.8.8.8. |
            ---------       GND-|GND               |
       VCC-| VCC  P1 |----------|DIO   *   *   *   |
           |      P2 |----------|CLK   *   *   *   |
       GND-| GND  P3 |----------|STB   *   *   *   |
            ----------      VCC-|VCC   *   *   *   |
            DIGISPARK            ------------------
                                     QYF-TM1638

  ____________________


*/


// INCLUDES AND DEFINES

//#define STROBE 11 // Strobe pin // Arduino/Genuino Micro
//#define CLOCK   6 // Clock pin
//#define DATA    3 // Data pin
#define DATA   1 // 5 green    // ATTINY85
#define CLOCK  2 // 6 yellow   // HW-pins: 8 7 6 5  SW-pins: 3 2 1 0
#define STROBE 3 // 7 orange   //          1 2 3 4           4 5 6 7

// Commands for QYF-TM1638-board
#define PRINTCMD 0x40 // Command code for display printing
#define READCMD  0x42 // Command code for key reading

// Constants
#define DIGITS            8 // Number of digits
#define SEGMENTS          8 // Number of segments per digit
#define KEYS             16 // Number of keys
#define STACKSIZE         5 // Size for stack (XYZTM) M=mem
#define FIX               3 // Display 3 decimals
#define FIXMANT           3 // FIX in 10th-exponent
#define FIXSHOW           6 // FIX for SHOW
#define FIXMANTSHOW       6 // FIX in 10th-exponent
#define BRIGHTMAX         7 // Maximal value for brightness
#define TINYNUMBER     1e-7 // Number for rounding to 0
#define MAXEE            38 // Maximal EE for Y10X
#define MAXITERATE      100 // Maximal number of Taylor series loops to iterate
#define RAD        57.29578 // 180/PI
#define SHIFTABLEKEYS    14 // Number of shiftable key

// Segment flags
#define _7A 0x00 //  --a--
#define _7B 0x02 // |     |
#define _7C 0x04 // f     b
#define _7D 0x06 // |--g--|
#define _7E 0x08 // e     c
#define _7F 0x0a // |     |
#define _7G 0x0c //  --d--  (h)
#define _7H 0x0e

// assign 2-nibble byte code to sign
// hgfe-dcba 8421-8421
#define _7APOSTROPHELEFT 0x20
#define _7MINUS 0x40
#define _7FULLSTOP 0x80
#define _7DIGIT0 0x3f
#define _7DIGIT1 0x06
#define _7DIGIT2 0x5b
#define _7DIGIT3 0x4f
#define _7DIGIT4 0x66
#define _7DIGIT5 0x6d
#define _7DIGIT6 0x7d
#define _7DIGIT7 0x07
#define _7DIGIT8 0x7f
#define _7DIGIT9 0x6f

static const byte digit[] = {
  _7DIGIT0             , _7DIGIT1             , _7DIGIT2             , _7DIGIT3,
  _7DIGIT4             , _7DIGIT5             , _7DIGIT6             , _7DIGIT7,
  _7DIGIT8             , _7DIGIT9
};

// Key values for querying with keycast()
#define KF 16    //  key number:   getkey():     keycast[]:    symbol:      playstring:
#define K7 8     //   1  2  3  4    1  3  5  7   16  8  9 10   F  7  8  9   ?  7  8  9
#define K8 9     //   5  6  7  8    9 11 13 15   15  5  6  7   G  4  5  6   >  4  5  6
#define K9 10    //   9 10 11 12   16  2  4  6   14  2  3  4   H  1  2  3   =  1  2  3
#define KG 15    //  13 14 15 16    8 10 12 14   13  1 11 12   C  0  D  X   <  0  :  ;
#define K4 5
#define K5 6
#define K6 7
#define KH 14
#define K1 2
#define K2 3
#define K3 4
#define KC 13
#define K0 1
#define KD 11
#define KX 12


// GLOBAL VARIABLES

static double stack[STACKSIZE]; // Stack [XYZTM] M=mem
static byte dispbuf[DIGITS] = {0, _7DIGIT0, _7DIGIT0, _7DIGIT0, _7DIGIT0, 0, _7DIGIT0, _7DIGIT0}; // Display buffer
static byte decimals = 0;       // Number of decimals entered - used for input after decimal dot
static byte isgf = 0;           // 0/1/2 (none/g/f pressed)
static byte ptr;                // Playstring position
static byte brightness = 0;     // Brightness

// flags
static boolean isshow = false;     // True, if SHOW was demanded
static boolean isnewnumber = true; // True if stack has to be lifted before entering a new number
static boolean ispushed = false;   // True if stack was already pushed by ENTER
static boolean isdot = false;      // True if dot was pressed and decimals will be entered
static boolean isplaystring = false; // True if string should be played
static boolean isdisplayoff = false; // True if display is deactivated

// Playstring variables (f=? g=>)
const char s00[] PROGMEM = "?:>5?6>4";         // YpowerX
const char s01[] PROGMEM = ">1;?6=1?;>7";      // COS
const char s02[] PROGMEM = ">1;;?6=1?;>7>9?6"; // TAN
const char s03[] PROGMEM = ">0=90?;";          // ACOS
const char s04[] PROGMEM = ";;;?6?6:07?6=?:1:6?6=?;>41?;>9?:;?6=2?9>40:3989423?6"; // GAUSS-CDF-PDF
// GAUSS-CDF: ENTER ENTER ENTER * * .07 * CHS SWAP 1.6 * CHS + EXP 1 + INV
//            SWAP
//       PDF: ENTER * CHS 2 / EXP 0.3989423 *
const char s05[] PROGMEM = "?:;;1?;?:?=>5?6>4>9=1?;?=?=?=?9";    // ANNU
const char s06[] PROGMEM = ";?=?=?:?=;?=?:?9>;?=?=?6?=?=?6?;>7"; // R->P
const char s07[] PROGMEM = "?:>1;;?6=1?;>7?=?6?=?6?:?=";         // P->R
//const char s08[] PROGMEM = "1?;;;>9>4;>9=?;2?9?6>5?62?9?:=?;?=?=>5?::5?3?6?;:9189385?;>4"; // GAMMA

// PSEUDO CODE EXAMPLES:
// YpowerX: SWAP LN * EXP
// COS: SIN ENTER * CHS 1 + SQRT
// TAN: SIN ENTER ENTER * CHS 1 + SQRT INV *
// ACOS: ASIN CHS 90 +
// Y*10^X: 10 LN * EXP *
// GAUSS-CDF: ENTER ENTER ENTER * * .07 * CHS SWAP 1.6 * CHS + EXP 1 + INV
//            SWAP
//       PDF: ENTER * CHS 2 / EXP 0.3989423 *
// ANNU: SWAP ENTER ENTER 1 + LN * EXP INV CHS 1 + ROT ROT ROT /
// SINH: EXP ENTER INV CHS + 2 /
// COSH: EXP ENTER INV + 2 /
// TANH: EXP ENTER ENTER INV CHS + SWAP ENTER INV + /
// ASINH: ENTER ENTER * 1 + SQRT + LN
// ACOSH: ENTER ENTER * 1 - SQRT + LN
// ATANH: ENTER ENTER 1 + SWAP CHS 1 + / SQRT LN
// R2P: ENTER ROT ROT SWAP ROT ENTER ROT SWAP / ATAN ROT ROT * ROT ROT * + SQRT
// P2R: SWAP SIN ENTER ENTER * CHS 1 + SQRT ROT * ROT * SWAP ROT
// GAMMA: 1 + ENTER ENTER INV EXP ENTER INV CHS + 2 / * LN * 2 /
//        SWAP CHS + ROT ROT LN SWAP .5 - * + .9189385 + EXP
// GAMMA - Higher precision:
//        1 + ENTER ENTER INV EXP ENTER INV CHS + 2 / * SWAP 6 PWR 810 * INV + LN * 2 /
//        SWAP CHS + ROT ROT LN SWAP .5 - * + .9189385 + EXP

const char* const s_table[] PROGMEM = {s00, s01, s02, s03, s04, s05, s06, s07};
char buffer[55]; // Holds sii[]


// SUBROUTINES

// Macros
#define _abs(x) ((x<0)?(-x):(x)) // abs()-substitute macro
#define _ones(x) ((x)%10)        // Calculates ones unit
#define _tens(x) (((x)/10)%10)   // Calculates tens unit

static void cmd(byte val) { // Send command to shield
  digitalWrite(STROBE, LOW);
  shiftOut(DATA, CLOCK, LSBFIRST, val);
  digitalWrite(STROBE, HIGH);
}

static unsigned int getbuttons(void) { // Get button code (only one key at a time) - 16 bits
  unsigned int buttons = 0;
  byte val = 0;
  digitalWrite(STROBE, LOW);
  shiftOut(DATA, CLOCK, LSBFIRST, READCMD); // Demand key input
  pinMode(DATA, INPUT); // Switch data line to INPUT
  for (byte i = 0; i < 4; i++) {
    val = shiftIn(DATA, CLOCK, LSBFIRST) >> 1; // Get button value (and half it)
    if (val > 2) val >>= 2; // Reduce values 16 and 32 to 4 and 8
    buttons |= val << (4 * i); // Write value-nibble to buttons-nibbles (4,3,2,1)
  }
  pinMode(DATA, OUTPUT); // Switch data line back to output
  digitalWrite(STROBE, HIGH);
  return (buttons);
}

static byte buttoncast(void) { // Returns byte value with key number
  unsigned int c = getbuttons();
  byte ans = NULL;
  if (c & 0x1) return (16);
  while (c >>= 1) ans++;
  return (ans);
}

static byte getkey(void) { // Get one debounced key       1  3  5  7
  static byte oldkey = 0; // Needed for debouncing        9 11 13 15
  byte key1 = buttoncast(), key2 = 0; //                 16  2  4  6
  if (key1 != oldkey) key2 = buttoncast(); //             8 10 12 14
  if (key1 == key2) {
    oldkey = key1;
    return (key1);
  }
  return (NULL);
}

static void printbuf(void) { // Write buffer to display
  byte digbuf;

  if (!isshow) dispbuf[1] |= _7FULLSTOP; // Dot for normal print
  else dispbuf[0] = _7APOSTROPHELEFT; // Sign for show

  cmd(PRINTCMD); // Demand write command
  for (byte j = 0; j < SEGMENTS; j++) { // Write segment of all digits
    digbuf = 0;
    for (byte i = 0; i < DIGITS; i++) {
      digbuf = (digbuf << 1) | ((dispbuf[i] & (0x01 << j)) ? 0x01 : 0x00);
    }
    for (byte i = 0; i < DIGITS; i++) { // Print segments for each digit
      digitalWrite(STROBE, LOW);
      shiftOut(DATA, CLOCK, LSBFIRST, 0xc0 | 2 * j); // Address - segment
      shiftOut(DATA, CLOCK, LSBFIRST, 0x00 | digbuf); // Data - digit
      digitalWrite(STROBE, HIGH);
    }
  }
}

static void printfloat(void) { // Prints double or mantissa (show=true)
  static const byte digits[] = { // Character set to display numbers
    _7DIGIT0, _7DIGIT1, _7DIGIT2, _7DIGIT3, _7DIGIT4,
    _7DIGIT5, _7DIGIT6, _7DIGIT7, _7DIGIT8, _7DIGIT9
  };
  double f = stack[0];
  long m; // Mantissa
  int8_t e; // Exponent
  memset(dispbuf, NULL, sizeof(dispbuf));

  byte mantdigits = 4; // Set local variables for "normal print"
  byte fix = FIX;
  byte fixmant = FIXMANT;

  if (isshow) { // Set local variables for "SHOW print" if SHOW was demanded
    mantdigits = 7;
    fix = FIXSHOW;
    fixmant = FIXMANTSHOW;
  }

  if (f < 0.0) {
    f = - f;
    dispbuf[0] = _7MINUS;
  }
  e = log10(f); // Exponent
  m = (f / _pow10(e - fix)) + 0.5; // Mantissa
  if (m > 0 && m < _pow10(fixmant)) { // Change (n-1)-digit-mantissa to n digits
    e -= 1;
    m = (f / _pow10(e - fix)) + 0.5; // "New" mantissa
  }
  for (byte i = mantdigits; i > 0; i--) { // Print mantissa
    dispbuf[i] = digits[m % 10];
    m /= 10;
  }
  if (!isshow) { // Rest of "normal print"
    if (e < 0) { // Negative exponent
      dispbuf[5] = _7MINUS;
      e = -e;
    }
    if (e < 10) { // Print exponent
      dispbuf[6] = _7DIGIT0;
      dispbuf[7] = digits[e];
    }
    else {
      dispbuf[6] = digits[_tens(e)];
      dispbuf[7] = digits[_ones(e)];
    }
  }
  printbuf(); // Write dispbuf to display
}

static double _pow10(int8_t e) { // Returns 10^e
  boolean neg = (e < 0);
  double f = 1.0;
  if (neg) while (e++)  f *= 10;
  else while (e--)  f *= 10;
  if (neg) f = 1 / f;
  return (f);
}

static double _exp_sin(double f, boolean isexp) { // Calculate exp with Taylor series
  double result = f;
  double frac = f;
  if (isexp) result = frac = 1.0;
  for (byte i = 1; _abs(frac) > TINYNUMBER && i < MAXITERATE; i++) {
    if (isexp) frac *= f / i; // Fraction for exp
    else frac *=  f  / (-2 * i * (2 * i + 1)) * f; // Fraction for sin
    result += frac;
  }
  return (result);
}

static double _sqrt(double f) { // Calculating square root without using sqrt()
  return (_exp_sin(0.5 * log(f), true));
}

static void push(void) { // Push stack
  memmove(&stack[1], &stack[0], 3 * sizeof(double));
}
static void pull_upper(void) { // Pull upper part of stack
  memmove(&stack[1], &stack[2], 2 * sizeof(double));
}
void pull(void) { // Pull stack fully
  memmove(&stack[0], &stack[1], 3 * sizeof(double));
}

static void playstring(byte slot) {  // Intitialize playing string
  strcpy_P(buffer, (char*)pgm_read_word(&(s_table[slot]))); // Copy playstring to buffer
  ptr = 0;
  isnewnumber = true;
  isplaystring = true;
}

// Function pointer array
static void (*dispatch[])(void) = { // Function pointer array for g and f
  &do_nop, // null
  &do_asin, &do_sin, &do_cos, &do_tan,  // g (1...14)
  &do_exp, &do_ln, &do_pi, &do_sqrt,
  &do_pow, &do_inv, &do_acos, &do_atan,
  &do_nop, &do_ee,
  &do_show, &do_rcl, &do_sto, &do_sub, // f (15...28)
  &do_gauss, &do_annu, &do_mult, &do_r2p,
  &do_p2r, &do_div, &do_swap, &do_add,
  &do_lit, &do_rot
};

static void do_nop(void) {} // No operation

// g
static void do_sqrt(void) { // SQRT
  if (stack[0] > 0.0) stack[0] = _sqrt(stack[0]);
}
static void do_pow(void) {  // XpowerY (calculated with playstring)
  playstring(0);
}
static void do_inv(void) {  // 1/x
  stack[0] = 1 / stack[0];
}
static void do_exp(void) {  // EXP
  stack[0] = _exp_sin(stack[0], true);
}
static void do_ln(void) {   // LN
  stack[0] = log(stack[0]);
}
static void do_sin(void) {  // SIN
  stack[0] = _exp_sin(stack[0] / RAD, false);
}
static void do_cos(void) {  // COS (calculated with playstring)
  playstring(1);
}
static void do_tan(void) {  // TAN (calculated with playstring)
  playstring(2);
}
static void do_asin(void) { // ASIN
  stack[0] = atan(stack[0] / _sqrt(1 - stack[0] * stack[0])) * RAD;
}
static void do_acos(void) { // ACOS (calculated with playstring)
  playstring(3);
}
static void do_atan(void) { // ATAN
  stack[0] = atan(stack[0]) * RAD;
}

// f
static void do_div(void) {  // /
  stack[0] = stack[1] / stack[0];
  pull_upper();
}
static void do_mult(void) { // *
  stack[0] *= stack[1];
  pull_upper();
}
static void do_sub(void) {  // -
  stack[0] = stack[1] - stack[0];
  pull_upper();
}
static void do_add(void) {  // +
  stack[0] += stack[1];
  pull_upper();
}
static void do_ee(void) {   // EE = Y*10^X //(calculated with playstring)
  //if (stack[0] <= MAXEE) {
  stack[0] = stack[1] * _pow10(stack[0]);
  pull_upper();
  //slot = 4;
  //playstring();
  //}
}
static void do_sto(void) {  // STO
  stack[4] = stack[0];
}
static void do_rcl(void) {  // RCL
  push();
  stack[0] = stack[4];
}
static void do_show(void) { // SHOW demanded
  isshow = true;
}
static void do_swap(void) { // SWAP
  double tmp = stack[0];
  stack[0] = stack[1];
  stack[1] = tmp;
}
static void do_rot(void) {  // ROT
  double tmp = stack[0];
  pull();
  stack[3] = tmp;
}
static void do_pi(void) {   // PI
  if (!ispushed) push();
  stack[0] = PI;
}
static void do_lit(void) {  // BRIGHTNESS
  //if ((int)stack[0] == 0) cmd(0x80); // Deactivate display - "0000 abbb" (a=0)
  //else
  brightness = (stack[0] > BRIGHTMAX) ? BRIGHTMAX : (int)stack[0];
  cmd(0x88 | brightness);
  pull();
}
static void do_gauss(void) { // GAUSS
  playstring(4);
}
static void do_annu(void) {  // ANNU
  playstring(5);
}
static void do_r2p(void) {   // R2P
  playstring(6);
}
static void do_p2r(void) {   // P2R
  playstring(7);
}


// SETUP and LOOP

void setup() {
  pinMode(STROBE, OUTPUT);
  pinMode(CLOCK, OUTPUT);
  pinMode(DATA, OUTPUT);
  cmd(0x88); // Activate and brightness "1000 abbb" (a=1)
  printbuf();
}

void loop() {
  const byte keycast[KEYS + 1] = {0, KF, K1, K7, K2, K8, K3, K9, KC, KG, K0, K4, KD, K5, KX, K6, KH};
  byte key = NULL; // Pressed key

  key = keycast[getkey()]; // Read key

  if (isplaystring) { // ### Plays string
    if (buffer[ptr] == NULL) isplaystring = false;
    else key = buffer[ptr++] - '/';
    //Serial.print("slot="); Serial.print(slot); Serial.print(" ptr="); Serial.print(ptr); Serial.print(" key="); Serial.println(key);
  }

  if (key >= KG) { // ### Toggle function key g or f
    byte tmp = key - KG + 1;
    if (tmp == 2) {
      if (isgf == 2) {
        cmd(0x80); // Deactivate screen
        isdisplayoff = true;
      }
      else {
        if (isdisplayoff) {
          cmd(0x88 | brightness); // Activate screen
          isgf = 2; // Do like f was pressed before
          isdisplayoff = false;
        }
      }
    }
    isgf = (tmp != isgf) ? tmp : 0;
    key = NULL;
  }
  else if (key == KD) { // ### Entering decimals demanded
    if (!isgf) {
      if (isnewnumber) { // Enable starting new number with .
        if (ispushed) ispushed = false;
        else push();
        stack[0] = 0.0;
        decimals = 0;
        isnewnumber = false;
      }
      isdot = true;
      key = NULL;
    }
  }

  if (key != NULL) {

    if (isgf) { // Call function
      byte tmp = key + (isgf - 1) * SHIFTABLEKEYS;
      (*dispatch[tmp])();
      isgf = 0;
      ispushed = false;
      isnewnumber = true;
    }
    else if (key <= K9) { // Numeric input
      if (isnewnumber) { // New number
        if (ispushed) ispushed = false;
        else push();
        stack[0] = 0.0;
        decimals = 0;
        isdot = false;
        isnewnumber = false;
      }
      if (isdot) stack[0] += (key - 1) / _pow10(++decimals); // Append decimal to number
      else stack[0] = stack[0] * 10 + key - 1; // Append digit to number
    }
    else if (key == KX) { // ENTER
      push();
      ispushed = true;
      isnewnumber = true;
    }
    else if (key == KC) { // CLX/CE
      if (!isnewnumber) { // CE (clear entry) instead of CLX
        if (isdot) {
          if (stack[0] > TINYNUMBER && decimals > 0)
            stack[0] -= ((long)(stack[0] * _pow10(decimals)) % 10) / _pow10(decimals--);
          else isdot = false;
        }
        else stack[0] = (long)(stack[0] / 10);
      }
      else stack[0] = 0.0; // CLX
    }
    else { // CHS (no key==KH necessary)
      stack[0] = -stack[0];
      isnewnumber = true;
    }

    //Serial.print(stack[0]); Serial.print(" "); Serial.print(stack[1]); Serial.print(" "); Serial.println(stack[2]);
    printfloat();
    isshow = false;
  }
}

