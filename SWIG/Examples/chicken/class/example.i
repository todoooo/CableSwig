/* File : example.i */
%module example

%{
#include "example.h"
%}

/* Let "Shape" objects be converted back and forth from TinyCLOS into
   low-level CHICKEN SWIG procedures */

%typemap(clos_in) Shape * = SIMPLE_CLOS_OBJECT *;
%typemap(clos_out) Shape * = SIMPLE_CLOS_OBJECT *;

/* Let's just grab the original header file here */
%include "example.h"

