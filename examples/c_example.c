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

    int blockHeight, transactionPosition, txoIndex;
    char *txref = create_Txref_storage();


    // mainnet, standard txref example

    blockHeight = 10000;
    transactionPosition = 2;
    txoIndex = 0;

    assert(txref_encode(txref, max_Txref_length(), blockHeight, transactionPosition, txoIndex,
                        false, main_hrp, sizeof(main_hrp)) == E_TXREF_SUCCESS);

    printf("mainnet, standard txref for (blockHeight = 10000, transactionPosition=2):\n%s\n\n", txref);
    assert(strcmp(txref, "tx1:rq3n-qqzq-qrmx-h80") == 0);


    // testnet, standard txref example

    blockHeight = 10000;
    transactionPosition = 4;
    txoIndex = 0;

    assert(txref_encodeTestnet(txref, max_Txref_length(), blockHeight, transactionPosition, txoIndex,
                        false, test_hrp, sizeof(test_hrp)) == E_TXREF_SUCCESS);

    printf("testnet, standard txref for (blockHeight = 10000, transactionPosition=4):\n%s\n\n", txref);
    assert(strcmp(txref, "txtest1:xq3n-qqyq-qzlc-ypn") == 0);


    // mainnet, extended txref example

    blockHeight = 10000;
    transactionPosition = 2;
    txoIndex = 3;

    assert(txref_encode(txref, max_Txref_length(), blockHeight, transactionPosition, txoIndex,
                        false, main_hrp, sizeof(main_hrp)) == E_TXREF_SUCCESS);

    printf("mainnet, extended txref for (blockHeight = 10000, transactionPosition=2, txoIndex=3):\n%s\n\n", txref);
    assert(strcmp(txref, "tx1:yq3n-qqzq-qrqq-s79p-03") == 0);


    // testnet, extended txref example

    blockHeight = 10000;
    transactionPosition = 4;
    txoIndex = 6;

    assert(txref_encodeTestnet(txref, max_Txref_length(), blockHeight, transactionPosition, txoIndex,
                        false, test_hrp, sizeof(test_hrp)) == E_TXREF_SUCCESS);

    printf("testnet, extended txref for (blockHeight = 10000, transactionPosition=4, txoIndex=6):\n%s\n\n", txref);
    assert(strcmp(txref, "txtest1:8q3n-qqyq-qxqq-edke-8m") == 0);


    free_Txref_storage(txref);

}

void decodingExamples() {

    char main_hrp[] = "tx";
    char test_hrp[] = "txtest";

    char *txref = create_Txref_storage();
    txref_LocationData *locationData = create_LocationData_storage();


    // mainnet, standard txref example

    strcpy(txref, "tx1:rq3n-qqzq-qrmx-h80");

    assert(txref_decode(locationData, txref, strlen(txref) + 1) == E_TXREF_SUCCESS);
    assert(locationData->blockHeight == 10000);
    assert(locationData->transactionPosition == 2);
    assert(locationData->txoIndex == 0);
    assert(strcmp(locationData->txref, txref) == 0);
    assert(strcmp(locationData->hrp, main_hrp) == 0);


    // testnet, standard txref example

    strcpy(txref, "txtest1:xq3n-qqyq-qzlc-ypn");

    assert(txref_decode(locationData, txref, strlen(txref) + 1) == E_TXREF_SUCCESS);
    assert(locationData->blockHeight == 10000);
    assert(locationData->transactionPosition == 4);
    assert(locationData->txoIndex == 0);
    assert(strcmp(locationData->txref, txref) == 0);
    assert(strcmp(locationData->hrp, test_hrp) == 0);


    // mainnet, extended txref example

    strcpy(txref, "tx1:yq3n-qqzq-qrqq-s79p-03");

    assert(txref_decode(locationData, txref, strlen(txref) + 1) == E_TXREF_SUCCESS);
    assert(locationData->blockHeight == 10000);
    assert(locationData->transactionPosition == 2);
    assert(locationData->txoIndex == 3);
    assert(strcmp(locationData->txref, txref) == 0);
    assert(strcmp(locationData->hrp, main_hrp) == 0);


    // testnet, extended txref example

    strcpy(txref, "txtest1:8q3n-qqyq-qxqq-edke-8m");

    assert(txref_decode(locationData, txref, strlen(txref) + 1) == E_TXREF_SUCCESS);
    assert(locationData->blockHeight == 10000);
    assert(locationData->transactionPosition == 4);
    assert(locationData->txoIndex == 6);
    assert(strcmp(locationData->txref, txref) == 0);
    assert(strcmp(locationData->hrp, test_hrp) == 0);


    free_LocationData_storage(locationData);
    free_Txref_storage(txref);

}

int main() {

    encodingExamples();
    decodingExamples();

}


