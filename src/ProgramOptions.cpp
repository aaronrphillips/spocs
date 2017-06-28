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
//    * Function parameters are lower case with _ and have p_ prefix 
//    * Member variables always use 'this' pointer 
///////////////////////////////////////////////////////////////////////////////


// System Includes
#include <config.h>
#include <algorithm>
#include <fstream>
#include <iostream>
#if HAVE_UNISTD_H
# include <unistd.h>
#endif
// External Includes
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/version.hpp>
#include <boost/xpressive/xpressive.hpp>
// Internal Includes
// Application Includes
#include "global.hpp"
#include "osUtil.hpp"
#include "ProgramOptions.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "ProgramOptions"

// Namespaces used
using namespace std;
using namespace boost;
using boost::lexical_cast;


//==============================================================================
// Class ProgramOptions
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


ProgramOptions::ProgramOptions(void) throw() :
  command_only("Command Line Only"),
  visible("Allowed options on command line.\nDefault values are shown as (=DEFAULT)")
{

  //
  // Declare a group of options that will be allowed only on command line
  //
  this->command_only.add_options()
    ("help,h",                                                           "Print help information")
    ("version,v",                                                        "Print version information")
    ("debug,d",                                                          "Produce debug files in Paralog Output Directory")
    ("species-list,s",       value<vector<string> >(),                   "REQUIRED: Species FASTA files")
    ("paralog-output-dir,p", value<string>(),                            "REQUIRED: Paralog Output Directory")
    ("report-dir,R",         value<string>(),                            "REQUIRED: Cliques report directory")
    ("outgroup-file,o",      value<string>()->default_value(""),         "Outgroup FASTA file")
    ("force,f",                                                          "Force all processing to be done even if newer output files exist")
    ("inner-confidence,i",   value<float>()->default_value(0.05),        "Inner confidence cutoff")
    ("seq-overlap,q",        value<float>()->default_value(0.5),         "Sequence overlap cutoff")
    ("seg-overlap,g",        value<float>()->default_value(0.25),        "Segment overlap cutoff")
    ("group-overlap,r",      value<float>()->default_value(0.5),         "Group overlap cutoff")
    ("outgroup-cutoff,c",    value<u_int16_t>()->default_value(50),      "Outgroup cutoff")
    ("blast-score-cutoff,b", value<u_int16_t>()->default_value(50),      "Blast bit score cutoff")
    ("evalue,e",             value<float>()->default_value(10.0),        "Blast evalue cutoff")
    ("matrix,m",             value<string>()->default_value("BLOSUM62"), "Name of BLAST scoring matrix")
    ("write-scripts-only,w",                                             "Write BLAST scripts that need to be executed, then exit")
    ("html-cliques,H",                                                   "Clique - Produce HTML report (in addition to plain text report)")
    ("type,T",               value<string>()->default_value("CSID"),     "Clique - C for Complete, S for Semi-complete, I for Incomplete, D for Degenerate. Types can be combined in any order such as CS or DI")
    ("confidence,C",         value<float>()->default_value(1.0),         "Clique - Confidence cutoff")
    ("max-nodes,N",          value<u_int16_t>()->default_value(2000),    "Clique - Max number of nodes per subgraph")
    ("min-clique-size,M",    value<u_int16_t>()->default_value(2),       "Clique - Min number of nodes per clique")
    ("bad-paralog-cutoff,B", value<u_int16_t>()->default_value(5),       "Clique - Max number of nodes from same species per paralog. Use 0 to disable cutoff")
    ("edge-threshold,E",     value<float>()->default_value(0.05),        "Clique - Percentage of edges allowed to be missing in semi-complete cliques")
    ("metadata-file,D",      value<string>()->default_value(""),         "Metadata file to be used for graph overlay.")
    ("validate-metadata-only,V",                                         "Validate metadata content and structure, then exit")
    ;

/*
# MAKE SURE, however, that the score cutoff here matches what you used for BLAST!
score_cutoff = 40;    # In bits. Any match below this is ignored
outgroup_cutoff = 50; # In bits. Outgroup sequence hit must be this many bits stronger to reject best-best hit between A and B
conf_cutoff = 0.05;   # Include in-paralogs with this confidence or better
group_overlap_cutoff = 0.5; # Merge groups if ortholog in one group has more than this confidence in other group            #

seq_overlap_cutoff = 0.5;   # Match area should cover at least this much of longer sequence. Match area is defined as area from start of first segment to end of last segment, i.e segments 1-10 and 90-100 gives a match length of 100.

segment_coverage_cutoff = 0.25;     # Actually matching segments must cover this much of longer sequence. For example, segments 1-10 and 90-100 gives a total length of 20.
*/

  this->cmdline_options.add(this->command_only);
  this->visible.add(this->command_only);
  this->position_options.add("species-list", -1);

  return;
}


bool ProgramOptions::checkOptions(int p_argc, char ** p_argv) throw()
{
  try
  {
    try
    {
      store(command_line_parser(p_argc, p_argv).options(this->cmdline_options).positional(this->position_options).run(), getOptionMap());
    }
    catch(std::exception & e)
    {
      FATAL(BadOption, "Processing command line options", lexical_cast<string>(e.what()));
    }    

    // Handle command line only options

    displayHelp(getOptionMap().count("help") || p_argc == 1);    // HELP
    displayVersion(getOptionMap().count("version"));             // VERSION

    // Handle command line options

    this->debug_flag              = getOptionMap().count("debug");
    this->force_flag              = getOptionMap().count("force");
    this->species_files           = getOption< vector<string> >("species-list");
    this->outgroup_file           = getOption<string>("outgroup-file");
    this->output_dir              = getOption<string>("paralog-output-dir");
    this->score_matrix            = getOption<string>("matrix");
    this->inner_confidence_cutoff = getOption<float>("inner-confidence");
    this->sequence_overlap_cutoff = getOption<float>("seq-overlap");
    this->segment_overlap_cutoff  = getOption<float>("seg-overlap");
    this->group_overlap_cutoff    = getOption<float>("group-overlap");
    this->outgroup_cutoff         = getOption<u_int16_t>("outgroup-cutoff");
    this->score_cutoff            = getOption<u_int16_t>("blast-score-cutoff");
    this->produce_html            = getOptionMap().count("html-cliques");
    this->confidence_cutoff       = getOption<float>("confidence");
    this->bad_paralog_cutoff      = getOption<u_int16_t>("bad-paralog-cutoff");
    this->max_subgraph_nodes      = getOption<u_int16_t>("max-nodes");
    this->min_clique_size         = getOption<u_int16_t>("min-clique-size");
    this->edge_threshold          = getOption<float>("edge-threshold");
    this->clique_report_dir       = getOption<string>("report-dir");
    this->metadata_file           = getOption<string>("metadata-file");
    this->write_scripts_only      = getOptionMap().count("write-scripts-only");
    this->validate_metadata_only  = getOptionMap().count("validate-metadata-only");

    if (! outgroup_file.empty())
    {
      fileMustExist(outgroup_file, R_OK);
    }
    if (! metadata_file.empty() || validate_metadata_only)
    {
      fileMustExist(metadata_file, R_OK);
    }

    dirMustExist(getOutputDir());
    dirMustExist(getReportDir());

    vector<string>::iterator itor = species_files.begin();
    while (itor != species_files.end())
    {
      fileMustExist((*itor), R_OK);
   
      if ((! outgroup_file.empty()) && ((*itor) == outgroup_file))
      {
//cerr << "erasing: " << (*itor) << endl;
        itor = species_files.erase(itor);
      }
      else
      {
//cerr << "keeping: " << (*itor) << endl;
        ++itor;
      }
    }

    // NOTE: needs to happen AFTER the outgroup file has been removed from species_files
    if (species_files.size() < 2)
    {
      FATAL(BadOption, "Processing options", "Need at least 2 species FASTA files to process");
    }


    //check types
    string types = getOption<string>("type");

    types.erase(std::remove(types.begin(), types.end(), ','), types.end());
    std::transform(types.begin(), types.end(), types.begin(), ::toupper);

    vector<char> chars;
    chars.push_back('C');
    chars.push_back('S');
    chars.push_back('D');
    chars.push_back('I');

    if (types.size() > 0)
    {
      for(vector<char>::iterator itor = chars.begin(); itor != chars.end(); ++itor)
      {
        size_t pos = types.find(*itor);
        if (pos != string::npos)
        {
          setType(*itor);
          types.erase(pos, 1);
        }
      }

      if (types.size() > 0)
      {
        cerr << "ERROR: unknown type character(s): " << types << ". Valid Types: [CSDI]" << endl;
        exit(EXIT_FAILURE);
      }
    }
    else
    {
      for(vector<char>::iterator itor = chars.begin(); itor != chars.end(); ++itor)
      {
        setType(*itor);
      }
    }

    checkRange<float>("confidence", (float)(0.00 - 0.001), (float)1.001);
    checkRange<float>("group-overlap", (float)(0.00 - 0.001), (float)1.001);
    checkRange<float>("inner-confidence", (float)(0.00 - 0.001), (float)1.001);
    checkRange<float>("seg-overlap", (float)(0.00 - 0.001), (float)1.001);
    checkRange<float>("seq-overlap", (float)(0.00 - 0.001), (float)1.001);
    checkRange<u_int16_t>("outgroup-cutoff", (u_int16_t)(0), (u_int16_t)500);
    checkRange<u_int16_t>("blast-score-cutoff", (u_int16_t)(0), (u_int16_t)500);
    checkRange<float>("evalue", (float)(0.00 - 0.001), (float)100.001);
    checkRange<float>("edge-threshold", (float)(0.00 - 0.001), (float)50.001);
  }
  catch(std::exception & e)
  {
    FATAL(BadOption, "Processing options", lexical_cast<string>(e.what()));
  }    

  return(true);
}


// ======================================================================
// Private Functions
// ======================================================================


void ProgramOptions::displayHelp(bool const p_condition) throw()
{
  if (! p_condition)
  {
    return;
  }

  string message = "\n";
  stringstream ss("");
  ss << this->visible;
  message += ss.str();

  message += "\nExample:\n\n";
  message += "  "APP_NAME_LOWER" -s data/output/gold4_fasta/*.fasta -o data/output/gold4_fasta/OUTGROUP_M_thermoautotrophicus_DeltaH.fasta -R data/output/gold4_cliques -p data/output/gold4_out_dir -H\n\n";
  output(message);
  exit(EXIT_SUCCESS);
}


void ProgramOptions::displayVersion(bool const p_condition) throw()
{
  if (! p_condition)
  {
    return;
  }

  string cpr;
  string symbol = "\xC2\xA9";
  cpr += "   Copyright " + symbol + " 2013, Battelle Memorial Institute\n\n";

  string dis;
  dis += "This software was prepared as an account of work sponsored by\n";
  dis += "an agency of the United States Government. Neither the United\n";
  dis += "States Government nor any agency thereof, nor Battelle Memorial\n";
  dis += "Institute, nor any of their employees, makes any warranty,\n";
  dis += "express or implied, or assumes any legal liability or\n";
  dis += "responsibility for the accuracy, completeness, or usefulness\n";
  dis += "of any information, apparatus, product, or process disclosed,\n";
  dis += "or represents that its use would not infringe privately owned\n";
  dis += "rights. Reference herein to any specific commercial product,\n";
  dis += "process, or service by trade name, trademark, manufacturer, or\n";
  dis += "otherwise does not necessarily constitute or imply its\n";
  dis += "endorsement, recommendation, or favoring by the United States\n";
  dis += "Government or any agency thereof, or Battelle Memorial Institute.\n";
  dis += "The views and opinions of authors expressed herein do not\n";
  dis += "necessarily state or reflect those of the United States Government\n";
  dis += "or any agency thereof.\n\n";
  dis += "   PACIFIC NORTHWEST NATIONAL LABORATORY\n";
  dis += "       operated by BATTELLE for the\n";
  dis += "    UNITED STATES DEPARTMENT OF ENERGY\n";
  dis += "     under Contract DE-AC05-76RL01830\n";

#ifndef APP_NAME_LOWER
#define APP_NAME_LOWER "ERROR"
#endif
#ifndef APP_NAME_UPPER
#define APP_NAME_UPPER "ERROR"
#endif
#ifndef APP_DESC
#define APP_DESC "ERROR"
#endif
#ifndef CXX_VER
#define CXX_VER "ERROR"
#endif
#ifndef CXX_OPTIMIZE_LEVEL
#define CXX_OPTIMIZE_LEVEL "ERROR"
#endif
#ifndef CXX_DEBUG_LEVEL
#define CXX_DEBUG_LEVEL "ERROR"
#endif
#ifndef SPOCS_VERSION
#define SPOCS_VERSION "ERROR"
#endif
#ifndef PACKAGE_BUGREPORT
#define PACKAGE_BUGREPORT "ERROR"
#endif

  string message;
  message  = "\n"
             "\n   " APP_DESC "\n"
             "\n   " APP_NAME_LOWER " version:            "SPOCS_VERSION
             "\n   Compiled with:            "CXX_VER
             "\n     Optimize Level:         "CXX_OPTIMIZE_LEVEL
             "\n     Debug:                  "CXX_DEBUG_LEVEL
             "\n     Boost library version:  "BOOST_LIB_VERSION
             "\n   Compiled on:              "__DATE__", "__TIME__
             "\n   Report Bugs to:           "PACKAGE_BUGREPORT
             "\n\n";
  message += cpr;
  message += dis;
  output(message);

#undef CXX_VER

  exit(EXIT_SUCCESS);
}


//  Function used to check that 'option' is used
void ProgramOptions::requiredOption(string const & p_option) throw(logic_error)
{
  if (0 == getOptionMap().count(p_option))
  {
    throw(logic_error("Missing a required option '" + p_option + "'."));
  }

  return;
}


//  Function used to check that one of 'opt1' and 'opt2' are used
void ProgramOptions::requiredOptions(string const & p_opt1, string const & p_opt2) throw(logic_error)
{
  if (0 == getOptionMap().count(p_opt1) && 0 == getOptionMap().count(p_opt2))
  {
    throw(logic_error("Missing a required options '" + p_opt1 + "' or '" + p_opt2 + "'."));
  }

  return;
}


void ProgramOptions::checkEnum(string const & p_option, int unsigned const p_check0, int unsigned const p_check1)
{
  if (getOptionMap().count(p_option))
  {
    int unsigned value = getOption<int unsigned>(p_option);
    if ((p_check0 != value) && (p_check1 != value))
    {
      FATAL(RangeError, "Exception", "The value of " + p_option + ", " + lexical_cast<string>(value) + ", must be " + lexical_cast<string>(p_check0) + " or " + lexical_cast<string>(p_check1) + ".");
    }
  }
  return;
}


void ProgramOptions::checkGreater(string const & p_option0, string const & p_option1)
{
  if (getOption<int unsigned>(p_option0) > getOption<int unsigned>(p_option1))
  {
    FATAL(RangeError, "Exception", "The value of " + p_option0 + ", " + lexical_cast<string>(getOption<int unsigned>(p_option0)) + ", must be less than the value of " + p_option1 + ", " + lexical_cast<string>(getOption<int unsigned>(p_option1)) + ".");
  }
  return;
}


void ProgramOptions::fileMustExist(string const & p_file, int const p_flag) throw(logic_error)
{
  if (0 != access(p_file.c_str(), p_flag))
  {
    throw(logic_error("Required file, '" + p_file + "', does not exist"));
  }

  return;
}


void ProgramOptions::dirMustExist(string const & p_dir) throw(logic_error)
{
  if (! is_directory(boost::filesystem::path(p_dir)))
  {
    throw(logic_error("Required directory, '" + p_dir + "', does not exist"));
  }

  return;
}


//  Function used to check that 'opt1' and 'opt2' are not specified at the same time.
void ProgramOptions::conflictingOptions(string const & p_opt1, string const & p_opt2) throw(logic_error)
{
  if (
      getOptionMap().count(p_opt1) && ! getOptionMap()[p_opt1].defaulted() &&
      getOptionMap().count(p_opt2) && ! getOptionMap()[p_opt2].defaulted()
     )   
  {
    throw(logic_error("Conflicting options '" + p_opt1 + "' and '" + p_opt2 + "'."));
  }

  return;
}
