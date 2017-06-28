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
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
// Internal Includes
#include <boost/algorithm/string.hpp>
// Application Includes
#include "Clique.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "Clique"


// Namespaces used
using namespace std;
using namespace boost::algorithm;


// Constructors


Clique::Clique(int p_subgraph_number, sharedLookup const & p_lookup) throw() :
  subgraph_number(p_subgraph_number),
  clique_number(0),
  edge_count(0),
  lookup(p_lookup),
  max_name_len(lookup->getMaxNameLength())
{
  return;
}


// Operators


// Public Methods


string Clique::getShortId(string const & p_value) const throw()
{
  // Rip out all the lowercase letters from p_value
  string s = p_value;
  vector<string> v;
  split(v, s, is_lower());
  s = join(v, "");
  return(s);
}


vector<string> Clique::getNodeNames(void) const throw()
{
  vector<string> results;

  list<string> species = lookup->getSpecies();
  list<string>::iterator sitor = species.begin();
  list<string>::iterator setor = species.end();

  for (sitor = sitor; sitor != setor; ++sitor)
  {
    string species = (*sitor);
    string name = "-";

    list<string>::const_iterator itor = solution.begin();
    list<string>::const_iterator etor = solution.end();
    for (itor = itor; itor != etor; ++itor)
    {
      if (lookup->getSpeciesByName(*itor).compare(species) == 0)
      {
        name = (*itor);
        break;
      }
    }
    results.push_back(name);
  }

  return(results);
}


string Clique::print(void) const throw()
{
  stringstream output;

  output.setf(ios::left);

  // TODO: Replace all setw and other spacing with tabs
  string graph_type = string(getGraphTypeStr(type));
  output << getGraphNumber();
  output << "\t" << getCliqueNumber();
  output << "\t" << getShortId(graph_type);
  output << "\t" << getNodeCount();
  output << "\t" << getEdgeCount();

  list<string> species = lookup->getSpecies();
  list<string>::iterator sitor = species.begin();
  list<string>::iterator setor = species.end();
  for (sitor = sitor; sitor != setor; ++sitor)
  {
    string species = (*sitor);
//cout << "species:" << species << endl;
    string name = "-";

    list<string>::const_iterator itor = solution.begin();
    list<string>::const_iterator etor = solution.end();
    for (itor = itor; itor != etor; ++itor)
    {
//cout << "   name:" << (*itor) << "looking for:" << lookup->getSpeciesByName(*itor) << " num:" << lookup->getNumberByName(*itor)<< endl;
      if (lookup->getSpeciesByName(*itor).compare(species) == 0)
      {
        name = (*itor);
        break;
      }
    }

    output << "\t" << name;
  }

  output << "\n";

  return(output.str());
}


ostream & operator<<(ostream & p_os, Clique const * p_clique) throw()
{
  return(p_os << p_clique->print());
}


ostream & operator<<(ostream & p_os, Clique const & p_clique) throw()
{
  return(p_os << p_clique.print());
}
