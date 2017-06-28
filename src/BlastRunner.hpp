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


#ifndef BLAST_RUNNER_HPP
#define BLAST_RUNNER_HPP


// System Includes
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <sys/stat.h>
#include <vector>
// External Includes
// Internal Includes
// Application Includes
#include "Lookup.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "BlastRunner"


// Namespaces used
using namespace std;
using namespace boost;


//==============================================================================
// Class BlastRunner
//
// API Developer Documentation:   (those who develop/fix this class)
//   [put documentation here for developers that work on this code]
//------------------------------------------------------------------------------
// API User Documentation:  (those who use this class)
//
//   [put a single summary sentence describing this class here]
//
// SYNOPSIS:
//   [put an overview paragraph for this class here]
//
// DESCRIPTION:
//   [put a description section for this class here]
//
// FUTURE FEATURES:
//  None discovered
//
// KNOWN LIMITATIONS:
//  None discovered
//
// SEE ALSO:
//============================================================================== 


class BlastRunner
{

public:

  // Getters/Setters

  bool onlyCreateScripts(void)
  {
    return(this->only_create_scripts);
  }


  vector<string> getBlastScripts(void)
  {
    return(this->blast_scripts);
  }


  // Constructors

  BlastRunner(sharedLookup const & p_lookup,
              string       const & p_out_dir,
              string       const & p_A_fasta,
              string       const & p_B_fasta,
              string       const & p_C_outgroup,
              string       const & p_score_matrix,
              bool         const   p_only_create_scripts) throw();


  // Destructor

  ~BlastRunner(void) throw();


  // Public Functions

  vector< tuple<string, species_type, species_type> > run(void) throw();


private:

  // Getters/Setters


  void addScript(string p_name) throw()
  {
    this->blast_scripts.push_back(p_name);
    return;
  }


  // Constructors

  BlastRunner(BlastRunner const & p_blast_runner) throw();


  // Operators

  BlastRunner &    operator=(BlastRunner const & p_blast_runner) throw();
  bool            operator==(BlastRunner const & p_blast_runner) const throw();
  bool            operator!=(BlastRunner const & p_blast_runner) const throw();


  // Variables

  sharedLookup     lookup;
  string           output_dir;
  string           A_fasta;
  string           B_fasta;
  string           C_outgroup;
  string           blastall;
  string           formatdb;
  string           score_matrix;
  u_int64_t        A_db_size;
  u_int64_t        B_db_size;
  u_int64_t        C_db_size;
  vector<string>   blast_scripts;
  bool             only_create_scripts;


  // Private Functions

  bool formatDb(string    const & p_file) throw();
  bool runOne(  string    const & p_qfile,
                u_int64_t const   p_qsize,
                string    const & p_dfile,
                u_int64_t const   p_dsize,
                string    const & p_outfile) throw();

};

#endif // BLAST_RUNNER_HPP
