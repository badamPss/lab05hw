#include <gtest/gtest.h>
#include "Transaction.h"

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