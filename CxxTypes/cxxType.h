#ifndef _cxxType_h
#define _cxxType_h

#include "cxxTypes.h"

namespace _cxx_
{

class CvQualifiedType;

/**
 * Abstract interface to a C++ type representation.
 */
class Type
{
public:
  /**
   * Retrieve what kind of Type this is.
   */
  virtual RepresentationType GetRepresentationType() const = 0;
  
  virtual CvQualifiedType GetCvQualifiedType(bool, bool) const;

  virtual bool CanConvertTo(const CvQualifiedType&, bool, bool) const = 0;

protected:

  Type() {}
  virtual ~Type() {}
  
  /**
   * Let the CvQualifiedType class call CanConvertTo.
   */
  friend CvQualifiedType;
};


} // namespace _cxx_

#endif
