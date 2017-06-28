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

//  FROM: http://www.sicmm.org/~konc/maxclique/
//
//  KONC, Janez, JANE.I., Du.anka. An improved branch and bound algorithm for the maximum clique problem. MATCH Commun. Math. Comput. Chem., 2007, 58, 569-590
//
//  INPUT: Graph G = (V,E) must given:
//         V => ARRAY1[1..N] of vertices 
//         E => ARRAY1 of edges for each vertex (as many arrays as there are vertices)


//  DEVELOPER NOTE:  For Performance to turn off all BOOST assertions
#define BOOST_DISABLE_ASSERTS


// System Includes
#include <config.h>
#include <fstream>
#include <iostream>
#include <list>
#include <math.h>
#include <queue>
#include <set>
#include <sstream>
#include <stdio.h>
#if HAVE_STDLIB_H
#include <stdlib.h>
#endif
#include <string>
#include <vector>
// External Includes
// Internal Includes
// Application Includes
#include "Solver.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "Solver"


// Namespaces used
using namespace std;


// Constructors


Solver::Solver(u_int16_t const p_node_count) throw() :
  node_count(p_node_count),
  num_level((double)9.0),
  C(vector<ARRAY_FIX>(p_node_count)),
  E(vector<ARRAY1>(p_node_count)),
  e(vector< vector<int> >(p_node_count, vector<int>(p_node_count, 0))),
  pk(0),
  nk(0),
  nk_size(0)
{
  Q.ele.resize(node_count);
  QMAX.ele.resize(node_count);

  vector<ARRAY_FIX>::iterator itor = C.begin();
  vector<ARRAY_FIX>::iterator etor = C.end();
  for(itor=itor; itor != etor; ++itor)
  {
    (*itor).ele.resize(node_count);
  }
  return;
}


// Operators


// Public Methods


int Solver::CUT1(int p, ARRAY_FIX &B) throw()
{
  //  return 1 if intersection of A and B is not empty
  //  return 0 if there are no elements in common to A and B
  //  in C we return the intersecting elements

  int j;
  for (j = 0; j < B.size; ++j)
  {
    if (e[p][B.ele[j]] == 1)
    {
      break;
    }
  }

  if (j == B.size)
  {
    return(0);
  }
  else 
  {
    return(1);
  }
}


int Solver::CUT2(int p, ARRAY2 B, ARRAY2 & C) throw()
{
  //  return 1 if intersection of A and B is not empty
  //  return 0 if there are no elements in common to A and B
  //  in C we return the intersecting elements

  C.size = 0;
  for (int j = 0; j < B.size - 1; ++j)
  {
    if (e[p][B.ele[j].first] == 1)
    {
      C.ele[C.size++].first = B.ele[j].first;
    }
  }

  if (C.size == 0)
  {
    return(0);
  }
  else 
  {
    return(1);
  }
}


void Solver::NUMBER_SORT(ARRAY2 &R) throw()
{
  int k;
  int p;

  int i     = 0;
  int j     = 0;
  int min_k = QMAX.size - Q.size + 1;
  int maxno = 1;
  C[1].size = 0;
  C[2].size = 0;

  while (i < R.size)
  {
    p = R.ele[i].first;
    k = 1;

    while (CUT1(p, C[k]) != 0)
    {
      ++k;
    }

    if (k > maxno)
    {
      maxno = k;
      C[maxno+1].size = 0;
    }

    C[k].ele[C[k].size++] = R.ele[i].first;

    if (k < min_k)
    {
      R.ele[j++].first = R.ele[i].first;
    }
    ++i;
  }

  if (j > 0)
  {
    R.ele[j-1].second = 0;
  }

  if (min_k <= 0)
  {
    min_k = 1;
  }

  for (k = min_k; k <= maxno; ++k)
  {
    for (i = 0; i < C[k].size; ++i)
    {
      R.ele[j].first    = C[k].ele[i];
      R.ele[j++].second = k;
    }
  }

  return;
}


void Solver::COPY(ARRAY_FIX & A, ARRAY_FIX B) throw()
{
  for (int i = 0; i < B.size; ++i)
  {
    A.ele[i] = B.ele[i];
  }

  A.size = B.size;

  return;
}


void Solver::DEGREES_SORT(ARRAY2 R) throw()
{
  int    tmp;
  int    k;
  vector<int> E2(R.size, 0);

  for (int i = 0; i < R.size; ++i)
  {
    for (int j = 0; j < i; ++j)
    {
      if (e[R.ele[i].first][R.ele[j].first])
      {
        E2[i]++;
        E2[j]++;
      }
    }
  }

  for (int i = 0; i < R.size; ++i)
  {
    //  Sort vertices V in a descending order with respect to their degree
    k = i;
    for (int j = i + 1; j < R.size; ++j) 
    {
      if (E2[k] < E2[j])
      {
        k = j;
      }
    }

    tmp            = R.ele[i].first;
    R.ele[i].first = R.ele[k].first;
    R.ele[k].first = tmp;
    tmp            = E2[i];
    E2[i]          = E2[k];
    E2[k]          = tmp;
  }

  return;
}


void Solver::EXPAND(ARRAY2 R, int level) throw()
{
  //  First level is level = 1;

  int    p;
  ARRAY2 Rp;

  S.ele[level].first  = S.ele[level].first + S.ele[level - 1].first - S.ele[level].second;
  S.ele[level].second = S.ele[level - 1].first;
  
  while (R.size != 0)
  {
    p = R.ele[R.size - 1].first;  // p is assigned the last vertex in R
    if ((Q.size + R.ele[R.size - 1].second) > QMAX.size)
    {
      Q.ele[Q.size++] = p;
      Rp.ele = (ELEMENT*) calloc(R.size, sizeof(ELEMENT));
      if (CUT2(p, R, Rp) != 0)
      {
        if (((double)S.ele[level].first / pk) < num_level)
        {
          ++nk;
          nk_size += Rp.size;
          DEGREES_SORT(Rp);
        }
        NUMBER_SORT(Rp);
        S.ele[level].first++;
        ++pk;
        EXPAND(Rp, level + 1);
      }
      else if (Q.size > QMAX.size)
      { 
//cout << "at step " << pk << " I found a length of " << Q.size << " qmax" << "\n"; 
        COPY(QMAX, Q); 
      }    
      free(Rp.ele);
      Rp.ele = NULL;
      --Q.size;
    }
    else
    {
      return;
    }
    --R.size;
  }

  return;
}
        

void Solver::MCQ() throw()
{
  int tmp;

  Q.size    = 0;
  QMAX.size = 0;

  //  Sort vertices V in a descending order with respect to their degree
  for (int i = 0; i < V.size; ++i)
  {
    for (int j = i + 1; j < V.size; ++j) 
    {
      if (E[V.ele[i].first].size < E[V.ele[j].first].size)
      {
        tmp            = V.ele[i].first;
        V.ele[i].first = V.ele[j].first;
        V.ele[j].first = tmp;
      }
    }
  }

  int max_degree = E[V.ele[0].first].size;
//cout << "max degree = " << max_degree << "\n";

  for (int i = 0; i < max_degree; ++i)
  {
    V.ele[i].second = i + 1;
  }

  for (int i = max_degree; i < V.size; ++i)
  {
    V.ele[i].second = max_degree + 1;
  }

  S.ele = (ELEMENT*)  calloc(V.size, sizeof(ELEMENT));

  for (int i = 0; i < V.size; ++i)
  {
    S.ele[i].first  = 0;
    S.ele[i].second = 0;
  }

  EXPAND(V, 1);
  free(S.ele);
  S.ele = NULL;

  return;
}


void Solver::assignEdges(int const p_num_vertices, sharedEdges const p_edges) throw()
{
  int      vi;
  int      vj;

  for (int i = 0; i < node_count; ++i)
  {
    for (int j = 0; j < node_count; ++j) 
    {
      e[i][j] = 0;
    }
  }

  V.size = p_num_vertices;
  V.ele  = (ELEMENT*)  calloc(V.size, sizeof(ELEMENT));
  for (int i = 0; i < V.size; ++i)
  {
    V.ele[i].first = i;
    E[i].size      = 0;
    E[i].ele       = (int*)  calloc(V.size, sizeof(int));
  }

  Edges_t::iterator itor = p_edges->begin();
  Edges_t::iterator etor = p_edges->end();
  map<int, int> point_lookup;

  for (itor = itor; itor != etor; ++itor)
  {
    map<int, int>::iterator mitor1 = point_lookup.find(get<Vertex1>((*itor)));
    map<int, int>::iterator metor1 = point_lookup.end();
    if (mitor1 == metor1)
    {
      int idx = point_lookup.size();
      point_lookup[get<Vertex1>((*itor))] = idx;
      points.push_back(get<Vertex1>((*itor)));
    }
    map<int, int>::iterator mitor2 = point_lookup.find(get<Vertex2>((*itor)));
    map<int, int>::iterator metor2 = point_lookup.end();
    if (mitor2 == metor2)
    {
      int idx = point_lookup.size();
      point_lookup[get<Vertex2>((*itor))] = idx;
      points.push_back(get<Vertex2>((*itor)));
    }
  }

  itor = p_edges->begin();
  etor = p_edges->end();
  for (itor = itor; itor != etor; ++itor)
  {
    vi = point_lookup[get<Vertex1>((*itor))];
    vj = point_lookup[get<Vertex2>((*itor))];
//cout << "EDGE:" << vi << ":" << vj << ":\n";

    //--vi;
    //--vj;
    E[vi].ele[E[vi].size++] = vj;
    E[vj].ele[E[vj].size++] = vi;
    e[vi][vj]               = 1;
    e[vj][vi]               = 1;
  }

  return;
}



vector<int> Solver::solve(int const p_num_vertices, sharedEdges const p_edges) throw()
{
  points.clear();
  assignEdges(p_num_vertices, p_edges);

  pk      = 0;
  nk      = 0;
  nk_size = 0;

  
  MCQ();
  
  vector<int> clique;

  for (int j = 0; j < QMAX.size; ++j) 
  {
    clique.push_back(points[QMAX.ele[j]]);
  }
  
  return(clique);
}


string Solver::print(void) const throw()
{
  stringstream output;

  output << "NOT IMPLEMENTED YET!";

  return(output.str());
}


ostream & operator<<(ostream & p_os, Solver const * p_solver) throw()
{
  return(p_os << p_solver->print());
}


ostream & operator<<(ostream & p_os, Solver const & p_solver) throw()
{
  return(p_os << p_solver.print());
}
