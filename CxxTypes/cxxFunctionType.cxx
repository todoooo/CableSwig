/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    cxxFunctionType.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$


  Copyright (c) 2000 National Library of Medicine
  All rights reserved.

  See COPYRIGHT.txt for copyright details.

=========================================================================*/
#include "cxxTypes.h"

namespace _cxx_
{

/**
 * Retrieve what kind of Type this is.
 */
RepresentationType
FunctionType
::GetRepresentationType() const
{
  return FunctionType_id;
}


/**
 * Constructor takes the return type of the function.
 */
FunctionType
::FunctionType(const CvQualifiedType& in_type):
  m_ReturnType(in_type)
{
}


} // namespace _cxx_
