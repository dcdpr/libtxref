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

#include "libtxref.h"

// In this "API" test file, we should only be referring to symbols in the "txref" namespace.

//check that we correctly decode txrefs for mainnet, testnet and regtest
TEST(TxrefApiTest, txref_decode) {
    std::string txref;
    txref::DecodedResult decodedResult;

    txref = "tx1:rqqq-qqqq-qwtv-vjr";
    decodedResult = txref::decode(txref);
    EXPECT_EQ(decodedResult.hrp, "tx");
    EXPECT_EQ(decodedResult.magicCode, txref::MAGIC_CODE_MAIN);
    EXPECT_EQ(decodedResult.blockHeight, 0);
    EXPECT_EQ(decodedResult.transactionPosition, 0);
    EXPECT_EQ(decodedResult.txoIndex, 0);

    txref = "tx1:rqqq-qqll-lj68-7n2";
    decodedResult = txref::decode(txref);
    EXPECT_EQ(decodedResult.hrp, "tx");
    EXPECT_EQ(decodedResult.magicCode, txref::MAGIC_CODE_MAIN);
    EXPECT_EQ(decodedResult.blockHeight, 0);
    EXPECT_EQ(decodedResult.transactionPosition, 0x7FFF);
    EXPECT_EQ(decodedResult.txoIndex, 0);

    txref = "tx1:r7ll-llqq-qats-vx9";
    decodedResult = txref::decode(txref);
    EXPECT_EQ(decodedResult.hrp, "tx");
    EXPECT_EQ(decodedResult.magicCode, txref::MAGIC_CODE_MAIN);
    EXPECT_EQ(decodedResult.blockHeight, 0xFFFFFF);
    EXPECT_EQ(decodedResult.transactionPosition, 0);
    EXPECT_EQ(decodedResult.txoIndex, 0);

    txref = "tx1:r7ll-llll-lp6m-78v";
    decodedResult = txref::decode(txref);
    EXPECT_EQ(decodedResult.hrp, "tx");
    EXPECT_EQ(decodedResult.magicCode, txref::MAGIC_CODE_MAIN);
    EXPECT_EQ(decodedResult.blockHeight, 0xFFFFFF);
    EXPECT_EQ(decodedResult.transactionPosition, 0x7FFF);
    EXPECT_EQ(decodedResult.txoIndex, 0);

    txref = "tx1:rjk0-uqay-z9l7-m9m";
    decodedResult = txref::decode(txref);
    EXPECT_EQ(decodedResult.hrp, "tx");
    EXPECT_EQ(decodedResult.magicCode, txref::MAGIC_CODE_MAIN);
    EXPECT_EQ(decodedResult.blockHeight, 466793);
    EXPECT_EQ(decodedResult.transactionPosition, 2205);
    EXPECT_EQ(decodedResult.txoIndex, 0);

    txref = "txtest1:xjk0-uqay-zghl-p89";
    decodedResult = txref::decode(txref);
    EXPECT_EQ(decodedResult.hrp, "txtest");
    EXPECT_EQ(decodedResult.magicCode, txref::MAGIC_CODE_TEST);
    EXPECT_EQ(decodedResult.blockHeight, 466793);
    EXPECT_EQ(decodedResult.transactionPosition, 2205);
    EXPECT_EQ(decodedResult.txoIndex, 0);

    txref = "txrt1:q7ll-llll-lps4-p3p";
    decodedResult = txref::decode(txref);
    EXPECT_EQ(decodedResult.hrp, "txrt");
    EXPECT_EQ(decodedResult.magicCode, txref::MAGIC_CODE_REGTEST);
    EXPECT_EQ(decodedResult.blockHeight, 0xFFFFFF);
    EXPECT_EQ(decodedResult.transactionPosition, 0x7FFF);
    EXPECT_EQ(decodedResult.txoIndex, 0);
}

// check that we can deal with weird txref formatting (invalid characters)
TEST(TxrefApiTest, txref_decode_weird_formatting) {
    std::string txref;
    txref::DecodedResult decodedResult;

    txref = "tx1---rqqq-<qqqq>-q>w<tv-v#j#r--";
    decodedResult = txref::decode(txref);
    EXPECT_EQ(decodedResult.hrp, "tx");
    EXPECT_EQ(decodedResult.magicCode, txref::MAGIC_CODE_MAIN);
    EXPECT_EQ(decodedResult.blockHeight, 0);
    EXPECT_EQ(decodedResult.transactionPosition, 0);
    EXPECT_EQ(decodedResult.txoIndex, 0);
    EXPECT_EQ(decodedResult.txref, "tx1:rqqq-qqqq-qwtv-vjr");

    txref = "tx1-rqqq qqqq qwtv vjr";
    decodedResult = txref::decode(txref);
    EXPECT_EQ(decodedResult.hrp, "tx");
    EXPECT_EQ(decodedResult.magicCode, txref::MAGIC_CODE_MAIN);
    EXPECT_EQ(decodedResult.blockHeight, 0);
    EXPECT_EQ(decodedResult.transactionPosition, 0);
    EXPECT_EQ(decodedResult.txoIndex, 0);
    EXPECT_EQ(decodedResult.txref, "tx1:rqqq-qqqq-qwtv-vjr");

    txref = "tx1rqqq,qqqq.qwtv.vjr";
    decodedResult = txref::decode(txref);
    EXPECT_EQ(decodedResult.hrp, "tx");
    EXPECT_EQ(decodedResult.magicCode, txref::MAGIC_CODE_MAIN);
    EXPECT_EQ(decodedResult.blockHeight, 0);
    EXPECT_EQ(decodedResult.transactionPosition, 0);
    EXPECT_EQ(decodedResult.txoIndex, 0);
    EXPECT_EQ(decodedResult.txref, "tx1:rqqq-qqqq-qwtv-vjr");

    txref = "tx@test1-xj$$k0-uq@@ay---zghl-p%$8 9";
    decodedResult = txref::decode(txref);
    EXPECT_EQ(decodedResult.hrp, "txtest");
    EXPECT_EQ(decodedResult.magicCode, txref::MAGIC_CODE_TEST);
    EXPECT_EQ(decodedResult.blockHeight, 466793);
    EXPECT_EQ(decodedResult.transactionPosition, 2205);
    EXPECT_EQ(decodedResult.txoIndex, 0);
    EXPECT_EQ(decodedResult.txref, "txtest1:xjk0-uqay-zghl-p89");
}


// check that we can deal with missing HRPs at the start of the txref
TEST(TxrefApiTest, txref_decode_no_HRPs) {
    std::string txref;
    txref::DecodedResult decodedResult;

    txref = "rqqq-qqqq-qwtv-vjr";
    decodedResult = txref::decode(txref);
    EXPECT_EQ(decodedResult.hrp, "tx");
    EXPECT_EQ(decodedResult.magicCode, txref::MAGIC_CODE_MAIN);
    EXPECT_EQ(decodedResult.blockHeight, 0);
    EXPECT_EQ(decodedResult.transactionPosition, 0);
    EXPECT_EQ(decodedResult.txoIndex, 0);
    EXPECT_EQ(decodedResult.txref, "tx1:rqqq-qqqq-qwtv-vjr");

    txref = "xjk0-uqay-zghl-p89";
    decodedResult = txref::decode(txref);
    EXPECT_EQ(decodedResult.hrp, "txtest");
    EXPECT_EQ(decodedResult.magicCode, txref::MAGIC_CODE_TEST);
    EXPECT_EQ(decodedResult.blockHeight, 466793);
    EXPECT_EQ(decodedResult.transactionPosition, 2205);
    EXPECT_EQ(decodedResult.txoIndex, 0);
    EXPECT_EQ(decodedResult.txref, "txtest1:xjk0-uqay-zghl-p89");

    txref = "q7ll-llll-lps4-p3p";
    decodedResult = txref::decode(txref);
    EXPECT_EQ(decodedResult.hrp, "txrt");
    EXPECT_EQ(decodedResult.magicCode, txref::MAGIC_CODE_REGTEST);
    EXPECT_EQ(decodedResult.blockHeight, 0xFFFFFF);
    EXPECT_EQ(decodedResult.transactionPosition, 0x7FFF);
    EXPECT_EQ(decodedResult.txoIndex, 0);
    EXPECT_EQ(decodedResult.txref, "txrt1:q7ll-llll-lps4-p3p");
}


// check that we correctly decode extended txrefs for mainnet, testnet and regtest
TEST(TxrefApiTest, txref_extended_decode) {
    std::string txref;
    txref::DecodedResult decodedResult;

    txref = "tx1:yqqq-qqqq-qqqq-rvum-0c";
    decodedResult = txref::decode(txref);
    EXPECT_EQ(decodedResult.hrp, "tx");
    EXPECT_EQ(decodedResult.magicCode, txref::MAGIC_CODE_MAIN_EXTENDED);
    EXPECT_EQ(decodedResult.blockHeight, 0);
    EXPECT_EQ(decodedResult.transactionPosition, 0);
    EXPECT_EQ(decodedResult.txoIndex, 0);

    txref = "tx1:y7ll-llqq-qqqq-ggjg-w6";
    decodedResult = txref::decode(txref);
    EXPECT_EQ(decodedResult.hrp, "tx");
    EXPECT_EQ(decodedResult.magicCode, txref::MAGIC_CODE_MAIN_EXTENDED);
    EXPECT_EQ(decodedResult.blockHeight, 0xFFFFFF);
    EXPECT_EQ(decodedResult.transactionPosition, 0);
    EXPECT_EQ(decodedResult.txoIndex, 0);

    txref = "tx1:yjk0-uqay-zu4x-x22s-y6";
    decodedResult = txref::decode(txref);
    EXPECT_EQ(decodedResult.hrp, "tx");
    EXPECT_EQ(decodedResult.magicCode, txref::MAGIC_CODE_MAIN_EXTENDED);
    EXPECT_EQ(decodedResult.blockHeight, 466793);
    EXPECT_EQ(decodedResult.transactionPosition, 2205);
    EXPECT_EQ(decodedResult.txoIndex, 0x1ABC);

    txref = "txtest1:8jk0-uqay-zu4x-gj9m-8a";
    decodedResult = txref::decode(txref);
    EXPECT_EQ(decodedResult.hrp, "txtest");
    EXPECT_EQ(decodedResult.magicCode, txref::MAGIC_CODE_TEST_EXTENDED);
    EXPECT_EQ(decodedResult.blockHeight, 466793);
    EXPECT_EQ(decodedResult.transactionPosition, 2205);
    EXPECT_EQ(decodedResult.txoIndex, 0x1ABC);

    txref = "txrt1:p7ll-llll-lpqq-qa0d-vp";
    decodedResult = txref::decode(txref);
    EXPECT_EQ(decodedResult.hrp, "txrt");
    EXPECT_EQ(decodedResult.magicCode, txref::MAGIC_CODE_REGTEST_EXTENDED);
    EXPECT_EQ(decodedResult.blockHeight, 0xFFFFFF);
    EXPECT_EQ(decodedResult.transactionPosition, 0x7FFF);
    EXPECT_EQ(decodedResult.txoIndex, 1);
}

TEST(TxrefApiTest, txref_decode_check_encoding) {
    std::string txref;
    txref::DecodedResult decodedResult;

    txref = "txtest1:8jk0-uqay-zu4x-gj9m-8a";
    decodedResult = txref::decode(txref);
    EXPECT_EQ(decodedResult.encoding, txref::Encoding::Bech32m);

    txref = "txtest1:8jk0-uqay-zu4x-aw4h-zl";
    decodedResult = txref::decode(txref);
    EXPECT_EQ(decodedResult.encoding, txref::Encoding::Bech32);

}

TEST(TxrefApiTest, txref_decode_check_commentary_old_encoding) {
    std::string txref;
    txref::DecodedResult decodedResult;

    txref = "txtest1:8jk0-uqay-zu4x-gj9m-8a";
    decodedResult = txref::decode(txref);
    EXPECT_EQ(decodedResult.commentary, "");

    txref = "txtest1:8jk0-uqay-zu4x-aw4h-zl";
    decodedResult = txref::decode(txref);
    EXPECT_NE(decodedResult.commentary, "");
    EXPECT_TRUE(decodedResult.commentary.find("txtest1:8jk0-uqay-zu4x-gj9m-8a") != std::string::npos);
}

TEST(TxrefApiTest, txref_decode_check_commentary_mixedcase) {
    std::string txref;
    txref::DecodedResult decodedResult;

    txref = "txtest1:8jk0-uQay-zu4x-gj9m-8a";
    decodedResult = txref::decode(txref);
    EXPECT_NE(decodedResult.commentary, "");
    EXPECT_TRUE(decodedResult.commentary.find("txtest1:8jk0-uqay-zu4x-gj9m-8a") != std::string::npos);
}

//check that we correctly decode extended txrefs even with missing HRPs and weird formatting
TEST(TxrefApiTest, txref_extended_decode_weird) {
    std::string txref;
    txref::DecodedResult decodedResult;

    txref = "y q#q q- # q q q# %q   -#q % q qq# rv (um-)  0c";
    decodedResult = txref::decode(txref);
    EXPECT_EQ(decodedResult.hrp, "tx");
    EXPECT_EQ(decodedResult.magicCode, txref::MAGIC_CODE_MAIN_EXTENDED);
    EXPECT_EQ(decodedResult.blockHeight, 0);
    EXPECT_EQ(decodedResult.transactionPosition, 0);
    EXPECT_EQ(decodedResult.txoIndex, 0);

    txref = "y 7 l l - l l q q - q q q q - gg  j g - w  6   ";
    decodedResult = txref::decode(txref);
    EXPECT_EQ(decodedResult.hrp, "tx");
    EXPECT_EQ(decodedResult.magicCode, txref::MAGIC_CODE_MAIN_EXTENDED);
    EXPECT_EQ(decodedResult.blockHeight, 0xFFFFFF);
    EXPECT_EQ(decodedResult.transactionPosition, 0);
    EXPECT_EQ(decodedResult.txoIndex, 0);

    txref = "y#$%jk0$%^-u$%^&qa^&*(y-#$%z^&*u4x^&*-x2%^&*(2s-^&*(y6";
    decodedResult = txref::decode(txref);
    EXPECT_EQ(decodedResult.hrp, "tx");
    EXPECT_EQ(decodedResult.magicCode, txref::MAGIC_CODE_MAIN_EXTENDED);
    EXPECT_EQ(decodedResult.blockHeight, 466793);
    EXPECT_EQ(decodedResult.transactionPosition, 2205);
    EXPECT_EQ(decodedResult.txoIndex, 0x1ABC);

    txref = "8jk.0.-.u.q.a..y.-..z.u..4.x..-..g.j.9.m.-.8....a..";
    decodedResult = txref::decode(txref);
    EXPECT_EQ(decodedResult.hrp, "txtest");
    EXPECT_EQ(decodedResult.magicCode, txref::MAGIC_CODE_TEST_EXTENDED);
    EXPECT_EQ(decodedResult.blockHeight, 466793);
    EXPECT_EQ(decodedResult.transactionPosition, 2205);
    EXPECT_EQ(decodedResult.txoIndex, 0x1ABC);
}

// check that we correctly encode txrefs for mainnet, testnet and regtest
TEST(TxrefApiTest, txref_encode) {
    EXPECT_EQ(txref::encode(0, 0),
              "tx1:rqqq-qqqq-qwtv-vjr");
    EXPECT_EQ(txref::encode(0, 0x7FFF),
              "tx1:rqqq-qqll-lj68-7n2");
    EXPECT_EQ(txref::encode(0xFFFFFF, 0),
              "tx1:r7ll-llqq-qats-vx9");
    EXPECT_EQ(txref::encode(0xFFFFFF, 0x7FFF),
              "tx1:r7ll-llll-lp6m-78v");
    EXPECT_EQ(txref::encode(466793, 2205),
              "tx1:rjk0-uqay-z9l7-m9m");
    EXPECT_EQ(txref::encodeTestnet(0, 0),
              "txtest1:xqqq-qqqq-qrrd-ksa");
    EXPECT_EQ(txref::encodeTestnet(0xFFFFFF, 0x7FFF),
              "txtest1:x7ll-llll-lvj6-y9j");
    EXPECT_EQ(txref::encodeTestnet(467883, 2355),
              "txtest1:xk63-uqnf-zgve-zdz");
    EXPECT_EQ(txref::encodeRegtest(0, 0),
              "txrt1:qqqq-qqqq-qwpz-nyw");
    EXPECT_EQ(txref::encodeRegtest(0xFFFFFF, 0x7FFF),
              "txrt1:q7ll-llll-lps4-p3p");
}

// check that we correctly encode extended txrefs for mainnet, testnet and regtest
TEST(TxrefApiTest, txref_extended_encode) {

    EXPECT_EQ(txref::encode(0, 0, 100),
              "tx1:yqqq-qqqq-qyrq-62qj-df");

    EXPECT_EQ(txref::encode(0, 0, 0x1FFF),
              "tx1:yqqq-qqqq-qll8-5gma-9l");

    EXPECT_EQ(txref::encode(0, 0x1FFF, 100),
              "tx1:yqqq-qqll-8yrq-03w7-ne");

    EXPECT_EQ(txref::encode(0x1FFFFF, 0, 200),
              "tx1:y7ll-lrqq-qgxq-l3gu-ek");

    EXPECT_EQ(txref::encode(0x1FFFFF, 0x1FFF, 0x1FFF),
              "tx1:y7ll-lrll-8ll8-xzsy-f2");

    EXPECT_EQ(txref::encode(466793, 2205, 10),
              "tx1:yjk0-uqay-z2qq-r8h6-pa");

    EXPECT_EQ(txref::encode(466793, 2205, 0x1FFF),
              "tx1:yjk0-uqay-zll8-qhc8-8u");

    EXPECT_EQ(txref::encodeTestnet(0, 0, 100),
              "txtest1:8qqq-qqqq-qyrq-5j0e-ww");

    EXPECT_EQ(txref::encodeTestnet(0, 0, 0x1FFF),
              "txtest1:8qqq-qqqq-qll8-6s5k-xc");

    EXPECT_EQ(txref::encodeTestnet(0, 0x7FFF, 100),
              "txtest1:8qqq-qqll-lyrq-wd5y-wz");

    EXPECT_EQ(txref::encodeTestnet(0xFFFFFF, 0, 200),
              "txtest1:87ll-llqq-qgxq-auv3-fk");

    EXPECT_EQ(txref::encodeTestnet(0xFFFFFF, 0x7FFF, 0x1FFF),
              "txtest1:87ll-llll-lll8-ttpc-8k");

    EXPECT_EQ(txref::encodeTestnet(466793, 2205, 10),
              "txtest1:8jk0-uqay-z2qq-dlc3-z6");

    EXPECT_EQ(txref::encodeTestnet(466793, 2205, 0x1FFF),
              "txtest1:8jk0-uqay-zll8-w0hv-ym");

    EXPECT_EQ(txref::encodeRegtest(0, 0, 100),
              "txrt1:pqqq-qqqq-qyrq-2z0a-kx");

    EXPECT_EQ(txref::encodeRegtest(0, 0, 0x1FFF),
              "txrt1:pqqq-qqqq-qll8-yq5j-7s");
}

// check that we return regular txref for txoIndex=0, unless forceExtended is true
TEST(TxrefApiTest, txref_extended_encode_force_zero) {

    EXPECT_EQ(txref::encode(0, 0, 0),
              "tx1:rqqq-qqqq-qwtv-vjr");
    EXPECT_EQ(txref::encode(0, 0, 0, true),
              "tx1:yqqq-qqqq-qqqq-rvum-0c");

    EXPECT_EQ(txref::encode(0, 0x1FFF, 0),
              "tx1:rqqq-qqll-8e0t-2cy");
    EXPECT_EQ(txref::encode(0, 0x1FFF, 0, true),
              "tx1:yqqq-qqll-8qqq-khjh-3g");

    EXPECT_EQ(txref::encode(0x1FFFFF, 0, 0),
              "tx1:r7ll-lrqq-qyk0-way");
    EXPECT_EQ(txref::encode(0x1FFFFF, 0, 0, true),
              "tx1:y7ll-lrqq-qqqq-yaew-aa");

    EXPECT_EQ(txref::encode(0x1FFFFF, 0x1FFF, 0),
              "tx1:r7ll-lrll-8njg-ghr");
    EXPECT_EQ(txref::encode(0x1FFFFF, 0x1FFF, 0, true),
              "tx1:y7ll-lrll-8qqq-3xhz-rd");

    EXPECT_EQ(txref::encode(466793, 2205, 0),
              "tx1:rjk0-uqay-z9l7-m9m");
    EXPECT_EQ(txref::encode(466793, 2205, 0, true),
              "tx1:yjk0-uqay-zqqq-hnlp-dm");

    EXPECT_EQ(txref::encodeTestnet(0, 0, 0),
              "txtest1:xqqq-qqqq-qrrd-ksa");
    EXPECT_EQ(txref::encodeTestnet(0, 0, 0, true),
              "txtest1:8qqq-qqqq-qqqq-d5ns-vl");

    EXPECT_EQ(txref::encodeTestnet(0, 0x7FFF, 0),
              "txtest1:xqqq-qqll-lljx-y35");
    EXPECT_EQ(txref::encodeTestnet(0, 0x7FFF, 0, true),
              "txtest1:8qqq-qqll-lqqq-htgd-vn");

    EXPECT_EQ(txref::encodeTestnet(0xFFFFFF, 0, 0),
              "txtest1:x7ll-llqq-qsr3-kym");
    EXPECT_EQ(txref::encodeTestnet(0xFFFFFF, 0, 0, true),
              "txtest1:87ll-llqq-qqqq-xsar-da");

    EXPECT_EQ(txref::encodeTestnet(0xFFFFFF, 0x7FFF, 0),
              "txtest1:x7ll-llll-lvj6-y9j");
    EXPECT_EQ(txref::encodeTestnet(0xFFFFFF, 0x7FFF, 0, true),
              "txtest1:87ll-llll-lqqq-u0x7-d3");

    EXPECT_EQ(txref::encodeTestnet(466793, 2205, 0),
              "txtest1:xjk0-uqay-zghl-p89");
    EXPECT_EQ(txref::encodeTestnet(466793, 2205, 0, true),
              "txtest1:8jk0-uqay-zqqq-ets2-wu");

    EXPECT_EQ(txref::encodeRegtest(0, 0x7FFF, 0),
              "txrt1:qqqq-qqll-ljsf-p98");
    EXPECT_EQ(txref::encodeRegtest(0, 0x7FFF, 0, true),
              "txrt1:pqqq-qqll-lqqq-fmgf-5m");

    EXPECT_EQ(txref::encodeRegtest(0xFFFFFF, 0, 0),
              "txrt1:q7ll-llqq-qap7-nsg");
    EXPECT_EQ(txref::encodeRegtest(0xFFFFFF, 0, 0, true),
              "txrt1:p7ll-llqq-qqqq-cqa8-44");
}

RC_GTEST_PROP(TxrefApiTestRC, checkThatEncodeAndDecodeProduceSameParameters, ()
) {
    auto height = *rc::gen::inRange(0, 0xFFFFFF); // MAX_BLOCK_HEIGHT
    auto pos = *rc::gen::inRange(0, 0x7FFF); // MAX_TRANSACTION_POSITION

    auto txref = txref::encode(height, pos);
    auto decodedResult = txref::decode(txref);

    RC_ASSERT(decodedResult.blockHeight == height);
    RC_ASSERT(decodedResult.transactionPosition == pos);
}

RC_GTEST_PROP(TxrefApiTestRC, checkThatEncodeAndDecodeTestnetProduceSameParameters, ()
) {
    auto height = *rc::gen::inRange(0, 0xFFFFFF); // MAX_BLOCK_HEIGHT
    auto pos = *rc::gen::inRange(0, 0x7FFF); // MAX_TRANSACTION_POSITION

    auto txref = txref::encodeTestnet(height, pos);
    auto decodedResult = txref::decode(txref);

    RC_ASSERT(decodedResult.blockHeight == height);
    RC_ASSERT(decodedResult.transactionPosition == pos);
}

RC_GTEST_PROP(TxrefApiTestRC, checkThatEncodeAndDecodeRegtestProduceSameParameters, ()
) {
    auto height = *rc::gen::inRange(0, 0xFFFFFF); // MAX_BLOCK_HEIGHT
    auto pos = *rc::gen::inRange(0, 0x7FFF); // MAX_TRANSACTION_POSITION

    auto txref = txref::encodeRegtest(height, pos);
    auto decodedResult = txref::decode(txref);

    RC_ASSERT(decodedResult.blockHeight == height);
    RC_ASSERT(decodedResult.transactionPosition == pos);
}

RC_GTEST_PROP(TxrefApiTestRC, checkThatExtendedEncodeAndDecodeProduceSameParameters, ()
) {
    auto height = *rc::gen::inRange(0, 0xFFFFFF); // MAX_BLOCK_HEIGHT
    auto pos = *rc::gen::inRange(0, 0x7FFF); // MAX_TRANSACTION_POSITION
    auto index = *rc::gen::inRange(0, 0x7FFF); // MAX_TXO_INDEX

    auto txref = txref::encode(height, pos, index);
    auto decodedResult = txref::decode(txref);

    RC_ASSERT(decodedResult.blockHeight == height);
    RC_ASSERT(decodedResult.transactionPosition == pos);
    RC_ASSERT(decodedResult.txoIndex == index);
}

RC_GTEST_PROP(TxrefApiTestRC, checkThatExtendedEncodeAndDecodeTestnetProduceSameParameters, ()
) {
    auto height = *rc::gen::inRange(0, 0xFFFFFF); // MAX_BLOCK_HEIGHT
    auto pos = *rc::gen::inRange(0, 0x7FFF); // MAX_TRANSACTION_POSITION
    auto index = *rc::gen::inRange(0, 0x7FFF); // MAX_TXO_INDEX

    auto txref = txref::encodeTestnet(height, pos, index, true);
    auto decodedResult = txref::decode(txref);

    RC_ASSERT(decodedResult.blockHeight == height);
    RC_ASSERT(decodedResult.transactionPosition == pos);
    RC_ASSERT(decodedResult.txoIndex == index);
}

RC_GTEST_PROP(TxrefApiTestRC, checkThatExtendedEncodeAndDecodeRegtestProduceSameParameters, ()
) {
    auto height = *rc::gen::inRange(0, 0xFFFFFF); // MAX_BLOCK_HEIGHT
    auto pos = *rc::gen::inRange(0, 0x7FFF); // MAX_TRANSACTION_POSITION
    auto index = *rc::gen::inRange(0, 0x7FFF); // MAX_TXO_INDEX

    auto txref = txref::encodeRegtest(height, pos, index, true);
    auto decodedResult = txref::decode(txref);

    RC_ASSERT(decodedResult.blockHeight == height);
    RC_ASSERT(decodedResult.transactionPosition == pos);
    RC_ASSERT(decodedResult.txoIndex == index);
}

