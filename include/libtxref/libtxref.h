
#ifndef TXREF_TXREF_H
#define TXREF_TXREF_H

#ifdef __cplusplus

#include <string>

namespace txref {

    // bech32 "human readable part"s
    static const char BECH32_HRP_MAIN[] = "tx";
    static const char BECH32_HRP_TEST[] = "txtest";
    static const char BECH32_HRP_REGTEST[] = "txrt";

    // magic codes used for chain identification and namespacing
    static const char MAGIC_CODE_MAIN = 0x3;
    static const char MAGIC_CODE_MAIN_EXTENDED = 0x4;
    static const char MAGIC_CODE_TEST = 0x6;
    static const char MAGIC_CODE_TEST_EXTENDED = 0x7;
    static const char MAGIC_CODE_REGTEST = 0x0;
    static const char MAGIC_CODE_REGTEST_EXTENDED = 0x1;

    // characters used when pretty-printing
    static const char colon = ':';
    static const char hyphen = '-';

    // denotes which Bech32 encoding was used for the txref
    enum Encoding {
        Invalid, // no or invalid encoding was detected
        Bech32,  // encoding used original checksum constant (1)
        Bech32m  // encoding used default checksum constant (M = 0x2bc830a3)
    };

    struct DecodedResult {
        std::string hrp;
        std::string txref;
        int blockHeight = 0;
        int transactionIndex = 0;
        int txoIndex = 0;
        int magicCode = 0;
        Encoding encoding = Encoding::Invalid;
        std::string commentary;
    };

    // encodes the position of a confirmed bitcoin transaction on the
    // mainnet network and returns a bech32 encoded "transaction
    // position reference" (txref). If txoIndex is greater than 0, then
    // an extended reference is returned (txref-ext). If txoIndex is zero,
    // but forceExtended=true, then an extended reference is returned
    // (txref-ext).
    std::string encode(
            int blockHeight,
            int transactionIndex,
            int txoIndex = 0,
            bool forceExtended = false,
            const std::string & hrp = BECH32_HRP_MAIN
    );

    // encodes the position of a confirmed bitcoin transaction on the
    // testnet network and returns a bech32 encoded "transaction
    // position reference" (txref). If txoIndex is greater than 0, then
    // an extended reference is returned (txref-ext). If txoIndex is zero,
    // but forceExtended=true, then an extended reference is returned
    // (txref-ext).
    std::string encodeTestnet(
            int blockHeight,
            int transactionIndex,
            int txoIndex = 0,
            bool forceExtended = false,
            const std::string & hrp = BECH32_HRP_TEST
    );

    // encodes the position of a confirmed bitcoin transaction on the
    // regtest network and returns a bech32 encoded "transaction
    // position reference" (txref). If txoIndex is greater than 0, then
    // an extended reference is returned (txref-ext). If txoIndex is zero,
    // but forceExtended=true, then an extended reference is returned
    // (txref-ext).
    std::string encodeRegtest(
            int blockHeight,
            int transactionIndex,
            int txoIndex = 0,
            bool forceExtended = false,
            const std::string & hrp = BECH32_HRP_REGTEST
    );

    // decodes a bech32 encoded "transaction position reference" (txref) and
    // returns identifying data
    DecodedResult decode(const std::string & txref);


    enum class InputParam { unknown, address, txid, txref, txrefext };

    // determine if the input string is a Bitcoin address, txid, txref, or txrefext. This
    // is not meant to be an exhaustive test--should only be used as a first pass to see
    // what sort of string might be passed in as input.
    InputParam classifyInputString(const std::string & str);


    namespace limits {

        const int TXREF_STRING_MIN_LENGTH = 18;                    // ex: "tx1rqqqqqqqqmhuqhp"
        const int TXREF_STRING_NO_HRP_MIN_LENGTH = 15;             // ex: "rqqqqqqqqmhuqhp"

        const int TXREF_EXT_STRING_MIN_LENGTH = 21;                // ex: "tx1yqqqqqqqqqqqksvh26"
        const int TXREF_EXT_STRING_NO_HRP_MIN_LENGTH = 18;         // ex: "yqqqqqqqqqqqksvh26"

        const int TXREF_STRING_MIN_LENGTH_TESTNET = 22;            // ex: "txtest1rqqqqqqqqmhuqhp"

        const int TXREF_EXT_STRING_MIN_LENGTH_TESTNET = 25;        // ex: "txtest18jk0uqayzu4xaw4hzl"

        const int TXREF_STRING_MIN_LENGTH_REGTEST = 20;            // ex: "txrt1q7lllllllps4p3p"

        const int TXREF_EXT_STRING_MIN_LENGTH_REGTEST = 23;        // ex: "txrt1p7lllllllpqqqa0dvp"

        const int TXREF_EXTRA_PRETTY_PRINT_CHARS = 4;              // ex: "tx1:rqqq-qqqq-qmhu-qhp"
        const int TXREF_EXT_EXTRA_PRETTY_PRINT_CHARS = 5;          // ex: "tx1:yqqq-qqqq-qqqq-ksvh-26"

        const int TXREF_MAX_LENGTH =
                TXREF_EXT_STRING_MIN_LENGTH_TESTNET + TXREF_EXT_EXTRA_PRETTY_PRINT_CHARS;

    }
}

#endif // #ifdef __cplusplus

// C bindings - structs and functions

#ifndef __cplusplus
#include <stddef.h>
#include <stdbool.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef enum txref_encoding_e
{
    INVALID = 0,
    BECH32,
    BECH32M
} txref_encoding;

typedef struct txref_DecodedResult_s {
    char * hrp;
    size_t hrplen;
    char * txref;
    size_t txreflen;
    int blockHeight;
    int transactionIndex;
    int txoIndex;
    int magicCode;
    txref_encoding encoding;
    char *commentary;
    size_t commentarylen;
} txref_DecodedResult;

typedef enum txref_error_e
{
    E_TXREF_SUCCESS = 0,
    E_TXREF_UNKNOWN_ERROR,
    E_TXREF_NULL_ARGUMENT,
    E_TXREF_LENGTH_TOO_SHORT,
    E_TXREF_MAX_ERROR
} txref_error;

extern const char *txref_errordesc[];

/**
 * Returns error message string corresponding to the error code
 *
 * @param error_code the error code to convert
 *
 * @return error message string corresponding to the error code
 */
extern const char * txref_strerror(txref_error error_code);

/**
 * Get MAX length any txref can be: main/testnet, regular or extended
 *
 * The length includes the final '\0' character
 *
 * @return MAX length for any txref
 */
extern size_t max_Txref_length();

/**
 * Allocates memory for a txref and returns a pointer.
 *
 * This memory will be able to handle any size txref.
 *
 * This memory must be freed using the free_Txref_storage function.
 *
 * @return a pointer to a new txref, or NULL if error
 */
extern char * create_Txref_storage();

/**
 * Frees memory for a txref
 */
extern void free_Txref_storage(char *txref);

/**
 * Allocates memory for a txref_DecodedResult struct and returns a pointer.
 *
 * This struct will be able to handle any size txref.
 *
 * This memory must be freed using the free_DecodedResult_storage function.
 *
 * @return a pointer to a new txref_DecodedResult struct, or NULL if error
 */
extern txref_DecodedResult * create_DecodedResult_storage();

/**
 * Frees memory for a txref_DecodedResult struct.
 */
extern void free_DecodedResult_storage(txref_DecodedResult *decodedResult);

/**
 * encodes the position of a confirmed bitcoin transaction on the
 * mainnet network and returns a bech32 encoded "transaction position
 * reference" (txref). If txoIndex is greater than 0, then an extended
 * reference is returned (txref-ext). If txoIndex is zero, but
 * forceExtended=true, then an extended reference is returned (txref-ext).
 *
 * @param txref pointer to memory to copy the output encoded txref
 * @param txreflen number of bytes allocated at txref
 * @param blockHeight the block height of block containing the transaction to encode
 * @param transactionIndex the transaction index within the block of the transaction to encode
 * @param txoIndex the txo index within the transaction of the transaction to encode
 * @param forceExtended if true, will encode an extended txref, even if txoIndex is 0
 * @param hrp the "human-readable part" for the bech32 encoding (normally "tx")
 * @param hrplen the length of the "human-readable part" string
 *
 * @return E_TXREF_SUCCESS on success, others on error
 */
extern txref_error txref_encode(
        char * txref,
        size_t txreflen,
        int blockHeight,
        int transactionIndex,
        int txoIndex,
        bool forceExtended,
        const char * hrp,
        size_t hrplen);

/**
 * encodes the position of a confirmed bitcoin transaction on the
 * testnet network and returns a bech32 encoded "transaction position
 * reference" (txref). If txoIndex is greater than 0, then an extended
 * reference is returned (txref-ext). If txoIndex is zero, but
 * forceExtended=true, then an extended reference is returned (txref-ext).
 *
 * @param txref pointer to memory to copy the output encoded txref
 * @param txreflen number of bytes allocated at txref
 * @param blockHeight the block height of block containing the transaction to encode
 * @param transactionIndex the transaction index within the block of the transaction to encode
 * @param txoIndex the txo index within the transaction of the transaction to encode
 * @param forceExtended if true, will encode an extended txref, even if txoIndex is 0
 * @param hrp the "human-readable part" for the bech32 encoding (normally "txtest")
 * @param hrplen the length of the "human-readable part" string
 *
 * @return E_TXREF_SUCCESS on success, others on error
 */
extern txref_error txref_encodeTestnet(
        char * txref,
        size_t txreflen,
        int blockHeight,
        int transactionIndex,
        int txoIndex,
        bool forceExtended,
        const char * hrp,
        size_t hrplen);

/**
 * encodes the position of a confirmed bitcoin transaction on the
 * regtest network and returns a bech32 encoded "transaction position
 * reference" (txref). If txoIndex is greater than 0, then an extended
 * reference is returned (txref-ext). If txoIndex is zero, but
 * forceExtended=true, then an extended reference is returned (txref-ext).
 *
 * @param txref pointer to memory to copy the output encoded txref
 * @param txreflen number of bytes allocated at txref
 * @param blockHeight the block height of block containing the transaction to encode
 * @param transactionIndex the transaction index within the block of the transaction to encode
 * @param txoIndex the txo index within the transaction of the transaction to encode
 * @param forceExtended if true, will encode an extended txref, even if txoIndex is 0
 * @param hrp the "human-readable part" for the bech32 encoding (normally "txrt")
 * @param hrplen the length of the "human-readable part" string
 *
 * @return E_TXREF_SUCCESS on success, others on error
 */
extern txref_error txref_encodeRegtest(
        char * txref,
        size_t txreflen,
        int blockHeight,
        int transactionIndex,
        int txoIndex,
        bool forceExtended,
        const char * hrp,
        size_t hrplen);

/**
 * decodes a bech32 encoded "transaction position reference" (txref) and
 * returns identifying data
 *
 * @param decodedResult pointer to struct to copy the decoded transaction data
 * @param txref the txref string to decode
 * @param txreflen the length of the txref string
 *
 * @return E_TXREF_SUCCESS on success, others on error
 */
extern txref_error txref_decode(
        txref_DecodedResult *decodedResult,
        const char * txref,
        size_t txreflen);


#ifdef __cplusplus
}
#endif

#endif //TXREF_TXREF_H
