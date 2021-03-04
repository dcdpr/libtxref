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
    int transactionPosition = 2;
    int txoIndex = 3;

    char *txref = create_Txref_storage();

    assert(txref_encode(txref, max_Txref_length(), blockHeight, transactionPosition, txoIndex,
                        false, main_hrp, sizeof(main_hrp)) == E_TXREF_SUCCESS);

    printf("mainnet, extended txref for (blockHeight = 10000, transactionPosition=2, txoIndex=3):\n%s\n\n", txref);
    assert(strcmp(txref, "tx1:yq3n-qqzq-qrqq-9z4d-2n") == 0);

    // decode

    txref_DecodedResult *decodedResult = create_DecodedResult_storage();

    assert(txref_decode(decodedResult, txref, strlen(txref) + 1) == E_TXREF_SUCCESS);
    assert(decodedResult->blockHeight == 10000);
    assert(decodedResult->transactionPosition == 2);
    assert(decodedResult->txoIndex == 3);

    free_DecodedResult_storage(decodedResult);
    free_Txref_storage(txref);

}


