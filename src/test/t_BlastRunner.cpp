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


#define BOOST_TEST_MODULE BlastRunner

// System Includes
// External Includes
#include <boost/test/included/unit_test.hpp>
// Internal Includes
// Application Includes
#include "utilities.hpp"
#include "BlastRunner.hpp"


// Global Variables


struct BlastRunnerSuiteFixture
{
  
  BlastRunnerSuiteFixture()
  {
    BOOST_TEST_MESSAGE("BlastRunnerSuite setup fixture");

    utility_init();

    return;
  }


  ~BlastRunnerSuiteFixture()
  {
    BOOST_TEST_MESSAGE("BlastRunnerSuite teardown fixture");

    return;
  }
 
};


BOOST_FIXTURE_TEST_SUITE(BlastRunnerSuite, BlastRunnerSuiteFixture)


BOOST_AUTO_TEST_CASE(basic_test)
{
  BOOST_TEST_MESSAGE("basic test");

  // Pre-condition:
  //   
  // Condition:
  //   
  // Post-condition:
  //   

  BlastRunner br(::g_lookup, "/var/tmp", ::g_afile, ::g_bfile, ::g_cfile, "BLOSUM62", true);
  vector< tuple<string, species_type, species_type> > retVal = br.run();

  BOOST_CHECK_EQUAL(retVal.size(), 6);

  return;
}


BOOST_AUTO_TEST_SUITE_END()
