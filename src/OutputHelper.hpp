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


#ifndef OUTPUTHELPER_HPP
#define OUTPUTHELPER_HPP


// System Includes
#include <fstream>
#include <iostream>
#include <map>
#include <string>
// External Includes
#include <boost/shared_ptr.hpp>
// Internal Includes
// Application Includes
#include "osUtil.hpp"

#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "OutputHelper"


// Namespaces used
using namespace std;
using namespace boost;


typedef boost::shared_ptr<std::fstream> handle_type;


//==============================================================================
// Class OutputHelper
//
// API Developer Documentation:   (those who develop/fix this class)
//   [put documentation here for developers that work on this code]
//------------------------------------------------------------------------------
// API User Documentation:  (those who use this class)
//
//   [put a single summary sentence describing this class here]
//
// SYNOPSIS:
//   [put an overview paragraph for this class here]
//
// DESCRIPTION:
//   [put a description section for this class here]
//
// FUTURE FEATURES:
//  None discovered
//
// KNOWN LIMITATIONS:
//  None discovered
//
// SEE ALSO:
//============================================================================== 


class OutputHelper
{

public:

  // Getters/Setters


  // Constructors

  //explicit OutputHelper(void) throw();


  // Destructor

  ~OutputHelper(void) throw()
  {
    map<u_int16_t, handle_type>::const_iterator itor = this->file_lookup.begin();
    map<u_int16_t, handle_type>::const_iterator etor = this->file_lookup.end();

    for (itor = itor; itor != etor; ++itor)
    {
      handle_type fd = (*itor).second;
      fd->close();
    }   

    return;
  }


  // Public Functions

  void        write(u_int16_t const & p_file_id, string const & p_data) throw();
  handle_type open_file(string const & file_name) throw();
  void        register_file(u_int16_t const p_type, string const & p_filename) throw();
  void        append_content(u_int16_t const p_type, string const & p_filename) throw();
  void        close(u_int16_t const & p_file_id) throw();


private:

  // Getters/Setters


  // Constructors

  //OutputHelper(OutputHelper const & p_output_helper) throw();


  // Operators

  //OutputHelper operator=(OutputHelper const & p_output_helper) throw();


  // Variables

  map<u_int16_t, handle_type> file_lookup;

};


typedef boost::shared_ptr<OutputHelper>  sharedOutputHelper;


#endif // OUTPUTHELPER_HPP
