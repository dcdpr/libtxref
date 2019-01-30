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
    assert(txref == "tx1:rq3n-qqzq-qrmx-h80");


    // testnet, standard txref example

    blockHeight = 10000;
    transactionPosition = 4;

    txref = txref::encodeTestnet(blockHeight, transactionPosition);

    std::cout << "testnet, standard txref for (blockHeight = 10000, transactionPosition=4):" << std::endl;
    std::cout << txref << "\n\n";
    assert(txref == "txtest1:xq3n-qqyq-qzlc-ypn");


    // mainnet, extended txref example

    blockHeight = 10000;
    transactionPosition = 2;
    txoIndex = 3;

    txref = txref::encode(blockHeight, transactionPosition, txoIndex);

    std::cout << "mainnet, extended txref for (blockHeight = 10000, transactionPosition=2, txoIndex=3):" << std::endl;
    std::cout << txref << "\n\n";
    assert(txref == "tx1:yq3n-qqzq-qrqq-s79p-03");


    // testnet, extended txref example

    blockHeight = 10000;
    transactionPosition  = 4;
    txoIndex = 6;

    txref = txref::encodeTestnet(blockHeight, transactionPosition, txoIndex);

    std::cout << "testnet, extended txref for (blockHeight = 10000, transactionPosition=4, txoIndex=6):" << std::endl;
    std::cout << txref << "\n\n";
    assert(txref == "txtest1:8q3n-qqyq-qxqq-edke-8m");

}

void decodingExamples() {

    txref::LocationData loc;


    // mainnet, standard txref example

    loc = txref::decode("tx1:rq3n-qqzq-qrmx-h80");

    assert(loc.hrp == "tx");
    assert(loc.magicCode == txref::MAGIC_BTC_MAIN);
    assert(loc.blockHeight == 10000);
    assert(loc.transactionPosition == 2);


    // testnet, standard txref example

    loc = txref::decode("txtest1:xq3n-qqyq-qzlc-ypn");

    assert(loc.hrp == "txtest");
    assert(loc.magicCode == txref::MAGIC_BTC_TEST);
    assert(loc.blockHeight == 10000);
    assert(loc.transactionPosition == 4);


    // mainnet, extended txref example

    loc = txref::decode("tx1:yq3n-qqzq-qrqq-s79p-03");

    assert(loc.hrp == "tx");
    assert(loc.magicCode == txref::MAGIC_BTC_MAIN_EXTENDED);
    assert(loc.blockHeight == 10000);
    assert(loc.transactionPosition == 2);
    assert(loc.txoIndex == 3);


    // testnet, extended txref example

    loc = txref::decode("txtest1:8q3n-qqyq-qxqq-edke-8m");

    assert(loc.hrp == "txtest");
    assert(loc.magicCode == txref::MAGIC_BTC_TEST_EXTENDED);
    assert(loc.blockHeight == 10000);
    assert(loc.transactionPosition == 4);
    assert(loc.txoIndex == 6);

}

int main() {

    encodingExamples();
    decodingExamples();

}
