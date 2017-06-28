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


#ifndef ALIGNMENTS_COLLECTIONS_HPP
#define ALIGNMENTS_COLLECTIONS_HPP


// System Includes
#include <string>
// External Includes
#include "boost/shared_ptr.hpp"
#include "boost/tuple/tuple_io.hpp"
// Internal Includes
// Application Includes
#include "global.hpp"
#include "Alignments.hpp"
#include "Lookup.hpp"
#include "Ortholog.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "AlignmentsCollection"


// Namespaces used
using namespace std;
using namespace boost;


struct myKey
{
  myKey(species_type s1, species_type s2) :
    key(s1, s2)
  {
    return;
  }

  tuple<species_type, species_type> key;

  bool operator<(myKey const & b) const throw()
  {
    if (  key.get<0>() < b.key.get<0>()) { return(true);  }
    if (b.key.get<0>() <   key.get<0>()) { return(false); }
    if (  key.get<1>() < b.key.get<1>()) { return(true);  }
    if (b.key.get<1>() <   key.get<1>()) { return(false); }

    return(false);
  }

};


typedef  map<id_type, boost::shared_ptr<Alignments> >  qid_map;
typedef  map<myKey, qid_map >                   collection_type;


class AlignmentsCollection
{

public:

  // Getters & Setters

  inline void addScore(id_type const p_id_A, id_type const p_id_B, float const p_score) throw()
  {
    num_to_score[p_id_A][p_id_B] = p_score;
  }


  inline list<Ortholog> & getOrthologs(void) throw()
  {
    if (! orthologs_sorted)
    {
      orthologs.sort();
      orthologs_sorted = true;
    }

    return(this->orthologs);
  }


  inline void addOrtho(id_type p_species_1, id_type p_species_2) throw()
  {
    orthologs.push_back(Ortholog(p_species_1, p_species_2, getScore(p_species_1, p_species_2)));

    return;
  }


  inline float getScore(id_type const p_species_A, id_type const p_species_B)
  {
    map<id_type, map<id_type, float> >::const_iterator etor = num_to_score.end();
    map<id_type, map<id_type, float> >::const_iterator ftor = num_to_score.find(p_species_A);
    if (ftor != etor)
    {   
      map<id_type, float>::const_iterator ietor = (*ftor).second.end();
      map<id_type, float>::const_iterator iftor = (*ftor).second.find(p_species_B);

      if (iftor != ietor)
      {   
        return((*iftor).second);
      }   
    }   
    return(0.0);
  }


  // Constructors

  AlignmentsCollection(sharedLookup const & p_lookup) throw();


  // Destructor

  ~AlignmentsCollection(void) throw()
  {
    return;
  }


  // Public Methods

  void            equalize(void) throw();
  string          print(void) const throw();
  string          printOrthologs(void) throw();
  void            findOrthologs(void) throw();
  void            getAlignments(vector< tuple<string, species_type, species_type> > const & p_outfiles) throw();
  void            addHitFile(species_type const p_species_A, species_type const p_species_B, string const & p_filename) throw();
  vector<id_type> getHitsBetterThan(species_type const p_species, id_type const p_id, float const p_score) throw();


private:

  // Getters/Setters


  // Constructors

  AlignmentsCollection(void) throw();
  AlignmentsCollection(AlignmentsCollection const & p_alignments) throw();


  // Operators

  bool operator=(AlignmentsCollection const & p_alignments) const throw();


  // Methods


  // Variables

  sharedLookup    lookup;
  collection_type collection;
  list<Ortholog>  orthologs;
  bool            orthologs_sorted;

  //num_to_score - used to find the score of an alignment by it's
  map<id_type, map<id_type, float> >  num_to_score;


friend
  ostream & operator<<(ostream & p_os, AlignmentsCollection const * p_alignments) throw();
friend
  ostream & operator<<(ostream & p_os, AlignmentsCollection const & p_alignments) throw();

};


typedef boost::shared_ptr<AlignmentsCollection> sharedAlignmentsCollection;


#endif // ALIGNMENTS_COLLECTIONS_HPP
