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


#define BOOST_FILESYSTEM_NO_DEPRECATED 1


// System Includes
#include <config.h>
#include <iostream>
#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif
#include <string>
#ifndef _MSC_VER
#if HAVE_STRING_H
#include <string.h>           // for strtok
#endif
#include <sys/stat.h>
#include <time.h>
#endif
// External Includes
#undef BOOST_FILESYSTEM_NO_DEPRECATED
#include <boost/algorithm/string/regex.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/regex.hpp>
// Internal Includes
// Application Includes
#include "global.hpp"
#include "osUtil.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "osUtil"


// Namespaces
using namespace std;
using namespace boost;
using namespace boost::filesystem;


// Getters/Setters

string const & getPathSeparator(void) throw()
{
#ifndef _MSC_VER
  static string path_separator("/");
#else
  static string path_separator("\\");
#endif
  return(path_separator);
}


bool const isPriviledgedUser(void) throw()
{
#ifndef _MSC_VER
  return((0 == getuid()) || (0 == geteuid()));
#else
  //  NOTE: Assume user is Admin
  return(true);
#endif

}


void outputSeparator(void) throw()
{
  output("-------------------------------------------------------------------------------");
  return;
}


void output(string const & p_message) throw()
{
#ifndef _MSC_VER
  cout << p_message << endl;
#else
  cout << "LOG_ERR: " << p_message << endl;
#endif
  return;
}


bool const isLinux(void) throw()
{
#ifdef __linux__
  return(true);
#else
  return(false);
#endif
}

string const getEnvVar(string const & p_var) throw()
{
  return(string(getenv(p_var.c_str())));
}


string const getFileInPath(string const & p_file) throw()
{
  vector<string> parts;
  string path(getenv("PATH"));
  split_regex(parts, path, regex(":"));

  vector<string>::const_iterator itor = parts.begin();
  vector<string>::const_iterator etor = parts.end();
  for (itor = itor; itor != etor; ++itor)
  {
    string file((*itor) + '/' + p_file);
    if (exists(file))
    {   
      return(file);
    }   
  }

  FATAL("Missing requirements", "Searching for file in your path", "Unable to find " + p_file + " in your path.");

  return("");
}


bool const makeDirectory(string const & p_directory, string const & p_description) throw()
{
  if ((! exists(p_directory)) && (! create_directory(p_directory)))
  {
    FATAL("Create directory", "Creating " + p_description + " directory", "Could not create " + p_description + " directory, " + p_directory);
    return(false);
  }

  return(true);
}


bool removeAll(string const & p_dir, string const & p_ext) throw()
{
  path dir(p_dir);
//cerr << "DIR:" << dir << endl;

  try
  {
    // Iterate through the source directory
    for (directory_iterator file(dir); file != directory_iterator(); ++file)
    {
      try
      {
        path current(file->path());
        if (is_regular_file(current))
        {
          std::vector<string> parts;
//cerr << "CURRENT FILE:" << current << endl;
          boost::algorithm::split_regex(parts, current.c_str(), regex("[.]"));
//cerr << "CURRENT FILE AFTER:" << current << ":" << (parts[parts.size() - 1]) << ":" << endl;
          if (parts[parts.size() - 1] == p_ext)
          {
//cerr << "REMOVE FILE:" << current << endl;
            remove(current);
          }
          if (boost::filesystem::is_empty(current.parent_path()))
          {
//cerr << "REMOVE DIR:" << current.parent_path() << endl;
            remove(current.parent_path());
            return(true);
          }
          continue;
        }
        if (is_directory(current))
        {
          if (boost::filesystem::is_empty(current))
          {
//cerr << "REMOVE DIR:" << current << endl;
            remove(current);
          }
          else
          {
//cerr << "CURRENT DIR:" << current << endl;
            removeAll(current.c_str(), p_ext);
          }
        }
      }
      catch (std::exception & e)
      {
        cerr << "osUtil:removeAll: " << e.what() << " " << __LINE__ << endl;
      }
    }
  }
  catch (std::exception & e)
  {
    cerr << "osUtil:removeAll: " << e.what() << " " << __LINE__ << endl;
  }

  return(true);
}


bool copyDir(string const & p_source, string const & p_destination) throw()
{
  path   source(p_source);
  path   destination(p_destination);
  string svn_dir(".svn");
  bool   force = true;

  try
  {
//    if (is_directory(source) && (svn_dir == check_basename));
//    {
      //  NOTE: skip .svn directories
//cerr << "COPY DIR:" << p_source << ":" << source.filename().c_str() << ":" << ((string(".svn")) == string(source.filename().c_str())) << ":" << check_basename << ":" << endl;
//      return false;
//    }

    // Check whether the function call is valid
    if (! exists(source) || ! is_directory(source))
    {
      cerr << "Source directory " << p_source << " does not exist or is not a directory." << endl;
      return false;
    }

    //  TODO: Should we add a "force" option and make this optional?
    if (! force)
    {
      if (exists(destination))
      {
        cerr << "INFO: Destination directory " << destination.string() << " already exists." << endl;
        return false;
      }
    }
    // Create the destination directory
    if ((! is_directory(destination)) && (! create_directory(destination)))
    {
      cerr << "Unable to create destination directory, " << destination.string() << endl;
      return false;
    }
  }
  catch(filesystem_error const & e)
  {
    cerr << "osUtil:copyDir: " << e.what() << endl;
    return false;
  }

  // Iterate through the source directory
  for (directory_iterator file(source); file != directory_iterator(); ++file)
  {
    try
    {
      path current(file->path());
      string check_basename(current.filename().c_str());
      if (svn_dir != check_basename)
      {
        if (is_directory(current))
        {
          // Found directory: Recursion
          if (! copyDir(current.string(), destination.string() + "/" + current.filename().string()))
          {
            return false;
          }
        }
        else
        {
          // Found file: Copy
          if ((! exists(destination / current.filename())) || force)
          {
            copy_file(current, destination / current.filename(), copy_option::overwrite_if_exists);
          }
        }
      }
    }
    catch (filesystem_error const & e)
    {
      cerr << "osUtil:copyDir: " << e.what() << endl;
    }
  }

  return true;
}


bool const copyFiles(string const & p_directory, vector<string> const & p_files) throw()
{
  //  TODO:  2012/09/30: Darren: This is pretty messy - clean it up

  vector<string>::const_iterator itor = p_files.begin();
  vector<string>::const_iterator etor = p_files.end();
  for (itor = itor; itor != etor; ++itor)
  {
    path file((*itor));
    path destination_path(string(p_directory + "/" + file.filename().native()));
    path source_path((*itor));
    try 
    { 
      copy_file(source_path, destination_path, copy_option::overwrite_if_exists); 
//cerr << "copy :" << source_path << ": to :" << destination_path << ":\n";
    }
    catch (filesystem_error & e) 
    { 
      //FATAL("Copy file", string("Copy " + (*itor)), string("Could not copy " + (*itor) + " to " + destination_path + ": " + e.what()));
      FATAL("Copy file", "Can't copy file", e.what());
      return(false);
    } 
  }

  return(true);
}


string const getConfigDir(void) throw()
{
  // DEVELOPER NOTE: Derived from:
  // http://www.linuxquestions.org/questions/programming-9/get-full-path-of-a-command-in-c-117965/

  // /proc/self is a symbolic link to the process-ID subdir
  // of /proc, e.g. /proc/4323 when the pid of the process
  // of this program is 4323.
  //  
  // Inside /proc/<pid> there is a symbolic link to the 
  // executable that is running as this <pid>.  This symbolic
  // link is called "exe".
  //  
  // So if we read the path where the symlink /proc/self/exe
  // points to we have the full path of the executable.

  string config_file;
  int const MAXPATHLEN = 1024;
  char      fullpath[MAXPATHLEN];

#ifdef _MSC_VER
  GetModuleFileName(NULL, fullpath, MAX_PATH);
#elif __APPLE__
  uint32_t size = sizeof(fullpath);
  if (_NSGetExecutablePath(fullpath, &size) != 0)
  {
    fprintf(stderr, "buffer too small; need size %u\n", size);
    exit(EXIT_FAILURE);
  }
#else
  int length = readlink("/proc/self/exe", fullpath, sizeof(fullpath));

  // Catch some errors:
  if (length < 0)
  {
    fprintf(stderr, "Error resolving symlink /proc/self/exe.\n");
    exit(EXIT_FAILURE);
  }
  if (length >= MAXPATHLEN)
  {
    fprintf(stderr, "Path too long. Truncated.\n");
    exit(EXIT_FAILURE);
  }

  // The string this readlink() function returns is appended with a '@'.
  // Strip '@' off the end.
  fullpath[length] = '\0';
#endif

  path pathname(fullpath);
  path dirname = pathname.parent_path().parent_path();
  dirname /= path("conf/");

  return(dirname.string());
}


bool const can_exec(string const & p_command) throw()
{
  struct  stat st; 

  //  Find the first string (file) and see if it is an executable
  //  
  istringstream             iss(p_command);
  istream_iterator<string>  begin(iss);
  istream_iterator<string>  end;
  vector<string>            tokens(begin, end);
  string                    file = (*tokens.begin());

  //  Is file hardcoded to an executable?
  //  
  if (stat(file.c_str(), &st) >= 0)
  {
    if ((st.st_mode & S_IEXEC) != 0)
    {   
      return(true);
    }   
  }

  char   *search_path = getenv("PATH");
  char   *dir;
  string  executable;

  //  Search for "file" in the user's path
  //  
  for (dir = strtok(search_path, ":"); dir; dir = strtok(NULL, ":"))
  {
    string         check(dir);
    executable   = check + "/" + file;

//cerr << "CHECKING " << executable << endl;
    if (stat(executable.c_str(), &st) >= 0)
    {   
//cerr << "FOUND    " << executable << endl;
      if ((st.st_mode & S_IEXEC) != 0)
      {   
//cerr << "YEEHAW   " << executable << endl;
        return(true);
      }   
    }   
  }

  return(false);
}


void const timestamp(string const & p_message) throw()
{
  time_t ltime = time(NULL);
  cerr << p_message << " " << asctime(localtime(&ltime));
}


string const get_my_process_name(void) throw()
{
  string  myname;
  char    buff[1024];
  ssize_t len = ::readlink("/proc/self/exe", buff, sizeof(buff)-1);
  //int     status = EXIT_SUCCESS;

  if (len != -1)
  {
    buff[len] = '\0';
    myname = boost::filesystem::basename((boost::filesystem::path)buff);
  }
  else
  {
    //  Handle error condition
    myname = "Could not get this executable name!";
  }

  return(myname);
}
