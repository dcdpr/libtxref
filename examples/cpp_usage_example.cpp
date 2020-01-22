#include "libtxref.h"
#include <string>
#include <iostream>

// make sure we can check these examples even when building a release version
#undef NDEBUG
#include <cassert>

int main() {

    // encode: mainnet, extended txref example

    int blockHeight = 10000;
    int transactionPosition = 2;
    int txoIndex = 3;

    std::string txref = txref::encode(blockHeight, transactionPosition, txoIndex);

    std::cout << "mainnet, extended txref for (blockHeight = 10000, transactionPosition=2, txoIndex=3):" << std::endl;
    std::cout << txref << "\n\n";
    assert(txref == "tx1:yq3n-qqzq-qrqq-0p67-s0");

    // decode

    txref::LocationData loc = txref::decode(txref);

    assert(loc.hrp == "tx");
    assert(loc.magicCode == txref::MAGIC_BTC_MAIN_EXTENDED);
    assert(loc.blockHeight == 10000);
    assert(loc.transactionPosition == 2);
    assert(loc.txoIndex == 3);
}
