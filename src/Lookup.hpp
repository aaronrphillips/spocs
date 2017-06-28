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


#ifndef LOOKUP_HPP
#define LOOKUP_HPP

//  DEVELOPER NOTE:  For Performance to turn off all BOOST assertions
#define BOOST_DISABLE_ASSERTS

// System Includes
#include <list>
#include <map>
#include <string>
#include <vector>
// External Includes
#include <boost/serialization/access.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/vector.hpp>
#include "boost/shared_ptr.hpp"
// Internal Includes
// Application Includes
#include "global.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "Lookup"


// Namespaces used
using namespace std;
using namespace boost;


//void restoreLookup(Lookup s, string const & filename);


class Lookup
{

public:

  // Getters & Setters

  inline string getFileLocation(string const & p_value) const throw()
  {
    map<string, string>::const_iterator ftor = file_locations.find(p_value);
    if (ftor != file_locations.end())
    {
      return((*ftor).second);
    }

    return "";
  }

  
  inline void registerFileLocation(string const & p_value) throw()
  {
    this->file_locations[p_value] = getFileInPath(p_value);
    return;
  }


  inline float getConfidenceCutoff(void) const throw()
  {
    return(this->confidence_cutoff);
  }


  inline float getGroupOverlapCutoff(void) const throw()
  {
    return(this->group_overlap_cutoff);
  }


  inline float getSegmentCoverageCutoff(void) const throw()
  {
    return(this->segment_overlap_cutoff);
  }


  inline float getSeqOverlapCutoff(void) const throw()
  {
    return(this->seq_overlap_cutoff);
  }


  inline u_int16_t getOutgroupCutoff(void) const throw()
  {
    return(this->outgroup_cutoff);
  }


  inline u_int32_t getScoreCutoff(void) const throw()
  {
    return(this->score_cutoff);
  }


  inline u_int16_t getMaxNameLength(void) const throw()
  {
    return(this->max_name_len);
  }


  inline u_int32_t getLengthByName(string const & p_name) throw()
  {
    id_type id = getIdByName(p_name);
    map<id_type, u_int32_t>::const_iterator etor = num_to_len.end();
    map<id_type, u_int32_t>::const_iterator ftor = num_to_len.find(id);

    if (ftor != etor)
    {   
      return((*ftor).second);
    }   
    return(0);
  }


  inline id_type getMaxIdNum(void) const throw()
  {
    return(this->name_to_num.size());
  }


  inline id_type getIdByName(string const & p_value) const throw()
  {
    map<string, id_type>::const_iterator etor = name_to_num.end();
    map<string, id_type>::const_iterator ftor = name_to_num.find(p_value);

    if (ftor != etor)
    {   
      return((*ftor).second);
    }   
    return(0);
  }


  inline string getDescByName(string const & p_value) const throw()
  {
    map<string, string>::const_iterator etor = name_to_desc.end();
    map<string, string>::const_iterator ftor = name_to_desc.find(p_value);

    if (ftor != etor)
    {   
      return((*ftor).second);
    }   
    return("");
  }


  inline string getNameById(id_type const p_value) const throw()
  {
    map<id_type, string>::const_iterator etor = num_to_name.end();
    map<id_type, string>::const_iterator ftor = num_to_name.find(p_value);

    if (ftor != etor)
    {   
      return((*ftor).second);
    }   
    return("");
  }


  inline list<string> getSpecies(void) throw()
  {
    map<string, u_int32_t>::const_iterator itor = species_count.begin();
    map<string, u_int32_t>::const_iterator etor = species_count.end();
    list<string> species;

    for(itor = itor; itor != etor; ++itor)
    {
      if (this->outgroup_species != (*itor).first)
      {
        species.push_back((*itor).first);
      }
    }

    return(species);
  }


  inline u_int32_t getSpeciesCount(string const & p_value) throw()
  {
    map<string, u_int32_t>::const_iterator etor = species_count.end();
    map<string, u_int32_t>::const_iterator ftor = species_count.find(p_value);

    if (ftor != etor)
    {   
      return((*ftor).second);
    }   
    return(0);
  }


  inline u_int64_t getSearchSpace(void) throw()
  {
    //  TODO: This is arbitrary and should be looked at
    //return(this->search_space);
    return(5000000);
  }


  inline string getSpeciesById(u_int32_t const p_value) const throw()
  {
    map<u_int32_t, string>::const_iterator etor = num_to_species.end();
    map<u_int32_t, string>::const_iterator ftor = num_to_species.find(p_value);

    if (ftor != etor)
    {   
      return((*ftor).second);
    }   
cerr << "ERROR:" << p_value << " has no entry in the species lookup" << endl;

    return("");
  }


  inline u_int64_t getOffsetByName(string const & p_value) const throw()
  {
    map<string, u_int64_t>::const_iterator etor = name_to_offset.end();
    map<string, u_int64_t>::const_iterator ftor = name_to_offset.find(p_value);

    if (ftor != etor)
    {   
      return((*ftor).second);
    }   
cerr << "ERROR:" << p_value << " has no name in the offset lookup" << endl;

    return(0);
  }


  inline string getFilenameByName(string const & p_value) const throw()
  {
    string species(getSpeciesByName(p_value));

    map<string, string>::const_iterator etor = species_to_filename.end();
    map<string, string>::const_iterator ftor = species_to_filename.find(species);

    if (ftor != etor)
    {   
      return((*ftor).second);
    }   
cerr << "ERROR:" << p_value << " has no entry in the filename lookup" << endl;

    return("");  
  }


  inline string getSpeciesByName(string const & p_value) const throw()
  {
    map<string, string>::const_iterator etor = name_to_species.end();
    map<string, string>::const_iterator ftor = name_to_species.find(p_value);

    if (ftor != etor)
    {   
      return((*ftor).second);
    }   
cerr << "ERROR:" << p_value << " has no entry in the species lookup" << endl;

    return("");
  }


  inline void addFilename(string const & p_species, string const & p_filename) throw()
  {
    map<string, string>::const_iterator etor = species_to_filename.end();
    map<string, string>::const_iterator ftor = species_to_filename.find(p_species);

    if (ftor != etor)
    {   
      FATAL("Adding Fasta File", "Trying to add Fasta File to Lookup", "Cannot add filename, " + p_filename + ", for species, " + p_species + ", because the species already has a filename: " + (*ftor).second);
    }   

    species_to_filename[p_species] = p_filename;

    return;
  }


  // Constructors

  Lookup(bool const p_shell) throw();

  Lookup(vector<string> const & p_file_names,
         string         const & p_outgroup_file_name     = "",
         float          const   p_confidence_cutoff      = 0.05, 
         float          const   p_seq_overlap_cutoff     = 0.5,
         float          const   p_segment_overlap_cutoff = 0.25,
         float          const   p_group_overlap_cutoff   = 0.5,
         u_int16_t      const   p_outgroup_cutoff        = 50,
         u_int16_t      const   p_score_cutoff           = 40) throw(); 


  // Destructor

  ~Lookup(void) throw()
  {
    return;
  }


  // Public Methods

  string getSequence(string const & p_value) throw();
  string getSequenceRaw(string const & p_value) throw();
  string print(void) throw();


private:

  // Getters/Setters


  // Constructors

  Lookup(void) throw();
  Lookup(Lookup const & p_lookup) throw();


  // Operators

  bool operator=(Lookup const & p_lookup) const throw();


  // Methods

  void addName(string const & p_species, u_int64_t const p_offset, string const & p_name, string const & p_desc, u_int32_t const p_len) throw();

  // Allow serialization to access data members.
  template<typename Archive>
  void serialize(Archive & p_ar, const unsigned int version)
  {
    p_ar & this->file_names;
    p_ar & this->outgroup_species;
    p_ar & this->confidence_cutoff;
    p_ar & this->seq_overlap_cutoff;
    p_ar & this->segment_overlap_cutoff;
    p_ar & this->group_overlap_cutoff;
    p_ar & this->outgroup_cutoff;
    p_ar & this->max_name_len;
    p_ar & this->score_cutoff;
    p_ar & this->name_to_num;
    p_ar & this->num_to_name;
    p_ar & this->num_to_len;
    p_ar & this->name_to_species;
    p_ar & this->name_to_desc;
    p_ar & this->num_to_species;
    p_ar & this->species_count;
    p_ar & this->species_to_filename;
    p_ar & this->search_space;
    p_ar & this->name_to_offset;
    p_ar & this->file_locations;
  }


  // Variables

  // Assume:
  //   - num  is a unique id number for a species sequence
  //   - name is a unique string    for a species sequence (in a fasta file)

  // max_name_len - used to make it easier to print output by columns
  // name_to_num  - used to find an id for a name
  // num_to_name  - used to find a name for an id
  // num_to_len   - used to find the length of a sequence by it's id
  //                species A id and species B id

  vector<string>           file_names;
  string                   outgroup_species;
  float                    confidence_cutoff;
  float                    seq_overlap_cutoff;
  float                    segment_overlap_cutoff;
  float                    group_overlap_cutoff;
  u_int16_t                outgroup_cutoff;
  u_int16_t                max_name_len;
  u_int16_t                score_cutoff;
  map<string,  id_type>    name_to_num;
  map<id_type, string>     num_to_name;
  map<id_type, u_int32_t>  num_to_len;

  map<string, string>      name_to_species;
  map<string, string>      name_to_desc;
  map<u_int32_t, string>   num_to_species;
  map<string, u_int32_t>   species_count;
  map<string, string>      species_to_filename;
  u_int64_t                search_space;
  map<string, u_int64_t>   name_to_offset;
  map<string, string>      file_locations;


friend
  class boost::serialization::access;


friend
  ostream & operator<<(ostream & p_os, Lookup * p_lookup) throw();


friend
  ostream & operator<<(ostream & p_os, Lookup & p_lookup) throw();

};


typedef boost::shared_ptr<Lookup> sharedLookup;


#endif // LOOKUP_HPP
