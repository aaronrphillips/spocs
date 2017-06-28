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


#define BOOST_TEST_MODULE Lookup

// System Includes
// External Includes
#include <boost/test/included/unit_test.hpp>
// Internal Includes
// Application Includes
#include "Lookup.hpp"
#include "utilities.hpp"


// Global Variables


struct LookupSuiteFixture
{
  
  LookupSuiteFixture()
  {
    BOOST_TEST_MESSAGE("LookupSuite setup fixture");

    utility_init();

    return;
  }


  ~LookupSuiteFixture()
  {
    BOOST_TEST_MESSAGE("LookupSuite teardown fixture");

    return;
  }
 
};


BOOST_FIXTURE_TEST_SUITE(LookupSuite, LookupSuiteFixture)


BOOST_AUTO_TEST_CASE(basic_test)
{
  BOOST_TEST_MESSAGE("basic lookup test");

  // Pre-condition:
  //   
  // Condition:
  //   
  // Post-condition:
  //   

  string aname = "b0005";

  //BOOST_CHECK_EQUAL(::g_lookup.getIdByName(name), 1);
  BOOST_CHECK_EQUAL(::g_lookup->getLengthByName(aname), 98);
  //BOOST_CHECK_EQUAL(::g_lookup.getNameById(1), name);
  BOOST_CHECK_EQUAL(::g_lookup->getIdByName("DOESNTEXIST"), 0);
  BOOST_CHECK_EQUAL(::g_lookup->getNameById(1239847109), string(""));
  //BOOST_CHECK_EQUAL(::g_lookup.getScore(::g_lookup.getIdByName(name), ::g_lookup.getIdByName(name2)), 50);
  //BOOST_CHECK_EQUAL(::g_lookup.getScore(::g_lookup.getIdByName(name), 3), (float)0.0);

  return;
}


BOOST_AUTO_TEST_SUITE_END()
