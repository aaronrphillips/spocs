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
#include <config.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdio.h>
#if HAVE_STDLIB_H
#include <stdlib.h>
#endif
#include <string>
#include <vector>
// External Includes
// Internal Includes
#include <boost/lexical_cast.hpp>
// Application Includes
#include "Cluster.hpp"



#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "Cluster"


// Namespaces used
using namespace std;


// Constructors


Cluster::Cluster(Clusters                           & p_parent,
                 sharedLookup                 const & p_lookup,
                 sharedAlignmentsCollection   const & p_alignments,
                 Ortholog                     const   p_ortholog,
                 vector<id_type>              const   p_hits_A,
                 vector<id_type>              const   p_hits_B,
                 map<id_type, sharedCluster>  const   p_clus_ref_A,
                 map<id_type, sharedCluster>  const   p_clus_ref_B
                 ) throw() :
parent(p_parent),
lookup(p_lookup),
alignments(p_alignments),
ortholog_idA(p_ortholog.getIdA()),
ortholog_idB(p_ortholog.getIdB()),
ortholog_score(p_ortholog.getScore()),
A_id_to_confidence(map<id_type, float>()),
B_id_to_confidence(map<id_type, float>()),
A_clus_ref(p_clus_ref_A),
B_clus_ref(p_clus_ref_B),
ortholog_nodes(map<id_type, float>())
{
  ::g_output_helper->write(e_journal, "CLUSTER:" + lookup->getNameById(ortholog_idA) + "\t" +  lookup->getNameById(ortholog_idB) + "\t" + lexical_cast<string>(ortholog_score) + "\n");

  A_id_to_confidence.insert(pair<id_type, float>(ortholog_idA, (float)1.0));
  B_id_to_confidence.insert(pair<id_type, float>(ortholog_idB, 1.0));
  calculateConfidence(p_hits_A, p_hits_B);
  ortholog_nodes.insert(pair<id_type, float>(ortholog_idA, ortholog_score));
  ortholog_nodes.insert(pair<id_type, float>(ortholog_idB, ortholog_score));
  return;
}

// Public Methods

// calculate the confidence of each hit in the cluster
void Cluster::calculateConfidence(vector<id_type> const & p_hits_A, vector<id_type> const & p_hits_B) throw()
{
  vector<id_type>::const_iterator itor = p_hits_A.begin();
  vector<id_type>::const_iterator etor = p_hits_A.end();

  for (itor = itor; itor != etor; ++itor)
  {
    float inner = alignments->getScore(ortholog_idA, (*itor));
    float self  = alignments->getScore(ortholog_idA, ortholog_idA);
    float cross = alignments->getScore(ortholog_idA, ortholog_idB);

    float conf = 0.0;

    if (self != cross)
    {
      conf = (inner - cross) / (self - cross);
    }
    if ((self == cross) && (inner == cross))
    {
      conf = 1.0;
    }

    if (conf < lookup->getConfidenceCutoff()) 
    {
       ::g_output_helper->write(e_journal, "DROPPING id:" + lookup->getNameById(*itor) + " conf:" +  lexical_cast<string>(conf) + "\n");
      continue;
    }
    ::g_output_helper->write(e_journal,  "ADDING id:" + lookup->getNameById(*itor) + " conf:" +  lexical_cast<string>(conf) + "\n");

    A_id_to_confidence[(*itor)] = conf;
  }


  itor = p_hits_B.begin();
  etor = p_hits_B.end();

  for (itor = itor; itor != etor; ++itor)
  {
    float inner = alignments->getScore(ortholog_idB, (*itor));
    float self  = alignments->getScore(ortholog_idB, ortholog_idB);
    float cross = alignments->getScore(ortholog_idB, ortholog_idA);

    float conf = 0.0;

    if (self != cross)
    {
      conf = (inner - cross) / (self - cross);
    }
    if ((self == cross) && (inner == cross))
    {
      conf = 1.0;
    }

    if (conf < lookup->getConfidenceCutoff()) 
    {
       ::g_output_helper->write(e_journal, "DROPPING id:" + lookup->getNameById(*itor) + " conf:" +  lexical_cast<string>(conf) + "\n");
      continue;
    }

    ::g_output_helper->write(e_journal,  "ADDING id:" + lookup->getNameById(*itor) + " conf:" +  lexical_cast<string>(conf) + "\n");

    B_id_to_confidence[(*itor)] = conf;
  }

  return;
}


// go through all of this cluster's nodes and ensure that they are not already clustered somewhere else
// if they are, then either steal them or leave them alone and delete them here, depending on the confidence
bool Cluster::steal(species_type const p_species, id_type const p_parent_A, id_type const p_parent_B) throw()
{
  bool success = true;

  vector<id_type> nodes = getNodeIds(p_species);
  vector<id_type>::iterator itor = nodes.begin();
  vector<id_type>::iterator etor = nodes.end();

  sharedCluster other_clus;
  for (itor = itor; itor != etor; ++itor)
  {
    if (! checkForCluster(p_species, (*itor))) continue;
    other_clus = getCluster(p_species, (*itor));

    //  DEVELOPER NOTE: Don't steal from yourself
    if (other_clus->getIdA() == getIdA() && other_clus->getIdB() == getIdB()) continue;

    float conf       = getConfidence(p_species, (*itor));
    float conf_there = other_clus->getConfidence(p_species, (*itor));
    if (other_clus->partOfOrtho(p_species, (*itor)) && partOfOrtho(p_species, (*itor)))
    {
      // deleting from either cluster results in orphans: we're going to quarantine this cluster 
      success = false;

      //NOTE: we are going to 'continue' looking for other nodes to steal. It may make sense to 
      //      stop stealing, however, since all nodes in this cluster are going to be quarantined. 
      //      But in order to match the inparanoid script results we chose to 'continue' instead of
      //      'break'
      continue; 
    }

    if (other_clus->partOfOrtho(p_species, (*itor))                ||
        ((conf < conf_there) && (! partOfOrtho(p_species, (*itor)))) ||
        ( conf < lookup->getConfidenceCutoff()))
    {
      deleteNode(p_species, (*itor));
      continue; // don't steal
    }

    //
    // DEVELOPER NOTE: don't steal from the thing we're merging with
    //
    //if (p_parent_A == getIdA() && p_parent_B == getIdB())
    //{
    //  cout <<  "BORROW id:id from parent:p_parent_id\n";
    //}

    //
    // DEVELOPER NOTE:  We need to remove the node from the
    //                  previously existing cluster because we
    //                  have a better confidence.
    ::g_output_helper->write(e_journal, "STEAL id:" + lookup->getNameById(*itor) + " from cluster: " + lookup->getNameById(other_clus->getIdA()) + ":" + lookup->getNameById(other_clus->getIdB())  + "\n");
    other_clus->deleteNode(p_species, (*itor));
  }

  return(success);
}


// returns true if p_id is part of the ortholog for this cluster
bool Cluster::partOfOrtho(species_type const p_species, id_type const p_id) throw()
{
  if (p_species == e_species_A)
  {
    return(p_id == getIdA());
  }
  if (p_species == e_species_B)
  {
    return(p_id == getIdB());
  }

  return(false);
}


// delete a node from this cluster
void Cluster::deleteNode(species_type const p_species, id_type const p_id) throw()
{
  if (p_species == e_species_A)
  {
    A_id_to_confidence.erase(p_id);
  }
  if (p_species == e_species_B)
  {
    B_id_to_confidence.erase(p_id);
  }

  // let the parent 'Clusters' object know, so it can update its lookups and such
  parent.deleteNode(p_species, p_id);
  return;
}


// merge p_cluster into this cluster
bool Cluster::merge(sharedCluster p_cluster) throw()
{
  // tell the cluster we are merging with to steal all the nodes it needs ('hanger-onners')
  bool success1 = p_cluster->steal(e_species_A, getIdA(), getIdB());
  bool success2 = p_cluster->steal(e_species_B, getIdA(), getIdB());


  // add the A nodes
  vector<id_type> a_nodes = p_cluster->getNodeIds(e_species_A);
  vector<id_type>::const_iterator itor = a_nodes.begin();
  vector<id_type>::const_iterator etor = a_nodes.end();
  for (itor = itor; itor != etor; ++itor)
  {
    map<id_type, float>::iterator ftor  = A_id_to_confidence.find(*itor);
    map<id_type, float>::iterator fetor = A_id_to_confidence.end();
    if (ftor != fetor) continue;

    A_id_to_confidence.insert(pair<id_type, float>((*itor),  p_cluster->getConfidence(e_species_A, (*itor))));
  }

  // add the B nodes
  vector<id_type> b_nodes = p_cluster->getNodeIds(e_species_B);
  itor = b_nodes.begin();
  etor = b_nodes.end();
  for (itor = itor; itor != etor; ++itor)
  {
    map<id_type, float>::iterator ftor  = B_id_to_confidence.find(*itor);
    map<id_type, float>::iterator fetor = B_id_to_confidence.end();
    if (ftor != fetor) continue;

    B_id_to_confidence.insert(pair<id_type, float>((*itor),  p_cluster->getConfidence(e_species_B, (*itor))));
  }


  // add the ortholog_nodes
  map<id_type, float>::const_iterator mitor = p_cluster->getOrthologNodes().begin();
  map<id_type, float>::const_iterator metor = p_cluster->getOrthologNodes().end();
  for (mitor = mitor; mitor != metor; ++mitor)
  {
    map<id_type, float>::const_iterator ftor  = ortholog_nodes.find((*mitor).first);
    map<id_type, float>::const_iterator fetor = ortholog_nodes.end();
    if (ftor != fetor) continue;

    ortholog_nodes.insert(*mitor);
  }

  return(success1 && success2);

}


// returns true if p_id was ever an ortholog with score p_score in this cluster
bool Cluster::check4CompetingOrthologPair(species_type const p_species, id_type const p_id, float const p_score) throw()
{
  map<id_type, float>::iterator ftor = ortholog_nodes.find(p_id);
  map<id_type, float>::iterator etor = ortholog_nodes.end();

  if (ftor != etor)
  {
    if ((*ftor).second == p_score)
    {
      return(true);
    }
  }
  
  return(false);
}


// prints a line representing the cluster, which eventually is printed to a *.paralogs file
string Cluster::print(void) throw()
{
  stringstream output;

  output << setiosflags(ios::fixed) << setprecision(0) << getScore() << "\t";

  vector<id_type> a_nodes = getNodeIds(e_species_A);
  vector<id_type>::const_iterator itor = a_nodes.begin();
  vector<id_type>::const_iterator etor = a_nodes.end();

  u_int16_t max_width = lookup->getMaxNameLength();

  for (itor = itor; itor != etor; ++itor)
  {
    output << setw(max_width) << lookup->getNameById(*itor) << " " << setiosflags(ios::fixed) << setprecision(3) << A_id_to_confidence[*itor] << " ";
  }

  output << "\t";

  vector<id_type> b_nodes = getNodeIds(e_species_B);
  itor = b_nodes.begin();
  etor = b_nodes.end();

  for (itor = itor; itor != etor; ++itor)
  {
    output << setw(max_width) << lookup->getNameById(*itor) << " " << setiosflags(ios::fixed) << setprecision(3) << B_id_to_confidence[*itor] << " ";
  }

  return(output.str());
}


// Operators


ostream & operator<<(ostream & p_os, Cluster * p_cluster) throw()
{
  return(p_os << p_cluster->print());
}


ostream & operator<<(ostream & p_os, Cluster & p_cluster) throw()
{
  return(p_os << p_cluster.print());
}
