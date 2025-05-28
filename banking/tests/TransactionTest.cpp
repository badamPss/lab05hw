#include <gtest/gtest.h>
#include "Transaction.h"
#include "Account.h"

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

class MockAccount : public Account {
public:
    MockAccount(int id, int balance) : Account(id, balance) {}
    MOCK_METHOD(int, id, (), (const, override));
    MOCK_METHOD(int, GetBalance, (), (const, override));
    MOCK_METHOD(void, ChangeBalance, (int diff), (override));
    MOCK_METHOD(void, Lock, (), (override));
    MOCK_METHOD(void, Unlock, (), (override));
};

TEST_F(TransactionTest, MakeTransactionSuccess) {
    MockAccount from(1, 1000);
    MockAccount to(2, 500);
    Transaction transaction;

    EXPECT_CALL(from, id()).WillRepeatedly(Return(1));
    EXPECT_CALL(to, id()).WillRepeatedly(Return(2));
    EXPECT_CALL(from, Lock()).Times(1);
    EXPECT_CALL(from, Unlock()).Times(1);
    EXPECT_CALL(to, Lock()).Times(1);
    EXPECT_CALL(to, Unlock()).Times(1);
    EXPECT_CALL(to, ChangeBalance(500)).Times(1);
    EXPECT_CALL(to, GetBalance()).WillOnce(Return(1000)); // Баланс to после Credit(500)
    EXPECT_CALL(to, ChangeBalance(-501)).Times(1); // Debit(500 + fee)

    bool result = transaction.Make(from, to, 500);
    EXPECT_TRUE(result);
}

TEST_F(TransactionTest, MakeTransactionInsufficientFunds) {
    MockAccount from(1, 1000);
    MockAccount to(2, 500);
    Transaction transaction;

    EXPECT_CALL(from, id()).WillRepeatedly(Return(1));
    EXPECT_CALL(to, id()).WillRepeatedly(Return(2));
    EXPECT_CALL(from, Lock()).Times(1);
    EXPECT_CALL(from, Unlock()).Times(1);
    EXPECT_CALL(to, Lock()).Times(1);
    EXPECT_CALL(to, Unlock()).Times(1);
    EXPECT_CALL(to, ChangeBalance(50)).Times(1); // Credit
    EXPECT_CALL(to, GetBalance()).WillOnce(Return(500)); // Баланс to после Credit(50)
    EXPECT_CALL(to, ChangeBalance(-50)).Times(1); // Отмена Credit при недостаточном балансе для Debit

    bool result = transaction.Make(from, to, 50);
    EXPECT_FALSE(result);
}

TEST_F(TransactionTest, MakeTransactionSameAccount) {
    MockAccount acc(1, 1000);
    Transaction transaction;

    EXPECT_CALL(acc, id()).WillRepeatedly(Return(1));
    
    EXPECT_THROW(transaction.Make(acc, acc, 100), std::logic_error);
}

TEST_F(TransactionTest, MakeTransactionNegativeSum) {
    MockAccount from(1, 1000);
    MockAccount to(2, 500);
    Transaction transaction;

    EXPECT_CALL(from, id()).WillRepeatedly(Return(1));
    EXPECT_CALL(to, id()).WillRepeatedly(Return(2));

    EXPECT_THROW(transaction.Make(from, to, -100), std::invalid_argument);
}

TEST_F(TransactionTest, MakeTransactionTooSmallSum) {
    MockAccount from(1, 1000);
    MockAccount to(2, 500);
    Transaction transaction;

    EXPECT_CALL(from, id()).WillRepeatedly(Return(1));
    EXPECT_CALL(to, id()).WillRepeatedly(Return(2));

    EXPECT_THROW(transaction.Make(from, to, 50), std::logic_error); // Сумма меньше 100
}

TEST_F(TransactionTest, MakeTransactionFeeTooHigh) {
    MockAccount from(1, 1000);
    MockAccount to(2, 500);
    Transaction transaction;

    transaction.set_fee(60); // Устанавливаем комиссию так, чтобы fee * 2 > sum (120 > 100)

    EXPECT_CALL(from, id()).WillRepeatedly(Return(1));
    EXPECT_CALL(to, id()).WillRepeatedly(Return(2));

    bool result = transaction.Make(from, to, 100);
    EXPECT_FALSE(result);
}

TEST_F(TransactionTest, DefaultConstructorFee) {
    Transaction transaction;
    EXPECT_EQ(transaction.fee(), 1);
} 