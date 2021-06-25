#include "libtxref.h"
#include <stdio.h>

// make sure we can check these examples even when building a release version
#undef NDEBUG
#include <assert.h>


int main() {

    // create storage for decoded txref data
    txref_DecodedResult *decodedResult = txref_create_DecodedResult();
    if(!decodedResult) {
        printf("txref_DecodedResult can not be created");
        return E_TXREF_NO_MEMORY;
    }

    txref_error err = txref_decode(decodedResult, "tx1:yq3n-qqzq-qrqq-9z4d-2n");
    if(err != E_TXREF_SUCCESS) {
        printf("%s\n", txref_strerror(err));
        txref_free_DecodedResult(decodedResult);
        return err;
    }

    // check for expected values (see c_usage_encoding_example.c)
    assert(decodedResult->blockHeight == 10000);
    assert(decodedResult->transactionIndex == 2);
    assert(decodedResult->txoIndex == 3);

    // free memory
    txref_free_DecodedResult(decodedResult);

    return E_TXREF_SUCCESS;
}


