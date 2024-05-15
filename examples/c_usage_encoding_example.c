#include "txref.h"
#include <stdio.h>


int main() {
    int blockHeight = 10000;
    int transactionIndex = 2;
    int txoIndex = 3;
    char main_hrp[] = "tx";

    // create storage for txref string
    txref_tstring *tstring = txref_create_tstring();
    if(!tstring) {
        printf("txref string can not be created");
        return E_TXREF_NO_MEMORY;
    }

    // encode
    txref_error err = txref_encode(tstring, blockHeight, transactionIndex, txoIndex, false, main_hrp);
    if(err != E_TXREF_SUCCESS) {
        printf("%s\n", txref_strerror(err));
        txref_free_tstring(tstring);
        return (int)err;
    }

    printf("mainnet, txref for (blockHeight = 10000, transactionIndex=2, txoIndex=3):\n%s\n", tstring->string);
    // prints: tx1:yq3n-qqzq-qrqq-9z4d-2n

    // free memory
    txref_free_tstring(tstring);

    return E_TXREF_SUCCESS;
}


