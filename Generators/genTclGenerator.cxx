/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    genTclGenerator.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

Copyright (c) 2001 Insight Consortium
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

 * The name of the Insight Consortium, nor the names of any consortium members,
   nor of any contributors, may be used to endorse or promote products derived
   from this software without specific prior written permission.

  * Modified source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS ``AS IS''
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=========================================================================*/
#include "genTclGenerator.h"
#include "cableVersion.h"

#include <iostream>
#include <fstream>

namespace gen
{

typedef configuration::CableConfiguration CableConfiguration;

/**
 * Construct an instance of this generator and return it.
 */
GeneratorBase* TclGenerator::GetInstance(const configuration::CableConfiguration* in_cableConfiguration,
                                         const source::Namespace* in_globalNamespace,
                                         std::ostream& output)
{
  return new TclGenerator(in_cableConfiguration, in_globalNamespace, output);
}


/**
 * Constructor.
 */
TclGenerator::TclGenerator(
  const configuration::CableConfiguration* in_cableConfiguration,
  const source::Namespace* in_globalNamespace,
  std::ostream& output):
  GeneratorBase(in_cableConfiguration, in_globalNamespace, output)
{
}


/**
 * Generate 
 */
void TclGenerator::Generate()
{
  m_Output <<
    "/**\n"
    " * Automatically generated by CABLE.  Do not edit!\n"
    " */\n"
    "\n"
    "#include \"WrapTclFacility/wrapCalls.h\"\n"
    "\n"
    "#define _wrap_WRAPPER_GROUP_NAME \"" << m_CableConfiguration->GetGroupName().c_str() << "\"\n"
    "\n";
  
  if(m_GlobalNamespace)
    {
    this->GenerateWrappers();
    }
  
  if((m_CableConfiguration->GetPackageName() != "")
     && (m_CableConfiguration->GroupsBegin() != m_CableConfiguration->GroupsEnd()))
    {
    this->GeneratePackageInitializer();
    }
}


/**
 * Generate all Tcl wrappers specified in the configuration.
 */
void TclGenerator::GenerateWrappers()
{
  m_Output <<
    "// Include headers needed for wrapped types.\n";
  
  for(configuration::CableConfiguration::Headers::const_iterator
        h = m_CableConfiguration->HeadersBegin();
      h != m_CableConfiguration->HeadersEnd(); ++h)
    {
    m_Output << "#include \"" << h->c_str() << "\"\n";
    }
  
  // Add all types that will be used by the wrappers to our CvTypeGenerator.
  this->FindCvTypes(m_CableConfiguration->GetGlobalNamespace());

  // Write out the block of CvType specializations for all needed types.
  m_Output <<
    "\n"
    "namespace _wrap_\n"
    "{\n"
    "namespace\n"
    "{\n"
    "// Define a CvType specialization for each type used in these wrappers.\n";
  m_CvTypeGenerator.GenerateClasses(m_Output);
  m_Output <<
    "\n";
  m_CvTypeGenerator.GenerateDataDeclarations(m_Output);
  this->WriteReturnEnumClasses();
  m_Output <<
    "} // anonymous namespace\n"
    "} // namespace _wrap_\n"
    "\n";
  
  // Begin the recursive generation at the configuration's global namespace.
  this->GenerateNamespace(m_CableConfiguration->GetGlobalNamespace());

  // Write out the block of CvType initializations.
  m_Output <<
    "namespace _wrap_\n"
    "{\n"
    "\n"
    "namespace\n"
    "{\n"
    "void InitializeGroupTypeRepresentations()\n"
    "{\n";
  m_CvTypeGenerator.GenerateInitializations(m_Output);  
  m_Output <<
    "}\n"
    "void InitializeGroupTypeHandlers(WrapperFacility* wrapperFacility)\n"
    "{\n";
  this->WriteConversionInititialization();
  m_Output <<
    "}\n"
    "} // anonymous namespace\n"
    "\n"
    "void Wrapper_" << m_CableConfiguration->GetGroupName().c_str() << "_Initialize(Tcl_Interp* interp)\n"
    "{\n"
    "  WrapperFacility* wrapperFacility = WrapperFacility::GetForInterpreter(interp);\n"
    "  InitializeGroupForFacility(wrapperFacility);\n";
  for(WrapperList::const_iterator w = m_WrapperList.begin();
      w != m_WrapperList.end(); ++w)
    {
    m_Output <<
      "  new Wrapper< " << w->c_str() << " >(wrapperFacility);\n";
    }
  m_Output <<
    "}\n"
    "\n"
    "} // namespace _wrap_\n"
    "\n";
}


/**
 * Generate
 */
void TclGenerator::GeneratePackageInitializer()
{
  String packageName = m_CableConfiguration->GetPackageName();
  String tclName = packageName;
  String::iterator c = tclName.begin();
  if(c != tclName.end())
    {
    char ch = *c;
    if((ch >= 'a') && (ch <= 'z'))
      {
      *c = (ch+('A'-'a'));
      }
    for(++c; c != tclName.end(); ++c)
      {
      char ch = *c;
      if((ch >= 'A') && (ch <= 'Z'))
        {
        *c = (ch+('a'-'A'));
        }
      }
    }

  m_Output <<
    "namespace _wrap_\n"
    "{\n";
  
  for(configuration::CableConfiguration::Groups::const_iterator
        g = m_CableConfiguration->GroupsBegin();
      g != m_CableConfiguration->GroupsEnd(); ++g)
    {
    m_Output <<
      "extern void Wrapper_" << g->c_str() << "_Initialize(Tcl_Interp*);\n";
    }
    
  m_Output <<
    "} // namespace _wrap_\n"
    "\n";

  
  m_Output <<
    "extern \"C\" { WRAPPER_EXPORT int " << tclName.c_str() << "_Init(Tcl_Interp* interp); }\n"
    "\n"
    "int " << tclName.c_str() << "_Init(Tcl_Interp* interp)\n"
    "{\n"
    "  // Make sure a WrapperFacility has been initialized for this interpreter.\n"
    "  if(!Tcl_PkgRequire(interp, \"WrapTclFacility\", \"" CABLE_VERSION_STRING "\", 1))\n"
    "    {\n"
    "    return TCL_ERROR;\n"
    "    }\n"
    "  \n"
    "  // Initialize the types in this package.\n";
  
  for(configuration::CableConfiguration::Groups::const_iterator
        g = m_CableConfiguration->GroupsBegin();
      g != m_CableConfiguration->GroupsEnd(); ++g)
    {
    m_Output <<
      "  _wrap_::Wrapper_" << g->c_str() << "_Initialize(interp);\n";
    }
  
  m_Output <<
    "  \n"
    "  return TCL_OK;\n"
    "}\n"
    "\n";
}

/**
 * Generate Tcl wrappers for the given namespace and its nested namespaces.
 */
void TclGenerator::GenerateNamespace(const configuration::Namespace* ns)
{
  // Look through all the wrapper entries in this namespace.
  for(configuration::Namespace::Wrappers::const_iterator w =
        ns->WrappersBegin(); w != ns->WrappersEnd(); ++w)
    {
    String qualifedName = ns->GetQualifiedName()+"::"+(*w)->GetName();
    source::Class* c = m_GlobalNamespace->LookupClass(qualifedName);
    if(c)
      {
      this->GenerateClassWrapper(c, *w);
      }
    else
      {
      m_Output << "// Couldn't find type " << qualifedName.c_str() << std::endl;
      }
    }
  
  // Loop through all the nested namespaces.
  for(configuration::Namespace::Fields::const_iterator f =
        ns->FieldsBegin(); f != ns->FieldsEnd(); ++f)
    {
    const configuration::Named* n = f->second;
    if(n->IsNamespace())
      {
      const configuration::Namespace* nns =
        dynamic_cast<const configuration::Namespace*>(n);
      this->GenerateNamespace(nns);
      }
    }
}


void
TclGenerator
::GenerateClassWrapper(const source::Class* c,
                       const configuration::Class* classConfig)
{
  String cName = c->GetQualifiedName();
  String noTemplate = cName.substr(0, cName.find_first_of("<"));
  String constructorName = noTemplate.substr(noTemplate.find_last_of(":") + 1);
  
  m_WrapperList.push_back(cName);
  
  // Make a list of the methods in this class that will be wrapped.
  Methods methods;
  Methods converters;
  for(source::MethodsIterator methodItr = c->GetMethods().begin();
      methodItr != c->GetMethods().end(); ++methodItr)
    {
    source::Method* method = *methodItr;
    if(method->GetAccess() == source::Public)
      {
      if(method->IsMethod()
         || (method->IsConstructor() && !c->IsAbstract())
         || method->IsOperatorMethod())
        {
        unsigned int argumentCount = method->GetArgumentCount();
        unsigned int defaultArgumentCount = method->GetDefaultArgumentCount();
        for(unsigned int i=0; i <= defaultArgumentCount; ++i)
          {
          methods.push_back(MethodEntry(method, argumentCount-i));
          }
        
        if(!this->ReturnsVoid(method))
          {
          // If the return type is a class type (not a pointer or
          // reference to it), then it will be a temporary and must
          // have a public destructor.
          cxx::CvQualifiedType cxxType = this->GetCxxType(method->GetReturns()->GetType());
          if(cxxType.IsClassType())
            {
            m_ClassesThatNeedDestructor.insert(cxx::ClassType::SafeDownCast(cxxType.GetType()));
            }
          }
        }
      
      // If the method is a converter, add it to a separate list.
      if(method->IsConverter())
        {
        converters.push_back(MethodEntry(method, method->GetArgumentCount()));
        }
      
      // If there is a public destructor available, we want to register it.
      if(method->IsDestructor())
        {
        cxx::CvQualifiedType classCvType = c->GetCxxClassType(m_GlobalNamespace);
        m_ClassesThatNeedDestructor.insert(cxx::ClassType::SafeDownCast(classCvType.GetType()));
        }
      }
    }
  
  // Write out code to define the Wrapper template specialization
  // for this wrapped class.
  this->WriteWrapperClassDefinition(c, methods, classConfig);
  
  m_Output <<
    "namespace _wrap_\n"
    "{\n"
    "\n";

  for(unsigned int m = 0 ; m < methods.size() ; ++m)
    {
    this->WriteMethodComment(cName, methods[m]);
    if(methods[m]->IsMethod())
      {
      if(methods[m]->IsStatic())
        {
        m_Output <<
          "void\n"
          "Wrapper< " << cName.c_str() << " >\n"
          "::StaticMethod_" << m << "_" << methods[m]->GetName().c_str()
                 << "(const WrapperFacility* wrapperFacility, const Arguments& arguments)\n"
          "{\n";
        
        this->WriteReturnBegin(methods[m]);
        
        m_Output <<
          "  " << cName.c_str() << "::" << methods[m]->GetName() << "(";
        
        this->WriteArgumentList(methods[m]->GetArguments(), 1, methods[m].GetArgumentCount());
        
        this->WriteReturnEnd(methods[m]);
        
        m_Output << ");\n"
          "}\n"
          "\n";
        }
      else
        {
        m_Output <<
          "void\n"
          "Wrapper< " << cName.c_str() << " >\n"
          "::Method_" << m << "_" << methods[m]->GetName().c_str()
                 << "(const WrapperFacility* wrapperFacility, const Arguments& arguments)\n"
          "{\n";
        
        this->WriteImplicitArgument(c, methods[m]);
        this->WriteReturnBegin(methods[m]);
        
        m_Output <<
          "  instance." << methods[m]->GetName() << "(";
        
        this->WriteArgumentList(methods[m]->GetArguments(), 1, methods[m].GetArgumentCount());
        
        this->WriteReturnEnd(methods[m]);
        
        m_Output << ");\n"
          "}\n"
          "\n";
        }
      }
    else if(methods[m]->IsOperatorMethod())
      {
      if(methods[m]->IsStatic())
        {
        m_Output <<
          "void\n"
          "Wrapper< " << cName.c_str() << " >\n"
          "::StaticOperator_" << m << "_" << this->GetOperatorName(methods[m]->GetName()).c_str()
                 << "(const WrapperFacility* wrapperFacility, const Arguments& arguments)\n"
          "{\n";
        
        this->WriteReturnBegin(methods[m]);
        
        m_Output <<
          "  " << cName.c_str() << "::operator " << methods[m]->GetName() << "(";
        
        this->WriteArgumentList(methods[m]->GetArguments(), 1, methods[m].GetArgumentCount());
        
        this->WriteReturnEnd(methods[m]);
        
        m_Output << ");\n"
          "}\n"
          "\n";
        }
      else
        {
        m_Output <<
          "void\n"
          "Wrapper< " << cName.c_str() << " >\n"
          "::Operator_" << m << "_" << this->GetOperatorName(methods[m]->GetName()).c_str()
                 << "(const WrapperFacility* wrapperFacility, const Arguments& arguments)\n"
          "{\n";
      
        this->WriteImplicitArgument(c, methods[m]);
        this->WriteReturnBegin(methods[m]);
      
        m_Output <<
          "  instance.operator " << methods[m]->GetName() << "(";
      
        this->WriteArgumentList(methods[m]->GetArguments(), 1, methods[m].GetArgumentCount());

        this->WriteReturnEnd(methods[m]);
      
        m_Output << ");\n"
          "}\n"
          "\n";
        }
      }
    else if(methods[m]->IsConstructor())
      {
      m_Output <<
        "void\n"
        "Wrapper< " << cName.c_str() << " >\n"
        "::Constructor_" << m << "(const WrapperFacility* wrapperFacility, const Arguments& arguments)\n"
        "{\n"
        "  Return< " << cName.c_str() << " >::FromConstructor(\n"
        "    new " << cName.c_str() << "(";
      
      this->WriteArgumentList(methods[m]->GetArguments(), 0, methods[m].GetArgumentCount());
      
      m_Output << "), wrapperFacility);\n"
        "}\n"
        "\n";
      }
    }
  
  m_Output <<
    "\n"
    "void\n"
    "Wrapper< " << cName.c_str() << " >\n"
    "::RegisterMethodWrappers()\n"
    "{\n";

  for(unsigned int m = 0 ; m < methods.size() ; ++m)
    {
      this->WriteMethodRegistration(constructorName, methods[m], m);
    }
  for(unsigned int m = 0 ; m < converters.size() ; ++m)
    {
    this->WriteConverterRegistration(cName, converters[m]);
    }
  this->WriteEnumValueRegistration();
  
  m_Output <<
    "}\n"
    "\n";

  m_Output <<
    "} // namespace _wrap_\n"
    "\n";
}

bool TclGenerator::ReturnsVoid(const source::Function* f) const
{      
  if(!f->GetReturns() || !f->GetReturns()->GetType())
    { return true; }
  else
    {
    const cxx::Type* rt = this->GetCxxType(f->GetReturns()->GetType()).GetType();
    if(rt->IsFundamentalType())
      {
      return cxx::FundamentalType::SafeDownCast(rt)->IsVoid();
      }
    }
  return false;
}


void
TclGenerator
::WriteConverterRegistration(const String& cName,
                             const MethodEntry& method) const
{
  String sourceType = cName;
  String returnTypeName = "void";
  if(method->GetReturns() && method->GetReturns()->GetType())
    {
    returnTypeName = this->GetCxxType(method->GetReturns()->GetType()).GetName();
    }
  if(method->IsConst())
    {
    sourceType = "const "+sourceType;
    }
  m_Output <<
    "  m_WrapperFacility->SetConversion(\n"
    "    CvType< " << sourceType.c_str() << " >::type,\n"
    "    CvType< " << returnTypeName.c_str() << " >::type.GetType(),\n"
    "    Converter::ConversionOperator< " << sourceType.c_str() << ", \n" <<
    "                                   " << returnTypeName.c_str() << " >::GetConversionFunction());\n";
}


/**
 * Write code that generates enumeration object instances for each
 * value of the enumeration types used by the wrappers.
 */
void
TclGenerator
::WriteEnumValueRegistration() const
{
  for(EnumTypesThatNeedValues::const_iterator
        e = m_EnumTypesThatNeedValues.begin();
      e != m_EnumTypesThatNeedValues.end(); ++e)
    {
    const source::Enumeration* enumeration = *e;
    String typeName = enumeration->GetQualifiedName();
    String prefix = enumeration->GetContext()->GetQualifiedName();
    for(source::Enumeration::Values::const_iterator
          v = enumeration->ValuesBegin();
        v != enumeration->ValuesEnd(); ++v)
      {
      String value = prefix+"::"+*v;
      m_Output <<
        "  m_WrapperFacility->SetEnumerationConstant(\n" <<
        "    \"" << value.c_str() << "\", new " << typeName.c_str() << "(" << value.c_str() << "),\n"
        "    CvType< " << typeName.c_str() << " >::type.GetType());\n";
      }
    }
}


void
TclGenerator
::WriteMethodRegistration(const String& constructorName,
                          const MethodEntry& method, unsigned int m) const
{
  if(!method->GetArguments().empty())
    {
    m_Output <<
      "  {\n"
      "  Method::ParameterTypes parameterTypes;\n";
    unsigned int count=0;
    for(source::ArgumentsIterator a = method->GetArguments().begin();
        ((count++ < method.GetArgumentCount())
         && (a != method->GetArguments().end())); ++a)
      {
      const source::Type* t = (*a)->GetType();
      m_Output <<
        "  parameterTypes.push_back(CvType< " << this->GetCxxType(t).GetName() << " >::type.GetType());\n";
      }
    }
  
  m_Output <<
    "  this->AddFunction(\n";
  
  if(method->IsMethod())
    {
    String returnTypeName = "void";
    if(method->GetReturns() && method->GetReturns()->GetType())
      {
      returnTypeName = this->GetCxxType(method->GetReturns()->GetType()).GetName();
      }
    if(method->IsStatic())
      {
      m_Output <<
        "    new StaticMethod(this, &Wrapper::StaticMethod_" << m << "_" << method->GetName() << ",\n"
        "                     \"" << method->GetName() << "\", false,\n"
        "                     CvType< " << returnTypeName.c_str() << " >::type";
      }
    else
      {
      m_Output <<
        "    new Method(this, &Wrapper::Method_" << m << "_" << method->GetName() << ",\n"
        "               \"" << method->GetName() << "\", " << (method->IsConst() ? "true":"false") << ", false,\n"
        "               CvType< " << returnTypeName.c_str() << " >::type";
      }
    }
  else if(method->IsOperatorMethod())
    {
    String returnTypeName = "void";
    if(method->GetReturns() && method->GetReturns()->GetType())
      {
      returnTypeName = this->GetCxxType(method->GetReturns()->GetType()).GetName();
      }
    if(method->IsStatic())
      {
      m_Output <<
        "    new StaticMethod(this, &Wrapper::StaticOperator_" << m << "_" << this->GetOperatorName(method->GetName()).c_str() << ",\n"
        "                     \"" << method->GetName() << "\", true,\n"
        "                     CvType< " << returnTypeName.c_str() << " >::type";
      }
    else
      {
      m_Output <<
        "    new Method(this, &Wrapper::Operator_" << m << "_" << this->GetOperatorName(method->GetName()).c_str() << ",\n"
        "               \"" << method->GetName() << "\", " << (method->IsConst() ? "true":"false") << ", true,\n"
        "               CvType< " << returnTypeName.c_str() << " >::type";
      }
    }
  else if(method->IsConstructor())
    {
    m_Output <<
      "    new Constructor(this, &Wrapper::Constructor_" << m << ",\n"
      "                    \"" << constructorName << "\"";
    
    }
  
  if(method->GetArguments().empty())
    {
    m_Output << "));\n";
    }
  else
    {
    m_Output << ",\n"
      "      parameterTypes));\n"
      "  }\n";
    }
}


/**
 * Write the code to produce the class definition for this wrapper
 * class.  Macros are defined which fill in information for the
 * "wrapWrapperInclude.h" header template.
 */
void
TclGenerator
::WriteWrapperClassDefinition(const source::Class* c,
                              const Methods& methods,
                              const configuration::Class* classConfig) const
{
  String cName = c->GetQualifiedName();
  m_Output <<
    "//--------------------------------------------------------------------\n"
    "// Class wrapper definition for\n"
    "//   " <<  cName.c_str() << "\n"
    "//--------------------------------------------------------------------\n"
    "\n"
    "// These macros will control creation of the Wrapper class.\n"
    "#define _wrap_WRAPPED_TYPE " << cName.c_str() << "\n"
    "#define _wrap_WRAPPED_TYPE_NAME \"" << cName.c_str() << "\"\n"
    "#define _wrap_METHOD_WRAPPER_PROTOTYPES \\\n";
  
  if(!methods.empty())
    {
    for(unsigned int m = 0 ; m < methods.size() ;)
      {
      if(methods[m]->IsMethod())
        {
        if(methods[m]->IsStatic())
          {
          m_Output <<
            "  static void StaticMethod_" << m << "_" << methods[m]->GetName().c_str() << "(const WrapperFacility*, const Arguments&)";
          }
        else
          {
          m_Output <<
            "  static void Method_" << m << "_" << methods[m]->GetName().c_str() << "(const WrapperFacility*, const Arguments&)";
          }
        }
      if(methods[m]->IsOperatorMethod())
        {
        if(methods[m]->IsStatic())
          {
          m_Output <<
            "  static void StaticOperator_" << m << "_" << this->GetOperatorName(methods[m]->GetName()).c_str() << "(const WrapperFacility*, const Arguments&)";
          }
        else
          {
          m_Output <<
            "  static void Operator_" << m << "_" << this->GetOperatorName(methods[m]->GetName()).c_str() << "(const WrapperFacility*, const Arguments&)";
          }
        }
      else if(methods[m]->IsConstructor())
        {
        m_Output <<
          "  static void Constructor_" << m << "(const WrapperFacility*, const Arguments&)";
        }
      if(++m != methods.size())
        {
        m_Output << "; \\";
        }
      m_Output << "\n";
      }
    }
  else
    {
    m_Output <<
      "  typedef int PlaceholderForNoMethodWrappers\n";
    }

  // If there are any alternate names, write them now.
  if(classConfig->AlternateNamesBegin() != classConfig->AlternateNamesEnd())
    {    
    m_Output <<
      "#define _wrap_ALTERNATE_NAMES \\\n";
    configuration::Class::AlternateNames::const_iterator
      a = classConfig->AlternateNamesBegin();
    m_Output <<
      "  \"" << a->c_str() << "\"";
    for(++a;a != classConfig->AlternateNamesEnd(); ++a)
      {
      m_Output << ", \\\n" <<
        "  \"" << a->c_str() << "\"";
      }
    m_Output << "\n";
    }
  
  // Code to include the wrapper class definition header template.
  m_Output <<
    "\n"
    "// This include will produce a specialization of template class\n"
    "// Wrapper according to the macros we just defined.\n"
    "#include \"WrapTclFacility/wrapWrapperInclude.h\"\n"
    "\n"
    "// We are done with these macros.\n";
  if(classConfig->AlternateNamesBegin() != classConfig->AlternateNamesEnd())
    {
    m_Output <<
      "#undef _wrap_ALTERNATE_NAMES\n";
    }
  m_Output <<
    "#undef _wrap_METHOD_WRAPPER_PROTOTYPES\n"
    "#undef _wrap_WRAPPED_TYPE_NAME\n"
    "#undef _wrap_WRAPPED_TYPE\n"
    "\n";
}

void TclGenerator::WriteImplicitArgument(const source::Class* c, const source::Method* m) const
{
  String implicit = c->GetQualifiedName();
  if(m->IsConst())
    {
    implicit = "const "+implicit;
    }
  m_Output <<
    "  " << implicit.c_str() << "& instance = ArgumentAsReferenceTo< " << implicit.c_str() << " >(wrapperFacility)(arguments[0]);\n";
}

void TclGenerator::WriteReturnBegin(const source::Function* f) const
{
  if(!this->ReturnsVoid(f))
    {
    const source::Type* t = f->GetReturns()->GetType();
    const cxx::CvQualifiedType cvType = this->GetCxxType(t);
    if(cvType.IsPointerType())
      {
      const cxx::CvQualifiedType targetType =
        cxx::PointerType::SafeDownCast(cvType.GetType())->GetPointedToType();
      m_Output <<
        "  ReturnPointerTo< " << targetType.GetName().c_str() << " >::From(\n";
      }
    else if(cvType.IsReferenceType())
      {
      const cxx::CvQualifiedType targetType =
        cxx::ReferenceType::SafeDownCast(cvType.GetType())->GetReferencedType();
      m_Output <<
        "  ReturnReferenceTo< " << targetType.GetName().c_str() << " >::From(\n";
      }
    else if(cvType.IsEnumerationType())
      {
      m_Output <<
        "  ReturnEnum< " << cvType.GetName().c_str() << " >::From(\n";
      }
    else
      {
      m_Output <<
        "  Return< " << cvType.GetName().c_str() << " >::From(\n";
      }
    }
}

void TclGenerator::WriteReturnEnd(const source::Function* f) const
{      
  if(this->ReturnsVoid(f))
    {
    m_Output << ");\n"
      "  Return<void>::From(wrapperFacility";
    }
  else
    {
    m_Output << "), wrapperFacility";
    }
}

/**
 * Write the code to convert and pass a list of arguments to wrapped method
 * in its method wrapper's actual call to it.
 */
void TclGenerator::WriteArgumentList(const source::ArgumentContainer& arguments,
                                     unsigned int offset,
                                     unsigned int count) const
{
  unsigned int argCount = 0;
  source::ArgumentsIterator a = arguments.begin();
  if((argCount < count) && (a != arguments.end()))
    {
    m_Output << "\n";
    m_Output <<
      "    CvType< " << this->GetCxxType((*a)->GetType()).GetName() <<
      " >::ArgumentFor(wrapperFacility)(arguments[" << (argCount++ + offset) << "])";
    
    for(++a; (argCount < count) && (a != arguments.end()); ++a)
      {
      m_Output << ",\n";
      m_Output <<
        "    CvType< " << this->GetCxxType((*a)->GetType()).GetName() <<
        " >::ArgumentFor(wrapperFacility)(arguments[" << (argCount++ + offset) << "])";
      }
    }
}


/**
 * Write the comment for a method wrapper describing the prototype of
 * the wrapped method.
 */
void TclGenerator::WriteMethodComment(const String& className,
                                      const MethodEntry& method) const
{
  String prototype;
  if(this->ReturnsVoid(method))
    {
    prototype = "void ";
    }
  else
    {
    prototype = this->GetCxxType(method->GetReturns()->GetType()).GetName() + " ";
    }

  prototype += className + "::" + method->GetCallName() + "(";
  
  const source::ArgumentContainer& arguments = method->GetArguments();
  unsigned int count = method.GetArgumentCount();
  
  unsigned int argCount = 0;
  source::ArgumentsIterator a = arguments.begin();
  if((argCount < count) && (a != arguments.end()))
    {
    prototype += this->GetCxxType((*a)->GetType()).GetName().c_str();
    for(++a; (argCount < count) && (a != arguments.end()); ++a)
      {
      prototype += ", ";
      prototype += this->GetCxxType((*a)->GetType()).GetName().c_str();
      }
    }
  
  prototype += ")";
  
  if(method->IsConst())
    {
    prototype += " const";
    }
  
  m_Output <<
    "/**\n"
    " * " << prototype.c_str() << "\n"
    " */\n";
}


/**
 * Write out the code to register conversion functions for the wrappers.
 */
void TclGenerator::WriteConversionInititialization() const
{
  for(ClassesForDerivedToBase::const_iterator c = m_ClassesForDerivedToBase.begin();
      c != m_ClassesForDerivedToBase.end(); ++c)
    {
    const cxx::ClassType* derived = *c;
    cxx::ClassTypes superclasses;
    derived->GetAllSuperclasses(superclasses);
    for(cxx::ClassTypes::const_iterator b = superclasses.begin();
        b != superclasses.end(); ++b)
      {
      const cxx::ClassType* base = *b;
      {
      cxx::CvQualifiedType derivedType = derived->GetCvQualifiedType(false, false);
      cxx::CvQualifiedType baseType = base->GetCvQualifiedType(false, false);
      cxx::CvQualifiedType baseRef = source::CxxTypes::GetReferenceType(baseType);
      m_Output <<
        "  wrapperFacility->SetConversion(\n"
        "    CvType< " << derivedType.GetName().c_str() << " >::type,\n"
        "    CvType< " << baseRef.GetName().c_str() << " >::type.GetType(),\n"
        "    Converter::ReferenceDerivedToBase< " << derivedType.GetName().c_str() << ", \n" <<
        "                                       " << baseType.GetName().c_str() << " >::GetConversionFunction());\n";
      }
      {
      cxx::CvQualifiedType derivedType = derived->GetCvQualifiedType(true, false);
      cxx::CvQualifiedType baseType = base->GetCvQualifiedType(true, false);
      cxx::CvQualifiedType baseRef = source::CxxTypes::GetReferenceType(baseType);
      m_Output <<
        "  wrapperFacility->SetConversion(\n"
        "    CvType< " << derivedType.GetName().c_str() << " >::type,\n"
        "    CvType< " << baseRef.GetName().c_str() << " >::type.GetType(),\n"
        "    Converter::ReferenceDerivedToBase< " << derivedType.GetName().c_str() << ", \n" <<
        "                                       " << baseType.GetName().c_str() << " >::GetConversionFunction());\n";
      }
      {
      cxx::CvQualifiedType derivedType = derived->GetCvQualifiedType(false, false);
      cxx::CvQualifiedType derivedPtr = source::CxxTypes::GetPointerType(derivedType, false, false);
      cxx::CvQualifiedType baseType = base->GetCvQualifiedType(false, false);
      cxx::CvQualifiedType basePtr = source::CxxTypes::GetPointerType(baseType, false, false);
      m_Output <<
        "  wrapperFacility->SetConversion(\n"
        "    CvType< " << derivedPtr.GetName().c_str() << " >::type,\n"
        "    CvType< " << basePtr.GetName().c_str() << " >::type.GetType(),\n"
        "    Converter::PointerDerivedToBase< " << derivedType.GetName().c_str() << ", \n" <<
        "                                     " << baseType.GetName().c_str() << " >::GetConversionFunction());\n";
      }
      {
      cxx::CvQualifiedType derivedType = derived->GetCvQualifiedType(true, false);
      cxx::CvQualifiedType derivedPtr = source::CxxTypes::GetPointerType(derivedType, false, false);
      cxx::CvQualifiedType baseType = base->GetCvQualifiedType(true, false);
      cxx::CvQualifiedType basePtr = source::CxxTypes::GetPointerType(baseType, false, false);
      m_Output <<
        "  wrapperFacility->SetConversion(\n"
        "    CvType< " << derivedPtr.GetName().c_str() << " >::type,\n"
        "    CvType< " << basePtr.GetName().c_str() << " >::type.GetType(),\n"
        "    Converter::PointerDerivedToBase< " << derivedType.GetName().c_str() << ", \n" <<
        "                                     " << baseType.GetName().c_str() << " >::GetConversionFunction());\n";
      }
      }
    }
  for(ClassesThatNeedDestructor::const_iterator c = m_ClassesThatNeedDestructor.begin();
      c != m_ClassesThatNeedDestructor.end(); ++c)
    {
    const cxx::ClassType* curClass = *c;    
    m_Output <<
      "  wrapperFacility->SetDeleteFunction(CvType< " << curClass->Name().c_str() <<
      " >::type.GetType(), &OldObjectOf< " << curClass->Name().c_str() << " >::Delete);\n";
    }
  for(EnumTypesThatNeedValues::const_iterator
        e = m_EnumTypesThatNeedValues.begin();
      e != m_EnumTypesThatNeedValues.end(); ++e)
    {
    const source::Enumeration* enumeration = *e;
    String typeName = enumeration->GetQualifiedName();
    m_Output <<
      "  wrapperFacility->SetDeleteFunction(CvType< " << typeName.c_str() <<
      " >::type.GetType(), &OldObjectOf< " << typeName.c_str() << " >::Delete);\n";
    }
}


/**
 * For all enumeration types that are returned from a method, write the
 * corresponding ReturnEnum<T> class specialization code.
 */
void TclGenerator::WriteReturnEnumClasses() const
{
  for(EnumTypesThatNeedReturn::const_iterator
        e = m_EnumTypesThatNeedReturn.begin();
      e != m_EnumTypesThatNeedReturn.end(); ++e)
    {
    const source::Enumeration* enumeration = *e;
    String typeName = enumeration->GetQualifiedName();
    String prefix = enumeration->GetContext()->GetQualifiedName();
    m_Output <<
      "\n"
      "template <>\n"
      "struct ReturnEnum< "<< typeName.c_str() << " >\n"
      "{\n"
      "  static void From(const " << typeName.c_str() << "& result,\n"
      "                   const WrapperFacility* wrapperFacility)\n"
      "    {\n"
      "    const char* name=0;\n"
      "    switch (result)\n"
      "      {\n";
    for(source::Enumeration::Values::const_iterator
          v = enumeration->ValuesBegin();
        v != enumeration->ValuesEnd(); ++v)
      {
      String value = prefix+"::"+*v;
      m_Output <<
        "      case " << value.c_str() << ": name = \"" << value.c_str() << "\"; break;\n";
      }
    m_Output <<
      "      }\n"
      "    Tcl_SetObjResult(wrapperFacility->GetInterpreter(),\n"
      "                     Tcl_NewStringObj(const_cast<char*>(name), -1));\n"
      "    }\n"
      "};\n";    
    }
}


void TclGenerator::FindCvTypes(const configuration::Namespace* ns)
{
  for(configuration::Namespace::Wrappers::const_iterator w =
        ns->WrappersBegin(); w != ns->WrappersEnd(); ++w)
    {
    String qualifedName = ns->GetQualifiedName()+"::"+(*w)->GetName();
    source::Class* c = m_GlobalNamespace->LookupClass(qualifedName);
    if(c)
      {
      this->FindCvTypes(c);      }
    }
  for(configuration::Namespace::Fields::const_iterator f =
        ns->FieldsBegin(); f != ns->FieldsEnd(); ++f)
    {
    const configuration::Named* n = f->second;
    if(n->IsNamespace())
      {
      const configuration::Namespace* nns =
        dynamic_cast<const configuration::Namespace*>(n);
      this->FindCvTypes(nns);
      }
    }
}


void TclGenerator::FindCvTypes(const source::Class* c)
{
  // Add the implict argument types for method calls.
  cxx::CvQualifiedType classCvType = c->GetCxxClassType(m_GlobalNamespace);
  cxx::CvQualifiedType constClassCvType = classCvType.GetMoreQualifiedType(true, false);
  m_CvTypeGenerator.Add(source::CxxTypes::GetReferenceType(classCvType));
  m_CvTypeGenerator.Add(source::CxxTypes::GetReferenceType(constClassCvType));
  m_CvTypeGenerator.Add(source::CxxTypes::GetPointerType(classCvType, false, false));
  m_CvTypeGenerator.Add(source::CxxTypes::GetPointerType(constClassCvType, false, false));
  this->AddSuperclassCvTypes(cxx::ClassType::SafeDownCast(classCvType.GetType()));
  m_ClassesForDerivedToBase.insert(cxx::ClassType::SafeDownCast(classCvType.GetType()));
  
  for(source::MethodsIterator methodItr = c->GetMethods().begin();
      methodItr != c->GetMethods().end(); ++methodItr)
    {
    source::Method* method = *methodItr;
    if(method->GetAccess() == source::Public)
      {
      if(method->IsMethod()
         || method->IsConstructor()
         || method->IsOperatorMethod()
         || method->IsConverter())
        {
        this->FindCvTypes(method);
        }
      }
    }
}
  
void TclGenerator::FindCvTypes(const source::Method* method)
{
  if(method->GetReturns() && method->GetReturns()->GetType())
    {
    // Add the return type of the method.
    cxx::CvQualifiedType returnType = this->GetCxxType(method->GetReturns()->GetType());
    m_CvTypeGenerator.Add(returnType);
    
    if(returnType.IsEnumerationType())
      {
      source::Named* result = m_GlobalNamespace->LookupName(returnType.GetType()->Name());
      if(result && (result->GetTypeOfObject() == source::Enumeration_id))
        {
        source::Enumeration *e = dynamic_cast<source::Enumeration*>(result);
        m_EnumTypesThatNeedReturn.insert(e);
        m_EnumTypesThatNeedValues.insert(e);
        }
      }
    
    // If the return type is a class type or reference to class type,
    // we must also add the reference types to the class's superclasses so
    // that derived-to-base conversions can be registered.
    cxx::CvQualifiedType cvType;
    if(returnType.IsReferenceType())
      {
      cvType = cxx::ReferenceType::SafeDownCast(returnType.GetType())->GetReferencedType();
      }
    else
      {
      cvType = returnType;
      }
    if(cvType.IsClassType())
      {
      const cxx::ClassType* classType =
        cxx::ClassType::SafeDownCast(cvType.GetType());
      this->AddSuperclassCvTypes(classType);
      m_ClassesForDerivedToBase.insert(classType);
      }
    else if(cvType.IsPointerType())
      {
      cxx::CvQualifiedType pointedToType =
        cxx::PointerType::SafeDownCast(cvType.GetType())->GetPointedToType();
      if(pointedToType.IsClassType())
        {
        const cxx::ClassType* classType =
          cxx::ClassType::SafeDownCast(pointedToType.GetType());
        this->AddSuperclassCvTypes(classType);
        m_ClassesForDerivedToBase.insert(classType);
        }
      }
    }
  // Add the argument types of the method.
  for(source::ArgumentsIterator a = method->GetArguments().begin();
      a != method->GetArguments().end(); ++a)
    {
    cxx::CvQualifiedType argType = this->GetCxxType((*a)->GetType());
    m_CvTypeGenerator.Add(argType);

    // If the argument type is an enumeration type or a reference to
    // const enumeration type, we want to make sure the enumeration
    // value objects are provided.
    if(argType.IsEnumerationType())
      {
      source::Named* result = m_GlobalNamespace->LookupName(argType.GetType()->Name());
      if(result && (result->GetTypeOfObject() == source::Enumeration_id))
        {
        source::Enumeration *e = dynamic_cast<source::Enumeration*>(result);
        m_EnumTypesThatNeedValues.insert(e);
        }
      }
    else if(argType.IsReferenceType())
      {
      argType = cxx::ReferenceType::SafeDownCast(argType.GetType())->GetReferencedType();
      if(argType.IsEnumerationType() && argType.IsConst())
        {
        source::Named* result = m_GlobalNamespace->LookupName(argType.GetType()->Name());
        if(result && (result->GetTypeOfObject() == source::Enumeration_id))
          {
          source::Enumeration *e = dynamic_cast<source::Enumeration*>(result);
          m_EnumTypesThatNeedValues.insert(e);
          }
        }
      }
    }
}


void TclGenerator::AddSuperclassCvTypes(const cxx::ClassType* classType)
{
  // Add given class's pointer and reference types.
  cxx::CvQualifiedType self = classType->GetCvQualifiedType(false, false);
  cxx::CvQualifiedType constSelf = classType->GetCvQualifiedType(true, false);
  m_CvTypeGenerator.Add(source::CxxTypes::GetReferenceType(self));
  m_CvTypeGenerator.Add(source::CxxTypes::GetReferenceType(constSelf));
  m_CvTypeGenerator.Add(source::CxxTypes::GetPointerType(self, false, false));
  m_CvTypeGenerator.Add(source::CxxTypes::GetPointerType(constSelf, false, false));
  
  // Now add the parent class types.
  for(cxx::ClassTypes::const_iterator p = classType->ParentsBegin();
      p != classType->ParentsEnd(); ++p)
    {
    this->AddSuperclassCvTypes(*p);
    }
}


cxx::CvQualifiedType TclGenerator::GetCxxType(const source::Type* t) const
{
  return t->GetCxxType(m_GlobalNamespace);
}

} // namespace gen
