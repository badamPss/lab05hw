#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "Transaction.h"
#include "Account.h"
using ::testing::Return;
using ::testing::_;
using ::testing::NiceMock;

class MockAccount : public Account {
public:
    MockAccount(int id, int balance) : Account(id, balance) {}
    MOCK_METHOD(int, id, (), (const));
    MOCK_METHOD(int, GetBalance, (), (const, override));
    MOCK_METHOD(void, ChangeBalance, (int diff), (override));
    MOCK_METHOD(void, Lock, (), (override));
    MOCK_METHOD(void, Unlock, (), (override));
};

class TransactionTest : public ::testing::Test {
protected:
    void SetUp() override {}
};

TEST_F(TransactionTest, CreateTransaction) {
    Transaction transaction(500, "Test transaction");
    EXPECT_EQ(transaction.GetAmount(), 500);
    EXPECT_EQ(transaction.GetDescription(), "Test transaction");
}

TEST_F(TransactionTest, CreateNegativeTransaction) {
    Transaction transaction(-300, "Negative transaction");
    EXPECT_EQ(transaction.GetAmount(), -300);
    EXPECT_EQ(transaction.GetDescription(), "Negative transaction");
}

TEST_F(TransactionTest, CreateZeroAmountTransaction) {
    Transaction transaction(0, "Zero transaction");
    EXPECT_EQ(transaction.GetAmount(), 0);
    EXPECT_EQ(transaction.GetDescription(), "Zero transaction");
}

TEST_F(TransactionTest, CreateEmptyDescriptionTransaction) {
    Transaction transaction(100, "");
    EXPECT_EQ(transaction.GetAmount(), 100);
    EXPECT_EQ(transaction.GetDescription(), "");
}

TEST_F(TransactionTest, MakeTransactionSuccess) {
    NiceMock<MockAccount> from(1, 1000);
    NiceMock<MockAccount> to(2, 500);
    Transaction transaction;

    EXPECT_CALL(from, Lock()).Times(1);
    EXPECT_CALL(to, Lock()).Times(1);
    EXPECT_CALL(from, GetBalance()).WillOnce(Return(1000));
    EXPECT_CALL(from, ChangeBalance(-501)).Times(1);
    EXPECT_CALL(to, ChangeBalance(500)).Times(1);
    EXPECT_CALL(from, Unlock()).Times(1);
    EXPECT_CALL(to, Unlock()).Times(1);

    bool result = transaction.Make(from, to, 500);
    EXPECT_TRUE(result);
}

TEST_F(TransactionTest, MakeTransactionInsufficientFunds) {
    NiceMock<MockAccount> from(1, 1000);
    NiceMock<MockAccount> to(2, 500);
    Transaction transaction;

    EXPECT_CALL(from, Lock()).Times(1);
    EXPECT_CALL(to, Lock()).Times(1);
    EXPECT_CALL(from, GetBalance()).WillOnce(Return(1000));
    EXPECT_CALL(to, ChangeBalance(10000)).Times(1);
    EXPECT_CALL(to, ChangeBalance(-10000)).Times(1);
    EXPECT_CALL(from, Unlock()).Times(1);
    EXPECT_CALL(to, Unlock()).Times(1);

    bool result = transaction.Make(from, to, 10000);
    EXPECT_FALSE(result);
}

TEST_F(TransactionTest, MakeTransactionSameAccount) {
    Account acc(1, 1000);
    Transaction transaction;

    EXPECT_THROW(transaction.Make(acc, acc, 100), std::logic_error);
}

TEST_F(TransactionTest, MakeTransactionNegativeSum) {
    Account from(1, 1000);
    Account to(2, 500);
    Transaction transaction;

    EXPECT_THROW(transaction.Make(from, to, -100), std::invalid_argument);
}

TEST_F(TransactionTest, MakeTransactionTooSmallSum) {
    NiceMock<MockAccount> from(1, 1000);
    NiceMock<MockAccount> to(2, 500);
    Transaction transaction;

    EXPECT_CALL(from, Lock()).Times(0);
    EXPECT_CALL(to, Lock()).Times(0);
    EXPECT_CALL(from, GetBalance()).Times(0);
    EXPECT_CALL(from, ChangeBalance(_)).Times(0);
    EXPECT_CALL(to, ChangeBalance(_)).Times(0);

    EXPECT_THROW(transaction.Make(from, to, 50), std::logic_error);
}

TEST_F(TransactionTest, MakeTransactionFeeTooHigh) {
    NiceMock<MockAccount> from(1, 1000);
    NiceMock<MockAccount> to(2, 500);
    Transaction transaction;
    transaction.set_fee(60);

    EXPECT_CALL(from, Lock()).Times(0);
    EXPECT_CALL(to, Lock()).Times(0);
    EXPECT_CALL(from, GetBalance()).Times(0);
    EXPECT_CALL(from, ChangeBalance(_)).Times(0);
    EXPECT_CALL(to, ChangeBalance(_)).Times(0);

    bool result = transaction.Make(from, to, 100);
    EXPECT_FALSE(result);
}

TEST_F(TransactionTest, DefaultConstructorFee) {
    Transaction transaction;
    EXPECT_EQ(transaction.fee(), 1);
}

TEST_F(TransactionTest, SetFee) {
    Transaction transaction;
    transaction.set_fee(5);
    EXPECT_EQ(transaction.fee(), 5);
}

TEST_F(TransactionTest, GetAmountAndDescription) {
    Transaction transaction(100, "Test transaction");
    EXPECT_EQ(transaction.GetAmount(), 100);
    EXPECT_EQ(transaction.GetDescription(), "Test transaction");
}

TEST_F(TransactionTest, MakeTransactionFeeTooBig) {
    Account from(1, 1000);
    Account to(2, 500);
    Transaction transaction;
    transaction.set_fee(300);
    bool result = transaction.Make(from, to, 500);
    EXPECT_FALSE(result);
}

TEST_F(TransactionTest, DestructorCoverage) {
    Transaction* tr = new Transaction(10, "desc");
    delete tr;
} 