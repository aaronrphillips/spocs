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
#include <iostream>
#include <sstream>
// External Includes
#include <boost/lexical_cast.hpp>
// Internal Includes
// Application Includes
#include "global.hpp"
#include "Solver.hpp"
#include "Subgraph.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "Subgraph"


// Namespaces used
using namespace std;
using namespace boost;
using boost::lexical_cast;


// Static variables

u_int32_t Subgraph::subgraph_number = 0;


// Constructors


Subgraph::Subgraph(string       const & p_graph_dir,
                   sharedEdges  const & p_graph,
                   sharedLookup const & p_lookup,
                   u_int32_t    const   p_min_nodes,
                   u_int32_t    const   p_max_nodes,
                   float        const   p_edge_threshold,
                   bool         const   p_produce_html) throw() :
  lookup(p_lookup),
  min_nodes(p_min_nodes),
  max_nodes(p_max_nodes),
  edge_threshold(p_edge_threshold),
  orig_edge_count((u_int32_t) p_graph->size()),
  edge_count(0),
  max_edge_count(0),
  my_node_count(0),
  species_count(0),
  clique_number(0),
  produce_html(p_produce_html)
{
  sharedEdges tmp_edges(new vector<Edge_t>());
  original_edges = tmp_edges;
  Subgraph::subgraph_number++;
  graph_dir = p_graph_dir + "/subgraph." + lexical_cast<string>(subgraph_number);

  Edges_t::iterator itor = p_graph->begin();
  Edges_t::iterator etor = p_graph->end();
  string str("SUBGRAPH:");
  for (itor = itor; itor != etor; ++itor)
  {
    vertices[get<Vertex1>((*itor))] = 1;
    vertices[get<Vertex2>((*itor))] = 1;
    species[lookup->getSpeciesById(get<Vertex1>((*itor)))] = 1;
    species[lookup->getSpeciesById(get<Vertex2>((*itor)))] = 1;
    group[get<Vertex1>((*itor))] = 0;
    group[get<Vertex2>((*itor))] = 0;
    original_vertices[lookup->getNameById(get<Vertex1>((*itor)))] = get<Vertex1>((*itor));
    original_vertices[lookup->getNameById(get<Vertex2>((*itor)))] = get<Vertex2>((*itor));
    Edge_t tmp_tup(get<Score>((*itor)), get<Vertex1>((*itor)), get<Vertex2>((*itor)));
    original_edges->push_back(tmp_tup);
  }

  edge_count     = (u_int32_t) p_graph->size();
  my_node_count  = (u_int32_t) vertices.size();

  if (my_node_count < min_nodes)
  {
    return;
  }

  max_edge_count = (u_int32_t) (my_node_count * (my_node_count - 1)) >> 1; // # edges required for clique
  species_count  = (u_int32_t) species.size();
  
  u_int32_t tolerance      = (u_int32_t)((float)max_edge_count * this->edge_threshold);
  u_int32_t tol_edge_count = max_edge_count - tolerance;

  if      ((my_node_count == species_count) && (edge_count    == max_edge_count))
  {
    type = Complete;
  }
  else if ((my_node_count == species_count) && (edge_count    >= tol_edge_count))
  {
    type = Semicomplete;
  }
  else if ((my_node_count == species_count) && (edge_count    <  tol_edge_count))
  {
    type = Incomplete;
  }
  else if ((my_node_count >  species_count) && (my_node_count <  max_nodes))
  {
    type = Degenerate;
  }
  else
  {
    type     = Unknown;
    sub_type = Unknown;
cerr << "skipping subgraph #"  << Subgraph::subgraph_number  << ":" << endl;
cerr << "edge_count:"          << edge_count     << endl;
cerr << "my_node_count:"       << my_node_count  << endl;
cerr << "max_edge_count:"      << max_edge_count << endl;
cerr << "species_count:"       << species_count  << endl;
cerr << "tol_edge_count:"      << tol_edge_count << endl;
    return;
  }

  sub_type = type;
  ++clique_number;

  makeDirectory(graph_dir, "subgraph output");

  // we won't have 'subgraph' html pages (only clique pages), so we don't need the 
  // fasta file corresponding to the subgraph html page
  if (type == Degenerate || type == Incomplete)
  {
    stringstream file_name;
    file_name << getGraphOutputDir() << "/subgraph." << subgraph_number << ".fasta";
    saveFasta(p_graph, file_name.str());
  }

  return;
}


// this function clears out p_clique, then finds the largest clique in p_graph, removes the 
// corresponding edges from p_graph, and returns the corresponding nodes in p_clique 
void Subgraph::updateClique(sharedEdges & p_graph, sharedClique & p_clique) throw()
{
  // Wipe out all the nodes in the clique solution
  p_clique->solution.clear();
  p_clique->setCliqueNumber(clique_number);
  vector<int>   local_solution;
  map<int, int> clique_vert;

  Edges_t::iterator itor = p_graph->begin();
  Edges_t::iterator etor = p_graph->end();
  string str("SUBGRAPH:");
  map<int, map<int, int> > scores;
  vertices.clear();
  species.clear();

  for (itor = itor; itor != etor; ++itor)
  {
    u_int32_t v1 = get<Vertex1>((*itor));
    u_int32_t v2 = get<Vertex2>((*itor));
    string    s1 = lookup->getSpeciesById(v1);
    string    s2 = lookup->getSpeciesById(v2);

    vertices[v1] = 1;
    vertices[v2] = 1;
    species[s1]  = 1;
    species[s2]  = 1;

    scores[get<Vertex1>((*itor))][get<Vertex2>((*itor))] = get<Score>((*itor));
    scores[get<Vertex2>((*itor))][get<Vertex1>((*itor))] = get<Score>((*itor));
  }

  // decide on the 'subtype' of this clique (based on the type of the overall subgraph and the remaining
  // edges available to the clique

  edge_count     = (u_int32_t) p_graph->size();
  my_node_count  = (u_int32_t) vertices.size();
  max_edge_count = (u_int32_t) (my_node_count * (my_node_count - 1)) >> 1;
  species_count  = (u_int32_t) species.size();

  u_int32_t tolerance = (u_int32_t)((float)max_edge_count * this->edge_threshold);
  u_int32_t tol_edge_count = max_edge_count - tolerance;

  if      ((my_node_count == species_count) && (edge_count    == max_edge_count))
  {
    sub_type = Complete;
  }
  else if ((my_node_count == species_count) && (edge_count    >= tol_edge_count))
  {
    sub_type = Semicomplete;
  }
  else if ((my_node_count == species_count) && (edge_count    <  tol_edge_count))
  {
    sub_type = Incomplete;
  }
  else if ((my_node_count >  species_count) && (my_node_count <  max_nodes))
  {
    sub_type = Degenerate;
  }
  else
  {
    sub_type = Unknown;
cerr << "skipping clique #" << clique_number  << ":" << endl;
cerr << "edge_count:"       << edge_count     << endl;
cerr << "my_node_count:"    << my_node_count  << endl;
cerr << "max_edge_count:"   << max_edge_count << endl;
cerr << "species_count:"    << species_count  << endl;
cerr << "tol_edge_count:"   << tol_edge_count << endl;
    return;
  }

  if (edge_count < 1)
  {
    return; // done processing this subgraph
  }

  if (sub_type != Degenerate && sub_type != Incomplete)
  {
    // no need to solve - the remaining edges form a complete/semicomplete clique
    map<u_int32_t, u_int32_t>::iterator itor = vertices.begin(); 
    map<u_int32_t, u_int32_t>::iterator etor = vertices.end(); 
    for (itor = itor; itor != etor; ++itor)
    {
      p_clique->solution.push_back(lookup->getNameById((*itor).first));
      local_solution.push_back((*itor).first);
      clique_vert[(*itor).first]++;
    }
  }
  else
  {
//if (Subgraph::subgraph_number == 25) cout << "gettin' ready to solve... nodecount:" << getNodeCount() << endl;
    solver = new Solver((u_int16_t)getNodeCount());
    vector<int> clique = solver->solve(getNodeCount(), p_graph);
//if (Subgraph::subgraph_number == 25) cout << "solved... clique node count:" << clique.size() << endl;
    delete solver;
    vector<int>::iterator itor = clique.begin(); 
    vector<int>::iterator etor = clique.end(); 

    map<string, vector<int> > species_count;
    
    // build map of species counts
    for (itor = itor; itor != etor; ++itor)
    {
      species_count[lookup->getSpeciesById(*itor)].push_back(*itor);
    }

    map<string, vector<int> >::iterator mitor = species_count.begin();
    map<string, vector<int> >::iterator metor = species_count.end();

    // iterate over species count map
    for (mitor = mitor; mitor != metor; ++mitor)
    {
      if ((*mitor).second.size() > 1)
      {
        // more than one node with this species
        vector<int>::iterator vitor = (*mitor).second.begin();   
        vector<int>::iterator vetor = (*mitor).second.end();   
        
        map<int, int, greater<int> > sums;

        // iterate over the vertices that are of the same species
        for (vitor = vitor; vitor != vetor; ++vitor)
        {
          u_int32_t sum = 0;

          // sum the scores of all edges connected to this node (but only ones in the clique)
          for (itor = clique.begin(); itor != clique.end(); ++itor)
          {
            if ((*vitor) != (*itor))
            {
              sum += scores[(*vitor)][(*itor)];
            }
          }

          sums[sum] = (*vitor);
        }

        p_clique->solution.push_back(lookup->getNameById((*(sums.begin())).second));
        local_solution.push_back((*(sums.begin())).second);
        clique_vert[(*(sums.begin())).second]++;
      }
      else
      {
        // only one node with this species
        p_clique->solution.push_back(lookup->getNameById((*mitor).second[0]));
        local_solution.push_back((*mitor).second[0]);
        clique_vert[(*mitor).second[0]]++;
      }
    }
  }

  map<int, int>::iterator cvend = clique_vert.end();
  sharedEdges dumper(new vector<Edge_t>());
  // remove any edges connected to vertices in the solution
  vector<int>::iterator sitor = local_solution.begin();
  vector<int>::iterator setor = local_solution.end();
  for (sitor = sitor; sitor != setor; ++sitor)
  {
    itor = p_graph->begin();
    etor = p_graph->end();
    for (itor = itor; itor != p_graph->end();)
    {
      if (get<Vertex1>(*itor) == (*sitor) || get<Vertex2>(*itor) == (*sitor))
      {
        Edge_t tmp_tuple(*itor);
        if (clique_vert.find(get<Vertex1>(tmp_tuple)) != cvend && 
            clique_vert.find(get<Vertex2>(tmp_tuple)) != cvend)
        {
          dumper->push_back(tmp_tuple);
        }
        
        itor = p_graph->erase(itor);
      }
      else
      {
        ++itor;
      }
    }
  }

  p_clique->solution.sort();
  p_clique->setEdgeCount(dumper->size());

  if (p_clique->solution.size() > 1)
  {
    if (getGraphType() == Degenerate)
    {
      sub_type = DegenerateSolved;
    }
    if (getGraphType() == Incomplete)
    {
      sub_type = IncompleteSolved;
    }
  }
  p_clique->setGraphType(sub_type);

  saveCliqueJson(dumper);
  ++clique_number;

  return;
}


void Subgraph::saveCliqueJson(sharedEdges const & p_edges) throw()
{
  if (p_edges->size() < (min_nodes - 1))
  {
    return;
  }

  stringstream file_name;
  file_name << getGraphOutputDir() << "/subgraph." << subgraph_number << "." << clique_number << '_' << p_edges->size();
  saveFasta(p_edges, string(file_name.str() + ".fasta"));

  if (! produce_html)
  {
    return;
  }
  
  map<string, int> vertices;
  Edges_t::iterator itor = p_edges->begin();
  Edges_t::iterator etor = p_edges->end();

  for (itor = itor; itor != etor; ++itor)
  {
    u_int32_t v1    = get<Vertex1>((*itor));
    u_int32_t v2    = get<Vertex2>((*itor));
    //u_int32_t score = get<Score>((*itor));
    string    s1    = lookup->getNameById(v1);
    string    s2    = lookup->getNameById(v2);

    vertices[s1] = v1;
    vertices[s2] = v2;
    group[v1]    = clique_number;
    group[v2]    = clique_number;
  }

  saveJson(file_name.str(), p_edges, vertices);

  return;
}


void Subgraph::saveFasta(sharedEdges const & p_edges, string const & p_filename) throw()
{
  if (p_edges->size() < 1)
  {
    return;
  }


  Edges_t::iterator itor = p_edges->begin();
  Edges_t::iterator etor = p_edges->end();

  map<string, int> vertices;

  // get the set of vertices for this subgraph/clique
  for (itor = itor; itor != etor; ++itor)
  {
    u_int32_t v1 = get<Vertex1>((*itor));
    u_int32_t v2 = get<Vertex2>((*itor));
    string    s1 = lookup->getNameById(v1);
    string    s2 = lookup->getNameById(v2);
    vertices[s1] = v1; 
    vertices[s2] = v2; 
  }

  map<string, int>::iterator vitor = vertices.begin();
  map<string, int>::iterator vetor = vertices.end();
  if (produce_html)
  {
    string filename = p_filename + ".json";
    ofstream save(filename.c_str());
    save << "var fasta = new Object();\n";
    for (vitor = vitor; vitor != vetor; ++vitor)
    {
      string full_sequence = lookup->getSequenceRaw((*vitor).first);
      save << "fasta[\"" << (*vitor).first << "\"] = \"" << full_sequence << "\";\n";
    }
    save.close();
  }
  else
  {
    ofstream save(p_filename.c_str());
    for (vitor = vitor; vitor != vetor; ++vitor)
    {
      save << lookup->getSequence((*vitor).first) << endl;
    }
    save.close();
  }

  return;
}


void Subgraph::saveJson(string const & p_file_name, sharedEdges const & p_edges, map<string, int> & p_vertices) throw()
{
  map<string, int>::iterator vitor = p_vertices.begin();
  map<string, int>::iterator vetor = p_vertices.end();
  vector<int> vertex_vec;

  if (p_vertices.size() < this->min_nodes)
  {
    return;
  }

  ofstream writed3(string(p_file_name + ".json").c_str());
  writed3 << "var json = JSON.parse('{\"nodes\": [";

  // map the vertex id's down to 0-n
  int i = 0;
  for (vitor = vitor; vitor != vetor; ++vitor)
  {
    u_int32_t group_num = group[(*vitor).second];
    //cerr << "node " << (*vitor).second << " is in group " << group_num << endl;
    (*vitor).second = i;
    if (i > 0) writed3 << ",";
    writed3 << "{\"name\":\"" << (*vitor).first << "\"" << ",\"group\":" << group_num << ",\"species\":\"" << lookup->getSpeciesByName((*vitor).first) << "\"}";
    ++i;
  }

  writed3 << "],\"links\":[";

  Edges_t::iterator itor = p_edges->begin();
  Edges_t::iterator etor = p_edges->end();
  i = 0;
  for (itor = itor; itor != etor; ++itor)
  {
    u_int32_t v1    = get<Vertex1>((*itor));
    u_int32_t v2    = get<Vertex2>((*itor));
    u_int32_t score = get<Score>((*itor));
    string    s1    = lookup->getNameById(v1);
    string    s2    = lookup->getNameById(v2);

    if (i > 0) { writed3 << "," ;}
    writed3 << "{\"source\":" << p_vertices[s1] << ",\"target\":" << p_vertices[s2] << ",\"value\":" << score << "}";
    ++i;
  }
  writed3 << "]}');";

  return;
}


void Subgraph::saveSubgraphJson(void) throw()
{
  if (! produce_html)
  {
    return;
  }

  // if the subgraph is complete or semicomplete, then a subgraph page is redundant
  if (type == Complete || type == Semicomplete)
  {
    return;
  }

  stringstream file_name;
  file_name << getGraphOutputDir() << "/subgraph." << subgraph_number << ".all";
 
  saveJson(file_name.str(), original_edges, original_vertices);

  return;
}


// Operators


// Public Methods

string Subgraph::print(void) const throw()
{
  stringstream output;

  output << "Subgraph print - NOT IMPLEMENTED YET" << endl;

  return(output.str());
}


ostream & operator<<(ostream & p_os, Subgraph const * p_subgraph) throw()
{
  return(p_os << p_subgraph->print());
}


ostream & operator<<(ostream & p_os, Subgraph const & p_subgraph) throw()
{
  return(p_os << p_subgraph.print());
}
