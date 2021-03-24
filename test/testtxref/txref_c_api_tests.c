// test program calling txref library from C

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "libtxref.h"

// make sure we can run these tests even when building a release version
#undef NDEBUG
#include <assert.h>

void strerror_withValidErrorCode_returnsErrorMessage() {
    const char *message = txref_strerror(E_TXREF_SUCCESS);
    assert(message != NULL);
    assert(strcmp(message, "Success") == 0);
}

void strerror_withInvalidErrorCode_returnsUnknownErrorMessage() {
    const char *message = txref_strerror(1234);
    assert(message != NULL);
    assert(strcmp(message, "Unknown error") == 0);
}

void encode_withBadArgs_isUnsuccessful() {
    { // input is null
        char foo[] = "foo";
        assert(txref_encode(foo, sizeof(foo), 10, 10, 10, false, NULL, 0) == E_TXREF_NULL_ARGUMENT);
    }

    { // output is null
        char foo[] = "foo";
        assert(txref_encode(NULL, 0, 10, 10, 10, false, foo, sizeof(foo)) == E_TXREF_NULL_ARGUMENT);
    }

    { // allocated output is too short
        char hrp[] = "tx";
        const int txreflen = 22; // for mainnet, this should be 23 = 15 data chars + 1 ':' + 3 '-' + 'tx1' + '\0';
        char txref[txreflen];
        assert(txref_encode(txref, txreflen, 0, 0, 0, false, hrp, sizeof(hrp)) == E_TXREF_LENGTH_TOO_SHORT);
    }
}

void encode_whenMethodThrowsException_isUnsuccessful() {
    // bech32 string can only have HRPs that are 83 chars or less. Attempt to encode an HRP string
    // with more than 83 chars and make sure that the exception thrown in the C++ code is caught
    // and returns an error code
    char hrp[] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
    const int txreflen = 23;
    char txref[txreflen];
    assert(txref_encode(txref, txreflen, 0, 0, 0, false, hrp, sizeof(hrp)) == E_TXREF_UNKNOWN_ERROR);
}

void encode_mainnetExamples_areSuccessful() {
    char hrp[] = "tx";

    struct examples {
        int blockHeight, transactionPosition, txoIndex;
        const char *txref;
    };

    struct examples mainnet_examples[] = {
            { 0, 0, 0, "tx1:rqqq-qqqq-qwtv-vjr" },
            { 0, 0x7FFF, 0, "tx1:rqqq-qqll-lj68-7n2" },
            { 0xFFFFFF, 0x7FFF, 0, "tx1:r7ll-llll-lp6m-78v" },
            { 466793, 2205, 0, "tx1:rjk0-uqay-z9l7-m9m" },
    };
    int num_examples = sizeof(mainnet_examples) / sizeof(mainnet_examples[0]);

    for(int example_index = 0; example_index < num_examples; example_index++) {
        const char *expectedTxref = mainnet_examples[example_index].txref;
        const size_t expectedTxrefSize = strlen(expectedTxref)+1; // 23 = 15 data chars + 1 ':' + 3 '-' + 'tx1' + '\0';
        char *txref = (char *) calloc(expectedTxrefSize, 1);

        assert(txref_encode(
                txref, expectedTxrefSize,
                mainnet_examples[example_index].blockHeight,
                mainnet_examples[example_index].transactionPosition,
                mainnet_examples[example_index].txoIndex, false, hrp, sizeof(hrp)) == E_TXREF_SUCCESS);
        assert(strcmp(txref, expectedTxref) == 0);

        free(txref);
    }
}

void encode_mainnetExtendedExamples_areSuccessful() {
    char hrp[] = "tx";

    struct examples {
        int blockHeight, transactionPosition, txoIndex;
        const char *txref;
    };

    struct examples mainnet_examples[] = {
            { 0, 0, 100, "tx1:yqqq-qqqq-qyrq-62qj-df" },
            { 0, 0, 0x1FFF, "tx1:yqqq-qqqq-qll8-5gma-9l" },
            { 0, 0x1FFF, 100, "tx1:yqqq-qqll-8yrq-03w7-ne" },
            { 0x1FFFFF, 0, 200, "tx1:y7ll-lrqq-qgxq-l3gu-ek" },
            { 0x1FFFFF, 0x1FFF, 0x1FFF, "tx1:y7ll-lrll-8ll8-xzsy-f2" },
            { 466793, 2205, 10, "tx1:yjk0-uqay-z2qq-r8h6-pa" },
            { 466793, 2205, 0x1FFF, "tx1:yjk0-uqay-zll8-qhc8-8u" }
    };
    int num_examples = sizeof(mainnet_examples) / sizeof(mainnet_examples[0]);

    for(int example_index = 0; example_index < num_examples; example_index++) {
        const char *expectedTxref = mainnet_examples[example_index].txref;
        const size_t expectedTxrefSize = strlen(expectedTxref)+1; // 27 = 18 data chars + 1 ':' + 4 '-' + 'tx1' + '\0';
        char *txref = (char *) calloc(expectedTxrefSize, 1);

        assert(txref_encode(
                txref, expectedTxrefSize,
                mainnet_examples[example_index].blockHeight,
                mainnet_examples[example_index].transactionPosition,
                mainnet_examples[example_index].txoIndex, false, hrp, sizeof(hrp)) == E_TXREF_SUCCESS);
        assert(strcmp(txref, expectedTxref) == 0);

        free(txref);
    }
}

void encode_testnetExamples_areSuccessful() {
    char hrp[] = "txtest";

    struct examples {
        int blockHeight, transactionPosition, txoIndex;
        const char *txref;
    };

    struct examples testnet_examples[] = {
            { 0, 0, 0, "txtest1:xqqq-qqqq-qrrd-ksa"},
            { 0xFFFFFF, 0x7FFF, 0, "txtest1:x7ll-llll-lvj6-y9j"},
            { 467883, 2355, 0, "txtest1:xk63-uqnf-zgve-zdz"},
    };
    int num_examples = sizeof(testnet_examples) / sizeof(testnet_examples[0]);

    for(int example_index = 0; example_index < num_examples; example_index++) {
        const char *expectedTxref = testnet_examples[example_index].txref;
        const size_t expectedTxrefSize = strlen(expectedTxref)+1; // 27 = 15 data chars + 1 ':' + 3 '-' + 'txtest1' + '\0';
        char *txref = (char *) calloc(expectedTxrefSize, 1);

        assert(txref_encodeTestnet(
                txref, expectedTxrefSize,
                testnet_examples[example_index].blockHeight,
                testnet_examples[example_index].transactionPosition,
                testnet_examples[example_index].txoIndex, false, hrp, sizeof(hrp)) == E_TXREF_SUCCESS);
        assert(strcmp(txref, expectedTxref) == 0);

        free(txref);
    }
}

void encode_testnetExtendedExamples_areSuccessful() {
    char hrp[] = "txtest";

    struct examples {
        int blockHeight, transactionPosition, txoIndex;
        const char *txref;
    };

    struct examples testnet_examples[] = {
            { 0, 0, 0x1FFF, "txtest1:8qqq-qqqq-qll8-6s5k-xc" },
            { 0, 0x7FFF, 100, "txtest1:8qqq-qqll-lyrq-wd5y-wz" },
            { 0xFFFFFF, 0, 200, "txtest1:87ll-llqq-qgxq-auv3-fk" },
            { 0xFFFFFF, 0x7FFF, 0x1FFF, "txtest1:87ll-llll-lll8-ttpc-8k" },
            { 466793, 2205, 10, "txtest1:8jk0-uqay-z2qq-dlc3-z6" },
            { 466793, 2205, 0x1FFF, "txtest1:8jk0-uqay-zll8-w0hv-ym" }
    };
    int num_examples = sizeof(testnet_examples) / sizeof(testnet_examples[0]);

    for(int example_index = 0; example_index < num_examples; example_index++) {
        const char *expectedTxref = testnet_examples[example_index].txref;
        const size_t expectedTxrefSize = strlen(expectedTxref)+1; // 31 = 18 data chars + 1 ':' + 4 '-' + 'txtest1' + '\0';
        char *txref = (char *) calloc(expectedTxrefSize, 1);

        assert(txref_encodeTestnet(
                txref, expectedTxrefSize,
                testnet_examples[example_index].blockHeight,
                testnet_examples[example_index].transactionPosition,
                testnet_examples[example_index].txoIndex, false, hrp, sizeof(hrp)) == E_TXREF_SUCCESS);
        assert(strcmp(txref, expectedTxref) == 0);

        free(txref);
    }
}

void encode_AnyExamples_areSuccessful() {
    // this test function repeats some encoding tests above, but here we are testing that
    // the storage allocation functions are able to allocate enough memory to safely handle
    // any kind of txref (regular or extended)

    struct examples {
        int blockHeight, transactionPosition, txoIndex;
        const char *txref;
    };

    { // mainnet
        struct examples mainnet_examples[] = {
                {466793, 2205, 0,  "tx1:rjk0-uqay-z9l7-m9m"},
                {466793, 2205, 10, "tx1:yjk0-uqay-z2qq-r8h6-pa"}
        };
        int num_examples = sizeof(mainnet_examples) / sizeof(mainnet_examples[0]);

        char hrp[] = "tx";
        size_t hrplen = sizeof(hrp);

        for (int example_index = 0; example_index < num_examples; example_index++) {
            const char *expectedTxref = mainnet_examples[example_index].txref;

            const size_t expectedTxrefSize = max_Txref_length();
            char *txref = create_Txref_storage();

            assert(txref_encode(
                    txref, expectedTxrefSize,
                    mainnet_examples[example_index].blockHeight,
                    mainnet_examples[example_index].transactionPosition,
                    mainnet_examples[example_index].txoIndex, false, hrp, hrplen) == E_TXREF_SUCCESS);
            assert(strcmp(txref, expectedTxref) == 0);

            free_Txref_storage(txref);
        }
    }

    { // testnet
        struct examples testnet_examples[] = {
                {467883, 2355, 0,  "txtest1:xk63-uqnf-zgve-zdz"},
                {466793, 2205, 10, "txtest1:8jk0-uqay-z2qq-dlc3-z6"}
        };
        int num_examples = sizeof(testnet_examples) / sizeof(testnet_examples[0]);

        char hrp[] = "txtest";
        size_t hrplen = sizeof(hrp);

        for (int example_index = 0; example_index < num_examples; example_index++) {
            const char *expectedTxref = testnet_examples[example_index].txref;

            const size_t expectedTxrefSize = max_Txref_length();
            char *txref = create_Txref_storage();

            assert(txref_encodeTestnet(
                    txref, expectedTxrefSize,
                    testnet_examples[example_index].blockHeight,
                    testnet_examples[example_index].transactionPosition,
                    testnet_examples[example_index].txoIndex, false, hrp, hrplen) == E_TXREF_SUCCESS);
            assert(strcmp(txref, expectedTxref) == 0);

            free_Txref_storage(txref);
        }
    }
}

void decode_withBadArgs_isUnsuccessful() {

    { // output is null
        char txref[] = "foo";
        assert(txref_decode(NULL, txref, sizeof(txref)) == E_TXREF_NULL_ARGUMENT);
    }

    { // input is null
        txref_DecodedResult *decodedResult = create_DecodedResult_storage();
        assert(txref_decode(decodedResult, NULL, 0) == E_TXREF_NULL_ARGUMENT);
        free_DecodedResult_storage(decodedResult);
    }

    { // decodedResult->txref is null
        char txref[] = "foo";
        txref_DecodedResult *decodedResult = create_DecodedResult_storage();
        free_Txref_storage(decodedResult->txref);
        decodedResult->txref = NULL;
        assert(txref_decode(decodedResult, txref, sizeof(txref)) == E_TXREF_NULL_ARGUMENT);
        free_DecodedResult_storage(decodedResult);
    }

    { // decodedResult->hrp is null
        char txref[] = "foo";
        txref_DecodedResult *decodedResult = create_DecodedResult_storage();
        free(decodedResult->hrp);
        decodedResult->hrp = NULL;
        assert(txref_decode(decodedResult, txref, sizeof(txref)) == E_TXREF_NULL_ARGUMENT);
        free_DecodedResult_storage(decodedResult);
    }
}

void decode_whenMethodThrowsException_isUnsuccessful() {
    // bech32 string can only have HRPs that are 83 chars or less. Attempt to decode a string
    // with more than 83 chars and make sure that the exception thrown in the C++ code is caught
    // and returns an error code
    char txref[] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa1qpzry9x8gf2tvdw0s3jn54khce6mua7lmqqqxw";
    txref_DecodedResult *decodedResult = create_DecodedResult_storage();
    assert(txref_decode(decodedResult, txref, sizeof(txref)) == E_TXREF_UNKNOWN_ERROR);
    free_DecodedResult_storage(decodedResult);
}

void decode_mainnetExamples_areSuccessful() {
    char hrp[] = "tx";

    struct examples {
        const char *txref;
        int blockHeight, transactionPosition, txoIndex;
    };

    struct examples mainnet_examples[] = {
            { "tx1:rqqq-qqqq-qwtv-vjr", 0, 0, 0},
            { "tx1:rqqq-qqll-lj68-7n2", 0, 0x7FFF, 0},
            { "tx1:r7ll-llll-lp6m-78v", 0xFFFFFF, 0x7FFF, 0},
            { "tx1:rjk0-uqay-z9l7-m9m", 466793, 2205, 0},
    };
    int num_examples = sizeof(mainnet_examples) / sizeof(mainnet_examples[0]);

    for(int example_index = 0; example_index < num_examples; example_index++) {
        const char * txref = mainnet_examples[example_index].txref;
        txref_DecodedResult *decodedResult =
                (txref_DecodedResult *) calloc(1, sizeof(txref_DecodedResult));
        decodedResult->txreflen = strlen(txref)+1;
        decodedResult->txref = (char *) calloc(decodedResult->txreflen, 1);
        decodedResult->hrplen = sizeof(hrp);
        decodedResult->hrp = (char *) calloc(decodedResult->hrplen, 1);

        assert(txref_decode(decodedResult, txref, strlen(txref)+1) == E_TXREF_SUCCESS);
        assert(decodedResult->blockHeight == mainnet_examples[example_index].blockHeight);
        assert(decodedResult->transactionPosition == mainnet_examples[example_index].transactionPosition);
        assert(decodedResult->txoIndex == mainnet_examples[example_index].txoIndex);
        assert(strcmp(decodedResult->txref, txref) == 0);
        assert(strcmp(decodedResult->hrp, hrp) == 0);
        assert(decodedResult->encoding == BECH32M);

        free(decodedResult->hrp);
        free(decodedResult->txref);
        free(decodedResult);
    }
}

void decode_mainnetExtendedExamples_areSuccessful() {
    char hrp[] = "tx";

    struct examples {
        const char *txref;
        int blockHeight, transactionPosition, txoIndex;
    };

    struct examples mainnet_examples[] = {
            { "tx1:yqqq-qqqq-qyrq-62qj-df", 0, 0, 100 },
            { "tx1:yqqq-qqqq-qll8-5gma-9l", 0, 0, 0x1FFF },
            { "tx1:yqqq-qqll-8yrq-03w7-ne", 0, 0x1FFF, 100 },
            { "tx1:y7ll-lrqq-qgxq-l3gu-ek", 0x1FFFFF, 0, 200 },
            { "tx1:y7ll-lrll-8ll8-xzsy-f2", 0x1FFFFF, 0x1FFF, 0x1FFF },
            { "tx1:yjk0-uqay-z2qq-r8h6-pa", 466793, 2205, 10 },
            { "tx1:yjk0-uqay-zll8-qhc8-8u", 466793, 2205, 0x1FFF }
    };
    int num_examples = sizeof(mainnet_examples) / sizeof(mainnet_examples[0]);

    for(int example_index = 0; example_index < num_examples; example_index++) {
        const char * txref = mainnet_examples[example_index].txref;
        txref_DecodedResult *decodedResult =
                (txref_DecodedResult *) calloc(1, sizeof(txref_DecodedResult));
        decodedResult->txreflen = strlen(txref)+1;
        decodedResult->txref = (char *) calloc(decodedResult->txreflen, 1);
        decodedResult->hrplen = sizeof(hrp);
        decodedResult->hrp = (char *) calloc(decodedResult->hrplen, 1);

        assert(txref_decode(decodedResult, txref, strlen(txref)+1) == E_TXREF_SUCCESS);
        assert(decodedResult->blockHeight == mainnet_examples[example_index].blockHeight);
        assert(decodedResult->transactionPosition == mainnet_examples[example_index].transactionPosition);
        assert(decodedResult->txoIndex == mainnet_examples[example_index].txoIndex);
        assert(strcmp(decodedResult->txref, txref) == 0);
        assert(strcmp(decodedResult->hrp, hrp) == 0);
        assert(decodedResult->encoding == BECH32M);

        free(decodedResult->hrp);
        free(decodedResult->txref);
        free(decodedResult);
    }
}

void decode_testnetExamples_areSuccessful() {
    char hrp[] = "txtest";

    struct examples {
        const char *txref;
        int blockHeight, transactionPosition, txoIndex;
    };

    struct examples testnet_examples[] = {
            { "txtest1:xqqq-qqqq-qrrd-ksa", 0, 0, 0},
            { "txtest1:x7ll-llll-lvj6-y9j", 0xFFFFFF, 0x7FFF, 0},
            { "txtest1:xk63-uqnf-zgve-zdz", 467883, 2355, 0},
    };
    int num_examples = sizeof(testnet_examples) / sizeof(testnet_examples[0]);

    for(int example_index = 0; example_index < num_examples; example_index++) {
        const char * txref = testnet_examples[example_index].txref;
        txref_DecodedResult *decodedResult =
                (txref_DecodedResult *) calloc(1, sizeof(txref_DecodedResult));
        decodedResult->txreflen = strlen(txref)+1;
        decodedResult->txref = (char *) calloc(decodedResult->txreflen, 1);
        decodedResult->hrplen = sizeof(hrp);
        decodedResult->hrp = (char *) calloc(decodedResult->hrplen, 1);

        assert(txref_decode(decodedResult, txref, strlen(txref)+1) == E_TXREF_SUCCESS);
        assert(decodedResult->blockHeight == testnet_examples[example_index].blockHeight);
        assert(decodedResult->transactionPosition == testnet_examples[example_index].transactionPosition);
        assert(decodedResult->txoIndex == testnet_examples[example_index].txoIndex);
        assert(strcmp(decodedResult->txref, txref) == 0);
        assert(strcmp(decodedResult->hrp, hrp) == 0);
        assert(decodedResult->encoding == BECH32M);

        free(decodedResult->hrp);
        free(decodedResult->txref);
        free(decodedResult);
    }
}

void decode_testnetExtendedExamples_areSuccessful() {
    char hrp[] = "txtest";

    struct examples {
        const char *txref;
        int blockHeight, transactionPosition, txoIndex;
    };

    struct examples testnet_examples[] = {
            { "txtest1:8qqq-qqqq-qll8-6s5k-xc", 0, 0, 0x1FFF },
            { "txtest1:8qqq-qqll-lyrq-wd5y-wz", 0, 0x7FFF, 100, },
            { "txtest1:87ll-llqq-qgxq-auv3-fk", 0xFFFFFF, 0, 200 },
            { "txtest1:87ll-llll-lll8-ttpc-8k", 0xFFFFFF, 0x7FFF, 0x1FFF },
            { "txtest1:8jk0-uqay-z2qq-dlc3-z6", 466793, 2205, 10 },
            { "txtest1:8jk0-uqay-zll8-w0hv-ym", 466793, 2205, 0x1FFF }
    };
    int num_examples = sizeof(testnet_examples) / sizeof(testnet_examples[0]);

    for(int example_index = 0; example_index < num_examples; example_index++) {
        const char * txref = testnet_examples[example_index].txref;
        txref_DecodedResult *decodedResult =
                (txref_DecodedResult *) calloc(1, sizeof(txref_DecodedResult));
        decodedResult->txreflen = strlen(txref)+1;
        decodedResult->txref = (char *) calloc(decodedResult->txreflen, 1);
        decodedResult->hrplen = sizeof(hrp);
        decodedResult->hrp = (char *) calloc(decodedResult->hrplen, 1);

        assert(txref_decode(decodedResult, txref, strlen(txref)+1) == E_TXREF_SUCCESS);
        assert(decodedResult->blockHeight == testnet_examples[example_index].blockHeight);
        assert(decodedResult->transactionPosition == testnet_examples[example_index].transactionPosition);
        assert(decodedResult->txoIndex == testnet_examples[example_index].txoIndex);
        assert(strcmp(decodedResult->txref, txref) == 0);
        assert(strcmp(decodedResult->hrp, hrp) == 0);
        assert(decodedResult->encoding == BECH32M);

        free(decodedResult->hrp);
        free(decodedResult->txref);
        free(decodedResult);
    }
}

void decode_AnyExtendedExamples_areSuccessful() {
    // this test function repeats some decoding tests above, but here we are testing that
    // the storage allocation functions are able to allocate enough memory to safely handle
    // any kind of txref (regular or extended)

    struct examples {
        const char *txref;
        int blockHeight, transactionPosition, txoIndex;
    };

    { // mainnet
        struct examples mainnet_examples[] = {
                {"tx1:rjk0-uqay-z9l7-m9m",     466793, 2205, 0},
                {"tx1:yjk0-uqay-z2qq-r8h6-pa", 466793, 2205, 10}
        };
        int num_examples = sizeof(mainnet_examples) / sizeof(mainnet_examples[0]);

        char hrp[] = "tx";

        for (int example_index = 0; example_index < num_examples; example_index++) {
            const char *txref = mainnet_examples[example_index].txref;
            txref_DecodedResult *decodedResult = create_DecodedResult_storage();

            assert(txref_decode(decodedResult, txref, strlen(txref) + 1) == E_TXREF_SUCCESS);
            assert(decodedResult->blockHeight == mainnet_examples[example_index].blockHeight);
            assert(decodedResult->transactionPosition == mainnet_examples[example_index].transactionPosition);
            assert(decodedResult->txoIndex == mainnet_examples[example_index].txoIndex);
            assert(strcmp(decodedResult->txref, txref) == 0);
            assert(strcmp(decodedResult->hrp, hrp) == 0);
            assert(decodedResult->encoding == BECH32M);

            free_DecodedResult_storage(decodedResult);
        }
    }

    { // testnet
        struct examples testnet_examples[] = {
                {"txtest1:xk63-uqnf-zgve-zdz",     467883, 2355, 0},
                {"txtest1:8jk0-uqay-z2qq-dlc3-z6", 466793, 2205, 10}
        };
        int num_examples = sizeof(testnet_examples) / sizeof(testnet_examples[0]);

        char hrp[] = "txtest";

        for (int example_index = 0; example_index < num_examples; example_index++) {
            const char *txref = testnet_examples[example_index].txref;
            txref_DecodedResult *decodedResult = create_DecodedResult_storage();

            assert(txref_decode(decodedResult, txref, strlen(txref) + 1) == E_TXREF_SUCCESS);
            assert(decodedResult->blockHeight == testnet_examples[example_index].blockHeight);
            assert(decodedResult->transactionPosition == testnet_examples[example_index].transactionPosition);
            assert(decodedResult->txoIndex == testnet_examples[example_index].txoIndex);
            assert(strcmp(decodedResult->txref, txref) == 0);
            assert(strcmp(decodedResult->hrp, hrp) == 0);
            assert(decodedResult->encoding == BECH32M);

            free_DecodedResult_storage(decodedResult);
        }
    }
}

void decode_withOriginalChecksumConstant_hasCommentary() {
    char txref[] = "txtest1:8jk0-uqay-zu4x-aw4h-zl";
    txref_DecodedResult *decodedResult = create_DecodedResult_storage();

    assert(txref_decode(decodedResult, txref, sizeof(txref)) == E_TXREF_SUCCESS);
    assert(decodedResult->encoding == BECH32);
    assert(strstr(decodedResult->commentary, "txtest1:8jk0-uqay-zu4x-gj9m-8a") != NULL);

    free_DecodedResult_storage(decodedResult);
}

int main() {

    strerror_withValidErrorCode_returnsErrorMessage();
    strerror_withInvalidErrorCode_returnsUnknownErrorMessage();

    encode_withBadArgs_isUnsuccessful();
    encode_whenMethodThrowsException_isUnsuccessful();
    encode_mainnetExamples_areSuccessful();
    encode_mainnetExtendedExamples_areSuccessful();
    encode_testnetExamples_areSuccessful();
    encode_testnetExtendedExamples_areSuccessful();
    encode_AnyExamples_areSuccessful();

    decode_withBadArgs_isUnsuccessful();
    decode_whenMethodThrowsException_isUnsuccessful();
    decode_mainnetExamples_areSuccessful();
    decode_mainnetExtendedExamples_areSuccessful();
    decode_testnetExamples_areSuccessful();
    decode_testnetExtendedExamples_areSuccessful();
    decode_AnyExtendedExamples_areSuccessful();
    decode_withOriginalChecksumConstant_hasCommentary();

    return 0;
}
