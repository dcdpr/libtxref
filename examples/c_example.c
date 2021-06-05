#include "libtxref.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// make sure we can check these examples even when building a release version
#undef NDEBUG
#include <assert.h>

void encodingExamples() {

    char main_hrp[] = "tx";
    char test_hrp[] = "txtest";

    int blockHeight, transactionIndex, txoIndex;
    txref_tstring *tstring = txref_create_tstring();


    // mainnet, standard txref example

    blockHeight = 10000;
    transactionIndex = 2;
    txoIndex = 0;

    assert(txref_encode(tstring, blockHeight, transactionIndex, txoIndex,
                        false, main_hrp) == E_TXREF_SUCCESS);

    printf("mainnet, standard txref for (blockHeight = 10000, transactionIndex=2):\n%s\n\n", tstring->string);
    assert(strcmp(tstring->string, "tx1:rq3n-qqzq-qk8k-mzd") == 0);


    // testnet, standard txref example

    blockHeight = 10000;
    transactionIndex = 4;
    txoIndex = 0;

    assert(txref_encodeTestnet(tstring, blockHeight, transactionIndex, txoIndex,
                               false, test_hrp) == E_TXREF_SUCCESS);

    printf("testnet, standard txref for (blockHeight = 10000, transactionIndex=4):\n%s\n\n", tstring->string);
    assert(strcmp(tstring->string, "txtest1:xq3n-qqyq-qhrg-gy3") == 0);


    // mainnet, extended txref example

    blockHeight = 10000;
    transactionIndex = 2;
    txoIndex = 3;

    assert(txref_encode(tstring, blockHeight, transactionIndex, txoIndex,
                        false, main_hrp) == E_TXREF_SUCCESS);

    printf("mainnet, extended txref for (blockHeight = 10000, transactionIndex=2, txoIndex=3):\n%s\n\n", tstring->string);
    assert(strcmp(tstring->string, "tx1:yq3n-qqzq-qrqq-9z4d-2n") == 0);


    // testnet, extended txref example

    blockHeight = 10000;
    transactionIndex = 4;
    txoIndex = 6;

    assert(txref_encodeTestnet(tstring, blockHeight, transactionIndex, txoIndex,
                               false, test_hrp) == E_TXREF_SUCCESS);

    printf("testnet, extended txref for (blockHeight = 10000, transactionIndex=4, txoIndex=6):\n%s\n\n", tstring->string);
    assert(strcmp(tstring->string, "txtest1:8q3n-qqyq-qxqq-v3x4-ze") == 0);


    txref_free_tstring(tstring);

}

void decodingExamples() {

    char main_hrp[] = "tx";
    char test_hrp[] = "txtest";

    txref_DecodedResult *decodedResult = txref_create_DecodedResult();


    // mainnet, standard txref example

    char main_std_txref[] = "tx1:rq3n-qqzq-qk8k-mzd";

    assert(txref_decode(decodedResult, main_std_txref) == E_TXREF_SUCCESS);
    assert(decodedResult->blockHeight == 10000);
    assert(decodedResult->transactionIndex == 2);
    assert(decodedResult->txoIndex == 0);
    assert(strcmp(decodedResult->txref, main_std_txref) == 0);
    assert(strcmp(decodedResult->hrp, main_hrp) == 0);


    // testnet, standard txref example

    char test_std_txref[] = "txtest1:xq3n-qqyq-qhrg-gy3";

    assert(txref_decode(decodedResult, test_std_txref) == E_TXREF_SUCCESS);
    assert(decodedResult->blockHeight == 10000);
    assert(decodedResult->transactionIndex == 4);
    assert(decodedResult->txoIndex == 0);
    assert(strcmp(decodedResult->txref, test_std_txref) == 0);
    assert(strcmp(decodedResult->hrp, test_hrp) == 0);


    // mainnet, extended txref example

    char main_ext_txref[] = "tx1:yq3n-qqzq-qrqq-9z4d-2n";

    assert(txref_decode(decodedResult, main_ext_txref) == E_TXREF_SUCCESS);
    assert(decodedResult->blockHeight == 10000);
    assert(decodedResult->transactionIndex == 2);
    assert(decodedResult->txoIndex == 3);
    assert(strcmp(decodedResult->txref, main_ext_txref) == 0);
    assert(strcmp(decodedResult->hrp, main_hrp) == 0);


    // testnet, extended txref example

    char test_ext_txref[] = "txtest1:8q3n-qqyq-qxqq-v3x4-ze";

    assert(txref_decode(decodedResult, test_ext_txref) == E_TXREF_SUCCESS);
    assert(decodedResult->blockHeight == 10000);
    assert(decodedResult->transactionIndex == 4);
    assert(decodedResult->txoIndex == 6);
    assert(strcmp(decodedResult->txref, test_ext_txref) == 0);
    assert(strcmp(decodedResult->hrp, test_hrp) == 0);


    txref_free_DecodedResult(decodedResult);

}

int main() {

    encodingExamples();
    decodingExamples();

}


