#include "libtxref.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

// make sure we can check these examples even when building a release version
#undef NDEBUG
#include <assert.h>

txref_error encodingExamples() {

    char main_hrp[] = "tx";
    char test_hrp[] = "txtest";

    int blockHeight, transactionIndex, txoIndex;

    // create storage for txref string
    txref_tstring *tstring = txref_create_tstring();
    if(!tstring) {
        printf("txref string can not be created");
        return E_TXREF_NO_MEMORY;
    }

    txref_error err;

    // mainnet, standard txref example

    blockHeight = 10000;
    transactionIndex = 2;
    txoIndex = 0;

    err = txref_encode(tstring, blockHeight, transactionIndex, txoIndex, false, main_hrp);
    if(err != E_TXREF_SUCCESS) {
        printf("%s\n", txref_strerror(err));
        txref_free_tstring(tstring);
        return err;
    }

    printf("mainnet, standard txref for (blockHeight = 10000, transactionIndex=2):\n%s\n\n", tstring->string);
    assert(strcmp(tstring->string, "tx1:rq3n-qqzq-qk8k-mzd") == 0);


    // testnet, standard txref example

    blockHeight = 10000;
    transactionIndex = 4;
    txoIndex = 0;

    err = txref_encodeTestnet(tstring, blockHeight, transactionIndex, txoIndex, false, test_hrp);
    if(err != E_TXREF_SUCCESS) {
        printf("%s\n", txref_strerror(err));
        txref_free_tstring(tstring);
        return err;
    }

    printf("testnet, standard txref for (blockHeight = 10000, transactionIndex=4):\n%s\n\n", tstring->string);
    assert(strcmp(tstring->string, "txtest1:xq3n-qqyq-qhrg-gy3") == 0);


    // mainnet, extended txref example

    blockHeight = 10000;
    transactionIndex = 2;
    txoIndex = 3;

    err = txref_encode(tstring, blockHeight, transactionIndex, txoIndex, false, main_hrp);
    if(err != E_TXREF_SUCCESS) {
        printf("%s\n", txref_strerror(err));
        txref_free_tstring(tstring);
        return err;
    }

    printf("mainnet, extended txref for (blockHeight = 10000, transactionIndex=2, txoIndex=3):\n%s\n\n", tstring->string);
    assert(strcmp(tstring->string, "tx1:yq3n-qqzq-qrqq-9z4d-2n") == 0);


    // testnet, extended txref example

    blockHeight = 10000;
    transactionIndex = 4;
    txoIndex = 6;

    err = txref_encodeTestnet(tstring, blockHeight, transactionIndex, txoIndex, false, test_hrp);
    if(err != E_TXREF_SUCCESS) {
        printf("%s\n", txref_strerror(err));
        txref_free_tstring(tstring);
        return err;
    }

    printf("testnet, extended txref for (blockHeight = 10000, transactionIndex=4, txoIndex=6):\n%s\n\n", tstring->string);
    assert(strcmp(tstring->string, "txtest1:8q3n-qqyq-qxqq-v3x4-ze") == 0);


    // free memory
    txref_free_tstring(tstring);

    return E_TXREF_SUCCESS;
}

txref_error decodingExamples() {

    char main_hrp[] = "tx";
    char test_hrp[] = "txtest";

    // create storage for decoded txref data
    txref_DecodedResult *decodedResult = txref_create_DecodedResult();
    if(!decodedResult) {
        printf("txref_DecodedResult can not be created");
        return E_TXREF_NO_MEMORY;
    }

    txref_error err;

    // mainnet, standard txref example

    char main_std_txref[] = "tx1:rq3n-qqzq-qk8k-mzd";

    err = txref_decode(decodedResult, main_std_txref);
    if(err != E_TXREF_SUCCESS) {
        printf("%s\n", txref_strerror(err));
        txref_free_DecodedResult(decodedResult);
        return err;
    }

    assert(decodedResult->blockHeight == 10000);
    assert(decodedResult->transactionIndex == 2);
    assert(decodedResult->txoIndex == 0);
    assert(strcmp(decodedResult->txref, main_std_txref) == 0);
    assert(strcmp(decodedResult->hrp, main_hrp) == 0);


    // testnet, standard txref example

    char test_std_txref[] = "txtest1:xq3n-qqyq-qhrg-gy3";

    err = txref_decode(decodedResult, test_std_txref);
    if(err != E_TXREF_SUCCESS) {
        printf("%s\n", txref_strerror(err));
        txref_free_DecodedResult(decodedResult);
        return err;
    }

    // check for expected values
    assert(decodedResult->blockHeight == 10000);
    assert(decodedResult->transactionIndex == 4);
    assert(decodedResult->txoIndex == 0);
    assert(strcmp(decodedResult->txref, test_std_txref) == 0);
    assert(strcmp(decodedResult->hrp, test_hrp) == 0);


    // mainnet, extended txref example

    char main_ext_txref[] = "tx1:yq3n-qqzq-qrqq-9z4d-2n";

    err = txref_decode(decodedResult, main_ext_txref);
    if(err != E_TXREF_SUCCESS) {
        printf("%s\n", txref_strerror(err));
        txref_free_DecodedResult(decodedResult);
        return err;
    }

    // check for expected values
    assert(decodedResult->blockHeight == 10000);
    assert(decodedResult->transactionIndex == 2);
    assert(decodedResult->txoIndex == 3);
    assert(strcmp(decodedResult->txref, main_ext_txref) == 0);
    assert(strcmp(decodedResult->hrp, main_hrp) == 0);


    // testnet, extended txref example

    char test_ext_txref[] = "txtest1:8q3n-qqyq-qxqq-v3x4-ze";

    err = txref_decode(decodedResult, test_ext_txref);
    if(err != E_TXREF_SUCCESS) {
        printf("%s\n", txref_strerror(err));
        txref_free_DecodedResult(decodedResult);
        return err;
    }

    // check for expected values
    assert(decodedResult->blockHeight == 10000);
    assert(decodedResult->transactionIndex == 4);
    assert(decodedResult->txoIndex == 6);
    assert(strcmp(decodedResult->txref, test_ext_txref) == 0);
    assert(strcmp(decodedResult->hrp, test_hrp) == 0);


    // free memory
    txref_free_DecodedResult(decodedResult);

    return E_TXREF_SUCCESS;
}

int main() {

    txref_error err;

    err = encodingExamples();
    if (err != E_TXREF_SUCCESS)
        return err;

    err = decodingExamples();

    return err;
}


