%module(ruby_minherit="1") using_extend

%warnfilter(813,833) FooBar;   // C#, Java multiple inheritance

%extend Foo {
     int blah(int x, int y) {
        return x+y;
     }
};

%extend Bar {
     double blah(double x, double y) {
        return x+y;
     }
};

%inline %{
class Foo {
public:
     int blah(int x) { return x; }
     char *blah(char *x) { return x; }
};

class Bar {
public:
     double blah(double x) { return x; }
};

class FooBar : public Foo, public Bar {
public:
     using Foo::blah;
     using Bar::blah;
     char *blah(char *x) { return x; }
};

%}

