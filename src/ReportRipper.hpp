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


#ifndef REPORTRIPPER_HPP
#define REPORTRIPPER_HPP


// System Includes
#include <map>
#include <string>
#include <vector>
// External Includes
#include "boost/shared_ptr.hpp"
// Internal Includes
// Application Includes
#include "Lookup.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "ReportRipper"


// Namespaces used
using namespace std;
using namespace boost;


typedef vector< pair<u_int32_t, u_int32_t> > hsp_type;
typedef struct hit_line
{
  u_int32_t ids;
  u_int32_t hsp_len;
  u_int32_t mismatch;
  u_int32_t gaps;
  u_int32_t q_start;
  u_int32_t q_end;
  u_int32_t q_len;
  u_int32_t s_start;
  u_int32_t s_end;
  u_int32_t s_len;
  u_int32_t q_aggregate_len;
  u_int32_t s_aggregate_len;
  u_int32_t q_local_len;
  u_int32_t s_local_len;
  u_int32_t merges;
  double    score;
  double    e_value;
  string    key;
  string    query;
  string    subject;
  hsp_type  q_hsps;
  hsp_type  s_hsps;
} hit_line;


typedef map<string, hit_line> HitLines;
typedef boost::shared_ptr< HitLines > sharedHitLines;


class ReportRipper
{

public:

  // Getters & Setters

  inline sharedHitLines getSharedHitLines()
  {
    return(this->hit_lines);
  }


  // Constructors

  ReportRipper(string const & p_filename, sharedLookup const & p_lookup) throw();


  // Destructor

  ~ReportRipper(void) throw()
  {
    return;
  }


  // Public Methods

  string print(void) const throw();


private:

  // Getters/Setters


  // Constructors


  // Operators

  bool operator=(ReportRipper const & p_ripper) const throw();


  // Methods

  void merge(hit_line & p_existing, hit_line & p_tmp) throw();


  // Variables

  string         filename;
  sharedLookup   lookup;
  sharedHitLines hit_lines;


friend
  ostream & operator<<(ostream & p_os, ReportRipper const * p_ripper) throw();
friend
  ostream & operator<<(ostream & p_os, ReportRipper const & p_ripper) throw();

};


typedef boost::shared_ptr<ReportRipper> sharedReportRipper;


#endif // REPORTRIPPER_HPP
