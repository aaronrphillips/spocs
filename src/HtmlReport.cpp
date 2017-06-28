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
#include <iomanip>
#if HAVE_LIBGEN_H
#include <libgen.h>
#endif
#include <sstream>
#include <string>
#include <vector>
// External Includes
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/regex.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include <boost/xpressive/xpressive.hpp>
// Internal Includes
// Application Includes
#include "osUtil.hpp"
#include "HtmlReport.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "HtmlReport"


// Namespaces used
using namespace std;
using namespace boost::algorithm;
using namespace boost::filesystem;
using namespace boost::xpressive;


// Constructors


HtmlReport::HtmlReport(void) throw()
{
  return;
}


// Operators


// Public Methods


void HtmlReport::initialize(sharedLookup const & p_lookup, string const & p_output_dir) throw()
{
  lookup          = p_lookup;
  full_output_dir = p_output_dir;
  output_dir      = basename((char*)p_output_dir.c_str());

  string config_dir(getConfigDir());
  string include_dir(full_output_dir + "/../include/");
  copyDir(config_dir, include_dir); 

  return;
}


void   HtmlReport::add(graph_type_t const p_type, int const p_graph_number, int const p_clique_number, int const p_node_count, int const p_edge_count, vector<string> const p_node_names) throw()
{
  bool found = false;
  clique_struct tmp;
  tmp.type = p_type;
  tmp.clique_number = p_clique_number;
  tmp.node_count = p_node_count;
  tmp.edge_count = p_edge_count;
  tmp.node_names = p_node_names;

  list<subgraph_struct>::iterator itor = report[p_type].begin();
  list<subgraph_struct>::iterator etor = report[p_type].end();
  for (itor = itor; itor != etor; ++itor)
  {
    if ((*itor).graph_number == p_graph_number)
    {
      if (p_edge_count > (*itor).max_edges)
      {
        (*itor).max_edges = p_edge_count;
      }
      (*itor).cliques.push_back(tmp);
      found = true;
      break;
    }
  }

  if (! found)
  {
    subgraph_struct stmp;
    stmp.type = p_type;
    stmp.graph_number = p_graph_number;
    stmp.max_edges = p_edge_count;
    stmp.cliques.push_back(tmp);
    report[p_type].push_back(stmp);
  }

  return;
}

void HtmlReport::addSubgraphEdges(int const p_graph_number, sharedEdges & p_edges) throw()
{
  Edges_t::const_iterator itor = p_edges->begin();
  Edges_t::const_iterator etor = p_edges->end();

  for (itor = itor; itor != etor; ++itor)
  {
    string name1(lookup->getNameById(get<Vertex1>(*itor)));
    string name2(lookup->getNameById(get<Vertex2>(*itor)));
    string desc1(lookup->getDescByName(name1));
    string desc2(lookup->getDescByName(name2));

    subgraph_to_vertice_descriptions[p_graph_number][name1] = desc1;
    subgraph_to_vertice_descriptions[p_graph_number][name2] = desc2;
  }

  edge_count[p_graph_number] = p_edges->size();

  return;
}


// read in a json file
string HtmlReport::getJsonData(string const & p_file) throw()
{
  stringstream json("");
  ifstream in_file(p_file.c_str());
  string line;

  if (in_file.is_open())
  {
    while ( in_file.good() )
    {
      getline (in_file,line);
      json << line << endl;
    }
    in_file.close();

    unlink(p_file.c_str());
  }

  return json.str();
}


void HtmlReport::makeSubgraphReport(subgraph_struct & p_subgraph, graph_type_t const p_type) throw()
{
  // TODO: 2012-11-04: Darren: We should replace stringstream with strings and
  //       calls to "to_string(value)" instead - much faster.
  stringstream subgraph_path("");
  subgraph_path << this->full_output_dir << "/subgraph." << p_subgraph.graph_number << "/index.html";

  string    graph_type = getGraphTypeStr(p_type);

  list<clique_struct> cliques = p_subgraph.cliques;

  // create clique pages
  cliques.sort();
  list<clique_struct>::iterator citor = cliques.begin();
  list<clique_struct>::iterator cetor = cliques.end();

  map<string, string> name_to_id;

  for (citor = citor; citor != cetor; ++citor)
  {
    makeCliqueReport(p_subgraph.graph_number, *citor, graph_type);
    vector<string>::iterator nitor = (*citor).node_names.begin();
    vector<string>::iterator netor = (*citor).node_names.end();
    for (nitor = nitor; nitor != netor; ++nitor)
    {
      stringstream link("");
      link << "<a href=\"graph/subgraph." << p_subgraph.graph_number << "/clique." << (*citor).clique_number << ".html\">" << (*citor).clique_number << "</a>";
      name_to_id[(*nitor)] = link.str();
    }
  }

  if (p_type == Degenerate || p_type == Incomplete)
  {
    stringstream title("");
    title << "Cluster " << p_subgraph.graph_number << " (" << graph_type << ")";

    map<int, map<string, string> >::const_iterator ftor = subgraph_to_vertice_descriptions.find(p_subgraph.graph_number);
    map<int, map<string, string> >::const_iterator etor = subgraph_to_vertice_descriptions.end();
  
    if (ftor == etor)
    {
      FATAL("Building cluster vertex table", "Finding vertex names/descriptions", "Can't find vertex/description for cluster " + lexical_cast<string>(p_subgraph.graph_number));
    }
  
    ofstream subgraph_file(subgraph_path.str().c_str());
    stringstream noext_file("");
    noext_file << this->full_output_dir << "/subgraph." << p_subgraph.graph_number << "/subgraph." << p_subgraph.graph_number;
    string noext = noext_file.str();
    stringstream json_file("");
    json_file << noext << ".all.json";
    stringstream fasta_file("");
    fasta_file << noext << ".fasta.json";


    subgraph_file << "<!DOCTYPE html>\n";
    subgraph_file << "<html>\n";
    subgraph_file << "<head>\n";
    subgraph_file << "  <base href=\"../../\" target=\"_blank\">\n";
    subgraph_file << "  <title>" << title.str() << "</title>\n";
    subgraph_file << "  <meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" >\n";
    // TODO: 2013/02/22: Put Fasta File data here:
    subgraph_file << "  <script>" << getJsonData(json_file.str())  << "</script>\n";
    subgraph_file << "  <script>" << getJsonData(fasta_file.str()) << "</script>\n";
    subgraph_file << "  <script>\n";
    subgraph_file << "    var scripts      = new Array();\n";
    subgraph_file << "    var stylesheets  = new Array();\n";
    subgraph_file << "  </script>\n";
    subgraph_file << "  <script src=\"include/bootstrap_subgraph.js\"></script>\n";
    subgraph_file << "  <script src=\"include/bootstrap.js\"></script>\n";
    subgraph_file << "</head>\n";
    subgraph_file << "<body>\n";
  
    subgraph_file << "<h1 id=\"mainTitle\">" << title.str() << "</h1>\n";
    subgraph_file << "<table style=\"width : 100%;\">\n";
    subgraph_file << "  <tr>\n";
    subgraph_file << "    <th>\n";
    subgraph_file << "      <table>\n";
    subgraph_file << "        <thead>\n";
    subgraph_file << "          <tr>\n";
    //subgraph_file << "            <th>" << title.str() << "</th>\n";
    subgraph_file << "            <th>&nbsp;</th>\n";
    subgraph_file << "          </tr>\n";
    subgraph_file << "        </thead>\n";
    subgraph_file << "        <tbody>\n";
    subgraph_file << "          <tr>\n";
    subgraph_file << "            <th colspan=\"2\">\n";
    subgraph_file << "              <div class='gallery' id='chart'></div>\n";
    subgraph_file << "            </th>\n";
    subgraph_file << "          </tr>\n";
    subgraph_file << "        </tbody>\n";
    subgraph_file << "        <tfoot>\n";
    subgraph_file << "          <tr>\n";
    subgraph_file << "            <th>\n";
    subgraph_file << "              <table border=\"2\" cellpadding=\"3\">\n";
    subgraph_file << "                <tr>\n";
    subgraph_file << "                  <td>\n";
    subgraph_file << "                    Cluster ID\n";
    subgraph_file << "                  </td>\n";
    subgraph_file << "                  <td>\n";
    subgraph_file << "                    " << p_subgraph.graph_number << "\n";
    subgraph_file << "                  </td>\n";
    subgraph_file << "                </tr>\n";
    subgraph_file << "                <tr>\n";
    subgraph_file << "                  <td>Number of Vertices</td>\n";
    subgraph_file << "                  <td>" << (*ftor).second.size() << "</td>\n";
    subgraph_file << "                </tr>\n";
    subgraph_file << "                <tr>\n";
    subgraph_file << "                  <td>Number of Edges</td>\n";
    subgraph_file << "                  <td>" << edge_count[p_subgraph.graph_number] << " out of " << (((*ftor).second.size() * ((*ftor).second.size() - 1)) / 2) << "</td>\n";
    subgraph_file << "                </tr>\n";
    subgraph_file << "                <tr>\n";
    subgraph_file << "                  <td>Type</td>\n";
    subgraph_file << "                  <td>" << string(getGraphTypeStr(p_type)) << "</td>\n";
    subgraph_file << "                </tr>\n";
    subgraph_file << "              </table>\n";
    subgraph_file << "            </th>\n";
    subgraph_file << "            <th align=\"left\" style=\"width:100%;\">\n";
    subgraph_file << "              <span id=\"dynamic-form\"><span>\n";
    subgraph_file << "            </th>\n";
    subgraph_file << "          </tr>\n";
    subgraph_file << "        </tfoot>\n";
    subgraph_file << "      </table>\n";
    subgraph_file << "    </th>\n";
    subgraph_file << "  </tr>\n";
    subgraph_file << "  <tr>\n";
    subgraph_file << "    <td>\n";
    subgraph_file << "      <table border=\"2\" cellpadding=\"3\" class=\"sortable\">\n";
    subgraph_file << "        <thead>\n";
    subgraph_file << "          <tr>\n";
    subgraph_file << "            <th>Clique ID</th>\n";
    subgraph_file << "            <th>Species</th>\n";
    subgraph_file << "            <th>Name</th>\n";
    subgraph_file << "            <th>Description</th>\n";
    subgraph_file << "          </tr>\n";
    subgraph_file << "        </thead>\n";
    subgraph_file << "        <tbody>\n";
  
  
    map<string, string>::const_iterator vitor = (*ftor).second.begin();
    map<string, string>::const_iterator vetor = (*ftor).second.end();
    for (vitor = vitor; vitor != vetor; ++vitor)
    {
      string description((*vitor).second);
      if (description.empty())
      {
        description = "&nbsp;";
      }
      string id(name_to_id[(*vitor).first]);
      if (id.empty())
      {
        id = "&nbsp;";
      }

      subgraph_file << "          <tr>\n";
      subgraph_file << "            <td>" << id                                        << "</td>\n";
      subgraph_file << "            <td>" << lookup->getSpeciesByName((*vitor).first)  << "</td>\n";
      subgraph_file << "            <td>" << (*vitor).first                            << "</td>\n";
      subgraph_file << "            <td nowrap=\"nowrap\">" << description             << "</td>\n";
      subgraph_file << "          </tr>\n";
    }
  
    subgraph_file << "        </tbody>\n";
    subgraph_file << "        <tfoot>\n";
    subgraph_file << "          <tr>\n";
    subgraph_file << "            <th colspan=\"4\"><a class=\"function_link\" onclick=\"javascript:show_fasta_data();\">Cluster FASTA file</a></th>\n";
    subgraph_file << "          </tr>\n";
    subgraph_file << "        </tfoot>\n";
    subgraph_file << "      </table>\n";
    subgraph_file << "    </td>\n";
    subgraph_file << "  </tr>\n";
    subgraph_file << "</table>\n";
    subgraph_file << "<h1>Metadata</h1>\n";
    subgraph_file << "<table id=\"dynamic-metadata\" border=\"2\" cellpadding=\"3\" class=\"sortable\"></table>\n";
    subgraph_file << getDisclaimer();
    subgraph_file << "</body>\n";
    subgraph_file << "</html>\n";
  
    subgraph_file.close();
  }
  
  return;
}


void HtmlReport::makeCliqueReport(int const p_graph_number, clique_struct & p_clique, string const & p_graph_type) throw()
{
  stringstream clique_path("");
  clique_path << this->full_output_dir;
  clique_path << "/subgraph.";
  clique_path << p_graph_number;
  clique_path << "/clique.";
  clique_path <<  p_clique.clique_number;
  clique_path << ".html";
  ofstream clique_file(clique_path.str().c_str());

  stringstream title("");
  title << "Cluster ";
  title << p_graph_number;
  title << " - Clique ";
  title << p_clique.clique_number;

  vector<string>::const_iterator itor = p_clique.node_names.begin();
  vector<string>::const_iterator etor = p_clique.node_names.end();
  stringstream id_stream("");
  int num_valid = 0;;
  for (itor = itor; itor != etor; ++itor)
  {
    if ((*itor) == "-" || (*itor).size() < 1) continue;
    num_valid++;

    string description(lookup->getDescByName(*itor));
    if (description.empty())
    {
      description = "&nbsp;";
    }
    id_stream << "    <tr>\n";
    id_stream << "      <td>" << lookup->getSpeciesByName(*itor) << "</td>\n";
    id_stream << "      <td>" << (*itor)                         << "</td>\n";
    id_stream << "      <td nowrap=\"nowrap\">" << description   << "</td>\n";
    id_stream << "    </tr>\n";
  }

  stringstream noext_file("");
  noext_file << this->full_output_dir;
  noext_file << "/subgraph.";
  noext_file << p_graph_number;
  noext_file << "/subgraph.";
  noext_file << p_graph_number;
  noext_file << ".";
  noext_file << p_clique.clique_number ;
  noext_file << "_";
  noext_file << p_clique.edge_count;

  stringstream json_file("");
  json_file << noext_file.str() << ".json" ;
  stringstream fasta_file("");
  fasta_file << noext_file.str() << ".fasta.json" ;

  clique_file << "<!DOCTYPE html>\n";
  clique_file << "<html>\n";
  clique_file << "<head>\n";
  clique_file << "  <base href=\"../../\" target=\"_blank\">\n";
  clique_file << "  <title>" << title.str() << "</title>\n";
  clique_file << "  <meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" >\n";
  // TODO: 2013/02/22: Put Fasta File data here:
  clique_file << "  <script>" << getJsonData(json_file.str())  << "</script>\n";
  clique_file << "  <script>" << getJsonData(fasta_file.str()) << "</script>\n";
  clique_file << "  <script>\n";
  clique_file << "    var scripts      = new Array();\n";
  clique_file << "    var stylesheets  = new Array();\n";
  clique_file << "  </script>\n";
  clique_file << "  <script src=\"include/bootstrap_clique.js\"></script>\n";
  clique_file << "  <script src=\"include/bootstrap.js\"></script>\n";
  clique_file << "</head>\n";
  clique_file << "<body>\n";
  clique_file << "<h1 id=\"mainTitle\">" << title.str() << "</h1>\n";
  clique_file << "<table style=\"width : 100%;\">\n";
  clique_file << "  <tr>\n";
  clique_file << "    <th>\n";
  clique_file << "      <table>\n";
  clique_file << "        <thead>\n";
  clique_file << "          <tr>\n";
  //clique_file << "            <th>" << title.str() << "</th>\n";
  clique_file << "            <th>&nbsp;</th>\n";
  clique_file << "          </tr>\n";
  clique_file << "        </thead>\n";
  clique_file << "        <tbody>\n";
  clique_file << "          <tr>\n";
  clique_file << "            <th colspan=\"2\">\n";
  clique_file << "              <div class='gallery' id='chart'></div>\n";
  clique_file << "            </th>\n";
  clique_file << "          </tr>\n";
  clique_file << "        </tbody>\n";
  clique_file << "        <tfoot>\n";
  clique_file << "          <tr>\n";
  clique_file << "            <th>\n";
  clique_file << "              <table border=\"2\" cellpadding=\"3\">\n";
  clique_file << "                <tr>\n";
  clique_file << "                  <td>\n";
  clique_file << "                    Cluster ID\n";
  clique_file << "                  </td>\n";
  clique_file << "                  <td>\n";
  clique_file << "                    " << p_graph_number << "\n";
  clique_file << "                  </td>\n";
  clique_file << "                </tr>\n";
  clique_file << "                <tr>\n";
  clique_file << "                  <td>Clique ID</td>\n";
  clique_file << "                  <td>" << p_clique.clique_number << "</td>\n";
  clique_file << "                </tr>\n";
  clique_file << "                <tr>\n";
  clique_file << "                  <td>Number of Vertices</td>\n";
  clique_file << "                  <td>" << num_valid << "</td>\n";
  clique_file << "                </tr>\n";
  clique_file << "                <tr>\n";
  clique_file << "                  <td>Number of Edges</td>\n";
  clique_file << "                  <td>" << p_clique.edge_count << " out of " << ((num_valid * (num_valid - 1)) / 2) << "</td>\n";
  clique_file << "                </tr>\n";
  clique_file << "                <tr>\n";
  clique_file << "                  <td>Type</td>\n";
  clique_file << "                  <td>" << p_graph_type << "</td>\n";
  clique_file << "                </tr>\n";
  clique_file << "              </table>\n";
  clique_file << "            </th>\n";
  clique_file << "            <th align=\"left\" style=\"width : 100%;\">\n";
  clique_file << "              <span id=\"dynamic-form\"><span>\n";
  clique_file << "            </th>\n";
  clique_file << "          </tr>\n";
  clique_file << "        </tfoot>\n";
  clique_file << "      </table>\n";
  clique_file << "    </th>\n";
  clique_file << "  </tr>\n";
  clique_file << "  <tr>\n";
  clique_file << "    <td>\n";
  clique_file << "      <table border=\"2\" cellpadding=\"3\" class=\"sortable\">\n";
  clique_file << "        <thead>\n";
  clique_file << "          <tr>\n";
  clique_file << "            <th>Species</th>\n";
  clique_file << "            <th>ID</th>\n";
  clique_file << "            <th>Description</th>\n";
  clique_file << "          </tr>\n";
  clique_file << "        </thead>\n";
  clique_file << "        <tbody>\n";
  clique_file <<            id_stream.str();
  clique_file << "        </tbody>\n";
  clique_file << "        <tfoot>\n";
  clique_file << "          <tr>\n";
  clique_file << "            <th colspan=\"3\"><a class=\"function_link\" onclick=\"javascript:show_fasta_data();\">Cluster FASTA file</a></th>\n";
  clique_file << "          </tr>\n";
  clique_file << "        </tfoot>\n";
  clique_file << "      </table>\n";
  clique_file << "    </td>\n";
  clique_file << "  </tr>\n";
  clique_file << "</table>\n";
  clique_file << "<h1>Metadata</h1>\n";
  clique_file << "<table id=\"dynamic-metadata\" border=\"2\" cellpadding=\"3\" class=\"sortable\"></table>\n";
  clique_file << getDisclaimer();
  clique_file << "</body>\n";
  clique_file << "</html>\n";

  clique_file.close();

  return;
}


string HtmlReport::getDisclaimer(void) throw()
{
  stringstream output("");

  output << "<div class=\"disclaimer\">\n";
  output << "  <h2>DISCLAIMER</h2>\n\n";
  output << "  <p>\n";
  output << "    This material was prepared as an account of work\n";
  output << "    sponsored by an agency of the United States Government.\n";
  output << "    Neither the United States Government nor the United\n";
  output << "    States Department of Energy, nor Battelle, nor any of\n";
  output << "    their employees, MAKES ANY WARRANTY, EXPRESS OR IMPLIED,\n";
  output << "    OR ASSUMES ANY LEGAL LIABILITY OR RESPONSIBILITY FOR THE\n";
  output << "    ACCURACY, COMPLETENESS, OR USEFULNESS OF ANY INFORMATION,\n";
  output << "    APPARATUS, PRODUCT, SOFTWARE, OR PROCESS DISCLOSED, OR\n";
  output << "    REPRESENTS THAT ITS USE WOULD NOT INFRINGE PRIVATELY\n";
  output << "    OWNED RIGHTS.\n";
  output << "  </p>\n\n";
  output << "  <h2>ACKNOWLEDGMENT</h2>\n\n";
  output << "  <p>\n";
  output << "    This software and its documentation were produced with\n";
  output << "    United States Government support under Contract Number\n";
  output << "    DE-AC05-76RL01830 awarded by the United States Department\n";
  output << "    of Energy. The United States Government retains a paid-up\n";
  output << "    non-exclusive, irrevocable worldwide license to reproduce,\n";
  output << "    prepare derivative works, perform publicly and display\n";
  output << "    publicly by or for the US Government, including the right\n";
  output << "    to distribute to other US Government contractors.\n";
  output << "  </p>\n";
  output << "</div>\n";

  return(output.str());
}


string HtmlReport::getRejectedFastaIdTable(void) throw()
{
  stringstream output("");

  vector< vector< tuple<string, string> > > lines;

  u_int32_t line_idx    = 0;
  u_int32_t max_columns = 0;
  string file(this->full_output_dir + "/../cliques.report.rejected");
  if (is_regular_file(path(file)))
  {
    ifstream data(file.c_str());
    string line = "";
    regex re("\\t");

    // TODO: Make the first line a real comment line "#" and skip any commented lines
    getline(data, line);  // Get rid of the first line that is a comment
    while (getline(data, line))
    {
      string str = line;
      boost::sregex_token_iterator begin(str.begin(), str.end(), re, -1);
      boost::sregex_token_iterator end;

//cout << line_idx << ":";
      u_int32_t columns = 0;
      vector< tuple<string, string> > cells;
      while (begin != end)
      {
//cout << *begin << ":" << lookup->getDescByName(*begin) << ";";
         cells.push_back(make_tuple(*begin, lookup->getDescByName(*begin)));
         ++columns;
         ++begin;
         if (begin == end) { break; }
         ++begin;
      }
      ++line_idx;
      lines.push_back(cells);
      if (columns > max_columns) { max_columns = columns; }
//cout << "\n";

    }
  }
//cout << "MAX_COLS:" << max_columns << "\n";

  string header_row("");
  header_row += "    <th>Paralog #</th>\n";
  for (u_int32_t idx = 1; idx <= max_columns; ++idx)
  {
    header_row += "    <th>Fasta ID</th>\n";
  }

  output << "<div class=\"header\"><h2><a id=\"RejectedHeader\" onclick=\"javascript:toggle2('Rejected', 'RejectedContent', 'RejectedHeader');\">Rejected v</a></h2></div>\n";
  output << "<div class=\"content\" id=\"RejectedContent\" style=\"display:none;\">\n";
  output << "<table class=\"persist-area sortable\" border=\"1\" cols=\"" << (max_columns + 1) << "\">\n";
  output << "  <thead>\n";
  output << header_row;
  output << "  </thead>\n";
  output << "  <tbody>\n";

  u_int32_t r_idx = 1;
  vector< vector< tuple<string, string> > >::const_iterator itor = lines.begin();
  vector< vector< tuple<string, string> > >::const_iterator etor = lines.end();
  for (itor = itor; itor != etor; ++itor)
  {
    u_int32_t c_idx = 1;
    output << "  <tr class=\"r\">\n";
    output << "    <th>" << r_idx++ << "</th>";
//cout << "IDX:" << r_idx;
    vector< tuple<string, string> >::const_iterator iitor = (*itor).begin();
    vector< tuple<string, string> >::const_iterator eetor = (*itor).end();
    for (iitor = iitor; iitor != eetor; ++iitor)
    {
//cout << ": ID:" << (*iitor).get<0>() << ": TITLE:" << (*iitor).get<1>();
      output << "<td><a class=\"tt\" title=\"" << (*iitor).get<1>() << "\">" << (*iitor).get<0>() << "</a></td>";
      ++c_idx;
    }
//cout << "\n";
    while (c_idx < (max_columns + 1))
    {
      output << "<td>&nbsp;</td>";
      ++c_idx;
    }
    output << "  </tr>\n";
  }

  output << "  </tbody>\n";
  output << "</table>\n";
  output << "</div>\n";

  return(output.str());
}


//
//  DEVELOPER NOTE: The idea for toggling sections from
//  http://www.randomsnippets.com/2008/02/12/how-to-hide-and-show-your-div
//
void HtmlReport::makeReport(void) throw()
{
  stringstream output("");
  output << "<!DOCTYPE html>\n";
  output << "<html>\n";
  output << "<head>\n";
  output << "  <base href=\"./\" target=\"_blank\">\n";
  output << "  <title>" << APP_NAME_UPPER << "</title>\n";
  output << "  <meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" >\n";
  output << "  <script>\n";
  output << "    var scripts      = new Array();\n";
  output << "    var stylesheets  = new Array();\n";
  output << "  </script>\n";
  output << "  <script src=\"include/bootstrap_report.js\"></script>\n";
  output << "  <script src=\"include/bootstrap.js\"></script>\n";
  output << "</head>\n";
  output << "<body>\n";
  output << "  <table>\n";
  output << "    <tr style=\"background-color:inherit;\">\n";
  output << "      <td colspan=3 style=\"color:#4878A8; font-family:helvetica; font-size:normal;\">\n";
  output << "        Attributes required for filter match:\n";
  output << "      </td style=\"max-width:170px;\">\n";
  output << "      <td>\n";
  output << "      </td>\n";
  output << "      <td style=\"width:100%\">\n";
  output << "      </td>\n";
  output << "    </tr>\n";
  output << "    <tr style=\"background-color:inherit;\">\n";
  output << "      <td>\n";
  output << "        <button id=\"one-button\" style=\"width:80px; height:30px; max-height:30px; border:2px solid #4878A8; font-family:helvetica; background-color:#A8D8F0; color:#4878A8; border-radius: 15px; -moz-border-radius: 15px; -webkit-border-radius: 15px;\" type=\"button\" onmousedown=\"requireOne()\">One</button>\n";
  output << "      </td>\n";
  output << "      <td>\n";
  output << "        <button id=\"all-button\" style=\"width:80px; height:30px; max-height:30px; border:2px solid #A8D8F0; font-family:helvetica; background-color:#4878A8; color:#A8D8F0; border-radius: 15px; -moz-border-radius: 15px; -webkit-border-radius: 15px;\" type=\"button\" onmousedown=\"requireAll()\">All</button>\n";
  output << "      </td>\n";
  output << "      <td>\n";
  output << "      </td style=\"width:100%\">\n";
  output << "    </tr>\n";
  output << "    <tr style=\"background-color:inherit;\">\n";
  output << "      <td>\n";
  output << "        <br/>\n";
  output << "      </td style=\"max-width:170px;\">\n";
  output << "      <td>\n";
  output << "      </td>\n";
  output << "      <td style=\"width:100%\">\n";
  output << "      </td>\n";
  output << "    </tr>\n";
  output << "    <tr style=\"background-color:inherit;\">\n";
  output << "      <td>\n";
  output << "        <button style=\"width:80px; height:30px; max-height:30px; border:2px solid #A8D8F0; font-family:helvetica; background-color:#4878A8; color:#A8D8F0; border-radius: 15px; -moz-border-radius: 15px; -webkit-border-radius: 15px;\" type=\"button\" onmousedown=\"filter_tables()\">Filter</button>\n";
  output << "      </td>\n";
  output << "      <td colspan=\"2\">\n";
  output << "        <div style=\"width:100%;float:right\" id=\"search_box_container\" class=\"active_search empty_search noselect\"></div>\n";
  output << "      </td>\n";
  output << "      <td style=\"width:100%\">\n";
  output << "      </td>\n";
  output << "    </tr>\n";
  output << "  </table>\n";
  output << "  <div style=\"width:60%;float:right\" id=\"search_query\">&nbsp;</div>\n";
  output << "  <h1 id=\"mainTitle\">" << APP_NAME_UPPER << "</h1>\n";
  output << "  <div class=\"header\"><h2><a id=\"InstructionsHeader\" onclick=\"javascript:toggle2('Instructions', 'InstructionsContent', 'InstructionsHeader');\">Instructions v</a></h2></div>\n";
  output << "  <div class=\"content\" id=\"InstructionsContent\" style=\"display:none;\">\n";
  output << "    <h2>" APP_DESC "</h2>\n";
  output << "    <p>In viewing groups of orthologs as cliques, the nodes of the cliques are the proteins and the edges are the reciprocal best BLAST hits connecting those proteins.</p>\n";
  output << "    <p>The orthologs below are arranged by type:</p>\n";

  output << "    <table>\n";
  output << "      <thead>\n";
  output << "        <tr>\n";
  output << "          <th>Type</th>\n";
  output << "          <th>Description</th>\n";
  output << "        </tr>\n";
  output << "      </thead>\n";
  output << "      <tbody>\n";
  output << "        <tr>\n";
  output << "          <td>Complete</td>\n";
  output << "          <td>Those ortholog groups for which there is no more than one protein per species, and every protein (node) is connected to every other protein by a reciprocal best hit (edge)</td>\n";
  output << "        </tr>\n";
  output << "        <tr>\n";
  output << "          <td>SemiComplete</td>\n";
  output << "          <td>Those ortholog groups for which there is no more than one protein per species, and some number (default is 5%) of the edges may be missing</td>\n";
  output << "        </tr>\n";
  output << "        <tr>\n";
  output << "          <td>Incomplete</td>\n";
  output << "          <td>Those ortholog groups for which there is no more than one protein per species, and more than the allowed number of edges are missing</td>\n";
  output << "        </tr>\n";
  output << "        <tr>\n";
  output << "          <td>Degenerate</td>\n";
  output << "          <td>Those ortholog groups for which one or more of the species has more than one protein (node)</td>\n";
  output << "        </tr>\n";
  output << "      </tbody>\n";
  output << "    </table>\n";

  output << "    <p>For the Complete and SemiComplete groups below, by definition there is only one clique per ortholog cluster, and the <strong>clique</strong> link will display an image of the clique, a link to a fasta file of the proteins in that clique, and a table providing the functional annotation of the proteins (extracted from the fasta headers).</p>\n";
  output << "    <p>For the Incomplete and Degenerate groups, the <strong>cluster</strong> links below will render images showing all the nodes and edges, and provide the fasta sequences and functional annotation for all the proteins in that cluster.  When an incomplete or degenerate cluster of proteins are solved into discrete cliques, more than one clique may be generated; these solved cliques will be Complete, and their <strong>clique</strong> links will render images showing each discrete complete clique and provide the associated information for those proteins.</p>\n";
  output << "    <p>The cluster and clique images are interactive: mousing over a node will reveal the protein identifier for that node, plus any additional protein identifiers from the same species present in that image; mousing over an edge will reveal the BLAST bit score between those protein nodes.  In addition, for the Incomplete and Degenerate clusters, clicking on a protein node in the cluster image will take you to the html page and image for the clique containing that protein</p>\n";

  output << "    <h2>Searching</h2>\n";

  output << "    <p>To search this page, first select the required number of attributes for a match: 'One' (logical OR), or 'All' (logical AND).</p>\n";
  output << "    <p>There are a number of available search parameters available. Click in the search box, and select one of the parameters from the drop-down. Some parameters have associated drop-downs that provide search suggestions. Either select one of these suggestions or enter a different value.</p>\n";
  output << "    <p><strong>IMPORTANT:</strong> Before hitting the 'Filter' button, you must either click on a search suggestion or hit the 'Enter' button on the keyboard to ensure all search parameters are saved</p>\n";
  output << "    <p>The node-count and clique-id parameters require the following range format: [low_val-high_val]</p>\n";
  output << "    <p>The fasta-comment and fasta-id parameters accept text or regular expressions</p>\n";

  output << "  </div>\n";
  output << "  <p>&nbsp;</p>\n";

  string header_row("");
  string header_row_with_class("");

  header_row += "    <th>ClusterID</th>\n";
  header_row += "    <th>CliqueID</th>\n";
  header_row += "    <th>Nodes</th>\n";
  header_row += "    <th>Edges</th>\n";

  list<string> species        = lookup->getSpecies();
  list<string>::iterator itor = species.begin();
  list<string>::iterator etor = species.end();
  for (itor = itor; itor != etor; ++itor)
  {
    header_row += "    <th>" + (*itor) + "</th>\n";
  }
  header_row += "  </tr>\n";

  header_row_with_class = header_row;
  header_row = "  <tr>\n" + header_row;
  header_row_with_class = "  <tr class=\"h\">\n" + header_row_with_class;

  map<graph_type_t, list<subgraph_struct> >::const_iterator mitor = report.begin();
  map<graph_type_t, list<subgraph_struct> >::const_iterator metor = report.end();
  for (mitor = mitor; mitor != metor; ++mitor)
  {
    graph_type_t idx = (*mitor).first;
    string context   = getGraphTypeStr(idx);
    output << "<div class=\"header\"><h2><a id=\"" << context << "Header\" onclick=\"javascript:toggle2('" << context << "', '" << context << "Content', '" << context << "Header');\">" << context << " v</a></h2></div>\n";
    output << "<div class=\"content\" id=\"" << context << "Content\" style=\"display:none;\">\n";
    output << "<table class=\"persist-area sortable\" border=\"1\" cols=\"" << (species.size() + 5) << "\">\n"; // 5 is for the sub#, clique #, etc.
    output << "  <thead>\n";
    output << header_row;
    output << "  </thead>\n";
    output << "  <tbody>\n";

    //int counter = 0;
    list<subgraph_struct> tmp = report[idx];
    tmp.sort();
    list<subgraph_struct>::iterator itor = tmp.begin();
    list<subgraph_struct>::iterator etor = tmp.end();
    for (itor = itor; itor != etor; ++itor)
    {
      makeSubgraphReport(*itor, idx);

      list<clique_struct> ctmp = (*itor).cliques;
      ctmp.sort();
      list<clique_struct>::const_iterator citor = ctmp.begin();
      list<clique_struct>::const_iterator cetor = ctmp.end();
      for (citor = citor; citor != cetor; ++citor)
      {
        stringstream subgraph_path("");
        stringstream abs_subgraph_path("");
        subgraph_path << this->output_dir << "/subgraph." << (*itor).graph_number << "/index.html";
        abs_subgraph_path << this->full_output_dir << "/subgraph." << (*itor).graph_number << "/index.html";
        stringstream clique_path("");
        clique_path   << this->output_dir << "/subgraph." << (*itor).graph_number << "/clique." << (*citor).clique_number  << ".html";

        output << "<tr class=\"r\">\n";
        if (exists(abs_subgraph_path.str()))
        {
          output << "<td><a target=\"_blank\" href=\"" << subgraph_path.str() << "\" >" << (*itor).graph_number   << "</a></td>";
        }
        else
        {
          output << "<td>" << (*itor).graph_number << "</td>";
        }
        output << "<td><a target=\"_blank\" href=\"" << clique_path.str()   << "\" >" << (*citor).clique_number << "</a></td>";
        output << "<td>" << (*citor).node_count << "</td>";
        output << "<td>" << (*citor).edge_count << "</td>";
        vector<string>::const_iterator sitor = (*citor).node_names.begin();
        vector<string>::const_iterator setor = (*citor).node_names.end();
        for (sitor = sitor; sitor != setor; ++sitor)
        {
          if ((*sitor) == "-")
          {
            output << "<td>&nbsp;</td>";
          }
          else
          {
            output << "<td><a class=\"tt\" title=\"" << lookup->getDescByName(*sitor) << "\">" << (*sitor) << "</a></td>";
          }
        }
        output << "</tr>\n";

        //++counter;
        //if (counter % 40 == 39)
        //{
        //  //cerr << "Header row! (" << counter << ")" << endl;
        //  output << header_row_with_class;
        //}
      }
    }
    output << "  </tbody>\n";
    output << "</table>\n";
    output << "</div>\n";
  }
  output << getRejectedFastaIdTable();
  output << getDisclaimer();
  output << "</body>\n";
  output << "</html>\n";
  ::g_output_helper->write(e_html, output.str());

  // TODO: Move this up to the correct location

  return;
}

string HtmlReport::print(void) throw()
{
  return "HtmlReport::print - NOT IMPLEMENTED YET\n";
}


ostream & operator<<(ostream & p_os, HtmlReport * p_clique) throw()
{
  return(p_os << p_clique->print());
}


ostream & operator<<(ostream & p_os, HtmlReport & p_clique) throw()
{
  return(p_os << p_clique.print());
}
