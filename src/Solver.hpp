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


#ifndef SOLVER_HPP
#define SOLVER_HPP


// System Includes
#include <map>
#include <string>
#include <vector>
// External Includes
// Internal Includes
// Application Includes
#include "global.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "Solver"


// Namespaces used
using namespace std;


typedef pair<int, int> ELEMENT;


class ARRAY1
{
public:
  int *ele;
  int size;
  int first;
};


class ARRAY2
{
public:
  ELEMENT *ele;
  int size;
  int first;
};


class ARRAY_FIX
{
public:
  vector<int> ele;
  int size;
  int first;
};


class Solver
{

public:

  // Getters & Setters


  // Constructors

  Solver(u_int16_t const p_value) throw();


  // Destructor

  ~Solver(void) throw()
  {
    return;
  }


  // Public Methods

  vector<int> solve(int const p_num, sharedEdges const p_edges) throw();
  string print(void) const throw();


private:

  // Getters/Setters


  // Constructors

  Solver(void) throw();
  Solver(Solver const & p_solver) throw();


  // Operators

  bool operator=(Solver const & p_solver) const throw();


  // Methods

  int  CUT1(int p, ARRAY_FIX &B) throw();
  int  CUT2(int p, ARRAY2 B, ARRAY2 &C) throw();
  void NUMBER_SORT(ARRAY2 &R) throw();
  void COPY(ARRAY_FIX &A, ARRAY_FIX B) throw();
  void DEGREES_SORT(ARRAY2 R) throw();
  void EXPAND(ARRAY2 R, int level) throw();
  void MCQ() throw();
  void assignEdges(int const p_num_vertices, sharedEdges const p_edges) throw();

  // Variables

  u_int16_t             node_count;
  double                num_level;
  vector< ARRAY_FIX > C;
  vector<ARRAY1>        E;
  ARRAY2                V;
  ARRAY2                S;
  ARRAY_FIX             Q;
  ARRAY_FIX             QMAX;
  vector< vector<int> > e;
  int                   pk;
  int                   nk;
  int                   nk_size;
  vector<int>           points;


friend
  ostream & operator<<(ostream & p_os, Solver const * p_solver) throw();
friend
  ostream & operator<<(ostream & p_os, Solver const & p_solver) throw();

};


#endif // SOLVER_HPP
