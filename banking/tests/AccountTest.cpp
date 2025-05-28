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
        account = std::make_unique<Account>(1000);
    }

    std::unique_ptr<Account> account;
};

TEST_F(AccountTest, InitialBalance) {
    EXPECT_EQ(account->GetBalance(), 1000);
}

TEST_F(AccountTest, AddTransaction) {
    auto mockTransaction = std::make_shared<MockTransaction>();
    EXPECT_CALL(*mockTransaction, GetAmount()).WillOnce(Return(500));
    EXPECT_CALL(*mockTransaction, GetDescription()).WillOnce(Return("Test transaction"));

    account->AddTransaction(mockTransaction);
    EXPECT_EQ(account->GetBalance(), 1500);
}

TEST_F(AccountTest, AddNegativeTransaction) {
    auto mockTransaction = std::make_shared<MockTransaction>();
    EXPECT_CALL(*mockTransaction, GetAmount()).WillOnce(Return(-300));
    EXPECT_CALL(*mockTransaction, GetDescription()).WillOnce(Return("Negative transaction"));

    account->AddTransaction(mockTransaction);
    EXPECT_EQ(account->GetBalance(), 700);
}

TEST_F(AccountTest, GetTransactionHistory) {
    auto mockTransaction1 = std::make_shared<MockTransaction>();
    auto mockTransaction2 = std::make_shared<MockTransaction>();

    EXPECT_CALL(*mockTransaction1, GetAmount()).WillOnce(Return(500));
    EXPECT_CALL(*mockTransaction1, GetDescription()).WillOnce(Return("First transaction"));
    EXPECT_CALL(*mockTransaction2, GetAmount()).WillOnce(Return(-200));
    EXPECT_CALL(*mockTransaction2, GetDescription()).WillOnce(Return("Second transaction"));

    account->AddTransaction(mockTransaction1);
    account->AddTransaction(mockTransaction2);

    const auto& history = account->GetTransactionHistory();
    EXPECT_EQ(history.size(), 2);
    EXPECT_EQ(account->GetBalance(), 1300);
} 