//
// SWIG Typemap library
// Dave Beazley
// May 5, 1997
//
// Perl5 implementation
//
// This library provides standard typemaps for modifying SWIG's behavior.
// With enough entries in this file, I hope that very few people actually
// ever need to write a typemap.
//

/*
The SWIG typemap library provides a language independent mechanism for
supporting output arguments, input values, and other C function
calling mechanisms.  The primary use of the library is to provide a
better interface to certain C function--especially those involving
pointers.
*/

// INPUT typemaps.
// These remap a C pointer to be an "INPUT" value which is passed by value
// instead of reference.


/*
The following methods can be applied to turn a pointer into a simple
"input" value.  That is, instead of passing a pointer to an object,
you would use a real value instead.

         int            *INPUT
         short          *INPUT
         long           *INPUT
         unsigned int   *INPUT
         unsigned short *INPUT
         unsigned long  *INPUT
         unsigned char  *INPUT
         float          *INPUT
         double         *INPUT
         
To use these, suppose you had a C function like this :

        double fadd(double *a, double *b) {
               return *a+*b;
        }

You could wrap it with SWIG as follows :
        
        %include typemaps.i
        double fadd(double *INPUT, double *INPUT);

or you can use the %apply directive :

        %include typemaps.i
        %apply double *INPUT { double *a, double *b };
        double fadd(double *a, double *b);

*/

%define INPUT_TYPEMAP(type, converter) 
%typemap(in) type *INPUT(type temp), type &INPUT(type temp) {
  temp = (type) converter($input);
  $1 = &temp;
}
%typemap(typecheck) type *INPUT = type;
%typemap(typecheck) type &INPUT = type;
%enddef

INPUT_TYPEMAP(float, SvNV);
INPUT_TYPEMAP(double, SvNV);
INPUT_TYPEMAP(int, SvIV);
INPUT_TYPEMAP(long, SvIV);
INPUT_TYPEMAP(short, SvIV);
INPUT_TYPEMAP(signed char, SvIV);
INPUT_TYPEMAP(unsigned int, SvUV);
INPUT_TYPEMAP(unsigned long, SvUV);
INPUT_TYPEMAP(unsigned short, SvUV);
INPUT_TYPEMAP(unsigned char, SvUV);
INPUT_TYPEMAP(bool, SvIV);


#undef INPUT_TYPEMAP
                 
// OUTPUT typemaps.   These typemaps are used for parameters that
// are output only.   The output value is appended to the result as
// a list element.

/*
The following methods can be applied to turn a pointer into an "output"
value.  When calling a function, no input value would be given for
a parameter, but an output value would be returned.  In the case of
multiple output values, functions will return a Perl array.

         int            *OUTPUT
         short          *OUTPUT
         long           *OUTPUT
         unsigned int   *OUTPUT
         unsigned short *OUTPUT
         unsigned long  *OUTPUT
         unsigned char  *OUTPUT
         float          *OUTPUT
         double         *OUTPUT
         
For example, suppose you were trying to wrap the modf() function in the
C math library which splits x into integral and fractional parts (and
returns the integer part in one of its parameters).:

        double modf(double x, double *ip);

You could wrap it with SWIG as follows :

        %include typemaps.i
        double modf(double x, double *OUTPUT);

or you can use the %apply directive :

        %include typemaps.i
        %apply double *OUTPUT { double *ip };
        double modf(double x, double *ip);

The Perl output of the function would be an array containing both
output values. 

*/

// Force the argument to be ignored.

%typemap(in,numinputs=0) int            *OUTPUT(int temp),  int &OUTPUT(int temp),
                 short          *OUTPUT(short temp), short &OUTPUT(short temp),
                 long           *OUTPUT(long temp), long &OUTPUT(long temp),
                 unsigned int   *OUTPUT(unsigned int temp), unsigned int &OUTPUT(unsigned int temp),
                 unsigned short *OUTPUT(unsigned short temp), unsigned short &OUTPUT(unsigned short temp),
                 unsigned long  *OUTPUT(unsigned long temp), unsigned long &OUTPUT(unsigned long temp),
                 unsigned char  *OUTPUT(unsigned char temp), unsigned char &OUTPUT(unsigned char temp),
                 signed char    *OUTPUT(signed char temp), signed char &OUTPUT(signed char temp),
                 bool           *OUTPUT(bool temp), bool &OUTPUT(bool temp),
                 float          *OUTPUT(float temp), float &OUTPUT(float temp),
                 double         *OUTPUT(double temp), double &OUTPUT(double temp)
"$1 = &temp;";

%typemap(argout)  int            *OUTPUT, int &OUTPUT,
                  short          *OUTPUT, short &OUTPUT,
                  long           *OUTPUT, long &OUTPUT,
                  signed char    *OUTPUT, signed char &OUTPUT,
                  bool           *OUTPUT, bool &OUTPUT
{
  if (argvi >= items) {
    EXTEND(sp,1);
  }
  $result = sv_newmortal();
  sv_setiv($result,(IV) *($1));
  argvi++;
}

%typemap(argout)  unsigned int   *OUTPUT, unsigned int &OUTPUT,
                  unsigned short *OUTPUT, unsigned short &OUTPUT,
                  unsigned long  *OUTPUT, unsigned long &OUTPUT,
                  unsigned char  *OUTPUT, unsigned char &OUTPUT
{
  if (argvi >= items) {
    EXTEND(sp,1);
  }
  $result = sv_newmortal();
  sv_setuv($result,(UV) *($1));
  argvi++;
}



%typemap(argout) float    *OUTPUT, float &OUTPUT,
                 double   *OUTPUT, double &OUTPUT
{
  if (argvi >= items) {
    EXTEND(sp,1);
  }
  $result = sv_newmortal();
  sv_setnv($result,(double) *($1));
  argvi++;
}

// INOUT
// Mappings for an argument that is both an input and output
// parameter

/*
The following methods can be applied to make a function parameter both
an input and output value.  This combines the behavior of both the
"INPUT" and "OUTPUT" methods described earlier.  Output values are
returned in the form of a Perl array.

         int            *INOUT
         short          *INOUT
         long           *INOUT
         unsigned int   *INOUT
         unsigned short *INOUT
         unsigned long  *INOUT
         unsigned char  *INOUT
         float          *INOUT
         double         *INOUT
         
For example, suppose you were trying to wrap the following function :

        void neg(double *x) {
             *x = -(*x);
        }

You could wrap it with SWIG as follows :

        %include typemaps.i
        void neg(double *INOUT);

or you can use the %apply directive :

        %include typemaps.i
        %apply double *INOUT { double *x };
        void neg(double *x);

Unlike C, this mapping does not directly modify the input value.
Rather, the modified input value shows up as the return value of the
function.  Thus, to apply this function to a Perl variable you might
do this :

       $x = neg($x);

*/

%typemap(in) int *INOUT = int *INPUT;
%typemap(in) short *INOUT = short *INPUT;
%typemap(in) long *INOUT = long *INPUT;
%typemap(in) unsigned *INOUT = unsigned *INPUT;
%typemap(in) unsigned short *INOUT = unsigned short *INPUT;
%typemap(in) unsigned long *INOUT = unsigned long *INPUT;
%typemap(in) unsigned char *INOUT = unsigned char *INPUT;
%typemap(in) signed char *INOUT = signed char *INPUT;
%typemap(in) bool *INOUT = bool *INPUT;
%typemap(in) float *INOUT = float *INPUT;
%typemap(in) double *INOUT = double *INPUT;

%typemap(in) int &INOUT = int &INPUT;
%typemap(in) short &INOUT = short &INPUT;
%typemap(in) long &INOUT = long &INPUT;
%typemap(in) unsigned &INOUT = unsigned &INPUT;
%typemap(in) unsigned short &INOUT = unsigned short &INPUT;
%typemap(in) unsigned long &INOUT = unsigned long &INPUT;
%typemap(in) unsigned char &INOUT = unsigned char &INPUT;
%typemap(in) signed char &INOUT = signed char &INPUT;
%typemap(in) bool &INOUT = bool &INPUT;
%typemap(in) float &INOUT = float &INPUT;
%typemap(in) double &INOUT = double &INPUT;


%typemap(argout) int *INOUT = int *OUTPUT;
%typemap(argout) short *INOUT = short *OUTPUT;
%typemap(argout) long *INOUT = long *OUTPUT;
%typemap(argout) unsigned *INOUT = unsigned *OUTPUT;
%typemap(argout) unsigned short *INOUT = unsigned short *OUTPUT;
%typemap(argout) unsigned long *INOUT = unsigned long *OUTPUT;
%typemap(argout) unsigned char *INOUT = unsigned char *OUTPUT;
%typemap(argout) signed char *INOUT = signed char *OUTPUT;
%typemap(argout) bool *INOUT = bool *OUTPUT;
%typemap(argout) float *INOUT = float *OUTPUT;
%typemap(argout) double *INOUT = double *OUTPUT;

%typemap(argout) int &INOUT = int &OUTPUT;
%typemap(argout) short &INOUT = short &OUTPUT;
%typemap(argout) long &INOUT = long &OUTPUT;
%typemap(argout) unsigned &INOUT = unsigned &OUTPUT;
%typemap(argout) unsigned short &INOUT = unsigned short &OUTPUT;
%typemap(argout) unsigned long &INOUT = unsigned long &OUTPUT;
%typemap(argout) unsigned char &INOUT = unsigned char &OUTPUT;
%typemap(argout) signed char &INOUT = signed char &OUTPUT;
%typemap(argout) bool &INOUT = bool &OUTPUT;
%typemap(argout) float &INOUT = float &OUTPUT;
%typemap(argout) double &INOUT = double &OUTPUT;

// REFERENCE
// Accept Perl references as pointers

/*
The following methods make Perl references work like simple C
pointers.  References can only be used for simple input/output
values, not C arrays however.  It should also be noted that 
REFERENCES are specific to Perl and not supported in other
scripting languages at this time.

         int            *REFERENCE
         short          *REFERENCE
         long           *REFERENCE
         unsigned int   *REFERENCE
         unsigned short *REFERENCE
         unsigned long  *REFERENCE
         unsigned char  *REFERENCE
         float          *REFERENCE
         double         *REFERENCE
         
For example, suppose you were trying to wrap the following function :

        void neg(double *x) {
             *x = -(*x);
        }

You could wrap it with SWIG as follows :

        %include typemaps.i
        void neg(double *REFERENCE);

or you can use the %apply directive :

        %include typemaps.i
        %apply double *REFERENCE { double *x };
        void neg(double *x);

Unlike the INOUT mapping described previous, this approach directly
modifies the value of a Perl reference.  Thus, you could use it
as follows :

       $x = 3;
       neg(\$x);
       print "$x\n";         # Should print out -3.

*/

%typemap(in) double *REFERENCE (double dvalue), double &REFERENCE(double dvalue)
{
  SV *tempsv;
  if (!SvROK($input)) {
    SWIG_croak("expected a reference");
  }
  tempsv = SvRV($input);
  if ((!SvNOK(tempsv)) && (!SvIOK(tempsv))) {
	printf("Received %d\n", SvTYPE(tempsv));
	SWIG_croak("Expected a double reference.");
  }
  dvalue = SvNV(tempsv);
  $1 = &dvalue;
}

%typemap(in) float *REFERENCE (float dvalue), float &REFERENCE(float dvalue)
{
  SV *tempsv;
  if (!SvROK($input)) {
    SWIG_croak("expected a reference");
  }
  tempsv = SvRV($input);
  if ((!SvNOK(tempsv)) && (!SvIOK(tempsv))) {
    SWIG_croak("expected a double reference");
  }
  dvalue = (float) SvNV(tempsv);
  $1 = &dvalue;
}

%typemap(in) int *REFERENCE (int dvalue), int &REFERENCE (int dvalue)
{
  SV *tempsv;
  if (!SvROK($input)) {
    SWIG_croak("expected a reference");
  }
  tempsv = SvRV($input);
  if (!SvIOK(tempsv)) {
    SWIG_croak("expected a integer reference");
  }
  dvalue = SvIV(tempsv);
  $1 = &dvalue;
}

%typemap(in) short *REFERENCE (short dvalue), short &REFERENCE(short dvalue)
{
  SV *tempsv;
  if (!SvROK($input)) {
    SWIG_croak("expected a reference");
  }
  tempsv = SvRV($input);
  if (!SvIOK(tempsv)) {
    SWIG_croak("expected a integer reference");
  }
  dvalue = (short) SvIV(tempsv);
  $1 = &dvalue;
}
%typemap(in) long *REFERENCE (long dvalue), long &REFERENCE(long dvalue)
{
  SV *tempsv;
  if (!SvROK($input)) {
    SWIG_croak("expected a reference");
  }
  tempsv = SvRV($input);
  if (!SvIOK(tempsv)) {
    SWIG_croak("expected a integer reference");
  }
  dvalue = (long) SvIV(tempsv);
  $1 = &dvalue;
}
%typemap(in) unsigned int *REFERENCE (unsigned int dvalue), unsigned int &REFERENCE(unsigned int dvalue)
{
  SV *tempsv;
  if (!SvROK($input)) {
    SWIG_croak("expected a reference");
  }
  tempsv = SvRV($input);
  if (!SvIOK(tempsv)) {
    SWIG_croak("expected a integer reference");
  }
  dvalue = (unsigned int) SvUV(tempsv);
  $1 = &dvalue;
}
%typemap(in) unsigned short *REFERENCE (unsigned short dvalue), unsigned short &REFERENCE(unsigned short dvalue)
{
  SV *tempsv;
  if (!SvROK($input)) {
    SWIG_croak("expected a reference");
  }
  tempsv = SvRV($input);
  if (!SvIOK(tempsv)) {
    SWIG_croak("expected a integer reference");
  }
  dvalue = (unsigned short) SvUV(tempsv);
  $1 = &dvalue;
}
%typemap(in) unsigned long *REFERENCE (unsigned long dvalue), unsigned long &REFERENCE(unsigned long dvalue)
{
  SV *tempsv;
  if (!SvROK($input)) {
    SWIG_croak("expected a reference");
  }
  tempsv = SvRV($input);
  if (!SvIOK(tempsv)) {
    SWIG_croak("expected a integer reference");
  }
  dvalue = (unsigned long) SvUV(tempsv);
  $1 = &dvalue;
}

%typemap(in) unsigned char *REFERENCE (unsigned char dvalue), unsigned char &REFERENCE(unsigned char dvalue)
{
  SV *tempsv;
  if (!SvROK($input)) {
    SWIG_croak("expected a reference");
  }
  tempsv = SvRV($input);
  if (!SvIOK(tempsv)) {
    SWIG_croak("expected a integer reference");
  }
  dvalue = (unsigned char) SvUV(tempsv);
  $1 = &dvalue;
}

%typemap(in) signed char *REFERENCE (signed char dvalue), signed char &REFERENCE(signed char dvalue)
{
  SV *tempsv;
  if (!SvROK($input)) {
    SWIG_croak("expected a reference");
  }
  tempsv = SvRV($input);
  if (!SvIOK(tempsv)) {
    SWIG_croak("expected a integer reference");
  }
  dvalue = (signed char) SvIV(tempsv);
  $1 = &dvalue;
}

%typemap(in) bool *REFERENCE (bool dvalue), bool &REFERENCE(bool dvalue)
{
  SV *tempsv;
  if (!SvROK($input)) {
    SWIG_croak("expected a reference");
  }
  tempsv = SvRV($input);
  if (!SvIOK(tempsv)) {
    SWIG_croak("expected a integer reference");
  }
  dvalue = (bool) SvIV(tempsv);
  $1 = &dvalue;
}

%typemap(argout) double *REFERENCE, double &REFERENCE,
                 float  *REFERENCE, float &REFERENCE
{
  SV *tempsv;
  tempsv = SvRV($arg);
  sv_setnv(tempsv, (double) *$1);
}

%typemap(argout)       int            *REFERENCE, int &REFERENCE,
                       short          *REFERENCE, short &REFERENCE,
                       long           *REFERENCE, long  &REFERENCE,
                       signed char    *REFERENCE, unsigned char &REFERENCE,
                       bool           *REFERENCE, bool &REFERENCE
{
  SV *tempsv;
  tempsv = SvRV($input);
  sv_setiv(tempsv, (IV) *$1);
}

%typemap(argout)       unsigned int   *REFERENCE, unsigned int &REFERENCE,
                       unsigned short *REFERENCE, unsigned short &REFERENCE,
                       unsigned long  *REFERENCE, unsigned long &REFERENCE,
                       unsigned char  *REFERENCE, unsigned char &REFERENCE
{
  SV *tempsv;
  tempsv = SvRV($input);
  sv_setuv(tempsv, (UV) *$1);
}

/* Overloading information */

%typemap(typecheck) double *INOUT = double;
%typemap(typecheck) bool *INOUT = bool;
%typemap(typecheck) signed char *INOUT = signed char;
%typemap(typecheck) unsigned char *INOUT = unsigned char;
%typemap(typecheck) unsigned long *INOUT = unsigned long;
%typemap(typecheck) unsigned short *INOUT = unsigned short;
%typemap(typecheck) unsigned int *INOUT = unsigned int;
%typemap(typecheck) long *INOUT = long;
%typemap(typecheck) short *INOUT = short;
%typemap(typecheck) int *INOUT = int;
%typemap(typecheck) float *INOUT = float;
