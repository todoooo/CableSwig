#ifndef _cxxCvQualifiedType_h
#define _cxxCvQualifiedType_h

#include "cxxType.h"

#include <list>

namespace _cxx_
{


/**
 * Hold cv-qualifiers attached to a type.
 */
class CvQualifiedType
{
public:
  typedef CvQualifiedType Self;
  
  CvQualifiedType(const Type*);
  CvQualifiedType(const Type*, bool, bool);
  CvQualifiedType(const Self&);
  
  bool IsConst() const    { return m_Const; }
  bool IsVolatile() const { return m_Volatile; }
  
  const Type* GetType() const { return m_Type; }
  
  bool CanConvertTo(const Self&) const;

  bool operator== (const Self&) const;
  bool operator< (const Self&) const;
  
private:
  /**
   * The type to which these cv-qualifiers apply.
   */
  const Type* const m_Type;
  
  /**
   * Flag for presence of "const" cv-qualifier.
   */
  bool m_Const;

  /**
   * Flag for presence of "volatile" cv-qualifier.
   */
  bool m_Volatile;
};

/**
 * A list of cv-qualified types.
 */
typedef std::list<CvQualifiedType>  CvQualifiedTypeList;

} // namespace _cxx_


#endif
