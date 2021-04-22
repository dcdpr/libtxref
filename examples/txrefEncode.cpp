#include "libtxref.h"
#include <string>
#include <iostream>

int main(int argc, char* argv[])
{
    // Check the number of parameters
    if (argc < 3 || argc > 4) {
        std::cerr << "Usage:\n";
        std::cerr << argv[0] << " <blockHeight> <transactionPosition>\n";
        std::cerr << "or\n";
        std::cerr << argv[0] << " <blockHeight> <transactionPosition> <txoIndex>" << std::endl;
        return 1;
    }

    int blockHeight = std::stoi(argv[1]);
    int transactionPosition = std::stoi(argv[2]);
    int txoIndex = -1;
    if(argc == 4)
        txoIndex = std::stoi(argv[3]);


    std::string txref;
    if(txoIndex > 0) {
        txref = txref::encode(blockHeight, transactionPosition, txoIndex);
        std::cout << "mainnet:     " << txref << std::endl;
        txref = txref::encodeTestnet(blockHeight, transactionPosition, txoIndex);
        std::cout << "testnet: " << txref << std::endl;
    }
    else {
        txref = txref::encode(blockHeight, transactionPosition);
        std::cout << "mainnet:     " << txref << std::endl;
        txref = txref::encodeTestnet(blockHeight, transactionPosition);
        std::cout << "testnet: " << txref << std::endl;
    }

    return 0;
}
