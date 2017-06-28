///////////////////////////////////////////////////////////////////////////////
// Copyright © 2013, Battelle Memorial Institute
//
// THIS FILE INITIALLY CREATED WITH:
//     TEMPLATE NAME:  lang_cpp_hdr.template
//     COMMAND NAME:   gensrc
//
// CODING CONVENTIONS:
//    * Class names are CamelCase with first word upper case
//    * Functions are camelCase with first word lower case
//    * Function parameters are lower case with _ and have p_ prefix
//    * Member variables always use 'this' pointer
///////////////////////////////////////////////////////////////////////////////


// System Includes
// External Includes
// Internal Includes
// Application Includes
#include "Ortholog.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "Ortholog"


// Namespaces used
using namespace std;


// Constructors

Ortholog::Ortholog(id_type s1, id_type s2, float score) :
  ortholog(s1, s2, score) 
{
  return;
}


// Methods 

string Ortholog::print(void) throw()
{
  string output("Ortholog: Not implemented yet");

  return(output);
}


// Operators

ostream & operator<<(ostream & p_os, Ortholog * p_ortholog) throw()
{
  return(p_os << p_ortholog->print());
}


ostream & operator<<(ostream & p_os, Ortholog & p_ortholog) throw()
{
  return(p_os << p_ortholog.print());
}
