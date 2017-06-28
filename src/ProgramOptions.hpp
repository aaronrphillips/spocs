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


#ifndef PROGRAMOPTIONS_HPP
#define PROGRAMOPTIONS_HPP


// System Includes
#include <string>
// External Includes
#include <boost/program_options.hpp>
// Internal Includes
// Application Includes
#include "global.hpp"
#include "Exception.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "ProgramOptions"


// Namespaces used
using namespace std;
using namespace boost;
using namespace boost::program_options;


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


class ProgramOptions
{

public:

  // Getters/Setters

  inline bool produceHtml(void) const throw()
  {
    return(this->produce_html);
  }


  inline bool useOutgroup(void) const throw()
  {
    return(! this->outgroup_file.empty());
  }


  inline vector<string> getSpeciesFiles(void) const throw()
  {
    return(this->species_files);
  }


  inline string getOutputDir(void) const throw()
  {
    return(this->output_dir);
  }


  inline string getOutgroupFile(void) const throw()
  {
    return(this->outgroup_file);
  }


  inline string getMetadataFile(void) const throw()
  {
    return(this->metadata_file);
  }


  inline string getScoreMatrix(void) const throw()
  {
    return(this->score_matrix);
  }


  inline float getInnerConfidenceCutoff(void) const throw()
  {
    return(this->inner_confidence_cutoff);
  }


  inline float getSequenceOverlapCutoff(void) const throw()
  {
    return(this->sequence_overlap_cutoff);
  }


  inline float getSegmentOverlapCutoff(void) const throw()
  {
    return(this->segment_overlap_cutoff);
  }


  inline float getGroupOverlapCutoff(void) const throw()
  {
    return(this->group_overlap_cutoff);
  }


  inline u_int16_t getOutgroupCutoff(void) const throw()
  {
    return(this->outgroup_cutoff);
  }


  inline u_int16_t getBadParalogCutoff(void) const throw()
  {
    return(this->bad_paralog_cutoff);
  }


  inline u_int16_t getScoreCutoff(void) const throw()
  {
    return(this->score_cutoff);
  }


  inline bool getDebugFlag(void) const throw()
  {
    return(this->debug_flag);
  }


  inline bool getForceFlag(void) const throw()
  {
    return(this->force_flag);
  }


  // Cliques


  inline float getConfidenceCutoff(void) throw()
  {
    return(this->confidence_cutoff);
  }


  inline u_int16_t getMaxSubgraphNodes(void) throw() // Cannot be const
  {
    return(this->max_subgraph_nodes);
  }


  inline u_int16_t getMinCliqueSize(void) throw() // Cannot be const
  {
    return(this->min_clique_size);
  }


  inline float getEdgeThreshold(void) throw() // Cannot be const
  {
    return(this->edge_threshold);
  }


  inline string getReportFilename(void) throw()
  {
    return(this->clique_report_dir + "/cliques.report");
  }


  inline string getHtmlReportFilename(void) throw()
  {
    return(this->clique_report_dir + "/index.html");
  }


  inline string getReportDir(void) throw()
  {
    return(this->clique_report_dir);
  }


  inline variables_map & getOptionMap(void) throw() // Cannot be const
  {
    return(this->option_map);
  }

  inline bool getWriteBlastScriptsOnly(void) throw()
  {
    return(this->write_scripts_only);
  }

  inline bool getValidateMetadataOnly(void) throw()
  {
    return(this->validate_metadata_only);
  }


  // Constructors

  ProgramOptions(void) throw();


  // Destructor

  ~ProgramOptions(void) throw()
  {
    return;
  }


  // Public Functions

  bool   checkOptions(int argc, char ** argv) throw();


  template<class T>
  const T & getOption(string const & p_option) const
  {
    if (this->option_map.count(p_option))
    {
      return(this->option_map[p_option].as< T >());
    }
    FATAL(BadOption, "Processing options", p_option);
    return(*(T*)0);
  }


private:

  // Getters/Setters


  // Constructors

  ProgramOptions(ProgramOptions const & p_program_options) throw();


  // Operators

  ProgramOptions &  operator=(ProgramOptions const & p_program_options) throw();
  bool                       operator==(ProgramOptions const & p_program_options) const throw();
  bool                       operator!=(ProgramOptions const & p_program_options) const throw();


  // Variables

  variables_map                   option_map;
  options_description             command_only;
  options_description             cmdline_options;
  options_description             visible;
  positional_options_description  position_options;

  float                inner_confidence_cutoff;
  float                sequence_overlap_cutoff;
  float                segment_overlap_cutoff;
  float                group_overlap_cutoff;
  float                confidence_cutoff;
  float                edge_threshold;

  string               outgroup_file;
  string               metadata_file;
  string               output_dir;
  string               score_matrix;
  string               clique_report_dir;
  vector<string>       species_files;

  bool                 debug_flag;
  bool                 force_flag;
  bool                 produce_html;
  bool                 write_scripts_only;
  bool                 validate_metadata_only;

  u_int16_t            max_subgraph_nodes;
  u_int16_t            min_clique_size;
  u_int16_t            outgroup_cutoff;
  u_int16_t            score_cutoff;
  u_int16_t            bad_paralog_cutoff;

  // Private Functions

  void displayHelp(bool const p_condition) throw();
  void displayVersion(bool const p_condition) throw();

  void checkEnum(string const & p_option, int unsigned const p_check0, int unsigned const p_check1);
  void checkGreater(string const & p_option0, string const & p_option1);
  void checkRelation(string const & p_option0, string const & p_option1);
  void checkSpecial(string const & p_option, u_int64_t const p_check0, u_int64_t const p_check1);
  void conflictingOptions(string const & p_opt1, string const & p_opt2) throw(logic_error);
  void optionDependency(string const & p_for_what, string const & p_required_option) throw(logic_error);
  void dirMustExist(string const & p_dir) throw(logic_error);
  void fileMustExist(string const & p_file, int const p_flag) throw(logic_error);
  void fileMustNotExist(string const & p_file) throw(logic_error);
  void requiredFile(string const & p_file) throw(logic_error);
  void requiredOption(string const & p_option) throw(logic_error);
  void requiredOptions(string const & p_opt1, string const & p_opt2) throw(logic_error);

  template <class T>
  void checkRange(string const & p_option, T const p_check0, T const p_check1)
  {
    if (getOptionMap().count(p_option))
    {
      T value = getOption<T>(p_option);
      if (p_check0 >= value)
      {   
        FATAL(RangeError, "Exception", "The value of " + p_option + ", " + lexical_cast<string>(value) + ", must be greater than or equal to " + lexical_cast<string>(p_check0) + ".");
      }   
      else if (p_check1 <= value)
      {   
        FATAL(RangeError, "Exception", "The value of " + p_option + ", " + lexical_cast<string>(value) + ", should be less than or equal to "  + lexical_cast<string>(p_check1) + ".");
      }   
    }
    return;
  }

};

#endif // PROGRAMOPTIONS_HPP
