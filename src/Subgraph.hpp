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


#ifndef SUBGRAPH_HPP
#define SUBGRAPH_HPP


// System Includes
#include <list>
#include <string>
// External Includes
#include "boost/shared_ptr.hpp"
// Internal Includes
// Application Includes
#include "global.hpp"
#include "Clique.hpp"
#include "Lookup.hpp"
#include "Solver.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "Subgraph"


// Namespaces used
using namespace std;
using namespace boost;


class Subgraph
{

public:

  // Getters & Setters

  inline string getGraphOutputDir(void) const throw()
  {
    return(this->graph_dir + '/');
  }


  inline u_int32_t getNodeCount(void) const throw()
  {
    return(this->my_node_count);
  }


  inline graph_type_t getGraphType(void) throw()
  {
    return(this->type);
  }


  inline u_int32_t getGraphNumber(void) throw()
  {
    return(Subgraph::subgraph_number);
  }


  // Constructors

  Subgraph(string       const & p_graph_dir,
           sharedEdges  const & p_graph, 
           sharedLookup const & p_lookup, 
           u_int32_t    const   p_min_edges,
           u_int32_t    const   p_max_edges,
           float        const   p_edge_threshold,
           bool         const   p_produce_html) throw();


  // Destructor

  ~Subgraph(void) throw()
  {
    return;
  }


  // Public Methods

  void   updateClique(sharedEdges & p_graph, sharedClique & p_clique) throw();
  void   saveCliqueJson(sharedEdges const & p_graph) throw();
  void   saveSubgraphJson(void) throw();
  string getShortId(string const & p_value) const throw();
  string print(void) const throw();


  // Variables

  static u_int32_t subgraph_number;


private:

  // Getters/Setters


  // Constructors

  Subgraph(void) throw();
  Subgraph(Subgraph const & p_subgraph) throw();


  // Operators

  bool operator=(Subgraph const & p_subgraph) const throw();


  // Methods

  void   saveFasta(sharedEdges const & p_graph, string const & p_filename) throw();
  void   saveJson( string const & p_file_name,sharedEdges const & p_edges, map<string, int> & p_vertices) throw();


  // Variables
  string                    graph_dir;
  sharedEdges               edges;
  sharedLookup              lookup;
  graph_type_t              type;
  graph_type_t              sub_type;
  map<u_int32_t, u_int32_t> vertices;
  map<string,    u_int32_t> species;
  u_int32_t                 min_nodes;
  u_int32_t                 max_nodes;
  float                     edge_threshold;
  u_int32_t                 orig_edge_count;
  u_int32_t                 edge_count;
  u_int32_t                 max_edge_count;
  u_int32_t                 my_node_count;
  u_int32_t                 species_count;
  Solver *                  solver;
  list<string>              solution;
  u_int32_t                 clique_number;
  bool                      produce_html;
  sharedEdges               original_edges;
  map<u_int32_t, u_int32_t> group;
  map<string, int>          original_vertices;


friend
  ostream & operator<<(ostream & p_os, Subgraph const * p_subgraph) throw();
friend
  ostream & operator<<(ostream & p_os, Subgraph const & p_subgraph) throw();

};


typedef boost::shared_ptr<Subgraph> sharedSubgraph;


#endif // SUBGRAPH_HPP
