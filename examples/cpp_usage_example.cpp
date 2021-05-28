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
    assert(txref == "tx1:yq3n-qqzq-qrqq-9z4d-2n");

    // decode

    txref::DecodedResult decodedResult = txref::decode(txref);

    assert(decodedResult.hrp == "tx");
    assert(decodedResult.magicCode == txref::MAGIC_CODE_MAIN_EXTENDED);
    assert(decodedResult.blockHeight == 10000);
    assert(decodedResult.transactionPosition == 2);
    assert(decodedResult.txoIndex == 3);
}
