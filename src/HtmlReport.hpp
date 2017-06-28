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


#ifndef HTML_REPORT_HPP
#define HTML_REPORT_HPP


// System Includes
#include <list>
#include <string>
#include <vector>
// External Includes
#include "boost/shared_ptr.hpp"
// Internal Includes
// Application Includes
#include "global.hpp"
#include "Lookup.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "HtmlReport"


// Namespaces used
using namespace std;
using namespace boost;


struct clique_struct
{
  graph_type_t   type;
  int            clique_number;
  int            node_count;
  int            edge_count;
  vector<string> node_names;

  bool operator<(clique_struct const & p_clique) const throw()
  {
    if (edge_count > p_clique.edge_count) { return(true); }
    return(false);
  }

};


struct subgraph_struct
{
  graph_type_t        type;
  int                 graph_number;
  int                 max_edges;
  list<clique_struct> cliques;

  bool operator<(subgraph_struct const & p_subgraph) const throw()
  {
    if (max_edges > p_subgraph.max_edges) { return(true); }
    return(false);
  }

};



class HtmlReport
{

public:

  // Getters & Setters

  // Constructors

  HtmlReport(void) throw();


  // Destructor

  ~HtmlReport(void) throw()
  {
    return;
  }


  // Public Methods

  void add(graph_type_t const p_type, int const p_graph_number, int const p_clique_number, int const p_node_count, int const p_edge_count, vector<string> const p_node_names) throw();
  void addSubgraphEdges(int const p_graph_number, sharedEdges & p_edges) throw();
  void makeReport(void) throw();
  string getJsonData(const string&) throw();
  string print(void) throw();
  void initialize(sharedLookup const & p_lookup, string const & p_output_dir) throw();


  // Variables


private:

  // Getters/Setters


  // Constructors

  //HtmlReport(void) throw();
  HtmlReport(HtmlReport const & p_html_report) throw();


  // Operators

  bool operator=(HtmlReport const & p_html_report) const throw();


  // Methods

  string getDisclaimer(void) throw();
  void   makeCliqueReport(int const p_graph_number, clique_struct & p_clique, string const & p_graph_type) throw();
  void   makeSubgraphReport(subgraph_struct & p_subgraph, graph_type_t const p_type) throw();
  string getRejectedFastaIdTable(void) throw();


  // Variables

  sharedLookup                              lookup;
  string                                    full_output_dir;
  string                                    output_dir;
  map<graph_type_t, list<subgraph_struct> > report;
  map<int, map<string, string> >            subgraph_to_vertice_descriptions;
  map<int, int>                             edge_count;


  // Friends

friend
  ostream & operator<<(ostream & p_os, HtmlReport * p_html_report) throw();
friend
  ostream & operator<<(ostream & p_os, HtmlReport & p_html_report) throw();

};


typedef boost::shared_ptr<HtmlReport> sharedHtmlReport;


#endif // HTML_REPORT_HPP
