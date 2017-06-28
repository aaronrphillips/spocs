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
#include <algorithm>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#if HAVE_LIBGEN_H
#include <libgen.h>
#endif
#include <sstream>
// External Includes
#include <boost/filesystem.hpp>
// Internal Includes
// Application Includes
#include "global.hpp"
#include "osUtil.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "global"


// Namespaces
using namespace std;


map<char, int>            g_clique_types;
boost::shared_ptr<OutputHelper>  g_output_helper;


#undef  ENUM_OR_STRING
#define ENUM_OR_STRING( name ) # name

const char* graph_types[] =
{
  GRAPH_TYPES
};


const char* color_types[] =
{
  RAINBOW
};

#undef  ENUM_OR_STRING


// Getters/Setters

string getTimestamp(void) throw()
{
  time_t rawtime;
  struct tm * timeinfo;

  time ( &rawtime );
  timeinfo = localtime ( &rawtime );

  string timeWithNewline(asctime(timeinfo));
  return(timeWithNewline.erase(timeWithNewline.size() - 1, 1)); // get rid of the stupid newline
}


void global_init(void) throw()
{
  boost::shared_ptr<OutputHelper> tmp(new OutputHelper());
  ::g_output_helper = tmp;

  return;
}


const char* getColorTypeStr(color_type_t p_type) throw()
{
  if (p_type > MAX_COLOR)
  {
    return("ErrorType");
  }
  return(color_types[p_type]);
}


const char* getGraphTypeStr(graph_type_t p_type) throw()
{
  if (p_type > Last)
  {
    return("ErrorType");
  }
  return(graph_types[p_type]);
}


void setType(char type) throw()
{
  ::g_clique_types[type]++;
  return;
}


bool isTypeSet(graph_type_t type) throw()
{
  char t = string(getGraphTypeStr(type))[0];
  return(g_clique_types.find(t) != g_clique_types.end());
}


void splitTitle(string const title, string & name, string & comment) throw()
{
  size_t split = title.find_first_of(" ");

  size_t arrow_loc = title.find_first_of(">");
  int start = 0;
  if (arrow_loc != string::npos)
  {
    start = arrow_loc + 1;
  }

  if (split != string::npos)
  {
    name       = title.substr(start, split - start);
    //  1 to get rid of ' '
    comment    = title.substr(split + 1);
  }
  else
  {
    name    = title.substr(start);
    comment = "";
  }
  return;
}


string getSpeciesFromPath(string const & p_path) throw()
{
  // NOTE: must make a copy of p_path because basename
  //       (man basename) may MODIFY the argument passed in.
  string path    = p_path;
  string species = boost::filesystem::basename((char*)path.c_str());
  species        = species.substr(0, species.find_last_of("."));

  return(species);
}


string getParalogFileExt(void) throw()
{
  return(".paralogs");
}


double string_to_double( const std::string& s )
{
  std::istringstream i(s);
  double x;
  if (!(i >> x)) return 0;
  return x;
} 


void message(string const & p_ecode,
             string const & p_item,
             string const & p_context,
             string const & p_class,
             string const & p_func,
             string const & p_file,
             int    const   p_line,
             int    const   p_status) throw()
{
  u_int16_t pad = 14;

  cerr << "\nFATAL ERROR:\n";
  cerr << setw(pad) << "File:  "     << p_file        << "(line:" << p_line << ")\n";
  cerr << setw(pad) << "Class:  "    << p_class       << "\n";
  cerr << setw(pad) << "Function:  " << p_func        << "\n";
  cerr << setw(pad) << "Message:  "  << "Error Code=" << p_ecode  << "\n";
  cerr << setw(pad) << " "           << p_item        << "\n";
  cerr << setw(pad) << " "           << p_context     << "\n"     << endl;

  if (EXIT_FAILURE == p_status)
  {
    exit(EXIT_FAILURE);
  }

  return;
}
