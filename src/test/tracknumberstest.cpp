#include <gtest/gtest.h>

#include "track/tracknumbers.h"

#include <QtDebug>

namespace {

class TrackNumbersTest : public testing::Test {
  protected:

    TrackNumbersTest() {
    }

    virtual void SetUp() {
    }

    virtual void TearDown() {
    }

    void parseFromStringEmpty(const QString& inputValue) {
        TrackNumbers actualTrackNumbers;
        TrackNumbers::ParseResult actualParseResult = TrackNumbers::parseFromString(inputValue, &actualTrackNumbers);

        qDebug() << "parseFromString():" << inputValue << "->" << actualTrackNumbers.toString();

        EXPECT_EQ(TrackNumbers::ParseResult::EMPTY, actualParseResult);
        EXPECT_TRUE(actualTrackNumbers.isValid());
        EXPECT_EQ(TrackNumbers(), actualTrackNumbers);
    }

    TrackNumbers parseFromStringValid(const QString& inputValue, const TrackNumbers& expectedResult) {
        TrackNumbers actualTrackNumbers;
        TrackNumbers::ParseResult actualParseResult = TrackNumbers::parseFromString(inputValue, &actualTrackNumbers);

        qDebug() << "parseFromString():" << inputValue << "->" << actualTrackNumbers.toString();

        EXPECT_EQ(TrackNumbers::ParseResult::VALID, actualParseResult);
        EXPECT_TRUE(actualTrackNumbers.isValid());
        EXPECT_EQ(expectedResult, actualTrackNumbers);

        return actualTrackNumbers;
    }

    TrackNumbers parseFromStringInvalid(const QString& inputValue, const TrackNumbers& expectedResult) {
        TrackNumbers actualTrackNumbers;
        TrackNumbers::ParseResult actualParseResult = TrackNumbers::parseFromString(inputValue, &actualTrackNumbers);

        qDebug() << "parseFromString():" << inputValue << "->" << actualTrackNumbers.toString();

        EXPECT_EQ(TrackNumbers::ParseResult::INVALID, actualParseResult);
        EXPECT_EQ(expectedResult, actualTrackNumbers);

        return actualTrackNumbers;
    }

    void toString(const TrackNumbers& inputValue, const QString& expectedResult) {
        const QString actualResult(inputValue.toString());
        qDebug() << "toString():" << expectedResult << "->" << actualResult;

        EXPECT_EQ(expectedResult, actualResult);

        // Re-parse the formatted string if the input is
        // a valid TrackNumbers instance
        if (inputValue.isValid()) {
            if (actualResult.isEmpty()) {
                parseFromStringEmpty(actualResult);
            } else {
                parseFromStringValid(actualResult, inputValue);
            }
        }
    }

    void splitAndJointStrings(const QString& inputValue, const QString& expectedTrackNumber, const QString& expectedTrackTotal) {
        QString actualTrackNumber;
        QString actualTrackTotal;
        TrackNumbers::splitString(inputValue, &actualTrackNumber, &actualTrackTotal);
        qDebug() << "splitString():" << inputValue << "->" << actualTrackNumber << actualTrackTotal;

        EXPECT_EQ(expectedTrackNumber, actualTrackNumber);
        EXPECT_EQ(expectedTrackTotal, actualTrackTotal);
        EXPECT_EQ(inputValue, TrackNumbers::joinStrings(actualTrackNumber, actualTrackTotal));
    }
};

TEST_F(TrackNumbersTest, ParseFromStringEmpty) {
    parseFromStringEmpty("");
    parseFromStringEmpty(" \t \n   ");
}

TEST_F(TrackNumbersTest, ParseFromStringValid) {
    parseFromStringValid("0", TrackNumbers(0));
    parseFromStringValid("0/0", TrackNumbers(0, 0));
    parseFromStringValid("1 / 0", TrackNumbers(1, 0));
    parseFromStringValid(" 1\t/\t2  ", TrackNumbers(1, 2));
    parseFromStringValid("12/7", TrackNumbers(12, 7));
    parseFromStringValid("01234/12345", TrackNumbers(1234, 12345));
}

TEST_F(TrackNumbersTest, ParseFromStringInvalid) {
    parseFromStringInvalid("-2", TrackNumbers(-2));
    parseFromStringInvalid("1/ -1", TrackNumbers(1, -1));
    parseFromStringInvalid("-2 / 1", TrackNumbers(-2, 1));
    parseFromStringInvalid("-3/-4", TrackNumbers(-3, -4));
    parseFromStringInvalid("1/a", TrackNumbers(1, TrackNumbers::kValueUndefined));
    parseFromStringInvalid("a /1", TrackNumbers(TrackNumbers::kValueUndefined, 1));
}

TEST_F(TrackNumbersTest, ToString) {
    toString(TrackNumbers(), "");
    toString(TrackNumbers(0), "");
    toString(TrackNumbers(-3), "");
    toString(TrackNumbers(1), "1");
    toString(TrackNumbers(12), "12");
    toString(TrackNumbers(1, 0), "1");
    toString(TrackNumbers(12, 0), "12");
    toString(TrackNumbers(0, 1), "0/1");
    toString(TrackNumbers(0, 12), "00/12"); // with padding
    toString(TrackNumbers(7, 12), "07/12"); // with padding
    toString(TrackNumbers(-7, 12), "00/12"); // with padding
    toString(TrackNumbers(-7, 123), "000/123"); // with padding
    toString(TrackNumbers(63, 100), "063/100"); // with padding
    toString(TrackNumbers(7, -12), "7");
    toString(TrackNumbers(12, 7), "12/7");
    toString(TrackNumbers(12, 34), "12/34");
}

TEST_F(TrackNumbersTest, SplitAndJoinStrings) {
    splitAndJointStrings("", "", "");
    splitAndJointStrings(" 01 \n", " 01 \n", "");
    splitAndJointStrings(" 1 \n /\t 10  ", " 1 \n ", "\t 10  ");
    splitAndJointStrings(" 01 \n /\t 10 // 2 ", " 01 \n ", "\t 10 // 2 ");
    splitAndJointStrings("\n /\t 10 // 2 ", "\n ", "\t 10 // 2 ");
    splitAndJointStrings("/\t 10 // 2 ", "", "\t 10 // 2 ");
}

} // anonymous namespace
