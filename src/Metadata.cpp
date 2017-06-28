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
#include <algorithm>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
// External Includes
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/regex.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include <boost/regex.hpp>
#include <boost/shared_ptr.hpp>
// Internal Includes
// Application Includes
#include "Exception.hpp"
#include "Metadata.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "Metadata"


// Namespaces used
using namespace boost;
using namespace std;


// Constructors


Metadata::Metadata(string const & p_metadata_file, string const & p_output_dir) throw() :
  is_valid(true),
  metadata_file(p_metadata_file),
  output_dir(p_output_dir),
  json("")
{
  //  DEVELOPER NOTE: We have already checked to make sure the metadata_file
  //                  exists while parsing the command line options.

  //  Now create a json file from CSV formatted metadata file

  ifstream                                   data(this->metadata_file.c_str());
  string                                     line;
  string                                     cell;
  boost::regex_constants::syntax_option_type flags = boost::regex_constants::perl;
  boost::regex                               comments;
  boost::regex                               blanks;
  boost::regex                               header;
  boost::regex                               data_line;
  boost::smatch                              what;

  comments.assign("^#", flags);
  blanks.assign("^[[:space:]]*$", flags);
  //  A header line has to have a ":" somewhere after first comma
  header.assign("(?=^[[:alnum:]].*[,].*[:AaRr])", flags);
  //  The data lines have to have a commas
  data_line.assign("(?=^[[:alnum:]].*[,].*[^:])", flags);

  bool header_found = false;
  bool data_found   = false;

  //if (! data.is_open())
  //{ 
  //  FATAL(TSNH, metadata_file, "Cannot open metadata file");
  //}

  json << "var properties = new Object();\n";
  json << "var metadata   = new Object();\n";
  json << "function getMetadata () {return metadata;}\n";
  json << "function getProperties () {return properties;}\n";
  json << "function hasMetadata (id) \n";
  json << "{\n";
  json << "  var keys = Object.keys(metadata);\n";
  json << "  for (var i in keys)\n";
  json << "  {\n";
  json << "    var key = keys[i];\n";
  json << "    if (id in metadata[key]) return true;\n";
  json << "  }\n";
  json << "  return false;\n";
  json << "}\n";


  //  Now parse the metadata file

  map<int, string> col_to_name;
  map<string, double> col_mins;
  map<string, double> col_maxs;

  while (data.good())
  {
    getline(data, line);

    if (boost::regex_search(line, what, header))
    {
      if (data_found)
      {
        cerr << "ERROR: METADATA: missing header line in file." << line << endl;
        is_valid = false;
        break;
      }

      if (header_found)
      {
        cerr << "ERROR: METADATA: multiple header lines in file." << line << endl;
        is_valid = false;
        break;
      }

      std::vector<string> cells;
      boost::algorithm::split_regex(cells, line, regex(",") );
      int col = 0;

      for (vector<string>::iterator itor = cells.begin(); itor != cells.end(); ++itor)
      {
        string cell = (*itor);
        if (col > 0)
        {
          std::vector<string> parts;
          boost::algorithm::split_regex(parts, cell, regex(":"));
          // TODO: add another ':'-separated attribute for nulls (and anything else we'll need)
          if (parts.size() >= 4)
          {
            col_to_name[col] = parts[0];
            string name      = parts[0];
            string min       = parts[1];
            string max       = parts[2];
            string type      = parts[3];
            trim(name);
            trim(min);
            trim(max);
            trim(type);

            if (col_mins.find(name) != col_mins.end()) 
            {   
              cerr << "ERROR: metadata column " << col << " name is not unique" << endl;
              is_valid = false;
              break;
            } 

            double dMin    = string_to_double(min);
            double dMax    = string_to_double(max);
            col_mins[name] = dMin;
            col_maxs[name] = dMax;

            if (dMin > dMax)
            {
              cerr << "ERROR: metadata column " << col << " has min > max" << endl;
              is_valid = false;
              break;
            }

            if (min.size() < 1)
            {
              cerr << "ERROR: metadata column " << col << " has invalid min value" << endl;
              is_valid = false;
              break;
            }

            if (max.size() < 1)
            {
              cerr << "ERROR: metadata column " << col << " has invalid max value" << endl;
              is_valid = false;
              break;
            }

            if (name.size() < 1)
            {
              cerr << "ERROR: metadata column " << col << " has invalid name" << endl;
              is_valid = false;
              break;
            }

            if (type != "a" && type != "r" && type != "A" && type != "R")
            {
              cerr << "ERROR: metadata column " << col << " has invalid type (must be r or a)" << endl;
              is_valid = false;
              break;
            }

            if ((type == "r" || type == "R") && abs(dMin) != abs(dMax))
            {
              double abs_max = (abs(dMin) > abs(dMax)) ? abs(dMin) : abs(dMax);
              dMin = 0 - abs_max;
              dMax = abs_max;
              cerr << "WARNING: metadata column '" << name << "' is not balanced. New max/min: (" << dMin << ", " << dMax << ")" << endl;
            }

            json << "metadata['"   << name << "'] = new Object();\n";
            json << "properties['" << name << "'] = new Object();\n";
            json << "properties['" << name << "']['min']  = "  << dMin  << ";\n";
            json << "properties['" << name << "']['max']  = "  << dMax  << ";\n";
            json << "properties['" << name << "']['type'] = '" << type << "';\n";
          }
        }
        ++col;
      }

      header_found = true;
      continue;
    }

    if (boost::regex_search(line, what, data_line))
    {
      if (! header_found)
      {
        cerr << "ERROR: METADATA: missing header line in file." << line << endl;
        is_valid = false;
        break;
      }

      if (! data_found)
      {
        data_found = true;
      }

      std::vector<string> cells;
      boost::algorithm::split_regex(cells, line, regex(",") );
      string id = "";

      int col = 0;
      for (vector<string>::iterator itor = cells.begin(); itor != cells.end(); ++itor)
      {
        string cell = (*itor);
        trim(cell);
        if (col == 0) 
        {
          id = cell;
        }
        else
        {
          string col_name = col_to_name[col];
          trim(col_name);
          if (cell.size() == 0) cell = "\"null\"";
          if (col_name.size() > 0)
          {
            double cell_val = string_to_double(cell);
            if (cell.size() > 0 && (cell_val < col_mins[col_name] || cell_val > col_maxs[col_name]))
            {
              cerr << "ERROR: cell [" << id << ", " << col_name << "] (" << cell << ") is not within min/max values (" << col_mins[col_name] << ", " << col_maxs[col_name] << ")" << endl;
              is_valid = false;
              break;
            }
            json << "metadata['"   << col_name << "']['" << id << "'] = " << cell << ";\n";
          }
        }
        col++;
      }

      continue;
    }

    if (boost::regex_search(line, what, comments))
    {
      continue; // comment line - ignore and skip
    }

    if (boost::regex_search(line, what, blanks))
    {
      continue; // blank   line - ignore and skip
    }

  }
  data.close();

  return;
}


bool Metadata::create_javascript(void) throw()
{
  //  TODO: Assume is_valid is true - probably should check

  string   file(output_dir + "/include/metadata.js");
  ofstream json_file(file.c_str());
  if (json_file.is_open())
  {
    json_file << json.str() << endl;
  }
  else
  {
//cerr << "JAVASCRIPT FILE CANNOT OPEN" << endl;
  }
  json_file.close();
  
  //  TODO:  Actually check to see if file got create (e.g. permission denied?)
  return(true);
}


// Operators


// Public Methods

string Metadata::print(void) throw()
{
  return "Metadata::print - NOT IMPLEMENTED YET\n";
}


ostream & operator<<(ostream & p_os, Metadata * p_clique) throw()
{
  return(p_os << p_clique->print());
}


ostream & operator<<(ostream & p_os, Metadata & p_clique) throw()
{
  return(p_os << p_clique.print());
}
