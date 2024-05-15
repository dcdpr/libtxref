// test program calling txref library from C

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "txref.h"

// make sure we can run these tests even when building a release version
#undef NDEBUG
#include <assert.h>

void strerror_withValidErrorCode_returnsErrorMessage(void) {
    const char *message = txref_strerror(E_TXREF_SUCCESS);
    assert(message != NULL);
    assert(strcmp(message, "Success") == 0);
}

void strerror_withInvalidErrorCode_returnsUnknownErrorMessage(void) {
    const char *message = txref_strerror(1234);
    assert(message != NULL);
    assert(strcmp(message, "Unknown error") == 0);
}

void encode_withBadArgs_isUnsuccessful(void) {
    { // input is null
        txref_tstring * tstring = txref_create_tstring();
        assert(txref_encode(tstring, 10, 10, 10, false, NULL) == E_TXREF_NULL_ARGUMENT);
        txref_free_tstring(tstring);
    }

    { // output is null
        char hrp[] = "tx";
        assert(txref_encode(NULL, 10, 10, 10, false, hrp) == E_TXREF_NULL_ARGUMENT);
    }

    { // allocated output is too short
        char hrp[] = "tx";
        txref_tstring tstring;
        tstring.length = 21; // for mainnet, this should be 22 = 15 data chars + 1 ':' + 3 '-' + 'tx1';
        tstring.string = (char *)calloc(tstring.length + 1, 1); //string size should be = string.length + 1 for '\0'
        // Should use txref_create_tstring() to avoid this problem.

        assert(txref_encode(&tstring, 0, 0, 0, false, hrp) == E_TXREF_LENGTH_TOO_SHORT);

        free(tstring.string);
    }
}

void encode_whenMethodThrowsException_isUnsuccessful(void) {
    // bech32 string can only have HRPs that are 83 chars or less. Attempt to encode an HRP string
    // with more than 83 chars and make sure that the exception thrown in the C++ code is caught
    // and returns an error code
    char hrp[] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
    txref_tstring * tstring = txref_create_tstring();

    assert(txref_encode(tstring, 0, 0, 0, false, hrp) == E_TXREF_UNKNOWN_ERROR);

    txref_free_tstring(tstring);
}

void encode_mainnetExamples_areSuccessful(void) {
    char hrp[] = "tx";

    struct examples {
        int blockHeight, transactionIndex, txoIndex;
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
        txref_tstring * tstring = txref_create_tstring();

        assert(txref_encode(
                tstring,
                mainnet_examples[example_index].blockHeight,
                mainnet_examples[example_index].transactionIndex,
                mainnet_examples[example_index].txoIndex, false, hrp) == E_TXREF_SUCCESS);
        assert(strcmp(tstring->string, expectedTxref) == 0);

        txref_free_tstring(tstring);
    }
}

void encode_mainnetExtendedExamples_areSuccessful(void) {
    char hrp[] = "tx";

    struct examples {
        int blockHeight, transactionIndex, txoIndex;
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
        txref_tstring * tstring = txref_create_tstring();

        assert(txref_encode(
                tstring,
                mainnet_examples[example_index].blockHeight,
                mainnet_examples[example_index].transactionIndex,
                mainnet_examples[example_index].txoIndex, false, hrp) == E_TXREF_SUCCESS);
        assert(strcmp(tstring->string, expectedTxref) == 0);

        txref_free_tstring(tstring);
    }
}

void encode_testnetExamples_areSuccessful(void) {
    char hrp[] = "txtest";

    struct examples {
        int blockHeight, transactionIndex, txoIndex;
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
        txref_tstring * tstring = txref_create_tstring();

        assert(txref_encodeTestnet(
                tstring,
                testnet_examples[example_index].blockHeight,
                testnet_examples[example_index].transactionIndex,
                testnet_examples[example_index].txoIndex, false, hrp) == E_TXREF_SUCCESS);
        assert(strcmp(tstring->string, expectedTxref) == 0);

        txref_free_tstring(tstring);
    }
}

void encode_testnetExtendedExamples_areSuccessful(void) {
    char hrp[] = "txtest";

    struct examples {
        int blockHeight, transactionIndex, txoIndex;
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
        txref_tstring * tstring = txref_create_tstring();

        assert(txref_encodeTestnet(
                tstring,
                testnet_examples[example_index].blockHeight,
                testnet_examples[example_index].transactionIndex,
                testnet_examples[example_index].txoIndex, false, hrp) == E_TXREF_SUCCESS);
        assert(strcmp(tstring->string, expectedTxref) == 0);

        txref_free_tstring(tstring);
    }
}

void encode_regtestExamples_areSuccessful(void) {
    char hrp[] = "txrt";

    struct examples {
        int blockHeight, transactionIndex, txoIndex;
        const char *txref;
    };

    struct examples regtest_examples[] = {
            { 0, 0, 0, "txrt1:qqqq-qqqq-qwpz-nyw"},
            { 0xFFFFFF, 0x7FFF, 0, "txrt1:q7ll-llll-lps4-p3p"},
    };
    int num_examples = sizeof(regtest_examples) / sizeof(regtest_examples[0]);

    for(int example_index = 0; example_index < num_examples; example_index++) {
        const char *expectedTxref = regtest_examples[example_index].txref;
        txref_tstring * tstring = txref_create_tstring();

        assert(txref_encodeRegtest(
                tstring,
                regtest_examples[example_index].blockHeight,
                regtest_examples[example_index].transactionIndex,
                regtest_examples[example_index].txoIndex, false, hrp) == E_TXREF_SUCCESS);
        assert(strcmp(tstring->string, expectedTxref) == 0);

        txref_free_tstring(tstring);
    }
}

void encode_regtestExtendedExamples_areSuccessful(void) {
    char hrp[] = "txrt";

    struct examples {
        int blockHeight, transactionIndex, txoIndex;
        const char *txref;
    };

    struct examples regtest_examples[] = {
            { 0, 0, 100, "txrt1:pqqq-qqqq-qyrq-2z0a-kx" },
            { 0, 0, 0x1FFF, "txrt1:pqqq-qqqq-qll8-yq5j-7s" },
    };
    int num_examples = sizeof(regtest_examples) / sizeof(regtest_examples[0]);

    for(int example_index = 0; example_index < num_examples; example_index++) {
        const char *expectedTxref = regtest_examples[example_index].txref;
        txref_tstring * tstring = txref_create_tstring();

        assert(txref_encodeRegtest(
                tstring,
                regtest_examples[example_index].blockHeight,
                regtest_examples[example_index].transactionIndex,
                regtest_examples[example_index].txoIndex, false, hrp) == E_TXREF_SUCCESS);
        assert(strcmp(tstring->string, expectedTxref) == 0);

        txref_free_tstring(tstring);
    }
}

void decode_withBadArgs_isUnsuccessful(void) {

    { // output is null
        char txref[] = "foo";
        assert(txref_decode(NULL, txref) == E_TXREF_NULL_ARGUMENT);
    }

    { // input is null
        txref_DecodedResult *decodedResult = txref_create_DecodedResult();
        assert(txref_decode(decodedResult, NULL) == E_TXREF_NULL_ARGUMENT);
        txref_free_DecodedResult(decodedResult);
    }

    { // decodedResult->txref is null
        char txref[] = "foo";
        txref_DecodedResult *decodedResult = txref_create_DecodedResult();
        free(decodedResult->txref);
        decodedResult->txref = NULL;
        assert(txref_decode(decodedResult, txref) == E_TXREF_NULL_ARGUMENT);
        txref_free_DecodedResult(decodedResult);
    }

    { // decodedResult->hrp is null
        char txref[] = "foo";
        txref_DecodedResult *decodedResult = txref_create_DecodedResult();
        free(decodedResult->hrp);
        decodedResult->hrp = NULL;
        assert(txref_decode(decodedResult, txref) == E_TXREF_NULL_ARGUMENT);
        txref_free_DecodedResult(decodedResult);
    }
}

void decode_whenMethodThrowsException_isUnsuccessful(void) {
    // bech32 string can only have HRPs that are 83 chars or less. Attempt to decode a string
    // with more than 83 chars and make sure that the exception thrown in the C++ code is caught
    // and returns an error code
    char txref[] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa1qpzry9x8gf2tvdw0s3jn54khce6mua7lmqqqxw";
    txref_DecodedResult *decodedResult = txref_create_DecodedResult();
    assert(txref_decode(decodedResult, txref) == E_TXREF_UNKNOWN_ERROR);
    txref_free_DecodedResult(decodedResult);
}

void decode_mainnetExamples_areSuccessful(void) {
    char hrp[] = "tx";

    struct examples {
        const char *txref;
        int blockHeight, transactionIndex, txoIndex;
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
        txref_DecodedResult *decodedResult = txref_create_DecodedResult();

        assert(txref_decode(decodedResult, txref) == E_TXREF_SUCCESS);
        assert(decodedResult->blockHeight == mainnet_examples[example_index].blockHeight);
        assert(decodedResult->transactionIndex == mainnet_examples[example_index].transactionIndex);
        assert(decodedResult->txoIndex == mainnet_examples[example_index].txoIndex);
        assert(strcmp(decodedResult->txref, txref) == 0);
        assert(strcmp(decodedResult->hrp, hrp) == 0);
        assert(decodedResult->encoding == TXREF_ENCODING_BECH32M);

        txref_free_DecodedResult(decodedResult);
    }
}

void decode_mainnetExtendedExamples_areSuccessful(void) {
    char hrp[] = "tx";

    struct examples {
        const char *txref;
        int blockHeight, transactionIndex, txoIndex;
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
        txref_DecodedResult *decodedResult = txref_create_DecodedResult();

        assert(txref_decode(decodedResult, txref) == E_TXREF_SUCCESS);
        assert(decodedResult->blockHeight == mainnet_examples[example_index].blockHeight);
        assert(decodedResult->transactionIndex == mainnet_examples[example_index].transactionIndex);
        assert(decodedResult->txoIndex == mainnet_examples[example_index].txoIndex);
        assert(strcmp(decodedResult->txref, txref) == 0);
        assert(strcmp(decodedResult->hrp, hrp) == 0);
        assert(decodedResult->encoding == TXREF_ENCODING_BECH32M);

        txref_free_DecodedResult(decodedResult);
    }
}

void decode_testnetExamples_areSuccessful(void) {
    char hrp[] = "txtest";

    struct examples {
        const char *txref;
        int blockHeight, transactionIndex, txoIndex;
    };

    struct examples testnet_examples[] = {
            { "txtest1:xqqq-qqqq-qrrd-ksa", 0, 0, 0},
            { "txtest1:x7ll-llll-lvj6-y9j", 0xFFFFFF, 0x7FFF, 0},
            { "txtest1:xk63-uqnf-zgve-zdz", 467883, 2355, 0},
    };
    int num_examples = sizeof(testnet_examples) / sizeof(testnet_examples[0]);

    for(int example_index = 0; example_index < num_examples; example_index++) {
        const char * txref = testnet_examples[example_index].txref;
        txref_DecodedResult *decodedResult = txref_create_DecodedResult();

        assert(txref_decode(decodedResult, txref) == E_TXREF_SUCCESS);
        assert(decodedResult->blockHeight == testnet_examples[example_index].blockHeight);
        assert(decodedResult->transactionIndex == testnet_examples[example_index].transactionIndex);
        assert(decodedResult->txoIndex == testnet_examples[example_index].txoIndex);
        assert(strcmp(decodedResult->txref, txref) == 0);
        assert(strcmp(decodedResult->hrp, hrp) == 0);
        assert(decodedResult->encoding == TXREF_ENCODING_BECH32M);

        txref_free_DecodedResult(decodedResult);
    }
}

void decode_testnetExtendedExamples_areSuccessful(void) {
    char hrp[] = "txtest";

    struct examples {
        const char *txref;
        int blockHeight, transactionIndex, txoIndex;
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
        txref_DecodedResult *decodedResult = txref_create_DecodedResult();

        assert(txref_decode(decodedResult, txref) == E_TXREF_SUCCESS);
        assert(decodedResult->blockHeight == testnet_examples[example_index].blockHeight);
        assert(decodedResult->transactionIndex == testnet_examples[example_index].transactionIndex);
        assert(decodedResult->txoIndex == testnet_examples[example_index].txoIndex);
        assert(strcmp(decodedResult->txref, txref) == 0);
        assert(strcmp(decodedResult->hrp, hrp) == 0);
        assert(decodedResult->encoding == TXREF_ENCODING_BECH32M);

        txref_free_DecodedResult(decodedResult);
    }
}

void decode_regtestExamples_areSuccessful(void) {
    char hrp[] = "txrt";

    struct examples {
        const char *txref;
        int blockHeight, transactionIndex, txoIndex;
    };

    struct examples regtest_examples[] = {
            { "txrt1:qqqq-qqqq-qwpz-nyw", 0, 0, 0},
            { "txrt1:q7ll-llll-lps4-p3p", 0xFFFFFF, 0x7FFF, 0},
    };
    int num_examples = sizeof(regtest_examples) / sizeof(regtest_examples[0]);

    for(int example_index = 0; example_index < num_examples; example_index++) {
        const char * txref = regtest_examples[example_index].txref;
        txref_DecodedResult *decodedResult = txref_create_DecodedResult();

        assert(txref_decode(decodedResult, txref) == E_TXREF_SUCCESS);
        assert(decodedResult->blockHeight == regtest_examples[example_index].blockHeight);
        assert(decodedResult->transactionIndex == regtest_examples[example_index].transactionIndex);
        assert(decodedResult->txoIndex == regtest_examples[example_index].txoIndex);
        assert(strcmp(decodedResult->txref, txref) == 0);
        assert(strcmp(decodedResult->hrp, hrp) == 0);
        assert(decodedResult->encoding == TXREF_ENCODING_BECH32M);

        txref_free_DecodedResult(decodedResult);
    }
}

void decode_regtestExtendedExamples_areSuccessful(void) {
    char hrp[] = "txrt";

    struct examples {
        const char *txref;
        int blockHeight, transactionIndex, txoIndex;
    };

    struct examples testnet_examples[] = {
            { "txrt1:pqqq-qqqq-qyrq-2z0a-kx", 0, 0, 100 },
            { "txrt1:pqqq-qqqq-qll8-yq5j-7s", 0, 0, 0x1FFF },
    };
    int num_examples = sizeof(testnet_examples) / sizeof(testnet_examples[0]);

    for(int example_index = 0; example_index < num_examples; example_index++) {
        const char * txref = testnet_examples[example_index].txref;
        txref_DecodedResult *decodedResult = txref_create_DecodedResult();

        assert(txref_decode(decodedResult, txref) == E_TXREF_SUCCESS);
        assert(decodedResult->blockHeight == testnet_examples[example_index].blockHeight);
        assert(decodedResult->transactionIndex == testnet_examples[example_index].transactionIndex);
        assert(decodedResult->txoIndex == testnet_examples[example_index].txoIndex);
        assert(strcmp(decodedResult->txref, txref) == 0);
        assert(strcmp(decodedResult->hrp, hrp) == 0);
        assert(decodedResult->encoding == TXREF_ENCODING_BECH32M);

        txref_free_DecodedResult(decodedResult);
    }
}

void decode_withOriginalChecksumConstant_hasCommentary(void) {
    char txref[] = "txtest1:8jk0-uqay-zu4x-aw4h-zl";
    txref_DecodedResult *decodedResult = txref_create_DecodedResult();

    assert(txref_decode(decodedResult, txref) == E_TXREF_SUCCESS);
    assert(decodedResult->encoding == TXREF_ENCODING_BECH32);
    assert(strstr(decodedResult->commentary, "txtest1:8jk0-uqay-zu4x-gj9m-8a") != NULL);

    txref_free_DecodedResult(decodedResult);
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
    encode_regtestExamples_areSuccessful();
    encode_regtestExtendedExamples_areSuccessful();

    decode_withBadArgs_isUnsuccessful();
    decode_whenMethodThrowsException_isUnsuccessful();
    decode_mainnetExamples_areSuccessful();
    decode_mainnetExtendedExamples_areSuccessful();
    decode_testnetExamples_areSuccessful();
    decode_testnetExtendedExamples_areSuccessful();
    decode_regtestExamples_areSuccessful();
    decode_regtestExtendedExamples_areSuccessful();
    decode_withOriginalChecksumConstant_hasCommentary();

    return 0;
}
