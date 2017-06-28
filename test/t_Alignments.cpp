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


// System Includes
// External Includes
#include <gtest/gtest.h>
// Internal Includes
// Application Includes
#include "Lookup.hpp"
#include "utilities.hpp"


// Global Variables


namespace
{
class LookupTest : public ::testing::Test
{
protected:

  LookupTest()
  {
    // You can do set-up work for each test here.
    utility_init();
    return;
  }


  virtual ~LookupTest()
  {
    // You can do clean-up work that doesn't throw exceptions here.
    return;
  }


  // If the constructor and destructor are not enough for setting up
  // and cleaning up each test, you can define the following methods:
  virtual void SetUp()
  {
    // Code here will be called immediately after the constructor (right
    // before each test).
  }


  virtual void TearDown()
  {
    // Code here will be called immediately after each test (right
    // before the destructor).
  }


  // Objects declared here can be used by all tests in the test case for Lookup.

};


TEST_F(LookupTest, GetLengthByName)
{
  string aname = "b0005";

  EXPECT_EQ(::g_lookup->getLengthByName(aname), (u_int32_t)98);
  EXPECT_EQ(::g_lookup->getNameById(1239847109), string(""));

  return;
}


TEST_F(LookupTest, DoesNotExist)
{
  EXPECT_EQ(::g_lookup->getIdByName("DOESNTEXIST"), (id_type)0);

  return;
}

}


int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
