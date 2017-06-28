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


#ifndef CLUSTERS_HPP
#define CLUSTERS_HPP


// System Includes
#include <map>
#include <string>
// External Includes
#include "boost/shared_ptr.hpp"
// Internal Includes
// Application Includes
#include "global.hpp"
#include "AlignmentsCollection.hpp"
#include "Cluster.hpp"
#include "Lookup.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "Clusters"


// Namespaces used
using namespace std;
using namespace boost;


class Cluster;
typedef boost::shared_ptr<Cluster> sharedCluster;


class Clusters
{

public:

  // Getters & Setters


  inline bool isOrthologNode(id_type const p_id) const throw()
  {
    map<id_type, id_type>::const_iterator itor = Ortholog_node_to_cluster.find(p_id);
    map<id_type, id_type>::const_iterator etor = Ortholog_node_to_cluster.end();

    if (itor != etor)
    {
      return(true);
    }

    return(false);
  }


  inline sharedCluster getCluster(id_type const p_id) const throw()
  {
    //  NOTE: You should NEVER call this function without calling
    //        getClusterId first because this function cannot return
    //        an empty sharedCluster object and no error checking is done.

    map<id_type, sharedCluster>::const_iterator itor = clusters.find(p_id);
    //map<id_type, sharedCluster>::const_iterator etor = clusters.end();

    return((*itor).second);
  }


  inline id_type getClusterId(species_type const p_species, id_type const p_id) const throw()
  {
    map<id_type, id_type>::const_iterator itor;
    map<id_type, id_type>::const_iterator etor;
    
    if (p_species == e_species_A)
    {
      itor = A_node_to_cluster.find(p_id);
      etor = A_node_to_cluster.end();
    }
    else
    {
      itor = B_node_to_cluster.find(p_id);
      etor = B_node_to_cluster.end();
    }

    if (itor != etor)
    {
      return((*itor).second);
    }

    return(0);
  }


  // Constructors

  Clusters(sharedLookup const & p_lookup, sharedAlignmentsCollection const & p_alignments) throw(); 


  // Destructor

  ~Clusters(void) throw()
  {
    return;
  }


  // Public Methods

  int    addOrtholog(Ortholog const & p_ortholog, vector<id_type> const & p_hits_A, vector<id_type> const & p_hits_B) throw();
  void   makeClusters(void) throw();
  string print(void) throw();
  int    updateClusters(Ortholog const & p_ortholog, sharedCluster p_cluster) throw();
  void   updateClusterIds(id_type const p_id, sharedCluster p_cluster) throw();
  void   deleteNode(species_type const p_species, id_type const p_id) throw();


private:

  // Getters/Setters


  // Constructors

  Clusters(void) throw();
  Clusters(Clusters const & p_cluster) throw();


  // Operators

  bool operator=(Clusters const & p_cluster) const throw();


  // Methods


  // Variables

  id_type                     current_id;
  sharedLookup                lookup;
  sharedAlignmentsCollection  alignments;

  //  The following maps are node id to which cluster id they are in
  //  For example, A_node_to_cluster[5] = 74 means that the 5th node
  //  in the Species A FASTA file is currently in the 74th cluster
  map<id_type, id_type>       A_node_to_cluster;
  map<id_type, id_type>       B_node_to_cluster;
  map<id_type, id_type>       Ortholog_node_to_cluster;
  map<id_type, sharedCluster> clusters;
  map<id_type, sharedCluster> quarantine;


friend
  ostream & operator<<(ostream & p_os, Clusters * p_cluster) throw();
friend
  ostream & operator<<(ostream & p_os, Clusters & p_cluster) throw();

};


typedef boost::shared_ptr<Clusters> sharedClusters;


#endif // CLUSTERS_HPP
