%module overload_extendc

%inline %{
typedef struct Foo {
  int dummy;
} Foo;
%}

%extend Foo {
    int test(int x) { x = 0; return 1; }
    int test(char *s) { s = 0; return 2; }
    int test(double x, double y) { x = 0; y = 0; return 3; }
};


       

