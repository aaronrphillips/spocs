///////////////////////////////////////////////////////////////////////////////
// Copyright © 2013, Battelle Memorial Institute
//
// THIS FILE INITIALLY CREATED WITH:  
//     TEMPLATE NAME:  lang_cpp_class.template 
//     COMMAND NAME:   gensrc 
//
// CODING CONVENTIONS: 
//    * Class names are CamelCase with first word upper case 
//    * Functions are camelCase with first word lower case 
//    * Function parameters are lower case with _ and have a_ prefix 
//    * Member variables always use 'this' pointer 
///////////////////////////////////////////////////////////////////////////////


// System Includes
#include <config.h>
#include <errno.h>
#include <exception>
#if HAVE_FLOAT_H
#include <float.h>
#endif
#if HAVE_LIBGEN_H
#include <libgen.h>
#endif
#include <math.h>
#include <sstream>
#include <string>
#include <sys/wait.h>
#include <vector>
// External Includes
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
// Internal Includes
// Application Includes
#include "global.hpp"
#include "BlastRunner.hpp"
#include "Exception.hpp"
#include "FastaReader.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "BlastRunner"


// Namespaces used
using namespace boost;
using namespace boost::filesystem;
using boost::lexical_cast;


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


// ======================================================================
// Constructors
// ======================================================================


BlastRunner::BlastRunner(sharedLookup const & p_lookup,
                         string       const & p_out_dir,
                         string       const & p_A_fasta,
                         string       const & p_B_fasta,
                         string       const & p_C_outgroup,
                         string       const & p_score_matrix,
                         bool         const   p_only_create_scripts) throw():
lookup(p_lookup),
output_dir(string(absolute(path(p_out_dir)).c_str()) + '/'),
A_fasta(absolute(path(p_A_fasta)).c_str()),
B_fasta(absolute(path(p_B_fasta)).c_str()),
C_outgroup(""),
score_matrix(p_score_matrix),
A_db_size(0),
B_db_size(0),
C_db_size(0),
only_create_scripts(p_only_create_scripts)
{
  string search_path(getenv( "PATH" ));
  string  matrix;
  istringstream ss(search_path);

  if (! p_C_outgroup.empty())
  {
    C_outgroup = absolute(path(p_C_outgroup)).c_str();
  }

  while (!ss.eof())
  {
    string dir;
    getline(ss, dir, ':');
    blastall     = dir + "/blastall";
    formatdb     = dir + "/formatdb";
    path blast_exe(blastall);
    path matrix_abs(dir + "/../data/" + score_matrix);

    //NOTE : the scoring matrix must be relative to the absolute path of the blastall binary.
    if (exists(blastall) && is_symlink(blastall))
    {
      blast_exe = read_symlink(blast_exe);
      //cerr << "SYM#" << blast_exe << endl;
      path tmp(string(blast_exe.parent_path().parent_path().c_str()) + "/data/" + score_matrix);
      matrix_abs = tmp;
    }
    //cerr << blast_exe << ":" << matrix_abs << "\n";
    //cerr << exists(blast_exe) << ":" <<  exists(formatdb) << ":" <<  exists(matrix_abs) << endl;

    if (exists(blast_exe) && exists(formatdb) && exists(matrix_abs))
    {
      //score_matrix = string(matrix_abs.c_str()); // NOTE: we don't want to pass the full path to BLAST
      return;
    }
  }

  FATAL("Missing requirements", "Searching for blast components", "Unable to find blastall, formatdb, and scoring matrix in your path.");

  return;
}


BlastRunner::~BlastRunner(void) throw()
{
  return;
}


// ======================================================================
// Public Functions
// ======================================================================


bool BlastRunner::formatDb(string const & p_file) throw()
{
  pid_t cpid;
  int status;

  string file(p_file.c_str());
  string parent = dirname((char*)file.c_str());
  string outfile(p_file + ".pin");

  if (exists(outfile) && file_size(outfile) > 0)
  {
    // if fasta files were changed we need to re-run BLAST
    if (last_write_time(outfile) > last_write_time(p_file))
    {
      return(true);
    }
  }

  // Start forking process
  cpid = fork();
  if (cpid == -1)  
  {
    FATAL(TSNH, "fork()",  "Failed to fork()");
  }
  if (cpid == 0) // Code executed by child 
  {            
    ::g_output_helper->write(e_journal, "Running formatdb -i " + p_file + "\n");
    chdir(parent.c_str());
    exit(execl(formatdb.c_str(), formatdb.c_str(), "-i", p_file.c_str(), (char *)0)); 
  } 
  else           // Code executed by Parent
  {                    
    waitpid(cpid, &status, WUNTRACED | WCONTINUED);

    struct stat buf;
    bool finished = false;    
    //  NOTE: This is not an infinite loop because of waitpid
    //        The loop just makes sure that the filesystem has time
    //        finish flushing the output file.
    while (! finished) 
    {
      //  If file exists set finished to true
      int intStat = stat(outfile.c_str(), &buf);
      if (intStat != 0)
      { 
        sleep(10);
      }
      else
      {
        finished = true;
      }
    }
  }

  return(true);
}


vector< tuple<string, species_type, species_type> > BlastRunner::run(void) throw()
{
  string species_A_base = getSpeciesFromPath(A_fasta);  //get species (/somedir/ABC.fasta => ABC)
  string species_B_base = getSpeciesFromPath(B_fasta);
  A_db_size             = lookup->getSpeciesCount(species_A_base);
  B_db_size             = lookup->getSpeciesCount(species_B_base);

  string out_file;
  vector< tuple<string, species_type, species_type> > out_files;
  string blast_ext(".blastout"); // TODO: remove hardcoded value

  // for every combination of A, B, and C (*), do the corresponding BLAST run and push
  // the fasta files onto out_files
  // 
  // *: C vs A and C vs B are not done, only A vs C and B vs C (assuming an outgroup was specified)

  // A vs B
  out_file = output_dir + species_A_base + '-' + species_B_base;
  runOne(A_fasta, A_db_size, B_fasta, B_db_size, out_file + blast_ext);
  out_files.push_back(tuple<string, species_type, species_type> (out_file, e_species_A, e_species_B));

  // B vs A
  out_file = output_dir + species_B_base + '-' + species_A_base;
  runOne(B_fasta, B_db_size, A_fasta, A_db_size, out_file + blast_ext);
  out_files.push_back(tuple<string, species_type, species_type> (out_file, e_species_B, e_species_A));

  // B vs B
  out_file = output_dir + species_B_base + '-' + species_B_base;
  runOne(B_fasta, B_db_size, B_fasta, B_db_size, out_file + blast_ext);
  out_files.push_back(tuple<string, species_type, species_type> (out_file, e_species_B, e_species_B));

  // A vs A
  out_file = output_dir + species_A_base + '-' + species_A_base;
  runOne(A_fasta, A_db_size, A_fasta, A_db_size, out_file + blast_ext);
  out_files.push_back(tuple<string, species_type, species_type> (out_file, e_species_A, e_species_A));

  if (! C_outgroup.empty()) // if the user specified an outgroup, run those too
  {
    string species_C_base = getSpeciesFromPath(C_outgroup);
    C_db_size             = lookup->getSpeciesCount(species_C_base);

    // A vs C
    out_file = output_dir + species_A_base + '-' + species_C_base;
    runOne(A_fasta, A_db_size, C_outgroup, C_db_size, out_file + blast_ext);
    out_files.push_back(tuple<string, species_type, species_type> (out_file, e_species_A, e_outgroup));

    // B vs C
    out_file = output_dir + species_B_base + '-' + species_C_base;
    runOne(B_fasta, B_db_size, C_outgroup, C_db_size, out_file + blast_ext);
    out_files.push_back(tuple<string, species_type, species_type> (out_file, e_species_B, e_outgroup));
  }

  return(out_files);
}


bool BlastRunner::runOne(string const & p_qfile, u_int64_t const p_qsize, string const & p_dfile, u_int64_t const p_dsize, string const & p_outfile) throw()
{
  if (exists(p_outfile) && file_size(p_outfile) > 0)
  {
    // if fasta files were changed we need to re-run BLAST
    if ((last_write_time(p_outfile) > last_write_time(p_qfile)) &&
        (last_write_time(p_outfile) > last_write_time(p_dfile))
       )
    {
      ::g_output_helper->write(e_journal, "BLAST output file already exists: " + p_outfile + "\n");
      return(true);
    }
  }

  pid_t cpid;
  int status;

  if (! onlyCreateScripts())
  {
    ::g_output_helper->write(e_journal, "Processing " + p_outfile + "\n");
  }

  // TODO: Move this to spocs.cpp in master_init or prior to any MPI work
  //       and make formatDb a static method
  formatDb(p_qfile);
  formatDb(p_dfile);

  string space(lexical_cast<string>(lookup->getSearchSpace()));
  string dsize("10000");
  string tmp_outfile = p_outfile + ".tmp";
  string script_file = p_outfile + ".sh";

  const int num_args = 21;
  char * args[num_args];

  args[0]  = (char*)blastall.c_str();
  args[1]  = (char*)"-i";
  args[2]  = (char*)p_qfile.c_str();
  args[3]  = (char*)"-d";
  args[4]  = (char*)p_dfile.c_str();
  args[5]  = (char*)"-p";
  args[6]  = (char*)"blastp";
  args[7]  = (char*)"-e";
  args[8]  = (char*)"10.0";
  args[9]  = (char*)"-b";
  args[10] = (char*)dsize.c_str();
  args[11] = (char*)"-M";
  args[12] = (char*)score_matrix.c_str();
  args[13] = (char*)"-z";
  args[14] = (char*)space.c_str();
  args[15] = (char*)"-o";
  args[16] = (char*)tmp_outfile.c_str();
  args[17] = (char*)"-m";
  args[18] = (char*)"8";
  args[19] = (char*)"-F \"m S;C\"";
  args[20] = (char*)0;

  string str("");
  for (int i = 0; i < num_args - 1; ++i)
  {
    str += string(args[i]) + " ";
  }

  ::g_output_helper->write(e_journal, getTimestamp() + " : " + str + "\n");

  addScript(p_outfile + ".sh");

  ::g_output_helper->register_file(e_script, script_file);
  ::g_output_helper->write(e_script, "#!/bin/bash\n\n");
  ::g_output_helper->write(e_script, str);
  ::g_output_helper->write(e_script, " && mv " + tmp_outfile + " " + p_outfile);
  ::g_output_helper->write(e_script, "\n\nexit $?\n");
  ::g_output_helper->close(e_script);

  if (onlyCreateScripts()) { return true; }

  // Using fork() so that we can control the process and not 
  // lose our current working environment

  // Start forking process
  cpid = fork();
  if (cpid == -1)  
  {
    FATAL(TSNH, "fork()",  "Failed to fork()");
  }
  if (cpid == 0) 
  {
    // Code executed by child 
    exit(execv(script_file.c_str(), NULL)); 
  } 
  else
  {                    
    // Code executed by Parent
    cpid = waitpid(cpid, &status, WUNTRACED | WCONTINUED);

    if (cpid == -1)
    {
      FATAL(Process, "execv(" + script_file + ")",  "Failed to run blastall script");
    }

    if (WIFEXITED(status))
    {
      struct stat buf;
      bool finished = false;    

      //  DEVELOPER NOTE: There is the potential for an infinite loop here
      while (! finished) 
      {
        //  If file exists set finished to true
        int intStat = stat(p_outfile.c_str(),&buf);
        if (intStat != 0)
        { 
          sleep(10);
        }
        else
        {
          finished = true;
        }
      }
    }
    else
    {
      FATAL(Process, "execv(" + script_file + ")",  "script terminated abnormally");
    }
  }

  return(true);
}
