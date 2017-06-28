///////////////////////////////////////////////////////////////////////////////
// Copyright © 2013, Battelle Memorial Institute
//
// THIS FILE INITIALLY CREATED WITH:  
//     TEMPLATE NAME:  lang_cpp_test.template 
//     COMMAND NAME:   gensrc 
//
// ND file: $Id$ 
// _____________________________________________________________________________
// 
// PURPOSE OF TEST: 
//  [put a single sentence stating the purpose of this file 
//
// CODING CONVENTIONS: 
//    * Class names are CamelCase with first word upper case 
//    * Functions are camelCase with first word lower case 
//    * Function parameters are lower case with _ and have p_ prefix 
//    * Member variables always use 'this' pointer 
///////////////////////////////////////////////////////////////////////////////


#ifndef UTILITY_HPP
#define UTILITY_HPP

// System Includes
#include <map>
#include <string>
#include <vector>
// External Includes
#include <boost/shared_ptr.hpp>
#include <boost/test/included/unit_test.hpp>
// Internal Includes
// Application Includes
#include "global.hpp"
#include "osUtil.hpp"
#include "FastaReader.hpp"
#include "Lookup.hpp"


using namespace boost;
using namespace std;


float        g_stat_params_tolerance;
sharedLookup g_lookup;
string       g_fasta_dir;
string       g_blastoutfile;
string       g_afile;
string       g_bfile;
string       g_cfile;
string       g_outdir;


void utility_init()
{
  global_init();

  ::g_stat_params_tolerance = 0.005;

  //TODO: point to test data?

  ::g_fasta_dir     = getEnvVar("CLIQUES_HOME") + "/data/output/gold4_fasta";
  ::g_blastoutfile  = getEnvVar("CLIQUES_HOME") + "/data/output/gold4_out_dir/E_coli_K12_MG1655-S_enterica_Typhi_CT18.blastout";
  ::g_afile         = ::g_fasta_dir + "/E_coli_K12_MG1655.fasta";
  ::g_bfile         = ::g_fasta_dir + "/S_enterica_Typhi_CT18.fasta";
  ::g_cfile         = ::g_fasta_dir + "/OUTGROUP_M_thermoautotrophicus_DeltaH.fasta";

  vector<string> vec;
  vec.push_back(::g_afile);
  vec.push_back(::g_bfile);
  vec.push_back(::g_cfile);

  sharedLookup   lookup(new Lookup(vec, ::g_cfile));
  lookup->registerFileLocation("blastall");
  lookup->registerFileLocation("formatdb");
  lookup->registerFileLocation("/../data/BLOSUM62");
  ::g_output_helper->register_file(e_ortholog,"/var/tmp/test.ortho");
  ::g_output_helper->register_file(e_paralog, "/var/tmp/test.table");
  ::g_output_helper->register_file(e_journal, "/var/tmp/test.anno");
  ::g_output_helper->register_file(e_rejects, "/var/tmp/test.reje");

  ::g_lookup = lookup;

  /* NOTE: this code was moved into Lookup
  shared_ptr<FastaReader> areader(new FastaReader(::g_afile));
  shared_ptr<FastaReader> breader(new FastaReader(::g_bfile));
  FastaSequence seq;

  while (areader->good())
  {
    seq = areader->get_next();
    ::g_lookup->addName(e_species_A, seq.name, seq.sequence.size());
  }

  while (breader->good())
  {
    seq = breader->get_next();
    ::g_lookup->addName(e_species_B, seq.name, seq.sequence.size());
  }
  */

  return;
}


bool areNumbersClose(double d1, double d2, double tolerance)
{
   if (abs(d1 - d2) <= tolerance)
   {
      return true;
   }
   return false;
}


#endif
