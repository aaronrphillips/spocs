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
#include <fstream>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
// External Includes
#include <boost/make_shared.hpp>
// Internal Includes
// Application Includes
#include "global.hpp"
#include "osUtil.hpp"
#include "OutputHelper.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "OutputHelper"


// Namespaces used
using namespace std;
using namespace boost;


//==============================================================================
// Class OutputHelper
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


/*
OutputHelper::OutputHelper(void) throw() :
  ready(false)
{
  return;
}
*/
 

// ======================================================================
// Public Functions
// ======================================================================


void OutputHelper::append_content(u_int16_t const p_type, string const & p_filename) throw()
{
  string line;
  ifstream data(p_filename.c_str());
  if (data.is_open())
  {
    while (data.good())
    {
      getline(data, line);
      this->write(p_type, line + "\n");
    }
  }
  else
  {
    cerr << "ERROR: cannot write to file, " << p_filename << endl;
  }

  data.close();

  return;
}


void OutputHelper::register_file(u_int16_t const p_type, string const & p_filename) throw()
{
  this->file_lookup[p_type] = open_file(p_filename);
  if (p_type == e_script)
  {
    chmod(p_filename.c_str(), S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IWGRP|S_IXGRP|S_IROTH|S_IXOTH);
  }

  return;
}


void OutputHelper::write(u_int16_t const & p_file_id, string const & p_data) throw()
{
  map<u_int16_t, handle_type>::const_iterator itor = this->file_lookup.find(p_file_id);
  map<u_int16_t, handle_type>::const_iterator etor = this->file_lookup.end();

  if (etor != itor)
  {
    handle_type fd = (*itor).second;
    if (fd->good() && fd->is_open())
    {
      (*fd) << p_data;
    }
    else
    {
      cerr << p_file_id << ":" << p_data << endl;
    }
  }
  else
  {
    cout << p_data;
  }

  return;
}


void OutputHelper::close(u_int16_t const & p_file_id) throw()
{
  map<u_int16_t, handle_type>::const_iterator itor = this->file_lookup.find(p_file_id);
  map<u_int16_t, handle_type>::const_iterator etor = this->file_lookup.end();

  if (etor != itor)
  {
    handle_type fd = (*itor).second;
    if (fd->good() && fd->is_open())
    {   
      fd->close();
    }   
  }

  return;
}


handle_type OutputHelper::open_file(string const & file_name) throw()
{ 
  handle_type stream = make_shared<fstream>(file_name.c_str(), fstream::out);

  if (! (stream->good() && stream->is_open()))
  {
    FATAL("File Open", "Trying to create file", "Could not open file, " + file_name + ", for writing");
  }

  return(stream); 
}
