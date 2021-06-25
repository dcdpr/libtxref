#include "libtxref.h"
#include <string>

// make sure we can check these examples even when building a release version
#undef NDEBUG
#include <cassert>

int main() {
    txref::DecodedResult decodedResult = txref::decode("tx1:yq3n-qqzq-qrqq-9z4d-2n");

    // check for expected values (see cpp_usage_encoding_example.cpp)
    assert(decodedResult.hrp == "tx");
    assert(decodedResult.magicCode == txref::MAGIC_CODE_MAIN_EXTENDED);
    assert(decodedResult.blockHeight == 10000);
    assert(decodedResult.transactionIndex == 2);
    assert(decodedResult.txoIndex == 3);
    assert(decodedResult.encoding == txref::Bech32m);
}
