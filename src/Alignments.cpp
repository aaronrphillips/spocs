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


//  DEVELOPER NOTE:  For Performance to turn off all BOOST assertions
#define BOOST_DISABLE_ASSERTS

// System Includes
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
// External Includes
#include <boost/bind.hpp>
// Internal Includes
// Application Includes
#include "Alignments.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "Alignments"


// Namespaces used
using namespace std;
using namespace boost;


// Constructors


Alignments::Alignments(species_type const p_query_type, 
                       species_type const p_subject_type, 
                       id_type      const p_query_id,
                       id_type      const p_subject_id,
                       float        const p_score
                       ) throw() :
query_type(p_query_type),
subject_type(p_subject_type),
query_id(p_query_id),
subject_ids(vector<id_type>()),
hits(hits_type())
{
  add(p_subject_id, p_score);
  return;
}


// Operators


// Public Methods


// adds a hit (p_subject_id) against this 'Alignment's query with score = p_score
void Alignments::add(id_type const p_subject_id, float const p_score) throw()
{
  hits_type::const_iterator ftor = hits.find(p_subject_id);
  hits_type::const_iterator etor = hits.end();

  if (ftor != etor)
  {
    cerr << "WARNING: adding subject hit (" << p_subject_id << ", " << p_score << ") to alignments when one already exists (" << (*ftor).first << ", " << (*ftor).second << ")" << endl;
    return;
  }

  hits.insert(pair<id_type, float>(p_subject_id, p_score));
  subject_ids.push_back(p_subject_id);

  return;
}


// returns the hits sorted by score
vector< pair<id_type, float> > Alignments::getSortedHits(void) const throw()
{
  vector< pair<id_type, float> > pairs;

  hits_type::const_iterator itor = hits.begin();
  hits_type::const_iterator etor = hits.end();

  for (itor = itor; itor != etor; ++itor)
  {
    pairs.push_back(*itor);
  }
  
  std::sort(pairs.begin(), pairs.end(), 
            boost::bind(&std::pair<id_type, float>::second, _1) >
            boost::bind(&std::pair<id_type, float>::second, _2));

  return(pairs);
}


// return the (sorted) hits better than p_score
vector<id_type> Alignments::getHitsBetterThan(float const p_score) const throw()
{
  vector< pair<id_type, float> > hitlist = getSortedHits();
  vector< pair<id_type, float> >::const_iterator itor = hitlist.begin();
  vector< pair<id_type, float> >::const_iterator etor = hitlist.end();

  vector<id_type> results;
  
  for (itor = itor; itor != etor; ++itor)
  {
    float score = (*itor).second;

    if (score < p_score)
    {
      break;
    }

    results.push_back((*itor).first);
  }
  
  return(results); 
}


// return the hit(s) with the highest score
vector< pair<id_type, float> > Alignments::getBestHits(void) const throw()
{
  vector< pair<id_type, float> > hitlist = getSortedHits();
  vector< pair<id_type, float> >::const_iterator itor = hitlist.begin();
  vector< pair<id_type, float> >::const_iterator etor = hitlist.end();
  vector< pair<id_type, float> > results;

  float best_score = -1000;
  
  for (itor = itor; itor != etor; ++itor)
  {
    float score = (*itor).second;

    if (score < best_score)
    {
      break;
    }

    results.push_back((*itor));
    best_score = score;
  }
  
  return(results);
}


// returns whether or not p_subject_id is a 'best hit' (see getBestHits())
bool Alignments::isABestHit(id_type const p_subject_id) const throw()
{
  vector< pair<id_type, float> > besthits = getBestHits();
  vector< pair<id_type, float> >::const_iterator itor = besthits.begin();
  vector< pair<id_type, float> >::const_iterator etor = besthits.end();

  for (itor = itor; itor != etor; ++itor)
  {
    if (p_subject_id == (*itor).first)
    {
      return(true);
    }
  }
 
  return(false);
}


// returns the (a) best hit
id_type Alignments::getBestHit(void) const throw()
{
  vector< pair<id_type, float> > besthits = getBestHits();

  return(besthits[0].first);
}


// returns the score of the best hits (all best hits will share the same score)
float Alignments::getBestScore(void) const throw()
{
  vector< pair<id_type, float> > besthits = getBestHits();

  return(besthits[0].second);
}


string Alignments::print(void) const throw()
{
  stringstream output;

  output << "NOT IMPLEMENTED YET!";

  return(output.str());
}


ostream & operator<<(ostream & p_os, Alignments const * p_alignment) throw()
{
  return(p_os << p_alignment->print());
}


ostream & operator<<(ostream & p_os, Alignments const & p_alignment) throw()
{
  return(p_os << p_alignment.print());
}
