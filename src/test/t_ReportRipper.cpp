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


#define BOOST_TEST_MODULE reportRipper

// System Includes
// External Includes
#include <boost/test/included/unit_test.hpp>
// Internal Includes
// Application Includes
#include "ReportRipper.hpp"
#include "Lookup.hpp"
#include "FastaReader.hpp"
#include "utilities.hpp"


// Global Variables


struct ReportRipperSuiteFixture
{
  
  ReportRipperSuiteFixture()
  {
    BOOST_TEST_MESSAGE("ReportRipperSuite setup fixture");

    utility_init();

    return;
  }


  ~ReportRipperSuiteFixture()
  {
    BOOST_TEST_MESSAGE("ReportRipperSuite teardown fixture");

    return;
  }
 
};


BOOST_FIXTURE_TEST_SUITE(ReportRipperSuite, ReportRipperSuiteFixture)


BOOST_AUTO_TEST_CASE(basic_test)
{
  BOOST_TEST_MESSAGE("basic test");

  // Pre-condition:
  //   
  // Condition:
  //   
  // Post-condition:
  //   

  ReportRipper rr(::g_blastoutfile, ::g_lookup);

  BOOST_CHECK_EQUAL(rr.getSharedHitLines()->size(), 28270);

  return;
}


BOOST_AUTO_TEST_SUITE_END()
