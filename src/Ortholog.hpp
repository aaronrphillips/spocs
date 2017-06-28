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


#ifndef ORTHOLOG_HPP
#define ORTHOLOG_HPP


// System Includes
#include <list>
#include <map>
#include <string>
#include <vector>
// External Includes
#include "boost/shared_ptr.hpp"
// Internal Includes
// Application Includes
#include "global.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "Ortholog"


// Namespaces used
using namespace std;
using namespace boost;


typedef tuple<id_type, id_type, float>  ortholog_type;


class Ortholog
{

public:

  // Getters/Setters

  inline id_type getIdA(void) const throw()
  {
    return(this->ortholog.get<0>());
  }


  inline id_type getIdB(void) const throw()
  {
    return(this->ortholog.get<1>());
  }


  inline float getScore(void) const throw()
  {
    return(this->ortholog.get<2>());
  }


  // Constructors

  Ortholog(id_type s1, id_type s2, float score);


  // Destructors

  ~Ortholog(void) throw()
  {
    return;
  }


  // Operators

  inline bool operator<(Ortholog const & b) const throw()
  {
    if (  ortholog.get<2>() > b.ortholog.get<2>()) { return(true);  }
    if (b.ortholog.get<2>() >   ortholog.get<2>()) { return(false); }
    if (  ortholog.get<0>() > b.ortholog.get<0>()) { return(true);  }
    if (b.ortholog.get<0>() >   ortholog.get<0>()) { return(false); }
    if (  ortholog.get<1>() > b.ortholog.get<1>()) { return(true);  }
    if (b.ortholog.get<1>() >   ortholog.get<1>()) { return(false); }

    return(true);
  }


  // Methods

  string print(void) throw();


private:

  // Constructors

  Ortholog(void) throw();
  //Ortholog(Ortholog const & p_ortholog) throw();


  // Operators

  bool operator=(Ortholog const & p_ortholog) const throw();


  // Variables

  ortholog_type ortholog;


friend
  ostream & operator<<(ostream & p_os, Ortholog * p_ortholog) throw();
friend
  ostream & operator<<(ostream & p_os, Ortholog & p_ortholog) throw();

};


typedef boost::shared_ptr<Ortholog> sharedOrtholog;


#endif // ORTHOLOG_HPP
