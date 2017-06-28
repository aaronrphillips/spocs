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


//  DEVELOPER NOTE:  For Performance to turn off all BOOST assertions
#define BOOST_DISABLE_ASSERTS

// System Includes
// External Includes
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
// Internal Includes
// Application Includes
#include "Lookup.hpp"
#include "FastaReader.hpp"
#include "FastaSequence.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "Lookup"


// Namespaces used
using namespace std;


// Constructors


Lookup::Lookup(bool const p_shell) throw()
{
  return;
}


Lookup::Lookup(vector<string> const & p_file_names,
               string         const & p_outgroup_file_name,
               float          const   p_confidence_cutoff,
               float          const   p_seq_overlap_cutoff, 
               float          const   p_segment_overlap_cutoff,
               float          const   p_group_overlap_cutoff,
               u_int16_t      const   p_outgroup_cutoff,
               u_int16_t      const   p_score_cutoff) throw() :
  file_names(p_file_names),
  outgroup_species(getSpeciesFromPath(p_outgroup_file_name)),
  confidence_cutoff(p_confidence_cutoff),
  seq_overlap_cutoff(p_seq_overlap_cutoff),
  segment_overlap_cutoff(p_segment_overlap_cutoff),
  group_overlap_cutoff(p_group_overlap_cutoff),
  outgroup_cutoff(p_outgroup_cutoff),
  max_name_len(0),
  score_cutoff(p_score_cutoff)
{
  vector<string>::const_iterator itor = file_names.begin();
  vector<string>::const_iterator etor = file_names.end();

  u_int32_t min_sequences      = 100000000;
  u_int32_t max_sequences      = 0;
  u_int32_t outgroup_sequences = 0;
  for (itor = itor; itor != etor; ++itor)
  {
    sharedFastaReader reader(new FastaReader(*itor));
    string species = getSpeciesFromPath(*itor);
    addFilename(species, (*itor));
    FastaSequence seq; 

    while (reader->good())
    {
      seq = reader->get_next();
      addName(species, seq.offset, seq.name, seq.comment, seq.sequence.size());
    }

    u_int32_t count = reader->getSequenceCount();
    if (outgroup_species == species)
    {
      outgroup_sequences = count;
    }
    else
    {
      if (count < min_sequences) { min_sequences = count; }
      if (count > max_sequences) { max_sequences = count; }
    }
  }

  if (outgroup_sequences > max_sequences)
  {
    cerr << "WARNING: The outgroup species has more sequences, " << outgroup_sequences << ", than your largest species, " << max_sequences << endl;
  }

  if (outgroup_sequences < min_sequences)
  {
    cerr << "WARNING: The outgroup species has fewer sequences, " << outgroup_sequences << ", than your smallest species, " << min_sequences << endl;
  }

  return;
}


void Lookup::addName(string const & p_species, u_int64_t const p_offset, string const & p_name, string const & p_desc, u_int32_t const p_len) throw()
{
  id_type cid               = name_to_num.size();
  name_to_num[p_name]       = cid;
  name_to_offset[p_name]    = p_offset;
  num_to_name[cid]          = p_name;
  num_to_len[cid]           = p_len;
  name_to_species[p_name]   = p_species;
  num_to_species[cid]       = p_species;
  name_to_desc[p_name]      = p_desc;
  species_count[p_species]++;

  //  TODO: 2012/10/10: Replace 500000 with a variable to represent the maximum
  //                    value to be used with the -z option to blastall command
  //if (search_space < 5000000)
  //{
  //  if (outgroup_species != p_species)
  //  {
  //    search_space             += p_len;
  //  }
  //}

  if (p_name.size() > max_name_len)
  {
    max_name_len = p_name.size();
  }

  return;
}


string Lookup::getSequenceRaw(string const & p_value) throw()
{
  FastaReader reader(getFilenameByName(p_value), getOffsetByName(p_value));
  FastaSequence seq = reader.get_next();
//cerr << "FILE:" << file << ": offset:" << offset << ":" << endl;

  return(seq.print_raw());
}


string Lookup::getSequence(string const & p_value) throw()
{
  FastaReader reader(getFilenameByName(p_value), getOffsetByName(p_value));
  FastaSequence seq = reader.get_next();
//cerr << "FILE:" << file << ": offset:" << offset << ":" << endl;

  return(seq.print());
}


string Lookup::print(void) throw()
{
  string output("Lookup print: Not implemented yet");

  return(output);
}


// Operators


ostream & operator<<(ostream & p_os, Lookup * p_lookup) throw()
{
  return(p_os << p_lookup->print());
}


ostream & operator<<(ostream & p_os, Lookup & p_lookup) throw()
{
  return(p_os << p_lookup.print());
}
