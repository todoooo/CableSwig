TOP        = ../..
SWIG       = $(TOP)/../swig
SWIGOPT    = -noruntime
RUNTIMEDIR = $(TOP)/../Runtime/.libs

all::
	$(MAKE) -f $(TOP)/Makefile SWIG='$(SWIG)' SWIGOPT='$(SWIGOPT)' \
	RUNTIMEDIR='$(RUNTIMEDIR)' TARGET='base' INTERFACE='base.i' tcl_multi_cpp
	$(MAKE) -f $(TOP)/Makefile SWIG='$(SWIG)' SWIGOPT='$(SWIGOPT)' \
	RUNTIMEDIR='$(RUNTIMEDIR)' TARGET='foo' INTERFACE='foo.i' tcl_multi_cpp
	$(MAKE) -f $(TOP)/Makefile SWIG='$(SWIG)' SWIGOPT='$(SWIGOPT)' \
	RUNTIMEDIR='$(RUNTIMEDIR)' TARGET='bar' INTERFACE='bar.i' tcl_multi_cpp
	$(MAKE) -f $(TOP)/Makefile SWIG='$(SWIG)' SWIGOPT='$(SWIGOPT)' \
	RUNTIMEDIR='$(RUNTIMEDIR)' TARGET='spam' INTERFACE='spam.i' tcl_multi_cpp


clean::
	$(MAKE) -f $(TOP)/Makefile tcl_clean

check: all
