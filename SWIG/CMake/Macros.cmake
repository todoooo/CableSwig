MACRO(BUILD_RUNTIME LANGUAGE RUNTIME_NAME LANGUAGE_RUNTIME RUNTIME_LINK_LIBRARIES)
  # Prepare output directory 
  SET(__temp_EXECUTABLE_OUTPUT_PATH "${EXECUTABLE_OUTPUT_PATH}")
  SET(__temp_LIBRARY_OUTPUT_PATH "${LIBRARY_OUTPUT_PATH}")
  SET(EXECUTABLE_OUTPUT_PATH "${SWIG_BINARY_DIR}/Runtime")
  SET(LIBRARY_OUTPUT_PATH "${SWIG_BINARY_DIR}/Runtime")

  SET(RUNTIME_SRC "${SWIG_BINARY_DIR}/Runtime/lib${RUNTIME_NAME}.c")

  ADD_DEFINITIONS(-DSWIG_GLOBAL -DHAVE_CONFIG_H)
  SET(SWIG_CONTENT "")
  FOREACH(file ${LANGUAGE_RUNTIME})
    FILE(READ "${SWIG_SOURCE_DIR}/Lib/${file}" LANGUAGE_RUNTIME_SWIG)
    SET(SWIG_CONTENT "${SWIG_CONTENT}${LANGUAGE_RUNTIME_SWIG}")
  ENDFOREACH(file ${LANGUAGE_RUNTIME})
  CONFIGURE_FILE("${SWIG_SOURCE_DIR}/Runtime/empty.c.in" "${RUNTIME_SRC}" @ONLY)
  SET_SOURCE_FILES_PROPERTIES(${RUNTIME_SRC} PROPERTIES GENERATED 1)
  ADD_LIBRARY(swig${RUNTIME_NAME} SHARED "${RUNTIME_SRC}")
  TARGET_LINK_LIBRARIES(swig${RUNTIME_NAME} "${RUNTIME_LINK_LIBRARIES}")

  # Test if ok
  #ADD_EXECUTABLE(tmp ${SWIG_SOURCE_DIR}/Runtime/tmp.c)
  #TARGET_LINK_LIBRARIES(tmp swig${RUNTIME_NAME})

  # Cleanup
  SET(${EXECUTABLE_OUTPUT_PATH} "${__temp_EXECUTABLE_OUTPUT_PATH}")
  SET(${LIBRARY_OUTPUT_PATH} "${__temp_LIBRARY_OUTPUT_PATH}")
ENDMACRO(BUILD_RUNTIME)

