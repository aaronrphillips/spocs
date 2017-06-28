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


#define BOOST_TEST_MODULE Alignments

// System Includes
// External Includes
#include <boost/test/included/unit_test.hpp>
// Internal Includes
// Application Includes
#include "Alignments.hpp"
#include "utilities.hpp"
#include "global.hpp"


// Global Variables


struct AlignmentsSuiteFixture
{
  
  AlignmentsSuiteFixture()
  {
    BOOST_TEST_MESSAGE("AlignmentsSuite setup fixture");

    utility_init();

    return;
  }


  ~AlignmentsSuiteFixture()
  {
    BOOST_TEST_MESSAGE("AlignmentsSuite teardown fixture");

    return;
  }
 
};


BOOST_FIXTURE_TEST_SUITE(AlignmentsSuite, AlignmentsSuiteFixture)


BOOST_AUTO_TEST_CASE(basic_test)
{
  BOOST_TEST_MESSAGE("basic lookup test");

  // Pre-condition:
  //   
  // Condition:
  //   
  // Post-condition:
  //   

  u_int32_t score = 5;
  id_type q_id = 1;
  id_type s_id = 2;

  Alignments alignment(e_species_A, e_species_B, q_id, s_id, score);
  alignment.add(3, .7);
  alignment.add(4, .5);
  alignment.add(5, .9);

  BOOST_CHECK_EQUAL(alignment.getNumHits(), 4);
  BOOST_CHECK_EQUAL(alignment.isABestHit(5), false);
  BOOST_CHECK_EQUAL(alignment.isABestHit(s_id), true);
  BOOST_CHECK_EQUAL(alignment.getHitsBetterThan(0.6).size(), 3);
  BOOST_CHECK_EQUAL(alignment.getBestHits().size(), 1);
  BOOST_CHECK_EQUAL(alignment.getBestHit(), s_id);
  BOOST_CHECK_EQUAL(alignment.getBestScore(), score);

  return;
}


BOOST_AUTO_TEST_SUITE_END()
