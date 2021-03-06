%module preproc

/* This interface file tests whether SWIG's extended C
   preprocessor is working right. 

   In this example, SWIG 1.3.6 chokes on "//" in a #define with a
   syntax error.
*/

#define SLASHSLASH "//"

/* This SWIG -*- c -*- interface is to test for some strange
   preprocessor bug.

   I get syntax errors unless I remove the apostrophe in the comment
   or the sharp-sign substitution.  (The apostrophe seems to disable
   sharp-sign substitution.)
*/


%define TYPEMAP_LIST_VECTOR_INPUT_OUTPUT(SCM_TYPE)

     /* Don't check for NULL pointers (override checks). */

     %typemap(argout, doc="($arg <vector of <" #SCM_TYPE ">>)") 
          int *VECTORLENOUTPUT
     {
     }

%enddef

TYPEMAP_LIST_VECTOR_INPUT_OUTPUT(boolean)

// preproc_3

#define Sum( A, B, \
             C)    \
        A + B + C 


// preproc_4
%{
  int hello0()
  {
    return 0;
  }

  int hello1()
  {
    return 1;
  }

  int hello2()
  {
    return 2;
  }  
  int f(int min) { return min; }
%}

#define ARITH_RTYPE(A1, A2) A2

#define HELLO_TYPE(A, B) ARITH_RTYPE(A, ARITH_RTYPE(A,B))

//
// These two work fine
//
int hello0();
ARITH_RTYPE(double,int) hello1();


//
// This doesn't work with 1.3.17+ ( but it was ok in 1.3.16 )
// it gets expanded as (using -E)
// 
//   ARITH_RTYPE(double,int) hello2();
//
HELLO_TYPE(double,int) hello2();

#define min(x,y) ((x) < (y)) ? (x) : (y) 
int f(int min);

// preproc_5

%warnfilter(801) a5;	// Ruby, wrong constant name
%warnfilter(801) b5;	// Ruby, wrong constant name
%warnfilter(801) c5;	// Ruby, wrong constant name
%warnfilter(801) d5;	// Ruby, wrong constant name

// Various preprocessor bits of nastiness.


/* Test argument name substitution */
#define foo(x,xx) #x #xx
#define bar(x,xx) x + xx

%constant char *a5 = foo(hello,world);
%constant int   b5 = bar(3,4);

// Wrap your brain around this one ;-)

%{
#define cat(x,y) x ## y
%}

#define cat(x,y) x ## y

/* This should expand to cat(1,2);  
   See K&R, p. 231 */

%constant int c5 = cat(cat(1,2),;)

#define xcat(x,y) cat(x,y)

/* This expands to 123.  See K&R, p. 231 */
%constant int d5 = xcat(xcat(1,2),3);


#define C1\
"hello"

#define C2
#define C3 C2

#define ALONG_\
NAME 42

#define C4"Hello"

// preproc_6

%warnfilter(801) a6; /* Ruby, wrong constant name */
%warnfilter(801) b6; /* Ruby, wrong constant name */
%warnfilter(801) c6; /* Ruby, wrong constant name */
%warnfilter(801) d6; /* Ruby, wrong constant name */

#define add(a, b) (a + b)
#define times(a, b) (a * b)
#define op(x) x(1, 5)
 
/* expand to (1 + 5) */
%constant int a6 = op(add);
/* expand to (1 * 5) */
%constant int b6 = op(times);
/* expand to ((1 + 5) * 5) */
%constant int c6 = times(add(1, 5), 5);
/* expand to ((1 + 5) * 5) */
%constant int d6 = times(op(add), 5);                 

/* This interface file tests whether SWIG's extended C
   preprocessor is working right. 

   In this example, SWIG 1.3a5 reports missing macro arguments, which
   is bogus.
*/

%define MACRO1(C_TYPE, GETLENGTH)
     /* nothing */
%enddef

%define MACRO2(XYZZY)
  MACRO1(XYZZY, 1)
%enddef

MACRO2(int)

// cpp_macro_noarg.  Tests to make sure macros with no arguments work right.
#define MACROWITHARG(x) something(x) 

typedef int MACROWITHARG; 

/* 
This testcase tests for embedded defines and embedded %constants
*/

%inline %{

typedef struct EmbeddedDefines {
  int dummy;
#define  EMBEDDED_DEFINE 44
#ifdef SWIG
%constant EMBEDDED_SWIG_CONSTANT = 55;
#endif
} EmbeddedDefines;

%}

/* 
This testcase tests operators for defines
*/

#define A1   1 + 2
#define A2   3 - 4
#define A3   5 * 6
#define A4   7 / 8
#define A5   9 >> 10
#define A6   11 << 12
#define A7   13 & 14
#define A8   15 | 16
#define A9   17 ^ 18
#define A10  19 && 20
#define A11  21 || 21
#define A12  ~22
#define A13  !23




