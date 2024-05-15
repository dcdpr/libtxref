#include "txref.h"
#include <string>
#include <iostream>

int main(int argc, char* argv[])
{
    // Check the number of parameters
    if (argc != 2) {
        std::cerr << "Usage:\n";
        std::cerr << argv[0] << " <txref>\n";
        return 1;
    }

    std::string txref = argv[1];

    txref::DecodedResult decodedResult = txref::decode(txref);

    if(decodedResult.magicCode == txref::MAGIC_CODE_TEST ||
       decodedResult.magicCode == txref::MAGIC_CODE_TEST_EXTENDED)
        std::cout << "testnet: ";
    else if(decodedResult.magicCode == txref::MAGIC_CODE_REGTEST ||
            decodedResult.magicCode == txref::MAGIC_CODE_REGTEST_EXTENDED)
        std::cout << "regtest: ";
    else
        std::cout << "mainnet: ";

    std::cout << " blockHeight: " << decodedResult.blockHeight;
    std::cout << " transactionIndex: " << decodedResult.transactionIndex;
    std::cout << " txoIndex: " << decodedResult.txoIndex;
    std::cout << std::endl;

    return 0;
}
