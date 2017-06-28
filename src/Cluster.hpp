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


#ifndef CLUSTER_HPP
#define CLUSTER_HPP


// System Includes
#include <map>
#include <string>
// External Includes
#include "boost/bind.hpp"
#include "boost/shared_ptr.hpp"
// Internal Includes
// Application Includes
#include "global.hpp"
#include "AlignmentsCollection.hpp"
#include "Clusters.hpp"
#include "Lookup.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "Cluster"


// Namespaces used
using namespace std;
using namespace boost;


class Clusters;
typedef boost::shared_ptr<Clusters> sharedClusters;
class Cluster;
typedef boost::shared_ptr<Cluster>  sharedCluster;


class Cluster
{

public:

  // Getters & Setters

  inline id_type getIdA(void) const throw()
  {
    return(ortholog_idA);
  }


  inline id_type getIdB(void) const throw()
  {
    return(ortholog_idB);
  }


  inline float getScore(void) const throw()
  {
    return(ortholog_score);
  }


  inline float getConfidence(species_type const p_species, id_type const p_id) throw()
  {
    if (p_species == e_species_A)
    {
      return(A_id_to_confidence[p_id]);
    }

    if (p_species == e_species_B)
    {
      return(B_id_to_confidence[p_id]);
    }

    return(0.0);
  }


  inline bool checkForCluster(species_type const p_species, id_type const p_id) throw()
  {
    map<id_type, sharedCluster >::const_iterator itor;
    map<id_type, sharedCluster >::const_iterator etor;

    if (p_species == e_species_A)
    {   
      itor = A_clus_ref.find(p_id);
      etor = A_clus_ref.end();
    }     

    if (p_species == e_species_B)
    {   
      itor = B_clus_ref.find(p_id);
      etor = B_clus_ref.end();
    }     

    if (itor != etor)
    {   
      return(true);
    }     

    return(false);
  }


  inline sharedCluster getCluster(species_type const p_species, id_type const p_id) throw()
  {
    map<id_type, sharedCluster >::const_iterator itor;
    map<id_type, sharedCluster >::const_iterator etor;

    if (p_species == e_species_A)
    {
      itor = A_clus_ref.find(p_id);
      etor = A_clus_ref.end();
    }

    if (p_species == e_species_B)
    {
      itor = B_clus_ref.find(p_id);
      etor = B_clus_ref.end();
    }

    //  DEVELOPER NOTE: The call to getCluster MUST ONLY be used AFTER
    //                  checkForCluster returns true;
    return((*itor).second);
  }


  inline vector<id_type> getNodeIds(species_type const p_species) const throw()
  {
    vector< pair<id_type, float> > pairs;
    map<id_type, float>::const_iterator itor;
    map<id_type, float>::const_iterator etor;

    if (p_species == e_species_A)
    {
      itor = A_id_to_confidence.begin();
      etor = A_id_to_confidence.end();
    }
    if (p_species == e_species_B)
    {
      itor = B_id_to_confidence.begin();
      etor = B_id_to_confidence.end();
    }

    for (itor = itor; itor != etor; ++itor)
    {
      pairs.push_back(*itor);
    }

    std::sort(pairs.begin(), pairs.end(),
              boost::bind(&std::pair<id_type, float>::second, _1) >
              boost::bind(&std::pair<id_type, float>::second, _2));

    vector<id_type> keys;
    vector< pair<id_type, float> >::const_iterator pitor = pairs.begin();
    vector< pair<id_type, float> >::const_iterator petor = pairs.end();
    for (pitor = pitor; pitor != petor; ++pitor)
    {
      keys.push_back((*pitor).first);
    }

    return(keys);
  }


  inline map<id_type, float> & getOrthologNodes(void) throw()
  {
    return(ortholog_nodes);
  }


  // Constructors

  Cluster(Clusters                            & p_parent,
          sharedLookup                  const & p_lookup,
          sharedAlignmentsCollection    const & p_alignments,
          Ortholog                      const   p_ortholog,
          vector<id_type>               const   p_hits_A,
          vector<id_type>               const   p_hits_B,
          map<id_type, sharedCluster >  const   p_clus_ref_A,
          map<id_type, sharedCluster >  const   p_clus_ref_B) throw(); 


  // Destructor

  ~Cluster(void) throw()
  {
    return;
  }


  // Public Methods

  bool   check4CompetingOrthologPair(species_type const p_species, id_type const p_id, float const p_score) throw();
  bool   merge(sharedCluster const p_cluster) throw();
  bool   steal(species_type const p_species, id_type const p_idA, id_type const p_idB) throw();
  void   calculateConfidence(vector<id_type> const & p_hits_A, vector<id_type> const & p_hits_B) throw();
  void   deleteNode(species_type const p_species, id_type const p_id) throw();
  bool   partOfOrtho(species_type const p_species, id_type const p_id) throw();
  string print(void) throw();


private:

  // Getters/Setters


  // Constructors

  Cluster(void) throw();
  Cluster(Cluster const & p_cluster) throw();


  // Operators

  bool operator=(Cluster const & p_cluster) const throw();


  // Methods


  // Variables

  Clusters                    & parent;
  sharedLookup                  lookup;
  sharedAlignmentsCollection    alignments;
  id_type                       ortholog_idA;
  id_type                       ortholog_idB;
  float                         ortholog_score;

  map<id_type, float>           A_id_to_confidence;
  map<id_type, float>           B_id_to_confidence;
  map<id_type, sharedCluster >  A_clus_ref;
  map<id_type, sharedCluster >  B_clus_ref;
  map<id_type, float>           ortholog_nodes;


friend
  ostream & operator<<(ostream & p_os, Cluster * p_cluster) throw();
friend
  ostream & operator<<(ostream & p_os, Cluster & p_cluster) throw();

};


#endif // CLUSTER_HPP
