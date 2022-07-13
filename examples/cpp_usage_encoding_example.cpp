#include "txref.h"
#include <string>
#include <iostream>


int main() {
    // encode: mainnet, extended txref example
    int blockHeight = 10000;
    int transactionIndex = 2;
    int txoIndex = 3;

    std::string txref = txref::encode(blockHeight, transactionIndex, txoIndex);

    std::cout << "mainnet, extended txref for (blockHeight = 10000, transactionIndex=2, txoIndex=3):" << std::endl;
    std::cout << txref << "\n\n";
    // prints: "tx1:yq3n-qqzq-qrqq-9z4d-2n"
}
