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
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include "stdio.h"
#include <string>
#include <vector>
// External Includes
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/xpressive/xpressive.hpp>
// Internal Includes
// Application Includes
#include "InparaParser.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "InparaParser"


// Namespaces used
using namespace std;
using namespace boost::xpressive;
using namespace boost::algorithm;
using namespace boost::filesystem;


// Constructors

InparaParser::InparaParser(string         const & p_data_dir, 
                           float          const   p_confidence_cutoff, 
                           u_int16_t      const   p_bad_paralog_cutoff, 
                           vector<string> const & p_fasta_files,
                           sharedLookup   const & p_lookup) throw() :
  data_dir(p_data_dir),
  confidence_cutoff(p_confidence_cutoff),
  bad_paralog_cutoff(p_bad_paralog_cutoff),
  fasta_files(p_fasta_files),
  lookup(p_lookup)
{
  if (! exists(data_dir))
  {
    cerr << "YOUR DIRECTORY, " + data_dir + ", IS MISSING." << endl;
    exit(EXIT_FAILURE);
  }
  
  vector<string>::iterator fasta_tor1 = fasta_files.begin();
  vector<string>::iterator fasta_tor2 = fasta_files.begin();
  vector<string>::iterator fasta_etor = fasta_files.end();
  vector<string> files;

  string ext = getParalogFileExt();

  // loop over all possible paralog files; remember the ones that exist
  for (fasta_tor1 = fasta_tor1; fasta_tor1 != fasta_etor; ++fasta_tor1)
  {
    fasta_tor2 = fasta_tor1;
    fasta_tor2++;
    for (; fasta_tor2 != fasta_etor; ++fasta_tor2)
    {
      string spec_1 = getSpeciesFromPath(*fasta_tor1);
      string spec_2 = getSpeciesFromPath(*fasta_tor2);

      string file_path1(data_dir + "/" + spec_1 + '-' + spec_2 + ext );
      string file_path2(data_dir + "/" + spec_2 + '-' + spec_1 + ext );

      if (is_regular_file(path(file_path1)))
      {
        files.push_back(file_path1);
      }
      else if(is_regular_file(path(file_path2)))
      {
        files.push_back(file_path2);
      }
      else
      {
        cerr << "error finding inparanoid table file for species pair " << spec_1 << " " << spec_2 << endl;
      }

    }
  }


  // for every line in every paralog file, 'correlate' the ids on the line
  // (correlate will create edges between ids)

  vector<string>::iterator itor = files.begin();
  vector<string>::iterator etor = files.end();
  for (itor = itor; itor != etor; ++itor)
  {
    string file = (*itor);
    ifstream data(file.c_str());
    string line = "";
    sregex re = sregex::compile("^\\d+\t(\\d+)\t\\s*(.*?)\t\\s*(.*?)\\s*$");
    int const sub_matches[] = {1, 2, 3};
    while (getline(data, line))
    {
      string str = line;
      sregex_token_iterator begin( str.begin(), str.end(), re, sub_matches ); 
      sregex_token_iterator end;
      
      if (begin == end) { continue; }
    
      u_int32_t blast_score = atoi(string((*begin)).c_str());

      ++begin;

      vector< pair<string, float> > vec1;
      vector< pair<string, float> > vec2;
    
      // lines look like:
      // [line_num] [blast_score] A_id_1 1.0 A_id_1 1.0   [tab]    B_id_1 1.0

      string s1 = (*begin);
      createGeneVector(s1, vec1);  // get a vector of the A ids (above a conf cutoff)
    
      ++begin;
    
      string s2 = (*begin);
      createGeneVector(s2, vec2);  // get a vector of the B ids (above a conf cutoff)

      correlate(blast_score, vec1, vec2);
    }
    data.close();
  }

  return;
}

// make a vector out of all the space-separated ids, above a confidence threshold
void InparaParser::createGeneVector(string const & begin, vector< pair<string, float> > & vec) throw()
{
  string new_str = begin;
  sregex re_spaces = +_s;
  sregex_token_iterator begin_spaces(new_str.begin(), new_str.end(), re_spaces, -1); 
  sregex_token_iterator end_spaces;

  int idx = 0;
  string species = "";

  for (begin_spaces = begin_spaces; begin_spaces != end_spaces; ++begin_spaces, ++idx)
  {
    string temp = (*begin_spaces);
    trim(temp);

    if (idx % 2 == 1) // this token is a confidence value
    {
      float confidence = atof(temp.c_str());
      if (confidence >= confidence_cutoff)
      {
        pair<string, float> species_confidence(species, confidence);
        vec.push_back(species_confidence);
      }
    }
    else              // this token is an id
    {
      species = temp;
    }
  }

  return;
}


void InparaParser::correlate(u_int32_t const blast_score, vector< pair<string, float> > & vec1, vector< pair<string, float> > & vec2) throw()
{
  vector< pair<string, float> >::const_iterator itor_1 = vec1.begin();
  vector< pair<string, float> >::const_iterator etor_1 = vec1.end();
  vector< pair<string, float> >::const_iterator itor_2 = vec2.begin();
  vector< pair<string, float> >::const_iterator etor_2 = vec2.end();

  // TODO: this should not be here. (just a special case of the loop below)
  if (vec1.size() == 1 && vec2.size() == 1)
  {
    AddToList(blast_score, vec1[0], vec2[0]);
    AddToList(blast_score, vec2[0], vec1[0]);
    return;
  }

  // check for the size of the id vectors. getBadParalogCutoff() is the # of paralogs of the same id that 
  // is considered "noisy"
  if (getBadParalogCutoff() > 0 && (vec1.size() > getBadParalogCutoff() || vec2.size() > getBadParalogCutoff()))
  {
    stringstream ss("");
    for (itor_1 = vec1.begin(); itor_1 != etor_1; ++itor_1)
    {
      ss << (*itor_1).first << "\t" << setiosflags(ios::fixed) << setprecision(3) << (*itor_1).second << "\t";
    }
    for (itor_2 = vec2.begin(); itor_2 != etor_2; ++itor_2)
    {
      ss << (*itor_2).first << "\t" << setiosflags(ios::fixed) <<  setprecision(3) << (*itor_2).second << "\t";
    }

    ss << endl;

    ::g_output_helper->write(e_badparalogs, ss.str());
    return;
  }

  // for each pair of ids (every A id with every B id), AddToList
  for (itor_1 = itor_1; itor_1 != etor_1; ++itor_1)
  {
    for (itor_2 = vec2.begin(); itor_2 != etor_2; ++itor_2)
    {
      AddToList(blast_score, (*itor_1), (*itor_2));
      AddToList(blast_score, (*itor_2), (*itor_1));
    }
  }
  
  return;
}


// this function creates an edge between two ids, with a given score
void InparaParser::AddToList(u_int32_t const score, pair<string, float> const & pair1, pair<string, float> const & pair2) throw()
{
  int num1 = lookup->getIdByName(pair1.first);
  int num2 = lookup->getIdByName(pair2.first);

  edge_map_t::iterator ftor = edges.find(num1);
  edge_map_t::iterator etor = edges.end();

  if (ftor != etor) // there is already an edge to this node
  {
    node_list_t::iterator vitor = edges[num1].second.begin();
    node_list_t::iterator vetor = edges[num1].second.end();

    // check all of the nodes that this node connects to. (don't want to add the edge twice)
    for (vitor = vitor; vitor != vetor; ++vitor)
    {
      if ((*vitor).get<0>() == num2)
      {
        break;
      }
    }

    if (vitor == vetor)
    {
      edges[num1].second.push_back(make_tuple(num2, pair2.second, score));
    }
    else
    {
      cerr << "TODO FATAL AddToList():" << num1 << " " << num2 << endl;
      exit(-1);
    }
  }
  else 
  {
    pair<int, node_list_t > tmp;
    tmp.first = score;
    tmp.second.push_back(make_tuple(num2, pair2.second, score));
    edges[num1] = tmp;
  }

  return;
}


//
// This method returns a vector of 'subgraphs' (which here is just a bunch of edges)
//
// a subgraph is created by performing the following steps:
//   1 pick a node 'n'
//   2 for every edge <n, n1> connected to n, add n1 to the subgraph 
//   3 for every n1 added above, perform step 2 (recursive)
//
// basically, pick a node, get all of the nodes connected to it, get all of 
// the nodes connected to those nodes, until you have recursed completely
//
// the idea is to use an edge matrix (although we use a map, not an actual matrix)
// TODO: ASCII art
//
boost::shared_ptr< vector< sharedEdges > > InparaParser::getSubgraphs(void) throw()
{
  boost::shared_ptr< vector<sharedEdges> > all_subgraphs(new vector<sharedEdges>());

  /*
 
  NOTE: the code below is for debugging purposes. 
        to run, uncommment, then modify debug_edges to mimic
        what 'edges' should look like.
        uncomment 'cout's below to depending on how much debug output
        you want...


  edges.clear();
  vector< pair< int, int > > debug_edges;
  debug_edges.push_back(make_pair(0, 8));
  debug_edges.push_back(make_pair(1, 2));
  debug_edges.push_back(make_pair(1, 11));
  debug_edges.push_back(make_pair(1, 6));
  debug_edges.push_back(make_pair(2, 11));
  debug_edges.push_back(make_pair(2, 6));
  debug_edges.push_back(make_pair(3, 7));
  debug_edges.push_back(make_pair(3, 10));
  debug_edges.push_back(make_pair(4, 10));
  debug_edges.push_back(make_pair(11, 6));
  debug_edges.push_back(make_pair(11, 8));
  debug_edges.push_back(make_pair(7, 10));
  debug_edges.push_back(make_pair(8, 9));

  debug_edges.push_back(make_pair(5, 12));

for (vector< pair< int, int > >::iterator itor = debug_edges.begin(); itor != debug_edges.end(); ++itor)
{
  node_list_t forward;
  edges[(*itor).first].second.push_back(make_tuple((*itor).second, 1.0, 50));

  node_list_t backward;
  edges[(*itor).second].second.push_back(make_tuple((*itor).first, 1.0, 50));
}

  map< int, pair<int, node_list_t > >::iterator itor = edges.begin();
  map< int, pair<int, node_list_t > >::iterator etor = edges.end();
  for (itor = itor; itor != etor; ++itor)
  {
    node_list_t::iterator nitor = (*itor).second.second.begin();
    node_list_t::iterator netor = (*itor).second.second.end();
cout << (*itor).first << ":" << (*itor).second.first << "\n";    
    for (nitor = nitor; nitor != netor; ++nitor)
    {
cout << "  " << (*itor).first << ":" << "  " << (*nitor).get<0>() << ";" << (*nitor).get<1>() << ";" << (*nitor).get<2>() << "\n";
    }
cout << "\n";
  }
cout << endl;
*/

  int len = lookup->getMaxIdNum(); // going from 0 -> max ensures the algo. works even with 'holes' in the ids
  int num_graphs = 0;
  int num_valid_graphs = 0;
  map<int, bool> visited;          // keeps track of which nodes we have visited (globally)

  //  node sub
  map<int, int> already_added;        // keeps track of which nodes have been added, and to what subgraph
  map<int, map<int, int> > all_edges; // keeps track of which edges have been added, and to what subgraph

  // loop over all rows of our matrix
  //   (since we're going to maxIdNum, we need to inclue the max)
  for (int i = 0; i <= len; ++i) 
  {
//cout << "i: " << i << endl;
    if (visited.find(i) != visited.end() && visited[i])
    {
//cout << "already visited i: " << i << endl;
      continue;
    }

    sharedEdges subgraph(new Edges_t()); // we'll build this in the following loops
    ++num_graphs;
    visited[i] = true;                   // remember that we've been to this node

    vector<int> nodes_to_visit;
    nodes_to_visit.push_back(i);      // this is the stack of nodes to visit. Start with this one
    map<int, bool> subgraph_visited;  // we use this to make sure we don't recurse
                                      // to a row we've already been to on this subgraph

    string debug_node = "y3775";
    //string debug_node = "STM4542";
    // while we have nodes that we need to visit in our subgraph, keep recursing
    while (nodes_to_visit.size() > 0)
    {
      int row = nodes_to_visit.back();
      nodes_to_visit.pop_back();
//cout << "  row: " << row << " cols:" << edges[row].second.size()<< endl;

  //if (lookup->getNameById(row).compare(debug_node) == 0) cout << "START OF ROW s1:" << lookup->getNameById(row) << " id1:" << row << endl;

      if (edges.find(row) == edges.end() || edges[row].second.size() == 0) continue; // there is no edge for this id (id was thrown out early in processing)

      node_list_t::iterator nitor = edges[row].second.begin();
      node_list_t::iterator netor = edges[row].second.end();
      float c1    = edges[row].second[0].get<1>(); // the confidence of the first node

      // loop over all columns on this row (not necessarily in order)
      for (nitor = nitor; nitor != netor; ++nitor)
      {
        int   col   = (*nitor).get<0>();
        float score = (*nitor).get<2>();

        map<int, int>::iterator rftor  = already_added.find(row);
        map<int, int>::iterator cftor  = already_added.find(col);
        map<int, int>::iterator rcetor = already_added.end();
        map<int, map<int, int> >::iterator aftor = all_edges.find(row);
        map<int, map<int, int> >::iterator eftor = all_edges.end();

  //if (lookup->getNameById(row).compare(debug_node) == 0 || lookup->getNameById(col).compare(debug_node) == 0) cout << "IN COL LOOP B4 CHECK s1:" << lookup->getNameById(row) << " id1:" << row << " s2:" << lookup->getNameById(col) << " id2:" << col << " score:" << score << " already_row:" << (rftor == rcetor) << " || " << ((*rftor).second == i) << " already_col:" << (cftor == rcetor) << " || " << ((*cftor).second == i) << endl;

        if (col <= row)
        {
          //continue; // DO NOT UNCOMMENT. uncommenting effectively limits us to the top half of the matrix
        }
//cout << "    col: " << col << endl;

  //if (lookup->getNameById(row).compare(debug_node) == 0 || lookup->getNameById(col).compare(debug_node) == 0) cout << "IN COL LOOP AFTER CHECK s1:" << lookup->getNameById(row) << " id1:" << row << " s2:" << lookup->getNameById(col) << " id2:" << col << " score:" << score << " already_row:" << (rftor == rcetor) << " || " << ((*rftor).second == num_graphs) << " already_col:" << (cftor == rcetor) << " || " << ((*cftor).second == num_graphs) << endl;


     // if ((we haven't seen this row or we have only seen it on this subgraph) AND
     //     (we haven't seen this col or we have only seen it on this subgraph) AND
     //     (we haven't added this edge before)) 
        if ((rftor == rcetor || (*rftor).second == num_graphs) &&
            (cftor == rcetor || (*cftor).second == num_graphs) &&
            (aftor == eftor || (*aftor).second.find(col) == (*aftor).second.end()))
        {
          // scale the score by the confidence values
          int new_score = (int) (score * ((c1 + (*nitor).get<1>()) / 2.0));
          //if (lookup->getNameById(row).compare(debug_node) == 0 || lookup->getNameById(col).compare(debug_node) == 0) cout << " ADD s1:" << lookup->getNameById(row) << " id1:" << row <<  " s2:" << lookup->getNameById(col) << " id2:"  << col << " score:" << new_score << " subgraph:" << all_subgraphs->size() << endl;
          Edge_t p(new_score, row, col);
          subgraph->push_back(p);
          already_added[row] = num_graphs;
          already_added[col] = num_graphs;
          all_edges[row][col] = num_graphs; // remember that we've seen this edge (in both directions)
          all_edges[col][row] = num_graphs;
        
          //cout << "      ADD EDGE:" << row << ":" << col << endl;
        }

        // if we need to visit col
        if ( visited.find(col) == visited.end() || !subgraph_visited[col]) 
        {
          nodes_to_visit.push_back(col);
//cout << "      push col:" << col << endl;
        }

        visited[col] = true;          // we've visited this column (globally)
        subgraph_visited[col] = true; // we've visited this column (in this subgraph)
      }
    }

    // if we added edges to this subgraph, save it
    if (subgraph->size() > 0)
    {
//cout << "new subgraph starting with " << i << endl;
      ++num_valid_graphs;
      all_subgraphs->push_back(subgraph);  // add graph to vector of graphs
    }
  }

  edges.clear();
  return(all_subgraphs);
}


// Operators


// Public Methods

string InparaParser::print(void) const throw()
{
  stringstream output;

  output << "NOT IMPLEMENTED";

  return(output.str());
}


ostream & operator<<(ostream & p_os, InparaParser const * p_inpara_parser) throw()
{
  return(p_os << p_inpara_parser->print());
}


ostream & operator<<(ostream & p_os, InparaParser const & p_inpara_parser) throw()
{
  return(p_os << p_inpara_parser.print());
}
