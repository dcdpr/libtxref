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
    assert(strcmp(txref, "tx1:yq3n-qqzq-qrqq-0p67-s0") == 0);

    // decode

    txref_LocationData *locationData = create_LocationData_storage();

    assert(txref_decode(locationData, txref, strlen(txref) + 1) == E_TXREF_SUCCESS);
    assert(locationData->blockHeight == 10000);
    assert(locationData->transactionPosition == 2);
    assert(locationData->txoIndex == 3);

    free_LocationData_storage(locationData);
    free_Txref_storage(txref);

}


