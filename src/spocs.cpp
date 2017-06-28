///////////////////////////////////////////////////////////////////////////////
// Copyright © 2013, Battelle Memorial Institute
//
// THIS FILE INITIALLY CREATED WITH:  
//     TEMPLATE NAME:  lang_cpp_exe.template 
//     COMMAND NAME:   gensrc 
//
// ND exe: $Id$ 
// _____________________________________________________________________________
// 
// SYNOPSIS: 
//  [put a single sentence stating purpose of this file] 
//
// OPTIONS: 
//    [list options and their descriptions here] 
// 
// DESCRIPTION: 
//  [describe in detail the purpose and uses of this file] 
//
// EXAMPLES: 
//  [list examples of usage, or whatever is appropriate] 
//
// DESIGN: 
//  [describe in detail the design of this file] 
//
// ENVIRONMENT VARIABLES: 
//  [describe any environment variables used in this file] 
//
// LIMITATIONS: 
//  [list any known limitations/problems with this file] 
//
// SEE ALSO: 
//  [list any other relevant documentation] 
//
// CODING CONVENTIONS: 
//    * Class names are CamelCase with first word upper case 
//    * Functions are camelCase with first word lower case 
//    * Function parameters are lower case with _ and have p_ prefix 
//    * Member variables always use 'this' pointer 
///////////////////////////////////////////////////////////////////////////////

// System Includes
#include <config.h>
#include <ctime>
#include <fstream>
#include <iostream>
#if HAVE_LIBGEN_H
#include <libgen.h>
#endif
#include <map>
//#include <mpi.h>
#include <sstream>
#include <stdio.h>
#if HAVE_STDLIB_H
#include <stdlib.h>
#endif
#include <string>
#include <vector>
// External Includes
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/regex.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/progress.hpp> 
#include <boost/serialization/string.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/xpressive/xpressive.hpp>
// Internal Includes
// Application Includes
#include "global.hpp"
#include "AlignmentsCollection.hpp"
#include "BlastRunner.hpp"
#include "CliqueRunner.hpp"
#include "Clusters.hpp"
#include "FastaReader.hpp"
#include "Lookup.hpp"
#include "Metadata.hpp"
#include "OutputHelper.hpp"
#include "ProgramOptions.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "main"


// Namespaces used
using namespace std;
using namespace boost;
using namespace boost::filesystem;


bool            g_verbose(true);
int             g_num_procs(1);
sharedLookup    g_lookup(new Lookup(false));
string          g_lookup_file;
vector<string>  g_fasta_files;
ProgramOptions  g_po; 


void saveLookup(sharedLookup const & lookup, string const & filename)
{   
  //  NOTE: Using the boost library to serialize the global lookup
  //        object into a binary file so that it does not have to
  //        be recreated from scratch next time.
  std::ofstream ofs(filename.c_str());
  boost::archive::binary_oarchive oa(ofs);
  oa << *(lookup.get());

  return;
}
 

void restoreLookup(sharedLookup & lookup, string const & filename)
{
  //  NOTE: Using the boost library to deserialize the binary file
  //        into an object so that it does not have to
  //        be recreated from scratch.
cerr << "RESTORING lookup data from file: " << filename << endl;
  std::ifstream ifs(filename.c_str());
  boost::archive::binary_iarchive ia(ifs);
  ia >> *(lookup.get());

  return;
} 


u_int32_t calculateNumberFiles(u_int16_t const p_num_species, bool const p_debug)
{
  //  NOTE: Formula from http://en.wikipedia.org/wiki/Combination
  //  result = 16 + (12 * (p_num_species - 2)) + (8 * ((p_num_species - 1) choose 2))

  u_int32_t N_choose_2 = ((p_num_species - 1) * (p_num_species - 2)) / 2;
  u_int32_t result     = 16 + (12 * (p_num_species - 2)) + (8 * N_choose_2);

  if (! p_debug)
  {
    result -= (3 * (((p_num_species) * (p_num_species - 1)) / 2));
  }

  return(result);
}


void checkFastaFiles(vector<string> const & p_files) throw()
{
  vector<string>::const_iterator itor = p_files.begin();
  vector<string>::const_iterator etor = p_files.end();
  map<string, string> lookup;

  // NOTE: blast search space is the total search space used by the statistics
  //       model. The blast search space does not include the outgroup file.
  //       The blast search space is multiplied by 1.5 just in case the user
  //       decides to add another species later.

  for (itor = itor; itor != etor; ++itor)
  {
    boost::shared_ptr<FastaReader> species_file(new FastaReader((*itor)));
    FastaSequence seq;
    while (species_file->good())
    {   
      seq = species_file->get_next();
      map<string, string>::const_iterator lftor = lookup.find(seq.name);
      map<string, string>::const_iterator letor = lookup.end();
      if (lftor != letor)
      {
        if ((*itor) == lftor->second)
        {
          FATAL("Duplicates", "Looking for duplicate names", "Found duplicate sequences named, " + seq.name + ", in file, " + lftor->second + "\n");
        }
        else
        {
          FATAL("Duplicates", "Looking for duplicate names", "Duplicate sequence, " + seq.name + ", found in file, " + lftor->second + ", and file, " + (*itor) + "\n");
        }
      }
      lookup[seq.name] = (*itor);
    }
  }

  return;
}


void process(sharedLookup const & p_lookup,
             string       const & p_species_A,
             string       const & p_species_B,
             string       const & p_outgroup_file,
             string       const & p_output_dir,
             string       const & p_score_matrix,
             bool         const   p_write_scripts_only)
{
  //  Generate BLAST *.blastout files
  BlastRunner blast_runner(p_lookup, p_output_dir, p_species_A, p_species_B, p_outgroup_file, p_score_matrix, p_write_scripts_only);
  vector< tuple<string, species_type, species_type> > out_files = blast_runner.run();

  if (g_po.getWriteBlastScriptsOnly()) { return;}

  //  Parse BLAST *.blastout files - generate *.hits files
  sharedAlignmentsCollection alignments(new AlignmentsCollection(p_lookup));
  alignments->getAlignments(out_files);
  alignments->findOrthologs();
  g_output_helper->write(e_ortholog, alignments->printOrthologs());

  // cluster all the hits into paralogs/clusters/groups/etc...
  Clusters clusters(p_lookup, alignments);
  clusters.makeClusters();
  g_output_helper->write(e_paralog, clusters.print());


  return;
}


void master_init(void)
{
  checkFastaFiles(g_fasta_files);

  vector<string> file_names(g_po.getSpeciesFiles()); 
  if (! g_po.getOutgroupFile().empty())
  {
    file_names.push_back(g_po.getOutgroupFile());
  }

  if (exists(g_lookup_file))
  {
    // TODO: This should only happen if the date on the lookup_file
    //       is newer than ALL the other files in the directory
cerr << "RESTORING Lookup..." << endl;
    restoreLookup(g_lookup, g_lookup_file);
  }
  else
  {
cerr << "BUILDING Lookup..." << endl;
    sharedLookup tmp(new Lookup(file_names,
                                g_po.getOutgroupFile(),
                                g_po.getInnerConfidenceCutoff(),
                                g_po.getSequenceOverlapCutoff(),
                                g_po.getSegmentOverlapCutoff(),
                                g_po.getGroupOverlapCutoff(),
                                g_po.getOutgroupCutoff()));
    g_lookup = tmp;
    saveLookup(g_lookup, g_lookup_file);
  }
 
  //TODO: should we modify registerFileLocation to work, and use it in BlastRunner?
  //g_lookup->registerFileLocation("blastall");
  //g_lookup->registerFileLocation("formatdb");
  //g_lookup->registerFileLocation("/../data/" + g_po.getScoreMatrix());

  cerr << "Total number of Species      = " << g_fasta_files.size() << endl;
  cerr << "Total number of output files = " << calculateNumberFiles(g_fasta_files.size(), g_po.getDebugFlag()) << "\n";

  return;
}


int main(int argc, char ** argv)
{
  timestamp("SPOCS START:");

  if (! g_po.checkOptions(argc, argv))
  {
    FATAL("Program options", "Checking program options", "Bad command line options\n");
  }

  ::global_init();

  Metadata metadata(g_po.getMetadataFile(), g_po.getReportDir());

  if (! metadata.isValid())
  {
    exit(-1);
  }

  //  DEVELOPER NOTE:  This is needed for the Web App to check to make sure that
  //                   the metadata that will be uploaded and used to overlay data
  //                   over the cliques is a valid format. If it is invalid, there
  //                   is no need to waste time on the cluster runnning the blastall
  //                   code and generating tons of invalid files.
  if (g_po.getValidateMetadataOnly())
  {
    exit(0);
  }

  if (g_po.getForceFlag())
  {
    //  NOTE: If g_po.getForceFlag() is set, delete all files in the out_dir
    removeAll(g_po.getOutputDir(), "bin");
    removeAll(g_po.getOutputDir(), "blastout");
    removeAll(g_po.getOutputDir(), "hits");
    removeAll(g_po.getOutputDir(), "paralogs");
    removeAll(g_po.getOutputDir(), "sh");
  }

  if (! g_po.getOutgroupFile().empty())
  {
    // DEVELOPER NOTE: We can't predict if the outgroup file is different
    //                 than a previous run so we can only assume that we
    //                 have to delete all *.paralog files from out_dir
    removeAll(g_po.getOutputDir(), "paralogs");
  }

  g_lookup_file                       = g_po.getOutputDir() + "/lookup.bin";
  g_fasta_files                       = g_po.getSpeciesFiles();
  vector<string>::const_iterator itor = g_fasta_files.begin();
  vector<string>::const_iterator etor = g_fasta_files.end();
  vector<string>::const_iterator jtor;

  //  NOTE: This HAS to happen after we remove lookup.bin from g_po.getForceFlag()
  master_init();

  cerr << "Finding PARALOGS\n"; 
  progress_display show_progress(g_fasta_files.size() * (g_fasta_files.size() - 1) / 2);

  for (itor = g_fasta_files.begin(); itor != (etor - 1); ++itor)
  {
    for (jtor = itor + 1; jtor != etor; ++jtor)
    {
      // always register each file
      g_output_helper->register_file(e_ortholog, "/dev/null");
      g_output_helper->register_file(e_journal,  "/dev/null");
      g_output_helper->register_file(e_rejects,  "/dev/null");
      g_output_helper->register_file(e_paralog,  "/dev/null");

      string spec_1 = getSpeciesFromPath(*itor);
      string spec_2 = getSpeciesFromPath(*jtor);
      string prefix = g_po.getOutputDir() + '/' + spec_1 + '-' + spec_2;
      if (! exists(prefix + getParalogFileExt()))
      {
        // if we do this, it will create an empty .paralogs file, at minimum. This is bad if we only
        // want to create scripts
        if (! g_po.getWriteBlastScriptsOnly())
        {
          g_output_helper->register_file(e_paralog,    prefix + getParalogFileExt());
        }
        if (g_po.getDebugFlag())
        {
          g_output_helper->register_file(e_ortholog, prefix + ".orthologs");
          g_output_helper->register_file(e_journal,  prefix + ".journal");
          g_output_helper->register_file(e_rejects,  prefix + ".rejected");
        }
        process(g_lookup, (*itor), (*jtor), g_po.getOutgroupFile(), g_po.getOutputDir(), g_po.getScoreMatrix(), g_po.getWriteBlastScriptsOnly());
        g_output_helper->close(e_paralog);
        g_output_helper->close(e_ortholog);
        g_output_helper->close(e_journal);
        g_output_helper->close(e_rejects);
      }
      ++show_progress;
    }
  }

  if (g_po.getWriteBlastScriptsOnly()) { exit(0);}

  cerr << "Finding CLIQUES:" << g_po.getMinCliqueSize() << ":" << endl;
  CliqueRunner cr(g_lookup, g_po.getOutputDir(), g_po.getReportDir(), g_po.getReportFilename(), g_po.getHtmlReportFilename(), g_po.getSpeciesFiles(), g_po.getMaxSubgraphNodes(), g_po.getConfidenceCutoff(), g_po.getBadParalogCutoff(), g_po.getEdgeThreshold(), g_po.getMinCliqueSize(), g_po.produceHtml());
  cr.run();

  if (g_po.produceHtml())
  {
    metadata.create_javascript();
  }

  //  DEVELOPER NOTE: cleanup
  //    There is an artifact of the way that we have to build the graph files
  //    (*.json) and fasta files during the creation of the Subgraphs but we
  //    don't know if we need the *.json files until we create the HtmlReport.
  //    In additon, if the minimum number of nodes in a clique is higher than
  //    the number of entries found in the Subgraph solving phase, we don't
  //    want to keep those unused *.json files and empty directories.
  //    We need to do the equivalent of UNIX command line commands:
  //      find g_po.getReportDir() -type f -name \*.json -exec rm -f {} \;
  //      find g_po.getReportDir() -type d -empty -exec rmdir {} \;
  removeAll(g_po.getReportDir(), "json");

  timestamp("SPOCS STOP:");

  return(0);
}
