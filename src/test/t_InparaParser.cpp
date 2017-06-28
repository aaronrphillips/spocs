///////////////////////////////////////////////////////////////////////////////
// Copyright © 2013, Battelle Memorial Institute
//
// THIS FILE INITIALLY CREATED WITH:  
//     TEMPLATE NAME:  lang_cpp_test.template 
//     COMMAND NAME:   gensrc 
//
// ND file: $Id$ 
// _____________________________________________________________________________
// 
// PURPOSE OF TEST: 
//  [put a single sentence stating the purpose of this file 
//
// CODING CONVENTIONS: 
//    * Class names are CamelCase with first word upper case 
//    * Functions are camelCase with first word lower case 
//    * Function parameters are lower case with _ and have p_ prefix 
//    * Member variables always use 'this' pointer 
///////////////////////////////////////////////////////////////////////////////


#define BOOST_TEST_MODULE InparaParser

// System Includes
// External Includes
#include <boost/test/included/unit_test.hpp>
// Internal Includes
// Application Includes
#include "InparaParser.hpp"
#include "utilities.hpp"
#include "global.hpp"


// Global Variables


struct InparaParserSuiteFixture
{
  
  InparaParserSuiteFixture()
  {
    BOOST_TEST_MESSAGE("InparaParserSuite setup fixture");

    utility_init();

    return;
  }


  ~InparaParserSuiteFixture()
  {
    BOOST_TEST_MESSAGE("InparaParserSuite teardown fixture");

    return;
  }
 
};


BOOST_FIXTURE_TEST_SUITE(InparaParserSuite, InparaParserSuiteFixture)


BOOST_AUTO_TEST_CASE(basic_test)
{
  BOOST_TEST_MESSAGE("basic lookup test");

  // Pre-condition:
  //   
  // Condition:
  //   
  // Post-condition:
  //   

  vector<string> vec;
  vec.push_back(::g_afile);
  vec.push_back(::g_bfile);
  sharedLookup lookup(new Lookup(vec, ""));
  u_int16_t bad_paralog_cutoff =   5;   //  From default in ProgramOptions.cpp
  float     confidence_cutoff  = 1.0;   //  From default in ProgramOptions.cpp
  InparaParser ip(getEnvVar("CLIQUES_HOME") + "/data/output/gold4_out_dir/", confidence_cutoff, bad_paralog_cutoff, vec, lookup);
  //BOOST_CHECK_EQUAL(alignment.getNumHits(), 4);
  BOOST_CHECK_EQUAL(0, 1);

  return;
}


BOOST_AUTO_TEST_SUITE_END()
