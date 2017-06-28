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


#ifndef ALIGNMENTS_HPP
#define ALIGNMENTS_HPP


// System Includes
#include <list>
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
#define __CLASS__ "Alignments"


// Namespaces used
using namespace std;
using namespace boost;


typedef  map<id_type, float>   hits_type;
typedef  pair<id_type, float>  hit_type;


class Alignments
{

public:

  // Getters & Setters

  inline u_int32_t getNumHits(void) const throw()
  {
    return(this->hits.size());
  }


  inline vector<id_type> getHits(void) const throw()
  {
    return(this->subject_ids);
  }


  // Constructors

  Alignments(
             species_type const p_species_A,
             species_type const p_species_B,
             id_type      const p_query_id,
             id_type      const p_subject_id,
             float        const p_score
            ) throw();


  // Destructor

  ~Alignments(void) throw()
  {
    return;
  }


  // Public Methods

  void               add(id_type const p_subject_id, float const p_score) throw();
  id_type            getBestHit(void) const throw();
  float              getBestScore(void) const throw();
  vector< hit_type > getSortedHits(void) const throw();
  vector< hit_type > getBestHits(void) const throw();
  vector<id_type>    getHitsBetterThan(float const p_score) const throw();
  bool               isABestHit(id_type const p_subject_id) const throw();
  string             print(void) const throw();


private:

  // Getters/Setters


  // Constructors

  Alignments(void) throw();
  Alignments(Alignments const & p_alignments) throw();


  // Operators

  bool operator=(Alignments const & p_alignments) const throw();


  // Methods


  // Variables

  species_type     query_type;
  species_type     subject_type;
  id_type          query_id;
  vector<id_type>  subject_ids;
  hits_type        hits;


friend
  ostream & operator<<(ostream & p_os, Alignments const * p_alignments) throw();
friend
  ostream & operator<<(ostream & p_os, Alignments const & p_alignments) throw();

};


typedef boost::shared_ptr<Alignments> sharedAlignments;


#endif // ALIGNMENTS_HPP
