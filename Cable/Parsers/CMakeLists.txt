INCLUDE_DIRECTORIES(${CABLE_SOURCE_DIR}/CxxTypes)
ADD_DEFINITIONS(-D_cxx_STATIC)

ADD_LIBRARY(CableParsers STATIC
  cableArrayType.cxx
  cableClass.cxx
  cableClassType.cxx
  cableConstructor.cxx
  cableContext.cxx
  cableConverter.cxx
  cableDestructor.cxx
  cableEnumeration.cxx
  cableEnumerationType.cxx
  cableField.cxx
  cableFunction.cxx
  cableFunctionType.cxx 
  cableFundamentalType.cxx
  cableIndent.cxx
  cableMethod.cxx
  cableMethodType.cxx
  cableNamed.cxx
  cableNamespace.cxx
  cableNamespaceAlias.cxx
  cableObject.cxx
  cableOffsetType.cxx
  cableOperatorFunction.cxx
  cableOperatorMethod.cxx
  cablePointerType.cxx
  cableReferenceType.cxx
  cableSourceObject.cxx
  cableSourceRepresentation.cxx
  cableStruct.cxx
  cableType.cxx
  cableTypedef.cxx
  cableUnion.cxx
  cableVariable.cxx
  cableXMLParser.cxx
  cableXMLSourceElement.cxx
  cableXMLSourceParser.cxx
)

TARGET_LINK_LIBRARIES(CableParsers CxxTypes CableExpat)