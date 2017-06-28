///////////////////////////////////////////////////////////////////////////////
// Copyright © 2013, Battelle Memorial Institute
//
// THIS FILE INITIALLY CREATED WITH:  
//     TEMPLATE NAME:  lang_cc_hdr 
//     COMMAND NAME:   gensrc 
//     CREATED FOR:     
//
// CODING CONVENTIONS: 
//    * Class names are CamelCase with first word upper case 
//    * Functions are camelCase with first word lower case 
//    * Function parameters are lower case with _ and have p_ prefix 
//    * Member variables always use 'this' pointer 
//  
///////////////////////////////////////////////////////////////////////////////


#ifndef FASTAREADER_HH
#define FASTAREADER_HH


// System Includes
#include <algorithm>
#include <fstream>
#include <string>
// External Includes
#include <boost/shared_ptr.hpp>
// Internal Includes
#include "global.hpp"
#include "FastaSequence.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "FastaReader"


using namespace std;


//==============================================================================
// Class: FastaReader
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
//   None discovered
//
// KNOWN LIMITATIONS:
//   None discovered
//
// SEE ALSO:
//------------------------------------------------------------------------------

// DEVELOPER DOCUMENTATION:   (those who develop/fix this class)
//   [put documentation here for developers that work on this code]
//==============================================================================


class FastaReader
{

public:

  // Getters/Setters

  inline string getFilename(void) const throw()
  {
    return(this->filename);
  }


  inline u_int32_t getMaxSequenceLength(void) const throw()
  {
    return(this->max_sequence_length);
  }


  inline u_int32_t getSequenceCount(void) const throw()
  {
    return(this->sequence_count);
  }


  inline u_int32_t getTotalLetterCount(void) const throw()
  {
    return(this->total_letter_count);
  }


  // Constructors

  FastaReader(string const & p_filename, u_int64_t const p_offset = 0) throw();


  // Destructors

  ~FastaReader(void);


  // Encapsulated Behavior

  bool          good() throw();
  FastaSequence get_next() throw();


private:

  // Constructors

  FastaReader(void);
  FastaReader(FastaReader const & p_reader);


  // Operators

  FastaReader & operator=( FastaReader const & p_reader);
  bool          operator==(FastaReader const & p_reader);
  bool          operator==(FastaReader const & p_reader) const;
  bool          operator!=(FastaReader const & p_reader) const;


  // Variables

  u_int64_t pos;
  ifstream  file;
  string    filename;
  string    comment;
  u_int32_t sequence_count;
  u_int32_t total_letter_count;
  u_int32_t max_sequence_length;

};


typedef  boost::shared_ptr<FastaReader>  sharedFastaReader;


#endif /* FASTAREADER_HH */
