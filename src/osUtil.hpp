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


#ifndef OSUTIL_HPP
#define OSUTIL_HPP


// System Includes
#include <config.h>
#include <string>
#ifndef _MSC_VER
#if HAVE_SYSLOG_H
#include <syslog.h>
#endif
#else
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#define NOGDI
#include <process.h>
#include <windows.h>
#endif
// External Includes
// Internal Includes
// Application Includes


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "osUtil"

// Typedefs
#ifndef _MSC_VER
	typedef int FileDescriptor;
	#define FileDescriptorNull -1;
#else
	typedef HANDLE FileDescriptor;
	#define FileDescriptorNull NULL;

	typedef          __int64   int64_t;
	typedef unsigned __int64 u_int64_t;
	typedef          __int32   int32_t;
	typedef unsigned __int32 u_int32_t;
	typedef          __int16   int16_t;
	typedef unsigned __int16 u_int16_t;
	typedef           __int8    int8_t;
	typedef unsigned  __int8  u_int8_t;
#endif


// Windows Defines
#ifdef _MSC_VER

//#define __restrict__ __restrict

#define fmax(a,b) (((a)>=(b))?(a):(b))
#define fmin(a,b) (((a)<=(b))?(a):(b))
#define rint(a)   (((a)>=0.f)?floor((a)+0.5f):ceil((a)-0.5f))

#endif


// Namespaces
using namespace std;


// Getters/Setters

string const & getPathSeparator(void) throw();
void           output(string const & p_message) throw();
void           outputSeparator(void) throw();
bool   const   isLinux(void) throw();
string const   expandPath(string const & p_path) throw();
string const   getEnvVar(string const & p_var) throw();
string const   getFileInPath(string const & p_var) throw();
bool   const   makeDirectory(string const & p_var, string const & p_description = "unknown") throw();
bool           removeAll(string const & p_dir, string const & p_ext) throw();
bool           copyDir(string const & p_source, string const & p_destination) throw();
bool   const   copyFiles(string const & p_directory, vector<string> const & p_files) throw();
string const   getConfigDir(void) throw();
bool   const   can_exec(string const & p_command) throw();
void   const   timestamp(string const & p_message) throw();
string const   get_my_process_name(void) throw();

#endif // OSUTIL_HPP
