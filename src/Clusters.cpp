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
// External Includes
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
// Internal Includes
#include <boost/lexical_cast.hpp>
// Application Includes
#include "Cluster.hpp"
#include "Clusters.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "Clusters"


// Namespaces used
using namespace std;


// Constructors


Clusters::Clusters(sharedLookup               const & p_lookup,
                   sharedAlignmentsCollection const & p_alignments) throw() :
current_id(1),
lookup(p_lookup),
alignments(p_alignments),
A_node_to_cluster(),
B_node_to_cluster(),
clusters()
{
  return;
}


// 'clusterize' the orthologs
void Clusters::makeClusters(void) throw()
{
  list<Ortholog>::const_iterator ort_itor = alignments->getOrthologs().begin();
  list<Ortholog>::const_iterator ort_etor = alignments->getOrthologs().end();

  for (ort_itor = ort_itor; ort_itor != ort_etor; ++ort_itor)
  {
    id_type idA   = (*ort_itor).getIdA();
    id_type idB   = (*ort_itor).getIdB();
    float   score = (*ort_itor).getScore();

    vector<id_type> hits_A = alignments->getHitsBetterThan(e_species_A, idA, score);
    vector<id_type> hits_B = alignments->getHitsBetterThan(e_species_B, idB, score);

    addOrtholog((*ort_itor), hits_A, hits_B);
  }

  return;
}


int Clusters::addOrtholog(Ortholog               const & p_ortholog,
                          vector<id_type>        const & p_hits_A,
                          vector<id_type>        const & p_hits_B) throw()
{
  vector<id_type>::const_iterator itor = p_hits_A.begin();
  vector<id_type>::const_iterator etor = p_hits_A.end();

  // find all of the hits on A
  vector<id_type> a_trimmed;
  map<id_type, sharedCluster> node_to_clus_A;

  for (itor = itor; itor != etor; ++itor)
  {
    ::g_output_helper->write(e_journal, "addOrthoA " + lookup->getNameById(*itor) + "\n");

    if(isOrthologNode(*itor)) continue;

    a_trimmed.push_back(*itor);
    // see if this node is in another cluster
    id_type clus_id = getClusterId(e_species_A, (*itor));

    if (clus_id > 0)
    {
      node_to_clus_A.insert(pair<id_type, sharedCluster>((*itor), getCluster(clus_id))); 
    }
  }

  // find all of the hits on B
  itor = p_hits_B.begin();
  etor = p_hits_B.end();

  vector<id_type> b_trimmed;
  map<id_type, sharedCluster> node_to_clus_B;

  for (itor = itor; itor != etor; ++itor)
  {
    ::g_output_helper->write(e_journal, "addOrthoB " + lookup->getNameById(*itor) + "\n");

    if(isOrthologNode(*itor)) continue;

    b_trimmed.push_back(*itor);
    // see if this node is in another cluster
    id_type clus_id = getClusterId(e_species_B, (*itor));

    if (clus_id > 0)
    {
      node_to_clus_B.insert(pair<id_type, sharedCluster>((*itor), getCluster(clus_id))); 
    }
  }

  // create a new cluster based on this ortholog, then try to fit it into the existing model (may stand on
  // its own, may merge with an existing cluster, or we may just throw it away)
  sharedCluster cluster(new Cluster(*this, lookup, alignments, p_ortholog, a_trimmed, b_trimmed, node_to_clus_A, node_to_clus_B));
  int ret_val = updateClusters(p_ortholog, cluster);
  return(ret_val);
}


// try to fit p_cluster into the existing model (may stand on its own, may merge with an existing 
// cluster, or we may just throw it away)
// NOTE: based off the inparanoid paper
int Clusters::updateClusters(Ortholog const & p_ortholog, sharedCluster p_cluster) throw()
{
  id_type idA   = p_ortholog.getIdA();
  id_type idB   = p_ortholog.getIdB();
  float   score = p_ortholog.getScore();

  id_type cluster_A = getClusterId(e_species_A, idA);
  id_type cluster_B = getClusterId(e_species_B, idB);

  bool    merge      = false;
  id_type merge_clus = 0;
  string  merge_case("-1");

  ::g_output_helper->write(e_journal, "CHECKING idA:" + lookup->getNameById(idA) + " cluster_A:" + lexical_cast<string>(cluster_A) + " idB:" + lookup->getNameById(idB) + " cluster_B:" + lexical_cast<string>(cluster_B) + "\n");

  if (cluster_A != 0 && cluster_A == cluster_B)
  {
    merge      = true;
    merge_clus = cluster_A;
    merge_case = "1";
  }
  else if ((cluster_A > 0) || (cluster_B > 0))
  {
    if (cluster_A > 0 && getCluster(cluster_A)->check4CompetingOrthologPair(e_species_A, idA, score))    {
      merge      = true;
      merge_clus = cluster_A;
      merge_case = "2 A";
    }
    else if (cluster_B > 0 && getCluster(cluster_B)->check4CompetingOrthologPair(e_species_B, idB, score))
    {
      merge      = true;
      merge_clus = cluster_B;
      merge_case = "2 B";
    }
    else if ((cluster_A > 0) && ((getCluster(cluster_A)->getScore() - p_cluster->getScore()) > lookup->getScoreCutoff()))
    {
      merge      = false;
      merge_clus = cluster_A;
      merge_case = "3 A";
      ::g_output_helper->write(e_journal, "CLUSTER CASE 3 A " + lookup->getNameById(idA) +  "-" +  lookup->getNameById(idB) +  "\n");
      return(-1);
    }
    else if ((cluster_B > 0) && ((getCluster(cluster_B)->getScore() - p_cluster->getScore()) > lookup->getScoreCutoff()))
    {
      merge      = false;
      merge_clus = cluster_B;
      merge_case = "3 B";
      ::g_output_helper->write(e_journal, "CLUSTER CASE 3 B PREV:" + lexical_cast<string>(merge_clus) + lookup->getNameById(idA) +  "-" +  lookup->getNameById(idB) +  "\n");
      return(-1);
    }
    else if ((cluster_A > 0) && (getCluster(cluster_A)->getConfidence(e_species_A, idA) >= lookup->getGroupOverlapCutoff()))
    {
      merge      = true;
      merge_clus = cluster_A;
      merge_case = "4 A";
    }
    else if ((cluster_B > 0) && (getCluster(cluster_B)->getConfidence(e_species_B, idB) >= lookup->getGroupOverlapCutoff()))
    {
      merge      = true;
      merge_clus = cluster_B;
      merge_case = "4 B";
    }
  }

  if (merge == true)
  {
    ::g_output_helper->write(e_journal, "CLUSTER CASE " + merge_case + " PREV:" + lexical_cast<string>(merge_clus) + " " + lookup->getNameById(idA) + '-' + lookup->getNameById(idB) + "\n");
    ::g_output_helper->write(e_journal, "NEW cluster\n");
    getCluster(merge_clus)->print();

    // if keeping this cluster will duplicate nodes (or otherwise wreak havoc), just throw it away
    if (! getCluster(merge_clus)->merge(p_cluster))
    {
      quarantine[merge_clus] = p_cluster;
    }
    ::g_output_helper->write(e_journal, "MERGED cluster\n");
    ::g_output_helper->write(e_journal, "END CLUSTER CASE " + merge_case + " PREV:" + lexical_cast<string>(merge_clus) + " " + lookup->getNameById(idA) + '-' + lookup->getNameById(idB) + "\n");
    updateClusterIds(merge_clus, getCluster(merge_clus));
    return(merge_clus);
  }

  ::g_output_helper->write(e_journal, "CLUSTER ADD CASE\n");
  id_type clus_id  = current_id;
  ++current_id;

  ::g_output_helper->write(e_journal, "CREATING CLUSTER " +  lexical_cast<string>(clus_id) + "\n");

  // tell this cluster to go steal any 'hanger-onners'
  bool success1 = p_cluster->steal(e_species_A, p_cluster->getIdA(), p_cluster->getIdB()); 
  bool success2 = p_cluster->steal(e_species_B, p_cluster->getIdA(), p_cluster->getIdB());
  if (! (success1 && success2))
  {
    quarantine[clus_id] = p_cluster;
    ::g_output_helper->write(e_journal, "QUARANTINING cluster " +  lexical_cast<string>(clus_id) + "\n");
  }
  else
  {
    clusters[clus_id] = p_cluster;
    updateClusterIds(clus_id, p_cluster);
  }

  return(current_id);
}


void Clusters::deleteNode(species_type p_species, id_type p_id) throw()
{
  if (p_species == e_species_A)
  {
    A_node_to_cluster.erase(p_id);
  }
  if (p_species == e_species_B)
  {
    B_node_to_cluster.erase(p_id);
  }

  return;
}


// update the master list of the location of nodes in clusters to reflect recent changes
void Clusters::updateClusterIds(id_type const p_clus_id, sharedCluster p_cluster) throw()
{
  Ortholog_node_to_cluster[p_cluster->getIdA()] = p_clus_id;
  Ortholog_node_to_cluster[p_cluster->getIdB()] = p_clus_id;

  vector<id_type> idsA = p_cluster->getNodeIds(e_species_A);
  vector<id_type>::iterator itor = idsA.begin();
  vector<id_type>::iterator etor = idsA.end();

  for (itor = itor; itor != etor; ++itor)
  {
    A_node_to_cluster[(*itor)] = p_clus_id;
  }

  vector<id_type> idsB = p_cluster->getNodeIds(e_species_B);
  itor = idsB.begin();
  etor = idsB.end();

  for (itor = itor; itor != etor; ++itor)
  {
    B_node_to_cluster[(*itor)] = p_clus_id;
  }
  
  return;
}


// Public Methods

string Clusters::print(void) throw()
{
  stringstream output;

  map<id_type, sharedCluster>::iterator itor = clusters.begin();
  map<id_type, sharedCluster>::iterator etor = clusters.end();

  for (itor = itor; itor != etor; ++itor)
  {
    output << (*itor).first << "\t" << (*itor).second << "\n";
  }

  return(output.str());
}


// Operators


ostream & operator<<(ostream & p_os, Clusters * p_clusters) throw()
{
  return(p_os << p_clusters->print());
}


ostream & operator<<(ostream & p_os, Clusters & p_clusters) throw()
{
  return(p_os << p_clusters.print());
}
