#ifndef CATCH_CONFIG_MAIN
#  define CATCH_CONFIG_MAIN
#endif

#include "atm.hpp"
#include "catch.hpp"

/////////////////////////////////////////////////////////////////////////////////////////////
//                             Helper Definitions //
/////////////////////////////////////////////////////////////////////////////////////////////

bool CompareFiles(const std::string& p1, const std::string& p2) {
  std::ifstream f1(p1);
  std::ifstream f2(p2);

  if (f1.fail() || f2.fail()) {
    return false;  // file problem
  }

  std::string f1_read;
  std::string f2_read;
  while (f1.good() || f2.good()) {
    f1 >> f1_read;
    f2 >> f2_read;
    if (f1_read != f2_read || (f1.good() && !f2.good()) ||
        (!f1.good() && f2.good()))
      return false;
  }
  return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Test Cases
/////////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("Simple deposit", "[ex-2]") {
  Atm atm;
  atm.RegisterAccount(12345678, 1234, "Sam Sepiol", 300.30);
  atm.DepositCash(12345678, 1234, 20);
  auto accounts = atm.GetAccounts();
  Account sam_account = accounts[{12345678, 1234}];
  REQUIRE(sam_account.balance == 320.30);
}

TEST_CASE("Deposit edge cases", "[ex-2]") {
  Atm atm;
  atm.RegisterAccount(12345678, 1234, "Sam Sepiol", 300.30);
  // check if no account exists, then make sure to throw an invalid_argument
  // exception
  REQUIRE_THROWS_AS(atm.DepositCash(87654321, 4567, 20), std::invalid_argument);

  // throw an invalid_argument exception if the amount is negative
  REQUIRE_THROWS_AS(atm.DepositCash(12345678, 1234, -20),
                    std::invalid_argument);
}

// check if account exists when calling register account and if it does, make
// sure to throw an invalid_argument exception
TEST_CASE("Register account edge cases", "[ex-0]") {
  Atm atm;
  atm.RegisterAccount(12345678, 1234, "Sam Sepiol", 300.30);

  atm.RegisterAccount(12345678, 1234, "John Doe", 300.30);
  // check if account exists, then make sure to throw an invalid_argument
  // exception
  REQUIRE_THROWS_AS(atm.RegisterAccount(12345678, 1234, "John Doe", 300.30),
                    std::invalid_argument);
}

TEST_CASE("Withdraw edge cases", "[ex-2]") {
  Atm atm;
  atm.RegisterAccount(12345678, 1234, "Sam Sepiol", 300.30);
  // check if no account exists, then make sure to throw an invalid_argument
  // exception
  REQUIRE_THROWS_AS(atm.WithdrawCash(87654321, 4567, 20),
                    std::invalid_argument);

  // throw an invalid_argument exception if the amount is negative
  REQUIRE_THROWS_AS(atm.WithdrawCash(12345678, 1234, -20),
                    std::invalid_argument);

  // throw an runtime_error exception if the amount is greater than the balance
  REQUIRE_THROWS_AS(atm.WithdrawCash(12345678, 1234, 1000), std::runtime_error);
}

TEST_CASE("Example: Create a new account", "[ex-1]") {
  Atm atm;
  atm.RegisterAccount(12345678, 1234, "Sam Sepiol", 300.30);
  auto accounts = atm.GetAccounts();
  REQUIRE(accounts.contains({12345678, 1234}));
  REQUIRE(accounts.size() == 1);

  Account sam_account = accounts[{12345678, 1234}];
  REQUIRE(sam_account.owner_name == "Sam Sepiol");
  REQUIRE(sam_account.balance == 300.30);

  auto transactions = atm.GetTransactions();
  REQUIRE(accounts.contains({12345678, 1234}));
  REQUIRE(accounts.size() == 1);
  std::vector<std::string> empty;
  REQUIRE(transactions[{12345678, 1234}] == empty);
}

TEST_CASE("Example: Simple widthdraw", "[ex-2]") {
  Atm atm;
  atm.RegisterAccount(12345678, 1234, "Sam Sepiol", 300.30);
  atm.WithdrawCash(12345678, 1234, 20);
  auto accounts = atm.GetAccounts();
  Account sam_account = accounts[{12345678, 1234}];

  REQUIRE(sam_account.balance == 280.30);
}

TEST_CASE("PrintLedger: exact prompt match with known transactions",
          "[ledger]") {
  Atm atm;
  atm.RegisterAccount(12345678, 1234, "Sam Sepiol", 300.30);

  auto& transactions = atm.GetTransactions();
  transactions[{12345678, 1234}].push_back(
      "Withdrawal - Amount: $200.40, Updated Balance: $99.90");
  transactions[{12345678, 1234}].push_back(
      "Deposit - Amount: $40000.00, Updated Balance: $40099.90");
  transactions[{12345678, 1234}].push_back(
      "Deposit - Amount: $32000.00, Updated Balance: $72099.90");

  atm.PrintLedger("./prompt.txt", 12345678, 1234);
  REQUIRE(CompareFiles("./ex-1.txt", "./prompt.txt"));
}

TEST_CASE("PrintLedger: throws for non-existent account", "[ledger]") {
  Atm atm;
  REQUIRE_THROWS_AS(atm.PrintLedger("/tmp/should_not_exist.txt", 1, 1),
                    std::invalid_argument);
}
