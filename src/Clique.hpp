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


#ifndef CLIQUE_HPP
#define CLIQUE_HPP


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
#define __CLASS__ "Clique"


// Namespaces used
using namespace std;
using namespace boost;


class Clique
{

public:

  // Getters & Setters

  inline bool hasEdges(void) const throw()
  {
    return(0 < this->solution.size());
  }


  inline u_int16_t getColumnWidth(void) const throw()
  {
    return(this->max_name_len + 2);
  }


  inline int getCliqueNumber(void) const throw()
  {
    return(this->clique_number);
  }


  inline int getGraphNumber(void) const throw()
  {
    return(this->subgraph_number);
  }


  inline graph_type_t getGraphType(void) const throw()
  {
    return(this->type);
  }


  inline void setGraphType(graph_type_t p_type) throw()
  {
    this->type = p_type;
    return;
  }


  inline void setCliqueNumber(int p_value) throw()
  {
    this->clique_number = p_value;
    return;
  }

  inline int getNodeCount(void) const throw()
  {
    return(this->solution.size());
  }


  inline void setEdgeCount(int p_value) throw()
  {
    this->edge_count = p_value;
    return;
  }


  inline int getEdgeCount(void) const throw()
  {
    return(this->edge_count);
  }



  // Constructors

  Clique(int subgraph_number, sharedLookup const & p_lookup) throw();


  // Destructor

  ~Clique(void) throw()
  {
    return;
  }


  // Public Methods

  vector<string> getNodeNames(void) const throw();
  string         getShortId(string const & p_value) const throw();
  string         print(void) const throw();


  // Variables

  list<string> solution;


private:

  // Getters/Setters


  // Constructors

  Clique(void) throw();
  Clique(Clique const & p_clique) throw();


  // Operators

  bool operator=(Clique const & p_clique) const throw();


  // Methods


  // Variables

  int           subgraph_number;
  int           clique_number;
  int           edge_count;
  sharedLookup  lookup;
  u_int16_t     max_name_len;
  graph_type_t  type;


  // Friends

friend
  ostream & operator<<(ostream & p_os, Clique const * p_clique) throw();
friend
  ostream & operator<<(ostream & p_os, Clique const & p_clique) throw();

};


typedef boost::shared_ptr<Clique> sharedClique;


#endif // CLIQUE_HPP
