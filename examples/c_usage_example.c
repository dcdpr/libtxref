#include "libtxref.h"
#include <string.h>
#include <stdio.h>

// make sure we can check these examples even when building a release version
#undef NDEBUG
#include <assert.h>


int main() {

    char main_hrp[] = "tx";

    // encode: mainnet, extended txref example

    int blockHeight = 10000;
    int transactionIndex = 2;
    int txoIndex = 3;

    txref_tstring *tstring = txref_create_tstring();

    assert(txref_encode(tstring, blockHeight, transactionIndex, txoIndex,
                        false, main_hrp) == E_TXREF_SUCCESS);

    printf("mainnet, extended txref for (blockHeight = 10000, transactionIndex=2, txoIndex=3):\n%s\n\n", tstring->string);
    assert(strcmp(tstring->string, "tx1:yq3n-qqzq-qrqq-9z4d-2n") == 0);

    // decode

    txref_DecodedResult *decodedResult = txref_create_DecodedResult();

    assert(txref_decode(decodedResult, tstring->string) == E_TXREF_SUCCESS);
    assert(decodedResult->blockHeight == 10000);
    assert(decodedResult->transactionIndex == 2);
    assert(decodedResult->txoIndex == 3);

    txref_free_DecodedResult(decodedResult);
    txref_free_tstring(tstring);

}


