#include "../src/scanner.hpp"
#include <gtest/gtest.h>
#include <iostream>
#include <memory>
#include <string>
using std::unique_ptr, std::make_unique, std::cout, std::endl, std::string;
TEST(ExampleTests, TestThatWorks) { EXPECT_TRUE(true); }

class ScannerTestFixture : public ::testing::Test {

protected:
  unique_ptr<Scanner> scanner;

  void SetUp() override { scanner = make_unique<Scanner>("1 + 1 + 1"); }

  void TearDown() override {
    cout << "Scanner:" << *scanner << endl;
    cout << "=======Test Ended!=======" << endl;
  }
};
TEST_F(ScannerTestFixture, MakeSureScannerConstructionHasCorrectFields) {
  EXPECT_EQ(scanner->getSourceFile(), "1 + 1 + 1");
  EXPECT_EQ(scanner->getHadError(), false);
  EXPECT_EQ(scanner->getStart(), 0);
  EXPECT_EQ(scanner->getCurrent(), 0);
  EXPECT_EQ(scanner->getLenOfFile(), 9);
  EXPECT_EQ(scanner->getTokenList()->size(), 0);
  EXPECT_EQ(scanner->getErrorList()->size(), 0);
}
