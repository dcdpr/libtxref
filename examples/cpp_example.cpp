#include "libtxref.h"
#include <string>
#include <iostream>

// make sure we can check these examples even when building a release version
#undef NDEBUG
#include <cassert>


void encodingExamples() {

    int blockHeight, transactionPosition, txoIndex;
    std::string txref;


    // mainnet, standard txref example

    blockHeight = 10000;
    transactionPosition = 2;

    txref = txref::encode(blockHeight, transactionPosition);

    std::cout << "mainnet, standard txref for (blockHeight = 10000, transactionPosition=2):" << std::endl;
    std::cout << txref << "\n\n";
    assert(txref == "tx1:rq3n-qqzq-qk8k-mzd");


    // testnet, standard txref example

    blockHeight = 10000;
    transactionPosition = 4;

    txref = txref::encodeTestnet(blockHeight, transactionPosition);

    std::cout << "testnet, standard txref for (blockHeight = 10000, transactionPosition=4):" << std::endl;
    std::cout << txref << "\n\n";
    assert(txref == "txtest1:xq3n-qqyq-qhrg-gy3");


    // mainnet, extended txref example

    blockHeight = 10000;
    transactionPosition = 2;
    txoIndex = 3;

    txref = txref::encode(blockHeight, transactionPosition, txoIndex);

    std::cout << "mainnet, extended txref for (blockHeight = 10000, transactionPosition=2, txoIndex=3):" << std::endl;
    std::cout << txref << "\n\n";
    assert(txref == "tx1:yq3n-qqzq-qrqq-9z4d-2n");


    // testnet, extended txref example

    blockHeight = 10000;
    transactionPosition  = 4;
    txoIndex = 6;

    txref = txref::encodeTestnet(blockHeight, transactionPosition, txoIndex);

    std::cout << "testnet, extended txref for (blockHeight = 10000, transactionPosition=4, txoIndex=6):" << std::endl;
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
    assert(decodedResult.transactionPosition == 2);
    assert(decodedResult.encoding == txref::Bech32m);


    // testnet, standard txref example

    decodedResult = txref::decode("txtest1:xq3n-qqyq-qhrg-gy3");

    assert(decodedResult.hrp == txref::BECH32_HRP_TEST);
    assert(decodedResult.magicCode == txref::MAGIC_CODE_TEST);
    assert(decodedResult.blockHeight == 10000);
    assert(decodedResult.transactionPosition == 4);
    assert(decodedResult.encoding == txref::Bech32m);


    // mainnet, extended txref example

    decodedResult = txref::decode("tx1:yq3n-qqzq-qrqq-9z4d-2n");

    assert(decodedResult.hrp == "tx");
    assert(decodedResult.magicCode == txref::MAGIC_CODE_MAIN_EXTENDED);
    assert(decodedResult.blockHeight == 10000);
    assert(decodedResult.transactionPosition == 2);
    assert(decodedResult.txoIndex == 3);
    assert(decodedResult.encoding == txref::Bech32m);


    // testnet, extended txref example

    decodedResult = txref::decode("txtest1:8q3n-qqyq-qxqq-v3x4-ze");

    assert(decodedResult.hrp == "txtest");
    assert(decodedResult.magicCode == txref::MAGIC_CODE_TEST_EXTENDED);
    assert(decodedResult.blockHeight == 10000);
    assert(decodedResult.transactionPosition == 4);
    assert(decodedResult.txoIndex == 6);
    assert(decodedResult.encoding == txref::Bech32m);

}

int main() {

    encodingExamples();
    decodingExamples();

}
