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
#include <fstream>
#include <string>
// External Includes
// Internal Includes
// Application Includes
#include "global.hpp"
#include "FastaReader.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "FastaReader"


// Namespaces used
using namespace std;


//==============================================================================
// Class FastaReader
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


FastaReader::FastaReader(string const & p_filename, u_int64_t const p_offset) throw() :
  file(),
  filename(p_filename),
  sequence_count(0),
  total_letter_count(0),
  max_sequence_length(0)
{
  // DEVELOPER NOTE: assuming ProgramOptions checked for file existence 
  //                 and that it is an valid FASTA file
  file.open(p_filename.c_str());
  string line;
  if (file.is_open())
  {
    file.seekg(p_offset);
    getline(file, line);
    comment = line;
pos = (u_int64_t) file.tellg() - line.length() - 1;
//cerr <<  "NAME:" << line << ":" << pos << ":" << endl;
  }

  return;
}


FastaReader::~FastaReader(void)
{
  file.close();
  return;
}


// ======================================================================
// Methods
// ======================================================================


bool FastaReader::good() throw()
{
  return(file.is_open() && file.good());
}


FastaSequence FastaReader::get_next() throw()
{
  string line;
  string seq = "";
  FastaSequence fs;
  fs.offset = pos;
  while (file.good())
  {
    getline(file, line);
    if (line.length() < 1)
    {
      continue;
    }

    if (line.at(0) == '>')
    {
      splitTitle(comment, fs.name, fs.comment);
      fs.sequence  = seq;
      this->sequence_count++;
      this->total_letter_count += seq.length();
      if (seq.length() > this->max_sequence_length) this->max_sequence_length = seq.length();

      comment = line;
pos = (u_int64_t) file.tellg() - line.length() - 1;
//cerr <<  "NAME:" << line << ":" << pos << ":" << endl;
      return(fs);
    }
    else
    {
      seq += line;
    }
  }
  file.close();

  // we hit the end...

  splitTitle(comment, fs.name, fs.comment);
  fs.sequence  = seq;
  this->sequence_count++;
  this->total_letter_count += seq.length();
  if (seq.length() > this->max_sequence_length) this->max_sequence_length = seq.length();

  return(fs);
}
