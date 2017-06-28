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


#ifndef METADATA_HPP
#define METADATA_HPP


// System Includes
#include <sstream>
#include <string>
// External Includes
#include "boost/shared_ptr.hpp"
// Internal Includes
// Application Includes
#include "global.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "Metadata"


// Namespaces used
using namespace std;
using namespace boost;


class Metadata
{

public:

  // Getters & Setters

  inline bool isValid(void) const throw()
  {
    return(this->is_valid);
  }

  // Constructors

  Metadata(string const & p_metadata_file, string const & p_output_dir) throw();


  // Destructor

  ~Metadata(void) throw()
  {
    return;
  }


  // Public Methods

  bool   create_javascript(void) throw();
  string print(void) throw();


  // Variables


private:

  // Getters/Setters


  // Constructors

  Metadata(void) throw();
  Metadata(Metadata const & p_metadata_file) throw();


  // Operators

  bool operator=(Metadata const & p_metadata_file) const throw();


  // Methods


  // Variables

  bool         is_valid;
  string       metadata_file;
  string       output_dir;
  stringstream json;


  // Friends

friend
  ostream & operator<<(ostream & p_os, Metadata * p_metadata_file) throw();
friend
  ostream & operator<<(ostream & p_os, Metadata & p_metadata_file) throw();

};


typedef boost::shared_ptr<Metadata> sharedMetadata;


#endif // METADATA_HPP
