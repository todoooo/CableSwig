#
# This module finds if PERL is installed and determines where the include files
# and libraries are. It also determines what the name of the library is. This
# code sets the following variables:
#
#  PERL_INCLUDE_PATH       = path to where object.h can be found
#  PERL_EXECUTABLE         = full path to the perl binary
#

SET(PERL_POSSIBLE_INCLUDE_PATHS
  /usr/lib/perl/5.8.0/CORE
  /usr/lib/perl/5.8.2/CORE
  )

SET(PERL_POSSIBLE_LIB_PATHS
  /usr/lib
  )

FIND_PATH(PERL_INCLUDE_PATH perl.h
  ${PERL_POSSIBLE_INCLUDE_PATHS})


FIND_PROGRAM(PERL_EXECUTABLE
  NAMES perl
  PATHS
  /usr/bin
  /usr/local/bin
  )

IF(PERL_EXECUTABLE)
  EXEC_PROGRAM(${PERL_EXECUTABLE}
    ARGS -e "'use Config; print $Config{libperl}, \"\\n\"'"
    OUTPUT_VARIABLE PERL_LIBRARY_OUTPUT_VARIABLE
    RETURN_VALUE PERL_LIBRARY_RETURN_VALUE
    )
  IF(NOT PERL_LIBRARY_RETURN_VALUE)
    FOREACH(path ${PERL_POSSIBLE_LIB_PATHS})
      SET(PERL_POSSIBLE_LIBRARY_NAME ${PERL_POSSIBLE_LIBRARY_NAME} "${path}/${PERL_LIBRARY_OUTPUT_VARIABLE}")
    ENDFOREACH(path ${PERL_POSSIBLE_LIB_PATHS})
  ENDIF(NOT PERL_LIBRARY_RETURN_VALUE)
ENDIF(PERL_EXECUTABLE)

FIND_LIBRARY(PERL_LIBRARY
  NAMES ${PERL_POSSIBLE_LIBRARY_NAME} perl5.8.0
  PATHS ${PERL_POSSIBLE_LIB_PATHS}
  )

MARK_AS_ADVANCED(
  PERL_INCLUDE_PATH
  PERL_EXECUTABLE
  PERL_LIBRARY
  )
