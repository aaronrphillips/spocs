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
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
// External Includes
#include <boost/filesystem.hpp>
#include <boost/progress.hpp>
// Internal Includes
// Application Includes
#include "global.hpp"
#include "CliqueRunner.hpp"
#include "HtmlReport.hpp"
#include "InparaParser.hpp"
#include "OutputHelper.hpp"
#include "Subgraph.hpp"

#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "CliqueRunner"


// Namespaces used
using namespace std;
using namespace boost::filesystem;


// Constructors


CliqueRunner::CliqueRunner(sharedLookup   const & p_lookup,
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
                           bool           const   p_produce_html
                          ) throw() :
lookup(p_lookup),
inparanoid_dir(p_inparanoid_dir),
output_dir(p_output_dir),
report(p_report),
html_report(p_html_report),
species_files(p_species_files),
max_subgraph_nodes(p_max_subgraph_nodes),
confidence_cutoff(p_confidence_cutoff),
bad_paralog_cutoff(p_bad_paralog_cutoff),
edge_threshold(p_edge_threshold),
min_clique_size(p_min_clique_size),
produce_html(p_produce_html)
{
  ::g_output_helper->register_file(e_report,      report);
  ::g_output_helper->register_file(e_badparalogs, report + ".rejected");
  if (bad_paralog_cutoff > 0)
  {
    stringstream ss("");
    ss << "Paralog lines exceed paralog cutoff (" << bad_paralog_cutoff << ")\n";
    ::g_output_helper->write(e_badparalogs, ss.str());
  }
  if (produce_html)
  {
    ::g_output_helper->register_file(e_html,   html_report);
  }

  //NOTE: the idea here was to have header and footer files that we could change w/o recompiling
  //      may want to revisit this idea
  //::g_output_helper->append_content(e_html,  getEnvVar("CLIQUES_HOME") + "/conf/html_report.header");

  return;
}

CliqueRunner::~CliqueRunner() throw()
{
  ::g_output_helper->close(e_report);
  ::g_output_helper->close(e_badparalogs);
  if (produce_html)
  {
    ::g_output_helper->close(e_html);
  }
  
}


// Operators


// Public Methods

string CliqueRunner::getReportHeader(void) throw()
{
  string header_row("");
  header_row += "ClusterID\t";
  header_row += "CliqueID\t";
  header_row += "CliqueType\t";
  header_row += "Nodes\t";
  header_row += "Edges\t";

  list<string> species = lookup->getSpecies();
  list<string>::iterator itor = species.begin();
  list<string>::iterator etor = species.end();
  for (itor = itor; itor != etor; ++itor)
  {
    header_row += (*itor) + "\t";
  }
  header_row += "\n";

  return(header_row);
}


void CliqueRunner::run(void) throw()
{
  //TODO: 9/10/2012: this bombs if you replace graph_dir with getGraphDir(). WHY?
  string graph_dir = getGraphDir();
  makeDirectory(graph_dir, "graph output");
  HtmlReport html_report;

  if (produce_html)
  {
    html_report.initialize(lookup, graph_dir);
  }

  ::g_output_helper->write(e_report, getReportHeader());

  //  NOTE: Parse all the spocs output tables in getInparanoidDir().
  InparaParser parser(getInparanoidDir(), getConfidenceCutoff(), getBadParalogCutoff(), getSpeciesFiles(), lookup);
  boost::shared_ptr< vector< sharedEdges > > all_subgraphs = parser.getSubgraphs();

  progress_display show_progress(all_subgraphs->size());

  vector< sharedEdges >::iterator itor = all_subgraphs->begin();
  vector< sharedEdges >::iterator etor = all_subgraphs->end();
  for (itor = itor; itor != etor; ++itor)
  {
    Subgraph subgraph(getGraphDir(), (*itor), lookup, getMinCliqueSize(), getMaxSubgraphNodes(), getEdgeThreshold(), produce_html);

    if (produce_html)
    {
      html_report.addSubgraphEdges(subgraph.getGraphNumber(), (*itor));
    }

    if (! isTypeSet(subgraph.getGraphType()))
    {
      continue;
    }

    sharedClique clique(new Clique(subgraph.getGraphNumber(), lookup));
    subgraph.updateClique((*itor), clique);

    do
    {
      if (clique->getNodeCount() >= getMinCliqueSize())
      {
        ::g_output_helper->write(e_report, clique->print());

        if (produce_html)
        {
          html_report.add(subgraph.getGraphType(), subgraph.getGraphNumber(), clique->getCliqueNumber(), clique->getNodeCount(), clique->getEdgeCount(), clique->getNodeNames());
        }
      }
      subgraph.updateClique((*itor), clique);
    }
    while (clique->hasEdges() && clique->getNodeCount() >= getMinCliqueSize());

    subgraph.saveSubgraphJson();
    ++show_progress;
  }

  if (produce_html)
  {
    html_report.makeReport();
  }

  return;
}
