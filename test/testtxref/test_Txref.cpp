#pragma ide diagnostic ignored "cert-err58-cpp"
#include <gtest/gtest.h>
#pragma clang diagnostic push
#pragma GCC diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#pragma GCC diagnostic ignored "-Wall"
#pragma GCC diagnostic ignored "-Wextra"
#include <rapidcheck/gtest.h>
#pragma clang diagnostic pop
#pragma GCC diagnostic pop

#include "txref.cpp"

// check that we accept block heights within the correct range
TEST(TxrefTest, accept_good_block_heights) {
    EXPECT_NO_THROW(checkBlockHeightRange(0));
    EXPECT_NO_THROW(checkBlockHeightRange(1));
    EXPECT_NO_THROW(checkBlockHeightRange(MAX_BLOCK_HEIGHT));
}

RC_GTEST_PROP(TxrefTestRC, goodBlockHeightsAreAccepted, ()
) {
    // generate valid block heights
    auto height = *rc::gen::inRange(0, MAX_BLOCK_HEIGHT);
    checkBlockHeightRange(height);
}


// check that we reject block heights outside of the range
TEST(TxrefTest, reject_bad_block_heights) {
    EXPECT_THROW(checkBlockHeightRange(-1), std::runtime_error);
    EXPECT_THROW(checkBlockHeightRange(MAX_BLOCK_HEIGHT + 1), std::runtime_error);
}

RC_GTEST_PROP(TxrefTestRC, badBlockHeightsAreRejected, ()
) {
    // generate out of range block heights
    auto height = *rc::gen::inRange(-MAX_BLOCK_HEIGHT, -1);
    RC_ASSERT_THROWS_AS(checkBlockHeightRange(height), std::runtime_error);

    height = *rc::gen::inRange(MAX_BLOCK_HEIGHT+1, 2*MAX_BLOCK_HEIGHT);
    RC_ASSERT_THROWS_AS(checkBlockHeightRange(height), std::runtime_error);
}


// check that we accept transaction positions within the correct range
TEST(TxrefTest, accept_good_transaction_position) {
    EXPECT_NO_THROW(checkTransactionPositionRange(0));
    EXPECT_NO_THROW(checkTransactionPositionRange(1));
    EXPECT_NO_THROW(checkTransactionPositionRange(MAX_TRANSACTION_POSITION));
}

RC_GTEST_PROP(TxrefTestRC, goodTransactionPositionsAreAccepted, ()
) {
    // generate valid transaction positions
    auto pos = *rc::gen::inRange(0, MAX_TRANSACTION_POSITION);
    checkTransactionPositionRange(pos);
}


// check that we reject transaction positions outside of the range
TEST(TxrefTest, reject_bad_transaction_position) {
    EXPECT_THROW(checkTransactionPositionRange(-1), std::runtime_error);
    EXPECT_THROW(checkTransactionPositionRange(MAX_TRANSACTION_POSITION + 1), std::runtime_error);
}

RC_GTEST_PROP(TxrefTestRC, badTransactionPositionsAreRejected, ()
) {
    // generate out of range transaction positions
    auto pos = *rc::gen::inRange(-MAX_TRANSACTION_POSITION, -1);
    RC_ASSERT_THROWS_AS(checkTransactionPositionRange(pos), std::runtime_error);

    pos = *rc::gen::inRange(MAX_TRANSACTION_POSITION+1, 2*MAX_TRANSACTION_POSITION);
    RC_ASSERT_THROWS_AS(checkTransactionPositionRange(pos), std::runtime_error);
}

// check that we accept magic codes within the correct range
TEST(TxrefTest, accept_good_magic_code) {
    EXPECT_NO_THROW(checkMagicCodeRange(0));
    EXPECT_NO_THROW(checkMagicCodeRange(1));
    EXPECT_NO_THROW(checkMagicCodeRange(MAX_MAGIC_CODE));
}

RC_GTEST_PROP(TxrefTestRC, goodMagicCodesAreAccepted, ()
) {
    // generate valid magic codes
    auto code = *rc::gen::inRange(0, MAX_MAGIC_CODE);
    checkMagicCodeRange(code);
}

// check that we reject magic codes outside of the range
TEST(TxrefTest, reject_bad_magic_code) {
    EXPECT_THROW(checkMagicCodeRange(-1), std::runtime_error);
    EXPECT_THROW(checkMagicCodeRange(MAX_MAGIC_CODE + 1), std::runtime_error);
}

RC_GTEST_PROP(TxrefTestRC, badMagicCodesAreRejected, ()
) {
    // generate out of range magic codes
    auto code = *rc::gen::inRange(-MAX_MAGIC_CODE, -1);
    RC_ASSERT_THROWS_AS(checkMagicCodeRange(code), std::runtime_error);

    code = *rc::gen::inRange(MAX_MAGIC_CODE + 1, 2 * MAX_MAGIC_CODE);
    RC_ASSERT_THROWS_AS(checkMagicCodeRange(code), std::runtime_error);
}

TEST(TxrefTest, addDashes_inputStringTooShort) {
    EXPECT_THROW(addGroupSeparators("", 0, 1), std::runtime_error);
    EXPECT_THROW(addGroupSeparators("0", 0, 1), std::runtime_error);
}

TEST(TxrefTest, addDashes_HRPLongerThanInput) {

    // hrplen is zero, then the "rest" of the input is of length two, so one hyphen should be inserted
    auto result = addGroupSeparators("00", 0, 1);
    EXPECT_EQ(result, "0-0");

    // hrplen is one, then the "rest" of the input is of length one, so zero hyphens should be inserted
    result = addGroupSeparators("00", 1, 1);
    EXPECT_EQ(result, "00");

    // hrplen is two, then the "rest" of the input is of length zero, so zero hyphens should be inserted
    result = addGroupSeparators("00", 2, 1);
    EXPECT_EQ(result, "00");

    // hrplen is three, then the "rest" of the input is of length -1, so exception is thrown
    EXPECT_THROW(addGroupSeparators("00", 3, 1), std::runtime_error);
}

TEST(TxrefTest, addDashes_HRPTooLong) {
    EXPECT_THROW(addGroupSeparators("00", bech32::limits::MAX_HRP_LENGTH+1, 1), std::runtime_error);
}

TEST(TxrefTest, addDashes_separatorOffsetTooSmall) {
    EXPECT_THROW(addGroupSeparators("00", 0, -1), std::runtime_error);
    EXPECT_THROW(addGroupSeparators("00", 0, 0), std::runtime_error);
}

TEST(TxrefTest, addDashes_separatorOffsetTooLarge) {
    // if separatorOffset is greater than input string length, output should be the same
    auto result = addGroupSeparators("00", 0, 2);
    EXPECT_EQ(result, "00");
    result = addGroupSeparators("00", 0, 10);
    EXPECT_EQ(result, "00");
}

TEST(TxrefTest, addDashes_everyOtherCharacter) {
    auto result = addGroupSeparators("00", 0, 1);
    EXPECT_EQ(result, "0-0");

    result = addGroupSeparators("000", 0, 1);
    EXPECT_EQ(result, "0-0-0");

    result = addGroupSeparators("0000", 0, 1);
    EXPECT_EQ(result, "0-0-0-0");

    result = addGroupSeparators("00000", 0, 1);
    EXPECT_EQ(result, "0-0-0-0-0");
}

TEST(TxrefTest, addDashes_everyFewCharacters) {
    auto result = addGroupSeparators("0000000", 0, 1);
    EXPECT_EQ(result, "0-0-0-0-0-0-0");

    result = addGroupSeparators("0000000", 0, 2);
    EXPECT_EQ(result, "00-00-00-0");

    result = addGroupSeparators("0000000", 0, 3);
    EXPECT_EQ(result, "000-000-0");

    result = addGroupSeparators("0000000", 0, 4);
    EXPECT_EQ(result, "0000-000");
}

TEST(TxrefTest, addDashes_withHRPs) {
    auto result = addGroupSeparators("A0000000", 1, 1);
    EXPECT_EQ(result, "A0-0-0-0-0-0-0");

    result = addGroupSeparators("AB0000000", 2, 2);
    EXPECT_EQ(result, "AB00-00-00-0");

    result = addGroupSeparators("ABCD0000000", 4, 4);
    EXPECT_EQ(result, "ABCD0000-000");

}

TEST(TxrefTest, prettyPrint) {
    std::string hrp = txref::BECH32_HRP_MAIN;
    std::string plain = "tx1rqqqqqqqqwtvvjr";
    std::string pretty = prettyPrint(plain, hrp.length());
    EXPECT_EQ(pretty, "tx1:rqqq-qqqq-qwtv-vjr");
}


// check that we can extract the magic code for a txref string
TEST(TxrefTest, extract_magicCode) {
    std::string txref;
    uint8_t magicCode;
    bech32::DecodedResult decodedResult;

    txref = "tx1rqqqqqqqqwtvvjr";
    decodedResult = bech32::decode(txref);
    extractMagicCode(magicCode, decodedResult.dp);
    EXPECT_EQ(magicCode, txref::MAGIC_CODE_MAIN);

    txref = "txtest1xjk0uqayzghlp89";
    decodedResult = bech32::decode(txref);
    extractMagicCode(magicCode, decodedResult.dp);
    EXPECT_EQ(magicCode, txref::MAGIC_CODE_TEST);
}

// check that we can extract the version for a txref string
TEST(TxrefTest, extract_version) {
    std::string txref;
    uint8_t version;
    bech32::DecodedResult decodedResult;

    txref = "tx1rqqqqqqqqwtvvjr";
    decodedResult = bech32::decode(txref);
    extractVersion(version, decodedResult.dp);
    EXPECT_EQ(version, 0);

    txref = "txtest1xjk0uqayzghlp89";
    decodedResult = bech32::decode(txref);
    extractVersion(version, decodedResult.dp);
    EXPECT_EQ(version, 0);
}

// check that we can extract the block height from txrefs
TEST(TxrefTest, extract_block_height) {
    std::string txref;
    int blockHeight;
    bech32::DecodedResult decodedResult;

    txref = "tx1rqqqqqqqqwtvvjr";
    decodedResult = bech32::decode(txref);
    extractBlockHeight(blockHeight, decodedResult.dp);
    EXPECT_EQ(blockHeight, 0);

    txref = "tx1rqqqqqlllj687n2";
    decodedResult = bech32::decode(txref);
    extractBlockHeight(blockHeight, decodedResult.dp);
    EXPECT_EQ(blockHeight, 0);

    txref = "tx1r7llllqqqatsvx9";
    decodedResult = bech32::decode(txref);
    extractBlockHeight(blockHeight, decodedResult.dp);
    EXPECT_EQ(blockHeight, 0xFFFFFF);

    txref = "tx1r7lllllllp6m78v";
    decodedResult = bech32::decode(txref);
    extractBlockHeight(blockHeight, decodedResult.dp);
    EXPECT_EQ(blockHeight, 0xFFFFFF);

    txref = "tx1rjk0uqayz9l7m9m";
    decodedResult = bech32::decode(txref);
    extractBlockHeight(blockHeight, decodedResult.dp);
    EXPECT_EQ(blockHeight, 466793);

    txref = "txtest1xjk0uqayzghlp89";
    decodedResult = bech32::decode(txref);
    extractBlockHeight(blockHeight, decodedResult.dp);
    EXPECT_EQ(blockHeight, 466793);
}

// check that we can extract the transaction position from txrefs
TEST(TxrefTest, extract_transaction_position) {
    std::string txref;
    int transactionPosition;
    bech32::DecodedResult decodedResult;

    txref = "tx1rqqqqqqqqwtvvjr";
    decodedResult = bech32::decode(txref);
    extractTransactionPosition(transactionPosition, decodedResult.dp);
    EXPECT_EQ(transactionPosition, 0);

    txref = "tx1rqqqqqlllj687n2";
    decodedResult = bech32::decode(txref);
    extractTransactionPosition(transactionPosition, decodedResult.dp);
    EXPECT_EQ(transactionPosition, 0x7FFF);

    txref = "tx1r7llllqqqatsvx9";
    decodedResult = bech32::decode(txref);
    extractTransactionPosition(transactionPosition, decodedResult.dp);
    EXPECT_EQ(transactionPosition, 0);

    txref = "tx1r7lllllllp6m78v";
    decodedResult = bech32::decode(txref);
    extractTransactionPosition(transactionPosition, decodedResult.dp);
    EXPECT_EQ(transactionPosition, 0x7FFF);

    txref = "tx1rjk0uqayz9l7m9m";
    decodedResult = bech32::decode(txref);
    extractTransactionPosition(transactionPosition, decodedResult.dp);
    EXPECT_EQ(transactionPosition, 2205);

    txref = "txtest1xjk0uqayzghlp89";
    decodedResult = bech32::decode(txref);
    extractTransactionPosition(transactionPosition, decodedResult.dp);
    EXPECT_EQ(transactionPosition, 2205);
}

// check that extracting the txo index from txrefs always returns 0
TEST(TxrefTest, extract_txo_position) {
    std::string txref;
    int txoIndex;
    bech32::DecodedResult decodedResult;

    txref = "tx1rqqqqqqqqwtvvjr";
    decodedResult = bech32::decode(txref);
    extractTxoIndex(txoIndex, decodedResult.dp);
    EXPECT_EQ(txoIndex, 0);

    txref = "tx1rqqqqqlllj687n2";
    decodedResult = bech32::decode(txref);
    extractTxoIndex(txoIndex, decodedResult.dp);
    EXPECT_EQ(txoIndex, 0);

    txref = "tx1r7llllqqqatsvx9";
    decodedResult = bech32::decode(txref);
    extractTxoIndex(txoIndex, decodedResult.dp);
    EXPECT_EQ(txoIndex, 0);

    txref = "tx1r7lllllllp6m78v";
    decodedResult = bech32::decode(txref);
    extractTxoIndex(txoIndex, decodedResult.dp);
    EXPECT_EQ(txoIndex, 0);

    txref = "tx1rjk0uqayz9l7m9m";
    decodedResult = bech32::decode(txref);
    extractTxoIndex(txoIndex, decodedResult.dp);
    EXPECT_EQ(txoIndex, 0);

    txref = "txtest1xjk0uqayzghlp89";
    decodedResult = bech32::decode(txref);
    extractTxoIndex(txoIndex, decodedResult.dp);
    EXPECT_EQ(txoIndex, 0);
}

// check that we can add missing standard HRPs if needed
TEST(TxrefTest, txref_add_hrps) {
    std::string txref;

    txref = "rqqqqqqqqwtvvjr";
    EXPECT_EQ(addHrpIfNeeded(txref), "tx1rqqqqqqqqwtvvjr");

    txref = "xjk0uqayzghlp89";
    EXPECT_EQ(addHrpIfNeeded(txref), "txtest1xjk0uqayzghlp89");

    txref = "q7lllllllps4p3p";
    EXPECT_EQ(addHrpIfNeeded(txref), "txrt1q7lllllllps4p3p");

}

// check that we correctly encode some sample txrefs
TEST(TxrefTest, txref_encode_mainnet) {
    EXPECT_EQ(txrefEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_CODE_MAIN, 0, 0),
              "tx1:rqqq-qqqq-qwtv-vjr");
    EXPECT_EQ(txrefEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_CODE_MAIN, 0, 0x7FFF),
              "tx1:rqqq-qqll-lj68-7n2");
    EXPECT_EQ(txrefEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_CODE_MAIN, 0xFFFFFF, 0),
              "tx1:r7ll-llqq-qats-vx9");
    EXPECT_EQ(txrefEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_CODE_MAIN, 0xFFFFFF, 0x7FFF),
              "tx1:r7ll-llll-lp6m-78v");
    EXPECT_EQ(txrefEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_CODE_MAIN, 466793, 2205),
              "tx1:rjk0-uqay-z9l7-m9m");

    EXPECT_EQ(txrefEncode(txref::BECH32_HRP_TEST, txref::MAGIC_CODE_TEST, 466793, 2205),
              "txtest1:xjk0-uqay-zghl-p89");
}

RC_GTEST_PROP(TxrefTestRC, checkThatEncodeAndDecodeProduceSameParameters, ()
) {
    auto height = *rc::gen::inRange(0, MAX_BLOCK_HEIGHT);
    auto pos = *rc::gen::inRange(0, MAX_TRANSACTION_POSITION);

    auto txref = txrefEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_CODE_MAIN, height, pos);
    auto decodedResult = txref::decode(txref);

    RC_ASSERT(decodedResult.blockHeight == height);
    RC_ASSERT(decodedResult.transactionPosition == pos);
}

// //////////////// Extended Txrefs /////////////////////

// check for magic codes that support extended txrefs
TEST(TxrefTest, accept_good_magic_code_for_extended) {
    EXPECT_NO_THROW(checkExtendedMagicCode(txref::MAGIC_CODE_MAIN_EXTENDED));
    EXPECT_NO_THROW(checkExtendedMagicCode(txref::MAGIC_CODE_TEST_EXTENDED));
    EXPECT_NO_THROW(checkExtendedMagicCode(txref::MAGIC_CODE_REGTEST_EXTENDED));

    EXPECT_THROW(checkExtendedMagicCode(txref::MAGIC_CODE_MAIN), std::runtime_error);
    EXPECT_THROW(checkExtendedMagicCode(txref::MAGIC_CODE_TEST), std::runtime_error);
    EXPECT_THROW(checkExtendedMagicCode(txref::MAGIC_CODE_REGTEST), std::runtime_error);
}

// check that we can extract the magic code for an extended txref string
TEST(TxrefTest, extract_extended_magicCode) {
    std::string txref;
    uint8_t magicCode;
    bech32::DecodedResult decodedResult;

    txref = "tx1yjk0uqayzu4xx22sy6";
    decodedResult = bech32::decode(txref);
    extractMagicCode(magicCode, decodedResult.dp);
    EXPECT_EQ(magicCode, txref::MAGIC_CODE_MAIN_EXTENDED);

    txref = "txtest18jk0uqayzu4xgj9m8a";
    decodedResult = bech32::decode(txref);
    extractMagicCode(magicCode, decodedResult.dp);
    EXPECT_EQ(magicCode, txref::MAGIC_CODE_TEST_EXTENDED);

}

// check that we can extract the block height from extended txrefs
TEST(TxrefTest, extract_extended_block_height) {
    std::string txref;
    int blockHeight;
    bech32::DecodedResult decodedResult;

    txref = "tx1yqqqqqqqqqqqrvum0c";
    decodedResult = bech32::decode(txref);
    extractBlockHeight(blockHeight, decodedResult.dp);
    EXPECT_EQ(blockHeight, 0);

    txref = "tx1y7llllqqqqqqggjgw6";
    decodedResult = bech32::decode(txref);
    extractBlockHeight(blockHeight, decodedResult.dp);
    EXPECT_EQ(blockHeight, 0xFFFFFF);

    txref = "tx1yjk0uqayzu4xx22sy6";
    decodedResult = bech32::decode(txref);
    extractBlockHeight(blockHeight, decodedResult.dp);
    EXPECT_EQ(blockHeight, 466793);

    txref = "txtest18jk0uqayzu4xgj9m8a";
    decodedResult = bech32::decode(txref);
    extractBlockHeight(blockHeight, decodedResult.dp);
    EXPECT_EQ(blockHeight, 466793);

}

// check that we can extract the transaction position from extended txrefs
TEST(TxrefTest, extract_extended_transaction_position) {
    std::string txref;
    int transactionPosition;
    bech32::DecodedResult decodedResult;

    txref = "tx1yqqqqqqqqqqqrvum0c";
    decodedResult = bech32::decode(txref);
    extractTransactionPosition(transactionPosition, decodedResult.dp);
    EXPECT_EQ(transactionPosition, 0);

    txref = "tx1yqqqqqlllqqqen8x05";
    decodedResult = bech32::decode(txref);
    extractTransactionPosition(transactionPosition, decodedResult.dp);
    EXPECT_EQ(transactionPosition, 0x7FFF);

    txref = "tx1yjk0uqayzu4xx22sy6";
    decodedResult = bech32::decode(txref);
    extractTransactionPosition(transactionPosition, decodedResult.dp);
    EXPECT_EQ(transactionPosition, 2205);

    txref = "txtest18jk0uqayzu4xgj9m8a";
    decodedResult = bech32::decode(txref);
    extractTransactionPosition(transactionPosition, decodedResult.dp);
    EXPECT_EQ(transactionPosition, 2205);

}

// check that we can extract the txo index from extended txrefs
TEST(TxrefTest, extract_extended_txo_index) {
    std::string txref;
    int txoIndex;
    bech32::DecodedResult decodedResult;

    txref = "tx1yqqqqqqqqqqqrvum0c";
    decodedResult = bech32::decode(txref);
    extractTxoIndex(txoIndex, decodedResult.dp);
    EXPECT_EQ(txoIndex, 0);

    txref = "tx1yqqqqqqqqpqqpw4vkq";
    decodedResult = bech32::decode(txref);
    extractTxoIndex(txoIndex, decodedResult.dp);
    EXPECT_EQ(txoIndex, 1);

    txref = "tx1yqqqqqqqqu4xj4f2xe";
    decodedResult = bech32::decode(txref);
    extractTxoIndex(txoIndex, decodedResult.dp);
    EXPECT_EQ(txoIndex, 0x1ABC);

    txref = "tx1yjk0uqayzu4xx22sy6";
    decodedResult = bech32::decode(txref);
    extractTxoIndex(txoIndex, decodedResult.dp);
    EXPECT_EQ(txoIndex, 0x1ABC);

    txref = "txtest18jk0uqayzu4xgj9m8a";
    decodedResult = bech32::decode(txref);
    extractTxoIndex(txoIndex, decodedResult.dp);
    EXPECT_EQ(txoIndex, 0x1ABC);

}

// check that we can add missing standard HRPs if needed
TEST(TxrefTest, txref_add_hrps_extended) {
    std::string txref;

    txref = "yjk0uqayzu4xnk6upc";
    EXPECT_EQ(addHrpIfNeeded(txref), "tx1yjk0uqayzu4xnk6upc");

    txref = "8jk0uqayzu4xaw4hzl";
    EXPECT_EQ(addHrpIfNeeded(txref), "txtest18jk0uqayzu4xaw4hzl");

    txref = "p7lllllllpqqqa0dvp";
    EXPECT_EQ(addHrpIfNeeded(txref), "txrt1p7lllllllpqqqa0dvp");

}

// check that we correctly encode some sample extended txrefs
TEST(TxrefTest, txref_encode_extended_mainnet) {
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_CODE_MAIN_EXTENDED, 0, 0, 0),
              "tx1:yqqq-qqqq-qqqq-rvum-0c");
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_CODE_MAIN_EXTENDED, 0, 0x7FFF, 0),
              "tx1:yqqq-qqll-lqqq-en8x-05");
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_CODE_MAIN_EXTENDED, 0xFFFFFF, 0, 0),
              "tx1:y7ll-llqq-qqqq-ggjg-w6");
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_CODE_MAIN_EXTENDED, 0xFFFFFF, 0x7FFF, 0),
              "tx1:y7ll-llll-lqqq-jhf4-wk");

    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_CODE_MAIN_EXTENDED, 0, 0, 1),
              "tx1:yqqq-qqqq-qpqq-pw4v-kq");
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_CODE_MAIN_EXTENDED, 0, 0x7FFF, 1),
              "tx1:yqqq-qqll-lpqq-m3w3-kv");
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_CODE_MAIN_EXTENDED, 0xFFFFFF, 0, 1),
              "tx1:y7ll-llqq-qpqq-22ml-hz");
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_CODE_MAIN_EXTENDED, 0xFFFFFF, 0x7FFF, 1),
              "tx1:y7ll-llll-lpqq-s4qz-hw");

    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_CODE_MAIN_EXTENDED, 0, 0, 0x1ABC),
              "tx1:yqqq-qqqq-qu4x-j4f2-xe");
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_CODE_MAIN_EXTENDED, 0, 0x7FFF, 0x1ABC),
              "tx1:yqqq-qqll-lu4x-g2jh-x4");
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_CODE_MAIN_EXTENDED, 0xFFFFFF, 0, 0x1ABC),
              "tx1:y7ll-llqq-qu4x-e38e-8m");
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_CODE_MAIN_EXTENDED, 0xFFFFFF, 0x7FFF, 0x1ABC),
              "tx1:y7ll-llll-lu4x-rwuy-8h");

    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_CODE_MAIN_EXTENDED, 466793, 2205, 0x1ABC),
              "tx1:yjk0-uqay-zu4x-x22s-y6");
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_TEST, txref::MAGIC_CODE_TEST_EXTENDED, 466793, 2205, 0x1ABC),
              "txtest1:8jk0-uqay-zu4x-gj9m-8a");
}

RC_GTEST_PROP(TxrefTestRC, checkThatEncodeAndDecodeProduceSameExtendedParameters, ()
) {
    auto height = *rc::gen::inRange(0, MAX_BLOCK_HEIGHT);
    auto pos = *rc::gen::inRange(0, MAX_TRANSACTION_POSITION);
    auto txoIndex = *rc::gen::inRange(0, MAX_TXO_INDEX);

    auto txref = txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_CODE_MAIN_EXTENDED, height, pos, txoIndex);
    auto decodedResult = txref::decode(txref);

    RC_ASSERT(decodedResult.blockHeight == height);
    RC_ASSERT(decodedResult.transactionPosition == pos);
    RC_ASSERT(decodedResult.txoIndex == txoIndex);
}

// //////////////// Examples from BIP-0136 /////////////////////

// check that we correctly encode some sample txrefs from BIP-0136. These may duplicate
// some tests above, but many of the examples in the BIP are present here for reference.
TEST(TxrefTest, txref_encode_bip_examples) {

    // Genesis Coinbase Transaction (Transaction #0 of Block #0):
    EXPECT_EQ(txrefEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_CODE_MAIN, 0, 0),
              "tx1:rqqq-qqqq-qwtv-vjr");

    // Transaction #1 of Block #170:
    EXPECT_EQ(txrefEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_CODE_MAIN, 170, 1),
              "tx1:r52q-qqpq-qpty-cfg");

    // Transaction #1234 of Block #456789, with outpoint 1:
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_CODE_MAIN_EXTENDED, 456789, 1234, 1),
              "tx1:y29u-mqjx-ppqq-sfp2-tt");

    // The following list gives properly encoded mainnet TxRef's
    EXPECT_EQ(txrefEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_CODE_MAIN, 0, 0),
              "tx1:rqqq-qqqq-qwtv-vjr");
    EXPECT_EQ(txrefEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_CODE_MAIN, 0, 0x7FFF),
              "tx1:rqqq-qqll-lj68-7n2");
    EXPECT_EQ(txrefEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_CODE_MAIN, 0xFFFFFF, 0x0),
              "tx1:r7ll-llqq-qats-vx9");
    EXPECT_EQ(txrefEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_CODE_MAIN, 0xFFFFFF, 0x7FFF),
              "tx1:r7ll-llll-lp6m-78v");

    // The following list gives properly encoded testnet TxRef's
    EXPECT_EQ(txrefEncode(txref::BECH32_HRP_TEST, txref::MAGIC_CODE_TEST, 0, 0),
              "txtest1:xqqq-qqqq-qrrd-ksa");
    EXPECT_EQ(txrefEncode(txref::BECH32_HRP_TEST, txref::MAGIC_CODE_TEST, 0, 0x7FFF),
              "txtest1:xqqq-qqll-lljx-y35");
    EXPECT_EQ(txrefEncode(txref::BECH32_HRP_TEST, txref::MAGIC_CODE_TEST, 0xFFFFFF, 0x0),
              "txtest1:x7ll-llqq-qsr3-kym");
    EXPECT_EQ(txrefEncode(txref::BECH32_HRP_TEST, txref::MAGIC_CODE_TEST, 0xFFFFFF, 0x7FFF),
              "txtest1:x7ll-llll-lvj6-y9j");

    // The following list gives valid (sometimes strangely formatted) TxRef's
    EXPECT_EQ(txrefEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_CODE_MAIN, 456789, 1234),
              "tx1:r29u-mqjx-putt-3p0");

    // The following list gives properly encoded mainnet TxRef's with Outpoints
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_CODE_MAIN_EXTENDED, 0, 0, 0),
              "tx1:yqqq-qqqq-qqqq-rvum-0c");
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_CODE_MAIN_EXTENDED, 0, 0x7FFF, 0),
              "tx1:yqqq-qqll-lqqq-en8x-05");
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_CODE_MAIN_EXTENDED, 0xFFFFFF, 0x0, 0),
              "tx1:y7ll-llqq-qqqq-ggjg-w6");
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_CODE_MAIN_EXTENDED, 0xFFFFFF, 0x7FFF, 0),
              "tx1:y7ll-llll-lqqq-jhf4-wk");

    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_CODE_MAIN_EXTENDED, 0, 0, 1),
              "tx1:yqqq-qqqq-qpqq-pw4v-kq");
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_CODE_MAIN_EXTENDED, 0, 0x7FFF, 1),
              "tx1:yqqq-qqll-lpqq-m3w3-kv");
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_CODE_MAIN_EXTENDED, 0xFFFFFF, 0x0, 1),
              "tx1:y7ll-llqq-qpqq-22ml-hz");
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_CODE_MAIN_EXTENDED, 0xFFFFFF, 0x7FFF, 1),
              "tx1:y7ll-llll-lpqq-s4qz-hw");

    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_CODE_MAIN_EXTENDED, 456789, 1234, 1),
              "tx1:y29u-mqjx-ppqq-sfp2-tt");

    // The following list gives properly encoded testnet TxRef's with Outpoints
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_TEST, txref::MAGIC_CODE_TEST_EXTENDED, 0, 0, 0),
              "txtest1:8qqq-qqqq-qqqq-d5ns-vl");
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_TEST, txref::MAGIC_CODE_TEST_EXTENDED, 0, 0x7FFF, 0),
              "txtest1:8qqq-qqll-lqqq-htgd-vn");
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_TEST, txref::MAGIC_CODE_TEST_EXTENDED, 0xFFFFFF, 0x0, 0),
              "txtest1:87ll-llqq-qqqq-xsar-da");
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_TEST, txref::MAGIC_CODE_TEST_EXTENDED, 0xFFFFFF, 0x7FFF, 0),
              "txtest1:87ll-llll-lqqq-u0x7-d3");

    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_TEST, txref::MAGIC_CODE_TEST_EXTENDED, 0, 0, 1),
              "txtest1:8qqq-qqqq-qpqq-0k68-48");
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_TEST, txref::MAGIC_CODE_TEST_EXTENDED, 0, 0x7FFF, 1),
              "txtest1:8qqq-qqll-lpqq-4fp6-4t");
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_TEST, txref::MAGIC_CODE_TEST_EXTENDED, 0xFFFFFF, 0x0, 1),
              "txtest1:87ll-llqq-qpqq-yj55-59");
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_TEST, txref::MAGIC_CODE_TEST_EXTENDED, 0xFFFFFF, 0x7FFF, 1),
              "txtest1:87ll-llll-lpqq-7d0f-5f");

    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_TEST, txref::MAGIC_CODE_TEST_EXTENDED, 456789, 1234, 1),
              "txtest1:829u-mqjx-ppqq-73wp-gv");
}

TEST(TxrefTest, txref_decode_bip_examples) {

    int height, pos, txoIndex;
    txref::DecodedResult decodedResult;
    std::string txref;

    // Genesis Coinbase Transaction (Transaction #0 of Block #0):
    height = 0; pos = 0; txref = "tx1:rqqq-qqqq-qwtv-vjr";
    decodedResult = txref::decode(txref);
    RC_ASSERT(decodedResult.blockHeight == height);
    RC_ASSERT(decodedResult.transactionPosition == pos);

    // Transaction #1 of Block #170:
    height = 170; pos = 1; txref = "tx1:r52q‑qqpq‑qpty‑cfg";
    decodedResult = txref::decode(txref);
    RC_ASSERT(decodedResult.blockHeight == height);
    RC_ASSERT(decodedResult.transactionPosition == pos);

    // Transaction #1234 of Block #456789 with outpoint 1:
    height = 456789; pos = 1234; txoIndex = 1; txref = "tx1:y29u‑mqjx‑ppqq‑sfp2‑tt";
    decodedResult = txref::decode(txref);
    RC_ASSERT(decodedResult.blockHeight == height);
    RC_ASSERT(decodedResult.transactionPosition == pos);
    RC_ASSERT(decodedResult.txoIndex == txoIndex);


    // The following list gives properly encoded mainnet TxRef's
    height = 0; pos = 0; txref = "tx1:rqqq-qqqq-qwtv-vjr";
    decodedResult = txref::decode(txref);
    RC_ASSERT(decodedResult.blockHeight == height);
    RC_ASSERT(decodedResult.transactionPosition == pos);

    height = 0; pos = 0x7FFF; txref = "tx1:rqqq-qqll-lj68-7n2";
    decodedResult = txref::decode(txref);
    RC_ASSERT(decodedResult.blockHeight == height);
    RC_ASSERT(decodedResult.transactionPosition == pos);

    height = 0xFFFFFF; pos = 0x0; txref = "tx1:r7ll-llqq-qats-vx9";
    decodedResult = txref::decode(txref);
    RC_ASSERT(decodedResult.blockHeight == height);
    RC_ASSERT(decodedResult.transactionPosition == pos);

    height = 0xFFFFFF; pos = 0x7FFF; txref = "tx1:r7ll-llll-lp6m-78v";
    decodedResult = txref::decode(txref);
    RC_ASSERT(decodedResult.blockHeight == height);
    RC_ASSERT(decodedResult.transactionPosition == pos);


    // The following list gives properly encoded testnet TxRef's
    height = 0; pos = 0; txref = "txtest1:xqqq-qqqq-qrrd-ksa";
    decodedResult = txref::decode(txref);
    RC_ASSERT(decodedResult.blockHeight == height);
    RC_ASSERT(decodedResult.transactionPosition == pos);

    height = 0; pos = 0x7FFF; txref = "txtest1:xqqq-qqll-lljx-y35";
    decodedResult = txref::decode(txref);
    RC_ASSERT(decodedResult.blockHeight == height);
    RC_ASSERT(decodedResult.transactionPosition == pos);

    height = 0xFFFFFF; pos = 0x0; txref = "txtest1:x7ll-llqq-qsr3-kym";
    decodedResult = txref::decode(txref);
    RC_ASSERT(decodedResult.blockHeight == height);
    RC_ASSERT(decodedResult.transactionPosition == pos);

    height = 0xFFFFFF; pos = 0x7FFF; txref = "txtest1:x7ll-llll-lvj6-y9j";
    decodedResult = txref::decode(txref);
    RC_ASSERT(decodedResult.blockHeight == height);
    RC_ASSERT(decodedResult.transactionPosition == pos);


    // The following list gives valid (sometimes strangely formatted) TxRef's
    height = 456789; pos = 1234; txref = "tx1:r29u-mqjx-putt-3p0";
    decodedResult = txref::decode(txref);
    RC_ASSERT(decodedResult.blockHeight == height);
    RC_ASSERT(decodedResult.transactionPosition == pos);

    txref = "TX1R29UMQJXPUTT3P0";
    decodedResult = txref::decode(txref);
    RC_ASSERT(decodedResult.blockHeight == height);
    RC_ASSERT(decodedResult.transactionPosition == pos);

    txref = "TX1R29umqJX--PUTT----3P0";
    decodedResult = txref::decode(txref);
    RC_ASSERT(decodedResult.blockHeight == height);
    RC_ASSERT(decodedResult.transactionPosition == pos);

    txref = "tx1 r29u mqjx putt 3p0";
    decodedResult = txref::decode(txref);
    RC_ASSERT(decodedResult.blockHeight == height);
    RC_ASSERT(decodedResult.transactionPosition == pos);

    txref = "tx1!r29u/mqj*x-putt^^3p0";
    decodedResult = txref::decode(txref);
    RC_ASSERT(decodedResult.blockHeight == height);
    RC_ASSERT(decodedResult.transactionPosition == pos);

    // The following list gives properly encoded mainnet TxRef's with Outpoints
    height = 0; pos = 0; txoIndex = 0; txref = "tx1:yqqq-qqqq-qqqq-rvum-0c";
    decodedResult = txref::decode(txref);
    RC_ASSERT(decodedResult.blockHeight == height);
    RC_ASSERT(decodedResult.transactionPosition == pos);
    RC_ASSERT(decodedResult.txoIndex == txoIndex);

    height = 0; pos = 0x7FFF; txoIndex = 0; txref = "tx1:yqqq-qqll-lqqq-en8x-05";
    decodedResult = txref::decode(txref);
    RC_ASSERT(decodedResult.blockHeight == height);
    RC_ASSERT(decodedResult.transactionPosition == pos);
    RC_ASSERT(decodedResult.txoIndex == txoIndex);

    height = 0xFFFFFF; pos = 0x0; txoIndex = 0; txref = "tx1:y7ll-llqq-qqqq-ggjg-w6";
    decodedResult = txref::decode(txref);
    RC_ASSERT(decodedResult.blockHeight == height);
    RC_ASSERT(decodedResult.transactionPosition == pos);
    RC_ASSERT(decodedResult.txoIndex == txoIndex);

    height = 0xFFFFFF; pos = 0x7FFF; txoIndex = 0; txref = "tx1:y7ll-llll-lqqq-jhf4-wk";
    decodedResult = txref::decode(txref);
    RC_ASSERT(decodedResult.blockHeight == height);
    RC_ASSERT(decodedResult.transactionPosition == pos);
    RC_ASSERT(decodedResult.txoIndex == txoIndex);


    height = 0; pos = 0; txoIndex = 1; txref = "tx1:yqqq-qqqq-qpqq-pw4v-kq";
    decodedResult = txref::decode(txref);
    RC_ASSERT(decodedResult.blockHeight == height);
    RC_ASSERT(decodedResult.transactionPosition == pos);
    RC_ASSERT(decodedResult.txoIndex == txoIndex);

    height = 0; pos = 0x7FFF; txoIndex = 1; txref = "tx1:yqqq-qqll-lpqq-m3w3-kv";
    decodedResult = txref::decode(txref);
    RC_ASSERT(decodedResult.blockHeight == height);
    RC_ASSERT(decodedResult.transactionPosition == pos);
    RC_ASSERT(decodedResult.txoIndex == txoIndex);

    height = 0xFFFFFF; pos = 0x0; txoIndex = 1; txref = "tx1:y7ll-llqq-qpqq-22ml-hz";
    decodedResult = txref::decode(txref);
    RC_ASSERT(decodedResult.blockHeight == height);
    RC_ASSERT(decodedResult.transactionPosition == pos);
    RC_ASSERT(decodedResult.txoIndex == txoIndex);

    height = 0xFFFFFF; pos = 0x7FFF; txoIndex = 1; txref = "tx1:y7ll-llll-lpqq-s4qz-hw";
    decodedResult = txref::decode(txref);
    RC_ASSERT(decodedResult.blockHeight == height);
    RC_ASSERT(decodedResult.transactionPosition == pos);
    RC_ASSERT(decodedResult.txoIndex == txoIndex);


    height = 456789; pos = 1234; txoIndex = 1; txref = "tx1:y29u-mqjx-ppqq-sfp2-tt";
    decodedResult = txref::decode(txref);
    RC_ASSERT(decodedResult.blockHeight == height);
    RC_ASSERT(decodedResult.transactionPosition == pos);
    RC_ASSERT(decodedResult.txoIndex == txoIndex);


    // The following list gives properly encoded testnet TxRef's with Outpoints
    height = 0; pos = 0; txoIndex = 0; txref = "txtest1:8qqq-qqqq-qqqq-d5ns-vl";
    decodedResult = txref::decode(txref);
    RC_ASSERT(decodedResult.blockHeight == height);
    RC_ASSERT(decodedResult.transactionPosition == pos);
    RC_ASSERT(decodedResult.txoIndex == txoIndex);

    height = 0; pos = 0x7FFF; txoIndex = 0; txref = "txtest1:8qqq-qqll-lqqq-htgd-vn";
    decodedResult = txref::decode(txref);
    RC_ASSERT(decodedResult.blockHeight == height);
    RC_ASSERT(decodedResult.transactionPosition == pos);
    RC_ASSERT(decodedResult.txoIndex == txoIndex);

    height = 0xFFFFFF; pos = 0x0; txoIndex = 0; txref = "txtest1:87ll-llqq-qqqq-xsar-da";
    decodedResult = txref::decode(txref);
    RC_ASSERT(decodedResult.blockHeight == height);
    RC_ASSERT(decodedResult.transactionPosition == pos);
    RC_ASSERT(decodedResult.txoIndex == txoIndex);

    height = 0xFFFFFF; pos = 0x7FFF; txoIndex = 0; txref = "txtest1:87ll-llll-lqqq-u0x7-d3";
    decodedResult = txref::decode(txref);
    RC_ASSERT(decodedResult.blockHeight == height);
    RC_ASSERT(decodedResult.transactionPosition == pos);
    RC_ASSERT(decodedResult.txoIndex == txoIndex);


    height = 0; pos = 0; txoIndex = 1; txref = "txtest1:8qqq-qqqq-qpqq-0k68-48";
    decodedResult = txref::decode(txref);
    RC_ASSERT(decodedResult.blockHeight == height);
    RC_ASSERT(decodedResult.transactionPosition == pos);
    RC_ASSERT(decodedResult.txoIndex == txoIndex);

    height = 0; pos = 0x7FFF; txoIndex = 1; txref = "txtest1:8qqq-qqll-lpqq-4fp6-4t";
    decodedResult = txref::decode(txref);
    RC_ASSERT(decodedResult.blockHeight == height);
    RC_ASSERT(decodedResult.transactionPosition == pos);
    RC_ASSERT(decodedResult.txoIndex == txoIndex);

    height = 0xFFFFFF; pos = 0x0; txoIndex = 1; txref = "txtest1:87ll-llqq-qpqq-yj55-59";
    decodedResult = txref::decode(txref);
    RC_ASSERT(decodedResult.blockHeight == height);
    RC_ASSERT(decodedResult.transactionPosition == pos);
    RC_ASSERT(decodedResult.txoIndex == txoIndex);

    height = 0xFFFFFF; pos = 0x7FFF; txoIndex = 1; txref = "txtest1:87ll-llll-lpqq-7d0f-5f";
    decodedResult = txref::decode(txref);
    RC_ASSERT(decodedResult.blockHeight == height);
    RC_ASSERT(decodedResult.transactionPosition == pos);
    RC_ASSERT(decodedResult.txoIndex == txoIndex);


    height = 456789; pos = 1234; txoIndex = 1; txref = "txtest1:829u-mqjx-ppqq-73wp-gv";
    decodedResult = txref::decode(txref);
    RC_ASSERT(decodedResult.blockHeight == height);
    RC_ASSERT(decodedResult.transactionPosition == pos);
    RC_ASSERT(decodedResult.txoIndex == txoIndex);

}

TEST(ClassifyInputStringTest, test_empty) {
    EXPECT_EQ(classifyInputString(""), InputParam::unknown);
}

TEST(ClassifyInputStringTest, test_random) {
    EXPECT_EQ(classifyInputString("oihjediouhwisdubch"), InputParam::unknown);
}

TEST(ClassifyInputStringTest, test_address) {
    EXPECT_EQ(classifyInputString("17VZNX1SN5NtKa8UQFxwQbFeFc3iqRYhem"), InputParam::address);
    EXPECT_EQ(classifyInputString("3EktnHQD7RiAE6uzMj2ZifT9YgRrkSgzQX"), InputParam::address);
    EXPECT_EQ(classifyInputString("2MzQwSSnBHWHqSAqtTVQ6v47XtaisrJa1Vc"), InputParam::address);
    EXPECT_EQ(classifyInputString("mzgjzyj9i9JyU5zBQyNBZMkm2QNz2MQ3Se"), InputParam::address);
    EXPECT_EQ(classifyInputString("mxgj4vFNNWPdRb45tHoJoVqfahYkc3QYZ4"), InputParam::address);
    EXPECT_EQ(classifyInputString("mxgj4vFNNWPdRb45tHoJoVqfahYk8ec3QYZ4"), InputParam::unknown);
}

TEST(ClassifyInputStringTest, test_bad_address) {
    // too long
    EXPECT_EQ(classifyInputString("17VZNX1SN5NtKa8UQFxwQbFeFc3iqRYhemse"), InputParam::unknown);
    EXPECT_EQ(classifyInputString("3EktnHQD7RiAE6uzMj2ZifT9YgRrkSgzQXdd"), InputParam::unknown);
    EXPECT_EQ(classifyInputString("2MzQwSSnBHWHqSAqtTVQ6v47XtaisrJa1Vcd"), InputParam::unknown);
    EXPECT_EQ(classifyInputString("mzgjzyj9i9JyU5zBQyNBZMkm2QNz2MQ3Sedd"), InputParam::unknown);
    EXPECT_EQ(classifyInputString("mxgj4vFNNWPdRb45tHoJoVqfahYkc3QYZ4dd"), InputParam::unknown);
    EXPECT_EQ(classifyInputString("mxgj4vFNNWPdRb45tHoJoVqfahYk8ec3QYZ4"), InputParam::unknown);
    // too short
    EXPECT_EQ(classifyInputString("17VZNX1SN5NtKa8UQFxwQbFeF"), InputParam::unknown);
    EXPECT_EQ(classifyInputString("3EktnHQD7RiAE6uzMj2ZffT9Y"), InputParam::unknown);
    EXPECT_EQ(classifyInputString("2MzQwSSnBHWHqSAqtTVQ6v47X"), InputParam::unknown);
}

TEST(ClassifyInputStringTest, test_anomolies) {
    // classifyInputString isn't perfect. Here are some examples where it is wrong

    // should be "unknown" since these are too-short bitcoin addresses, but they happen
    // to have the right number of characters after being cleaned of invalid characters
    EXPECT_EQ(classifyInputString("mzgjzyj9i9JyU5zBQyNBZMkm2"), InputParam::txref);
    EXPECT_EQ(classifyInputString("mxgj4vFNNWPdRb45tHoJoVqfa"), InputParam::txref);
}

TEST(ClassifyInputStringTest, test_txid) {
    EXPECT_EQ(classifyInputString("e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855"), InputParam::txid);
    EXPECT_EQ(classifyInputString("e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca4953991b7852b855"), InputParam::unknown);
}

TEST(ClassifyInputStringTest, test_txref) {
    // mainnet
    EXPECT_EQ(classifyInputString("tx1rqqqqqqqqwtvvjr"), InputParam::txref);
    EXPECT_EQ(classifyInputString("rqqqqqqqqwtvvjr"), InputParam::txref);
    // testnet
    EXPECT_EQ(classifyInputString("txtest1xjk0uqayzghlp89"), InputParam::txref);
    EXPECT_EQ(classifyInputString("xjk0uqayzghlp89"), InputParam::txref);
    // regtest
    EXPECT_EQ(classifyInputString("txrt1q7lllllllps4p3p"), InputParam::txref);
    EXPECT_EQ(classifyInputString("q7lllllllps4p3p"), InputParam::txref);
}

TEST(ClassifyInputStringTest, test_txrefext) {
    // mainnet
    EXPECT_EQ(classifyInputString("tx1yqqqqqqqqqqqrvum0c"), InputParam::txrefext);
    EXPECT_EQ(classifyInputString("yqqqqqqqqqqqrvum0c"), InputParam::txrefext);
    // testnet
    EXPECT_EQ(classifyInputString("txtest18jk0uqayzu4xaw4hzl"), InputParam::txrefext);
    EXPECT_EQ(classifyInputString("8jk0uqayzu4xaw4hzl"), InputParam::txrefext);
    // regtest
    EXPECT_EQ(classifyInputString("txrt1p7lllllllpqqqa0dvp"), InputParam::txrefext);
    EXPECT_EQ(classifyInputString("p7lllllllpqqqa0dvp"), InputParam::txrefext);
}

TEST(TxrefTest, containsUppercaseCharacters) {
    EXPECT_FALSE(cleanTxrefContainsUppercaseCharacters("test"));
    EXPECT_TRUE(cleanTxrefContainsUppercaseCharacters("TEST"));
    EXPECT_TRUE(cleanTxrefContainsUppercaseCharacters("Test"));
    EXPECT_FALSE(cleanTxrefContainsUppercaseCharacters("123abc"));
}

TEST(TxrefTest, containsLowercaseCharacters) {
    EXPECT_TRUE(cleanTxrefContainsLowercaseCharacters("test"));
    EXPECT_FALSE(cleanTxrefContainsLowercaseCharacters("TEST"));
    EXPECT_TRUE(cleanTxrefContainsLowercaseCharacters("Test"));
    EXPECT_FALSE(cleanTxrefContainsLowercaseCharacters("123ABC"));
}

TEST(TxrefTest, containsMixedcaseCharacters) {
    EXPECT_FALSE(cleanTxrefContainsMixedcaseCharacters("test"));
    EXPECT_FALSE(cleanTxrefContainsMixedcaseCharacters("TEST"));
    EXPECT_TRUE(cleanTxrefContainsMixedcaseCharacters("Test"));
    EXPECT_FALSE(cleanTxrefContainsMixedcaseCharacters("123"));
    EXPECT_FALSE(cleanTxrefContainsMixedcaseCharacters("123a"));
    EXPECT_TRUE(cleanTxrefContainsMixedcaseCharacters("A123a"));
}

TEST(TxrefTest, txref_encode_testnet) {
    EXPECT_EQ(txrefEncode(txref::BECH32_HRP_TEST, txref::MAGIC_CODE_TEST, 0, 0),
              "txtest1:xqqq-qqqq-qrrd-ksa");
    EXPECT_EQ(txrefEncode(txref::BECH32_HRP_TEST, txref::MAGIC_CODE_TEST, 0, 0x7FFF),
              "txtest1:xqqq-qqll-lljx-y35");
    EXPECT_EQ(txrefEncode(txref::BECH32_HRP_TEST, txref::MAGIC_CODE_TEST, 0xFFFFFF, 0),
              "txtest1:x7ll-llqq-qsr3-kym");
    EXPECT_EQ(txrefEncode(txref::BECH32_HRP_TEST, txref::MAGIC_CODE_TEST, 0xFFFFFF, 0x7FFF),
              "txtest1:x7ll-llll-lvj6-y9j");
}

TEST(TxrefTest, txref_encode_regtest) {
    EXPECT_EQ(txrefEncode(txref::BECH32_HRP_REGTEST, txref::MAGIC_CODE_REGTEST, 0, 0),
              "txrt1:qqqq-qqqq-qwpz-nyw");
    EXPECT_EQ(txrefEncode(txref::BECH32_HRP_REGTEST, txref::MAGIC_CODE_REGTEST, 0, 0x7FFF),
              "txrt1:qqqq-qqll-ljsf-p98");
    EXPECT_EQ(txrefEncode(txref::BECH32_HRP_REGTEST, txref::MAGIC_CODE_REGTEST, 0xFFFFFF, 0),
              "txrt1:q7ll-llqq-qap7-nsg");
    EXPECT_EQ(txrefEncode(txref::BECH32_HRP_REGTEST, txref::MAGIC_CODE_REGTEST, 0xFFFFFF, 0x7FFF),
              "txrt1:q7ll-llll-lps4-p3p");
}

TEST(TxrefTest, txref_encode_extended_testnet) {
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_TEST, txref::MAGIC_CODE_TEST_EXTENDED, 0, 0, 1),
              "txtest1:8qqq-qqqq-qpqq-0k68-48");
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_TEST, txref::MAGIC_CODE_TEST_EXTENDED, 0, 0x7FFF, 1),
              "txtest1:8qqq-qqll-lpqq-4fp6-4t");
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_TEST, txref::MAGIC_CODE_TEST_EXTENDED, 0xFFFFFF, 0, 1),
              "txtest1:87ll-llqq-qpqq-yj55-59");
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_TEST, txref::MAGIC_CODE_TEST_EXTENDED, 0xFFFFFF, 0x7FFF, 1),
              "txtest1:87ll-llll-lpqq-7d0f-5f");
}

TEST(TxrefTest, txref_encode_extended_regtest) {
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_REGTEST, txref::MAGIC_CODE_REGTEST_EXTENDED, 0, 0, 1),
              "txrt1:pqqq-qqqq-qpqq-3x6r-d0");
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_REGTEST, txref::MAGIC_CODE_REGTEST_EXTENDED, 0, 0x7FFF, 1),
              "txrt1:pqqq-qqll-lpqq-tep7-dr");
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_REGTEST, txref::MAGIC_CODE_REGTEST_EXTENDED, 0xFFFFFF, 0, 1),
              "txrt1:p7ll-llqq-qpqq-6z5s-vd");
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_REGTEST, txref::MAGIC_CODE_REGTEST_EXTENDED, 0xFFFFFF, 0x7FFF, 1),
              "txrt1:p7ll-llll-lpqq-qa0d-vp");
}
