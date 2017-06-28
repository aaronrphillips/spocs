///////////////////////////////////////////////////////////////////////////////
// Copyright © 2013, Battelle Memorial Institute
//
// THIS FILE INITIALLY CREATED WITH:  
//     TEMPLATE NAME:  lang_cc_hdr 
//     COMMAND NAME:   gensrc 
//     CREATED FOR:     
//
// CODING CONVENTIONS: 
//    * Class names are CamelCase with first word upper case 
//    * Functions are camelCase with first word lower case 
//    * Function parameters are lower case with _ and have p_ prefix 
//    * Member variables always use 'this' pointer 
//  
///////////////////////////////////////////////////////////////////////////////


// System Includes
#include <algorithm>
#include <string>
// External Includes
// Internal Includes
#include "FastaSequence.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "FastaSequence"


using namespace std;


ostream & operator<<(ostream & p_os, FastaSequence const * p_seq) throw()
{
  return(p_os << p_seq->print());
}


ostream & operator<<(ostream & p_os, FastaSequence const & p_seq) throw()
{
  return(p_os << p_seq.print());
}
