#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "Account.h"
#include "Transaction.h"

using ::testing::Return;
using ::testing::_;

class MockTransaction : public Transaction {
public:
    MOCK_METHOD(int, GetAmount, (), (const, override));
    MOCK_METHOD(std::string, GetDescription, (), (const, override));
};

class AccountTest : public ::testing::Test {
protected:
    void SetUp() override {
        account = std::make_unique<Account>(1, 1000);
    }

    std::unique_ptr<Account> account;
};

TEST_F(AccountTest, InitialBalance) {
    EXPECT_EQ(account->GetBalance(), 1000);
}

TEST_F(AccountTest, AddTransaction) {
    MockTransaction mockTransaction;
    EXPECT_CALL(mockTransaction, GetAmount()).WillOnce(Return(500));
    EXPECT_CALL(mockTransaction, GetDescription()).WillOnce(Return("Test transaction"));

    account->Lock();
    account->AddTransaction(std::shared_ptr<Transaction>(&mockTransaction, [](Transaction*){}));
    EXPECT_EQ(account->GetBalance(), 1500);
}

TEST_F(AccountTest, AddNegativeTransaction) {
    MockTransaction mockTransaction;
    EXPECT_CALL(mockTransaction, GetAmount()).WillOnce(Return(-300));
    EXPECT_CALL(mockTransaction, GetDescription()).WillOnce(Return("Negative transaction"));

    account->Lock();
    account->AddTransaction(std::shared_ptr<Transaction>(&mockTransaction, [](Transaction*){}));
    EXPECT_EQ(account->GetBalance(), 700);
}

TEST_F(AccountTest, GetTransactionHistory) {
    MockTransaction mockTransaction1;
    MockTransaction mockTransaction2;

    EXPECT_CALL(mockTransaction1, GetAmount()).WillOnce(Return(500));
    EXPECT_CALL(mockTransaction1, GetDescription()).WillOnce(Return("First transaction"));
    EXPECT_CALL(mockTransaction2, GetAmount()).WillOnce(Return(-200));
    EXPECT_CALL(mockTransaction2, GetDescription()).WillOnce(Return("Second transaction"));

    account->Lock();
    account->AddTransaction(std::shared_ptr<Transaction>(&mockTransaction1, [](Transaction*){}));
    account->AddTransaction(std::shared_ptr<Transaction>(&mockTransaction2, [](Transaction*){}));

    const auto& history = account->GetTransactionHistory();
    EXPECT_EQ(history.size(), 2);
    EXPECT_EQ(account->GetBalance(), 1300);
} 