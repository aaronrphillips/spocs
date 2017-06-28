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


#define BOOST_TEST_MODULE outputHelper

// System Includes
#include <map>
#include <string>
// External Includes
#include <boost/test/included/unit_test.hpp>
// Internal Includes
// Application Includes
#include "global.hpp"
#include "utilities.hpp"
#include "OutputHelper.hpp"


// Global Variables


struct OutputHelperSuiteFixture
{
  
  OutputHelperSuiteFixture()
  {
    BOOST_TEST_MESSAGE("OutputHelperSuite setup fixture");

    utility_init();

    return;
  }


  ~OutputHelperSuiteFixture()
  {
    BOOST_TEST_MESSAGE("OutputHelperSuite teardown fixture");

    return;
  }
 
};


BOOST_FIXTURE_TEST_SUITE(OutputHelperSuite, OutputHelperSuiteFixture)


BOOST_AUTO_TEST_CASE(basic_test)
{
  BOOST_TEST_MESSAGE("basic test");

  // Pre-condition:
  //   
  // Condition:
  //   
  // Post-condition:
  //   

  //::g_output_helper->register_file(e_ortholog, "/var/tmp/test.ortho");
  //::g_output_helper->register_file(e_paralog,  "/var/tmp/test.table");
  //::g_output_helper->register_file(e_journal,  "/var/tmp/test.anno");
  //::g_output_helper->register_file(e_rejects,  "/var/tmp/test.reje");

  ::g_output_helper->write(e_ortholog, "here is an ortho\n");
  ::g_output_helper->write(e_paralog,  "here is a table\n");
  ::g_output_helper->write(e_journal,  "here is an annotation\n");
  ::g_output_helper->write(e_rejects,  "here is a rejection\n");
  ::g_output_helper->write(e_rejects,  "here is another rejection\n");

  ::g_output_helper->close(e_ortholog);
  ::g_output_helper->close(e_paralog);
  ::g_output_helper->close(e_journal);
  ::g_output_helper->close(e_rejects);

  ::g_output_helper->write(999, "you should see this in stdout\n");

  BOOST_CHECK_EQUAL(0, 1);

  return;
}


BOOST_AUTO_TEST_SUITE_END()
