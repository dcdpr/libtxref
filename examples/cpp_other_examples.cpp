#include "txref.h"
#include <string>
#include <iostream>

// make sure we can check these examples even when building a release version
#undef NDEBUG
#include <cassert>


void encodingExamples() {

    int blockHeight, transactionIndex, txoIndex;
    std::string txref;


    // mainnet, standard txref example

    blockHeight = 10000;
    transactionIndex = 2;

    txref = txref::encode(blockHeight, transactionIndex);

    std::cout << "mainnet, standard txref for (blockHeight = 10000, transactionIndex=2):" << std::endl;
    std::cout << txref << "\n\n";
    assert(txref == "tx1:rq3n-qqzq-qk8k-mzd");


    // testnet, standard txref example

    blockHeight = 10000;
    transactionIndex = 4;

    txref = txref::encodeTestnet(blockHeight, transactionIndex);

    std::cout << "testnet, standard txref for (blockHeight = 10000, transactionIndex=4):" << std::endl;
    std::cout << txref << "\n\n";
    assert(txref == "txtest1:xq3n-qqyq-qhrg-gy3");


    // mainnet, extended txref example

    blockHeight = 10000;
    transactionIndex = 2;
    txoIndex = 3;

    txref = txref::encode(blockHeight, transactionIndex, txoIndex);

    std::cout << "mainnet, extended txref for (blockHeight = 10000, transactionIndex=2, txoIndex=3):" << std::endl;
    std::cout << txref << "\n\n";
    assert(txref == "tx1:yq3n-qqzq-qrqq-9z4d-2n");


    // testnet, extended txref example

    blockHeight = 10000;
    transactionIndex  = 4;
    txoIndex = 6;

    txref = txref::encodeTestnet(blockHeight, transactionIndex, txoIndex);

    std::cout << "testnet, extended txref for (blockHeight = 10000, transactionIndex=4, txoIndex=6):" << std::endl;
    std::cout << txref << "\n\n";
    assert(txref == "txtest1:8q3n-qqyq-qxqq-v3x4-ze");

}

void decodingExamples() {

    txref::DecodedResult decodedResult;


    // mainnet, standard txref example

    decodedResult = txref::decode("tx1:rq3n-qqzq-qk8k-mzd");

    assert(decodedResult.hrp == txref::BECH32_HRP_MAIN);
    assert(decodedResult.magicCode == txref::MAGIC_CODE_MAIN);
    assert(decodedResult.blockHeight == 10000);
    assert(decodedResult.transactionIndex == 2);
    assert(decodedResult.encoding == txref::Encoding::Bech32m);


    // testnet, standard txref example

    decodedResult = txref::decode("txtest1:xq3n-qqyq-qhrg-gy3");

    assert(decodedResult.hrp == txref::BECH32_HRP_TEST);
    assert(decodedResult.magicCode == txref::MAGIC_CODE_TEST);
    assert(decodedResult.blockHeight == 10000);
    assert(decodedResult.transactionIndex == 4);
    assert(decodedResult.encoding == txref::Encoding::Bech32m);


    // mainnet, extended txref example

    decodedResult = txref::decode("tx1:yq3n-qqzq-qrqq-9z4d-2n");

    assert(decodedResult.hrp == "tx");
    assert(decodedResult.magicCode == txref::MAGIC_CODE_MAIN_EXTENDED);
    assert(decodedResult.blockHeight == 10000);
    assert(decodedResult.transactionIndex == 2);
    assert(decodedResult.txoIndex == 3);
    assert(decodedResult.encoding == txref::Encoding::Bech32m);


    // testnet, extended txref example

    decodedResult = txref::decode("txtest1:8q3n-qqyq-qxqq-v3x4-ze");

    assert(decodedResult.hrp == "txtest");
    assert(decodedResult.magicCode == txref::MAGIC_CODE_TEST_EXTENDED);
    assert(decodedResult.blockHeight == 10000);
    assert(decodedResult.transactionIndex == 4);
    assert(decodedResult.txoIndex == 6);
    assert(decodedResult.encoding == txref::Encoding::Bech32m);


    // example of decoding an "original" which used the old Bech32 encoding
    decodedResult = txref::decode("txtest1:xjk0-uqay-zat0-dz8");

    assert(decodedResult.hrp == "txtest");
    assert(decodedResult.magicCode == txref::MAGIC_CODE_TEST);
    assert(decodedResult.blockHeight == 466793);
    assert(decodedResult.transactionIndex == 2205);
    assert(decodedResult.txoIndex == 0);
    assert(decodedResult.encoding == txref::Encoding::Bech32); // note: Bech32 rather than Bech32m
    assert(decodedResult.commentary.find("txtest1:xjk0-uqay-zat0-dz8 uses an old encoding scheme and should be updated") != std::string::npos);

}

int main() {

    encodingExamples();
    decodingExamples();

}
