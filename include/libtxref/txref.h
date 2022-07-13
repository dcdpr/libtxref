
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

    // represents which bech32 encoding was used for a txref
    enum Encoding {
        Invalid, // no or invalid encoding was detected
        Bech32,  // encoding used original checksum constant (1)
        Bech32m  // encoding used default checksum constant (M = 0x2bc830a3)
    };

    // represents the data results from decoding a txref, including the transaction
    // coordinates, the bech32 encoding, and optional commentary information.
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

/**
* Represents a txref string.
*/
typedef struct txref_tstring_s {
    char * string;
    size_t length;
} txref_tstring;

/**
 * Represents which bech32 encoding was used for a txref
 */
typedef enum txref_encoding_e
{
    TXREF_ENCODING_INVALID,
    TXREF_ENCODING_BECH32,
    TXREF_ENCODING_BECH32M
} txref_encoding;

/**
 * Represents the results from decoding a txref, including the transaction
 * coordinates, the bech32 encoding, and optional commentary information.
 */
typedef struct txref_DecodedResult_s {
    char * hrp;
    char * txref;
    int blockHeight;
    int transactionIndex;
    int txoIndex;
    int magicCode;
    txref_encoding encoding;
    char *commentary;
} txref_DecodedResult;

/**
 * libtxref error codes
 */
typedef enum txref_error_e
{
    E_TXREF_SUCCESS = 0,
    E_TXREF_UNKNOWN_ERROR,
    E_TXREF_NULL_ARGUMENT,
    E_TXREF_LENGTH_TOO_SHORT,
    E_TXREF_NO_MEMORY,
    E_TXREF_MAX_ERROR
} txref_error;

/**
 * Error messages corresponding to the error codes
 */
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
 * Allocates memory for a txref_tstring based on the maximum possible size txref.
 *
 * This memory must be freed using txref_free_tstring().
 *
 * @return a pointer to a new txref, or NULL if error
 */
extern txref_tstring * txref_create_tstring(void);

/**
 * Frees memory for a txref_tstring
 *
 * @param tstring pointer to a txref_tstring struct
 */
extern void txref_free_tstring(txref_tstring * tstring);

/**
 * Allocates memory for a txref_DecodedResult struct based on the maximum possible size txref.
 *
 * This memory must be freed using txref_free_DecodedResult().
 *
 * @return a pointer to a new txref_DecodedResult struct, or NULL if error
 */
extern txref_DecodedResult * txref_create_DecodedResult(void);

/**
 * Frees memory for a txref_DecodedResult struct.
 *
 * @param decodedResult pointer to a txref_DecodedResult struct
 */
extern void txref_free_DecodedResult(txref_DecodedResult *decodedResult);

/**
 * encodes the position of a confirmed bitcoin transaction on the
 * mainnet network and returns a bech32 encoded "transaction position
 * reference" (txref). If txoIndex is greater than 0, then an extended
 * reference is returned (txref-ext). If txoIndex is zero, but
 * forceExtended=true, then an extended reference is returned (txref-ext).
 *
 * @param tstring pointer to a txref_tstring struct to copy the encoded txref
 * @param blockHeight the block height of block containing the transaction to encode
 * @param transactionIndex the transaction index within the block of the transaction to encode
 * @param txoIndex the txo index within the transaction of the transaction to encode
 * @param forceExtended if true, will encode an extended txref, even if txoIndex is 0
 * @param hrp pointer to a NULL-terminated string of the "human-readable part" for the bech32 encoding (normally "tx")
 *
 * @return E_TXREF_SUCCESS on success, others on error
 */
extern txref_error txref_encode(
        txref_tstring * tstring,
        int blockHeight,
        int transactionIndex,
        int txoIndex,
        bool forceExtended,
        const char * hrp);

/**
 * encodes the position of a confirmed bitcoin transaction on the
 * testnet network and returns a bech32 encoded "transaction position
 * reference" (txref). If txoIndex is greater than 0, then an extended
 * reference is returned (txref-ext). If txoIndex is zero, but
 * forceExtended=true, then an extended reference is returned (txref-ext).
 *
 * @param tstring pointer to a txref_tstring struct to copy the encoded txref
 * @param blockHeight the block height of block containing the transaction to encode
 * @param transactionIndex the transaction index within the block of the transaction to encode
 * @param txoIndex the txo index within the transaction of the transaction to encode
 * @param forceExtended if true, will encode an extended txref, even if txoIndex is 0
 * @param hrp pointer to a NULL-terminated string of the "human-readable part" for the bech32 encoding (normally "txtest")
 *
 * @return E_TXREF_SUCCESS on success, others on error
 */
extern txref_error txref_encodeTestnet(
        txref_tstring * tstring,
        int blockHeight,
        int transactionIndex,
        int txoIndex,
        bool forceExtended,
        const char * hrp);

/**
 * encodes the position of a confirmed bitcoin transaction on the
 * regtest network and returns a bech32 encoded "transaction position
 * reference" (txref). If txoIndex is greater than 0, then an extended
 * reference is returned (txref-ext). If txoIndex is zero, but
 * forceExtended=true, then an extended reference is returned (txref-ext).
 *
 * @param tstring pointer to a txref_tstring struct to copy the encoded txref
 * @param blockHeight the block height of block containing the transaction to encode
 * @param transactionIndex the transaction index within the block of the transaction to encode
 * @param txoIndex the txo index within the transaction of the transaction to encode
 * @param forceExtended if true, will encode an extended txref, even if txoIndex is 0
 * @param hrp pointer to a NULL-terminated string of the "human-readable part" for the bech32 encoding (normally "txrt")
 *
 * @return E_TXREF_SUCCESS on success, others on error
 */
extern txref_error txref_encodeRegtest(
        txref_tstring * tstring,
        int blockHeight,
        int transactionIndex,
        int txoIndex,
        bool forceExtended,
        const char * hrp);

/**
 * decodes a bech32 encoded "transaction position reference" (txref) and
 * returns identifying data
 *
 * @param decodedResult pointer to struct to copy the decoded transaction data
 * @param txref pointer to a NULL-terminated string of the txref to decode
 *
 * @return E_TXREF_SUCCESS on success, others on error
 */
extern txref_error txref_decode(
        txref_DecodedResult *decodedResult,
        const char * txref);


#ifdef __cplusplus
}
#endif

#endif //TXREF_TXREF_H
