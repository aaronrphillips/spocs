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


#ifndef FASTASEQUENCE_HH
#define FASTASEQUENCE_HH


// System Includes
#include <algorithm>
#include <fstream>
#include <string>
// External Includes
// Internal Includes
#include "global.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "FastaSequence"


using namespace std;


struct FastaSequence
{
  string    sequence;
  string    comment;
  string    name;
  u_int64_t offset;


  inline string print_raw(void) const throw()
  {
    string out = ""; 
    string seq = this->sequence;

    seq.erase(std::remove(seq.begin(), seq.end(), '\r'), seq.end());
    seq.erase(std::remove(seq.begin(), seq.end(), '\n'), seq.end());

    out += ">" + this->name + " " + this->comment  + "__NL__";
    while (seq.length() > 60) 
    {   
      out += seq.substr(0, 60) + "__NL__";
      seq  = seq.substr(60);
    }   
    out += seq;

    return(out);
  }


  inline string print(void) const throw()
  {
    string out = "";
    string seq = this->sequence;

    seq.erase(std::remove(seq.begin(), seq.end(), '\r'), seq.end());
    seq.erase(std::remove(seq.begin(), seq.end(), '\n'), seq.end());

    out += ">" + this->name + " " + this->comment  + "\n";
    while (seq.length() > 60)
    {
      out += seq.substr(0, 60) + "\n";
      seq  = seq.substr(60);
    }
    out += seq;

    return(out);
  }


friend
  ostream & operator<<(ostream & p_os, FastaSequence const * p_seq) throw();
friend
  ostream & operator<<(ostream & p_os, FastaSequence const & p_seq) throw();

};


#endif /* FASTASEQUENCE_HH */
