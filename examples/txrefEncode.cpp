#include "libtxref.h"
#include <string>
#include <iostream>

int main(int argc, char* argv[])
{
    // Check the number of parameters
    if (argc < 3 || argc > 4) {
        std::cerr << "Usage:\n";
        std::cerr << argv[0] << " <blockHeight> <transactionIndex>\n";
        std::cerr << "or\n";
        std::cerr << argv[0] << " <blockHeight> <transactionIndex> <txoIndex>" << std::endl;
        return 1;
    }

    int blockHeight = std::stoi(argv[1]);
    int transactionIndex = std::stoi(argv[2]);
    int txoIndex = -1;
    if(argc == 4)
        txoIndex = std::stoi(argv[3]);


    std::string txref;
    if(txoIndex > 0) {
        txref = txref::encode(blockHeight, transactionIndex, txoIndex);
        std::cout << "mainnet:     " << txref << std::endl;
        txref = txref::encodeTestnet(blockHeight, transactionIndex, txoIndex);
        std::cout << "testnet: " << txref << std::endl;
        txref = txref::encodeRegtest(blockHeight, transactionIndex, txoIndex);
        std::cout << "regtest:   " << txref << std::endl;
    }
    else {
        txref = txref::encode(blockHeight, transactionIndex);
        std::cout << "mainnet:     " << txref << std::endl;
        txref = txref::encodeTestnet(blockHeight, transactionIndex);
        std::cout << "testnet: " << txref << std::endl;
        txref = txref::encodeRegtest(blockHeight, transactionIndex);
        std::cout << "regtest:   " << txref << std::endl;
    }

    return 0;
}
