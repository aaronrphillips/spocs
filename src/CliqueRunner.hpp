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


#ifndef CLIQUERUNNER_HPP
#define CLIQUERUNNER_HPP


// System Includes
#include <string>
// External Includes
#include "boost/shared_ptr.hpp"
// Internal Includes
// Application Includes
#include "Lookup.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "CliqueRunner"


// Namespaces used
using namespace std;
using namespace boost;


class CliqueRunner
{

public:

  // Getters & Setters

  inline float getConfidenceCutoff(void) const throw()
  {
    return(this->confidence_cutoff);
  }


  inline u_int16_t getBadParalogCutoff(void) const throw()
  {
    return(this->bad_paralog_cutoff);
  }


  inline string getReportFilename(void) const throw()
  {
    return(this->report);
  }


  inline string getGraphDir(void) const throw()
  {
    return(this->output_dir + "/graph");
  }


  inline string getInparanoidDir(void) const throw()
  {
    return(this->inparanoid_dir);
  }


  inline vector<string> getSpeciesFiles(void) const throw()
  {
    return(this->species_files);
  }


  inline int getMaxSubgraphNodes(void) throw() // Cannot be const
  {
    return(this->max_subgraph_nodes);
  }


  inline u_int16_t getMinCliqueSize(void) throw() // Cannot be const
  {
    return(this->min_clique_size);
  }


  inline float getEdgeThreshold(void) throw() // Cannot be const
  {
    return(this->edge_threshold);
  }


  // Constructors

  CliqueRunner(sharedLookup   const & p_lookup,
               string         const & p_inparanoid_dir,
               string         const & p_output_dir,
               string         const & p_report,
               string         const & p_html_report,
               vector<string> const & p_species_files,
               u_int16_t      const   p_max_subgraph_nodes,
               float          const   p_confidence_cutoff,
               u_int16_t      const   p_bad_paralog_cutoff,
               float          const   p_edge_threshold,
               u_int16_t      const   p_min_clique_size,
               bool           const   p_produce_html) throw();


  // Destructor

  ~CliqueRunner(void) throw();


  // Public Methods

  string getReportHeader(void) throw();
  string print(void) const throw();
  void   run(void) throw();


private:

  // Getters/Setters


  // Constructors

  CliqueRunner(void) throw();
  CliqueRunner(CliqueRunner const & p_clique_runner) throw();


  // Operators

  bool operator=(CliqueRunner const & p_clique_runner) const throw();


  // Methods


  // Variables

  sharedLookup   lookup;
  string         inparanoid_dir;
  string         output_dir;
  string         report;
  string         html_report;
  vector<string> species_files;
  u_int16_t      max_subgraph_nodes;
  float          confidence_cutoff;
  u_int16_t      bad_paralog_cutoff;
  float          edge_threshold;
  u_int16_t      min_clique_size;
  bool           produce_html;


friend
  ostream & operator<<(ostream & p_os, CliqueRunner const * p_clique_runner) throw();
friend
  ostream & operator<<(ostream & p_os, CliqueRunner const & p_clique_runner) throw();

};


typedef boost::shared_ptr<CliqueRunner> sharedCliqueRunner;


#endif // CLIQUERUNNER_HPP
