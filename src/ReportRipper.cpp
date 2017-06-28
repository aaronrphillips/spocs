///////////////////////////////////////////////////////////////////////////////
// Copyright © 2013, Battelle Memorial Institute
//
// THIS FILE INITIALLY CREATED WITH:  
//     TEMPLATE NAME:  lang_cpp_class.template 
//     COMMAND NAME:   gensrc 
//
// CODING CONVENTIONS: 
//    * Class names are CamelCase with first word upper case 
//    * Functions are camelCase with first word lower case 
//    * Function parameters are lower case with _ and have a_ prefix 
//    * Member variables always use 'this' pointer 
///////////////////////////////////////////////////////////////////////////////


// System Includes
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
// External Includes
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/regex.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include <boost/regex.hpp>
#include <boost/shared_ptr.hpp>
// Internal Includes
// Application Includes
#include "global.hpp"
#include "Exception.hpp"
#include "ReportRipper.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "ReportRipper"


// Namespaces used
using namespace boost;
using namespace std;
using boost::lexical_cast;


//==============================================================================
// Class ReportRipper
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


// ======================================================================
// Constructors
// ======================================================================


ReportRipper::ReportRipper(string                 const & p_filename, 
                           sharedLookup const & p_lookup) throw() :
  filename(p_filename),
  lookup(p_lookup),
  hit_lines(new  map<string, hit_line>())
{
  ifstream                                   data(this->filename.c_str()); // *.blastout
  string                                     line;
  string                                     cell;

  boost::regex_constants::syntax_option_type flags = boost::regex_constants::perl;
  boost::regex                               comments;
  boost::regex                               alignment;
  boost::smatch                              what;

  comments.assign("^#", flags);
  alignment.assign("^[[:alnum:]]", flags);

  // for each blastout line, create a 'hits' file line (possibly combining multiple lines)
  while (getline(data, line))
  {
    if (boost::regex_search(line, what, comments))
    {
      continue; // skip comments
    }

    if (boost::regex_search(line, what, alignment))
    {
      std::vector<string> sv;
      boost::algorithm::split_regex(sv, line, regex("[[:space:]]+"));

      hit_line tmp;
      istringstream os(sv[10]);
      os >> tmp.e_value;
      istringstream os1(sv[11]);
      os1 >> tmp.score;


      double percent_ids  = lexical_cast<double>(sv[2]);
      tmp.hsp_len         = lexical_cast<u_int32_t>(sv[3]);
      tmp.mismatch        = lexical_cast<u_int32_t>(sv[4]);
      tmp.q_start         = lexical_cast<u_int32_t>(sv[6]);
      tmp.q_end           = lexical_cast<u_int32_t>(sv[7]);
      tmp.s_start         = lexical_cast<u_int32_t>(sv[8]);
      tmp.s_end           = lexical_cast<u_int32_t>(sv[9]);
      tmp.query           = sv[0];
      tmp.subject         = sv[1];

      pair<u_int32_t, u_int32_t> q_hsp(tmp.q_start, tmp.q_end);
      pair<u_int32_t, u_int32_t> s_hsp(tmp.s_start, tmp.s_end);
      tmp.q_hsps.push_back(q_hsp);
      tmp.s_hsps.push_back(s_hsp);

      tmp.ids             = (u_int32_t)(((percent_ids / 100.0) * tmp.hsp_len) + 0.5);
      tmp.gaps            = tmp.hsp_len - tmp.mismatch - tmp.ids;
      tmp.q_len           = lookup->getLengthByName(tmp.query);
      tmp.s_len           = lookup->getLengthByName(tmp.subject);
      tmp.key             = tmp.query + ':' + tmp.subject;
      tmp.q_aggregate_len = tmp.q_end - tmp.q_start + 1;
      tmp.s_aggregate_len = tmp.s_end - tmp.s_start + 1;
      tmp.q_local_len     = tmp.q_end - tmp.q_start + 1;
      tmp.s_local_len     = tmp.s_end - tmp.s_start + 1;


      map<string, hit_line>::iterator ftor = hit_lines->find(tmp.key);
      map<string, hit_line>::iterator etor = hit_lines->end();
      
      // if we have already read a line for this query/subject pair, then merge the lines
      if (ftor != etor)
      {
        merge((*ftor).second, tmp); 
      }
      else
      {
        hit_lines->insert(pair<string, hit_line>(tmp.key, tmp));  // save 
      }

      continue;
    }
  }

  return;
}


// ======================================================================
// Public Functions
// ======================================================================

// each query/subject pair in the blastout file (there can be multiple lines for the same Q/S), can 
// only be represented by one line in the hit file. 
// when multiple Q/S lines are found - merge.
// NOTE: this section was derived from inparanoid's blast output (pairwise or XML depending on the version)
//       parser. Our goal was to reproduce the hit file exactly (except for the hsp columns at the end, 
//       which are never actually read)
void ReportRipper::merge(hit_line & p_existing, hit_line & p_tmp) throw()
{
  hsp_type::const_iterator qitor = p_existing.q_hsps.begin();
  hsp_type::const_iterator qetor = p_existing.q_hsps.end();
  hsp_type::const_iterator sitor = p_existing.s_hsps.begin();
  bool accept = true;
  u_int32_t qleft  = 99999;
  u_int32_t qright = 0;
  u_int32_t sleft  = 99999;
  u_int32_t sright = 0;

  // loop over the hsps for this query/subject
  for (qitor = qitor; qitor != qetor; ++qitor, ++sitor)
  {
    u_int32_t qs = (*qitor).first;
    u_int32_t qe = (*qitor).second;
    u_int32_t ss = (*sitor).first;
    u_int32_t se = (*sitor).second;

    if (qleft  > qs) qleft  = qs;
    if (qright < qe) qright = qe;
    if (sleft  > ss) sleft  = ss;
    if (sright < se) sright = se;

    u_int32_t q_start1, q_start2, s_start1, s_start2, q_end1, q_end2, s_end1, s_end2;

    
    if (qs < p_tmp.q_start)
    {
      q_start1 = qs;
      q_end1   = qe;
      s_start1 = ss;
      s_end1   = se;

      q_start2 = p_tmp.q_start;
      q_end2   = p_tmp.q_end;
      s_start2 = p_tmp.s_start;
      s_end2   = p_tmp.s_end;
    }
    else
    {
      q_start2 = qs;
      q_end2   = qe;
      s_start2 = ss;
      s_end2   = se;

      q_start1 = p_tmp.q_start;
      q_end1   = p_tmp.q_end;
      s_start1 = p_tmp.s_start;
      s_end1   = p_tmp.s_end;
    }


    u_int32_t q_length1 = q_end1 - q_start1 + 1;
    u_int32_t q_length2 = q_end2 - q_start2 + 1;
    u_int32_t q_length = (q_length1 < q_length2) ? q_length1 : q_length2;

    u_int32_t s_length1 = s_end1 - s_start1 + 1;
    u_int32_t s_length2 = s_end2 - s_start2 + 1;
    u_int32_t s_length = (s_length1 < s_length2) ? s_length1 : s_length2;


    if (q_length == 0 || s_length == 0) continue;

    if ((((double)q_start2 - (double)q_end1 - 1.0) / (double)q_length) < -0.05 ||
        (((double)s_start2 - (double)s_end1 - 1.0) / (double)s_length) < -0.05)
    {
      accept = 0;
      break;
    }
  }

  if (! accept) return;

  p_existing.merges++;

  if (qleft  > p_tmp.q_start) qleft  = p_tmp.q_start;
  if (qright < p_tmp.q_end)   qright = p_tmp.q_end;
  if (sleft  > p_tmp.s_start) sleft  = p_tmp.s_start;
  if (sright < p_tmp.s_end)   sright = p_tmp.s_end;

  p_existing.hsp_len += p_tmp.hsp_len;
  p_existing.score   += p_tmp.score;
  p_existing.ids     += p_tmp.ids;
  p_existing.gaps    += p_tmp.gaps;

  p_existing.q_hsps.push_back(pair<u_int32_t, u_int32_t>(p_tmp.q_start, p_tmp.q_end));
  p_existing.s_hsps.push_back(pair<u_int32_t, u_int32_t>(p_tmp.s_start, p_tmp.s_end));

  p_existing.q_aggregate_len = qright - qleft + 1;
  p_existing.s_aggregate_len = sright - sleft + 1;
  p_existing.q_local_len += p_tmp.q_local_len;
  p_existing.s_local_len += p_tmp.s_local_len;

  return;

}


string ReportRipper::print(void) const throw()
{
  stringstream ss;
  HitLines::const_iterator itor = hit_lines->begin();
  HitLines::const_iterator etor = hit_lines->end();

  for (itor = itor; itor != etor; ++itor)
  {
    string key = (*itor).first;

    if ((*itor).second.score < lookup->getScoreCutoff()) continue;

    string q = (*itor).second.query;
    string s = (*itor).second.subject;

    ss << q << "\t" << s << "\t";
    ss << setiosflags(ios::fixed) << setprecision(1) << (*itor).second.score;
    ss << "\t";
    ss << (*itor).second.q_len;
    ss << "\t";
    ss << (*itor).second.s_len;
    ss << "\t";
    ss << (*itor).second.q_aggregate_len;
    ss << "\t";
    ss << (*itor).second.s_aggregate_len;
    ss << "\t";
    ss << (*itor).second.q_local_len;
    ss << "\t";
    ss << (*itor).second.s_local_len;
    ss << "\t";
  
    hsp_type::const_iterator qitor = (*itor).second.q_hsps.begin();
    hsp_type::const_iterator qetor = (*itor).second.q_hsps.end();
    for (qitor = qitor; qitor != qetor; ++qitor)
    {
      ss << (*qitor).first << "-" << (*qitor).second << ",";
    }

    ss << "\t";

    hsp_type::const_iterator sitor = (*itor).second.s_hsps.begin();
    hsp_type::const_iterator setor = (*itor).second.s_hsps.end();
    for (sitor = sitor; sitor != setor; ++sitor)
    {
      ss << (*sitor).first << "-" << (*sitor).second << ",";
    }

    ss << "\n";
  }

  return(ss.str());
}


ostream & operator<<(ostream & p_os, ReportRipper const * p_ripper) throw()
{
  return(p_os << p_ripper->print());
}


ostream & operator<<(ostream & p_os, ReportRipper const & p_ripper) throw()
{
  return(p_os << p_ripper.print());
}

