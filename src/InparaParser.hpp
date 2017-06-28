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


#ifndef INPARAPARSER_HPP
#define INPARAPARSER_HPP


// System Includes
#include <map>
#include <string>
#include <vector>
// External Includes
#include <boost/shared_ptr.hpp>
// Internal Includes
// Application Includes
#include "global.hpp"
#include "Lookup.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "InparaParser"


// Namespaces used
using namespace std;
using namespace boost;


typedef vector< vector<int> >              graph_t;
//typedef pair<int, float>                   node_t;
typedef tuple<int, float, int>             node_t;
typedef vector< node_t >                   node_list_t;
//typedef map<int, node_list_t > edge_map_t;
typedef map<int, pair<int, node_list_t > > edge_map_t;


class InparaParser
{

public:

  // Getters & Setters

  inline u_int16_t getBadParalogCutoff(void) const throw()
  {
    return(this->bad_paralog_cutoff);
  }


  inline float getConfidenceCutoff(void) const throw()
  {
    return(this->confidence_cutoff);
  }


  // Constructors

  InparaParser(string          const & p_data_dir,
               float           const   p_confidence_cutoff,
               u_int16_t       const   p_bad_paralog_cutoff,
               vector<string>  const & p_fasta_files,
               sharedLookup    const & p_lookup) throw();


  // Destructor

  ~InparaParser(void) throw()
  {
//cout << "";
    return;
  }


  // Public Methods

  string  print(void) const throw();
  void    createGeneVector(string const & begin, vector< pair<string, float> > & vec) throw();
  void    correlate(u_int32_t const blast_score, vector< pair<string, float> > & vec1, vector< pair<string, float> > & vec2) throw();
  graph_t getGraph(void) throw();
  boost::shared_ptr< vector<sharedEdges> > getSubgraphs(void) throw();


private:

  // Getters/Setters


  // Constructors

  InparaParser(void) throw();
  InparaParser(InparaParser const & p_inpara_parser) throw();


  // Operators

  bool operator=(InparaParser const & p_inpara_parser) const throw();


  // Methods

  void AddToList(u_int32_t           const   p_score,
                 pair<string, float> const & p_pair1,
                 pair<string, float> const & p_pair2) throw();


  // Variables

  string          data_dir;
  float           confidence_cutoff;
  u_int16_t       bad_paralog_cutoff;
  vector<string>  fasta_files;
  sharedLookup    lookup;
  graph_t         graph;
  edge_map_t      edges;


friend
  ostream & operator<<(ostream & p_os, InparaParser const * p_inpara_parser) throw();
friend
  ostream & operator<<(ostream & p_os, InparaParser const & p_inpara_parser) throw();

};


#endif // INPARAPARSER_HPP
