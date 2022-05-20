
#include "txref.h"
#include "bech32.h"
#include <algorithm>
#include <vector>
#include <stdexcept>
#include <sstream>
#include <cassert>

namespace {

    using namespace txref;
    using namespace txref::limits;

    const int MAX_BLOCK_HEIGHT         = 0xFFFFFF; // 16777215

    const int MAX_TRANSACTION_INDEX    = 0x7FFF;   // 32767

    const int MAX_TXO_INDEX            = 0x7FFF;   // 32767

    const int MAX_MAGIC_CODE           = 0x1F;

    const int DATA_SIZE                = 9;

    const int DATA_EXTENDED_SIZE       = 12;


    bool isStandardSize(unsigned long dataSize) {
        return dataSize == DATA_SIZE;
    }

    bool isExtendedSize(unsigned long dataSize) {
        return dataSize == DATA_EXTENDED_SIZE;
    }

    bool isDataSizeValid(unsigned long dataSize) {
        return isStandardSize(dataSize) || isExtendedSize(dataSize);
    }

    // is a txref string, missing the HRP, still of a valid length for a txref?
    bool isLengthValid(unsigned long length) {
        return length == TXREF_STRING_NO_HRP_MIN_LENGTH ||
               length == TXREF_EXT_STRING_NO_HRP_MIN_LENGTH;
    }

    // a block's height can only be in a certain range
    void checkBlockHeightRange(int blockHeight) {
        if(blockHeight < 0 || blockHeight > MAX_BLOCK_HEIGHT)
            throw std::runtime_error("block height is too large");
    }

    // a transaction's index can only be in a certain range
    void checkTransactionIndexRange(int transactionIndex) {
        if(transactionIndex < 0 || transactionIndex > MAX_TRANSACTION_INDEX)
            throw std::runtime_error("transaction index is too large");
    }

    // a TXO's index can only be in a certain range
    void checkTxoIndexRange(int txoIndex) {
        if(txoIndex < 0 || txoIndex > MAX_TXO_INDEX)
            throw std::runtime_error("txo index is too large");
    }

    // the magic code can only be in a certain range
    void checkMagicCodeRange(int magicCode) {
        if(magicCode < 0 || magicCode > MAX_MAGIC_CODE)
            throw std::runtime_error("magic code is too large");
    }

    // check that the magic code is for one of the extended txrefs
    void checkExtendedMagicCode(int magicCode) {
        if(magicCode != txref::MAGIC_CODE_MAIN_EXTENDED &&
           magicCode != txref::MAGIC_CODE_TEST_EXTENDED &&
           magicCode != txref::MAGIC_CODE_REGTEST_EXTENDED)
            throw std::runtime_error("magic code does not support extended txrefs");
    }

    // separate groups of chars in the txref string to make it look nicer
    std::string addGroupSeparators(
            const std::string & raw,
            std::string::size_type hrplen,
            std::string::difference_type separatorOffset = 4) {

        if(hrplen > bech32::limits::MAX_HRP_LENGTH)
            throw std::runtime_error("HRP must be less than 84 characters long");

        if(separatorOffset < 1)
            throw std::runtime_error("separatorOffset must be > 0");

        auto rawLength = static_cast<std::string::difference_type>(raw.length());
        auto hrpLength = static_cast<std::string::difference_type>(hrplen);

        if(rawLength < 2)
            throw std::runtime_error("Can't add separator characters to strings with length < 2");

        if(rawLength == hrpLength) // no separators needed
            return raw;

        if(rawLength < hrpLength)
            throw std::runtime_error("HRP length can't be greater than input length");

        // number of separators that will be inserted
        auto numSeparators = (rawLength - hrpLength - 1) / separatorOffset;

        // output length
        auto outputLength = static_cast<std::string::size_type>(rawLength + numSeparators);

        // create output string, starting with all hyphens
        std::string output(outputLength, txref::hyphen);

        // copy over the raw string, skipping every offset # chars, after the HRP
        std::string::difference_type rawPos = 0;
        std::string::size_type outputPos = 0;
        for(const auto &c : raw) {

            output[outputPos++] = c;

            ++rawPos;
            if(rawPos > hrpLength && (rawPos - hrpLength) % separatorOffset == 0)
                ++outputPos;
        }

        return output;
    }

    // pretty print a txref returned by bech32::encode()
    std::string prettyPrint(
            const std::string & plain,
            std::string::size_type hrplen) {

        std::string result = plain;

        // add colon after the HRP and bech32 separator character
        auto hrpPlusSeparatorLength =
                static_cast<std::string::difference_type>(hrplen + 1);
        result.insert(result.cbegin()+hrpPlusSeparatorLength, txref::colon);

        // now add hyphens every 4th character after that
        auto hrpPlusSeparatorAndColonLength = hrplen + 2;
        result = addGroupSeparators(result, hrpPlusSeparatorAndColonLength);

        return result;
    }

    // extract the magic code from the decoded data part
    void extractMagicCode(uint8_t & magicCode, const std::vector<unsigned char> &dp) {
        assert(!dp.empty());
        magicCode = dp[0];
    }

    // extract the version from the decoded data part
    void extractVersion(uint8_t & version, const std::vector<unsigned char> &dp) {
        assert(dp.size() > 1);
        version = dp[1] & 0x1u;
    }

    // extract the block height from the decoded data part
    void extractBlockHeight(int & blockHeight, const std::vector<unsigned char> &dp) {
        uint8_t version = 0;
        extractVersion(version, dp);

        if(version == 0) {
            blockHeight = (dp[1] >> 1u);
            blockHeight |= (dp[2] << 4u);
            blockHeight |= (dp[3] << 9u);
            blockHeight |= (dp[4] << 14u);
            blockHeight |= (dp[5] << 19u);
        }
        else {
            std::stringstream ss;
            ss << "Unknown txref version detected: " << static_cast<int>(version);
            throw std::runtime_error(ss.str());
        }
    }

    // extract the transaction index from the decoded data part
    void extractTransactionIndex(int & transactionIndex, const std::vector<unsigned char> &dp) {
        uint8_t version = 0;
        extractVersion(version, dp);

        if(version == 0) {
            transactionIndex = dp[6];
            transactionIndex |= (dp[7] << 5u);
            transactionIndex |= (dp[8] << 10u);
        }
        else {
            std::stringstream ss;
            ss << "Unknown txref version detected: " << static_cast<int>(version);
            throw std::runtime_error(ss.str());
        }
    }

    // extract the TXO index from the decoded data part
    void extractTxoIndex(int &txoIndex, const std::vector<unsigned char> &dp) {
        if(dp.size() < 12) {
            // non-extended txrefs don't store the txoIndex, so just return 0
            txoIndex = 0;
            return;
        }

        uint8_t version = 0;
        extractVersion(version, dp);

        if(version == 0) {
            txoIndex = dp[9];
            txoIndex |= (dp[10] << 5u);
            txoIndex |= (dp[11] << 10u);
        }
        else {
            std::stringstream ss;
            ss << "Unknown txref version detected: " << static_cast<int>(version);
            throw std::runtime_error(ss.str());
        }
    }

    // some txref strings may have had the HRP stripped off. Attempt to prepend one if needed.
    // assumes that bech32::stripUnknownChars() has already been called
    std::string addHrpIfNeeded(const std::string & txref) {
        if(isLengthValid(txref.length()) && (txref.at(0) == 'r' || txref.at(0) == 'y')) {
            return std::string(txref::BECH32_HRP_MAIN) + bech32::separator + txref;
        }
        if(isLengthValid(txref.length()) && (txref.at(0) == 'x' || txref.at(0) == '8')) {
            return std::string(txref::BECH32_HRP_TEST) + bech32::separator + txref;
        }
        if(isLengthValid(txref.length()) && (txref.at(0) == 'q' || txref.at(0) == 'p')) {
            return std::string(txref::BECH32_HRP_REGTEST) + bech32::separator + txref;
        }
        return txref;
    }

    // upper-case or mixed-case txrefs will fail bech32 decoding, so we should lower-case if needed.
    std::string convertToLowercase(const std::string & txref) {
        std::string str = txref;
        std::transform(str.begin(), str.end(), str.begin(), &::tolower);
        return str;
    }

    // returns true if cleaned txref contains upper-case characters. "cleaned" means that the
    // txref has already been processed by bech32::stripUnknownChars() and only contains alpha-numeric
    // characters from the valid bech32 charset.
    bool cleanTxrefContainsUppercaseCharacters(const std::string & txref) {
        return std::any_of(txref.begin(), txref.end(), &::isupper);
    }

    // returns true if cleaned txref contains lower-case characters. "cleaned" means that the
    // txref has already been processed by bech32::stripUnknownChars() and only contains alpha-numeric
    // characters from the valid bech32 charset.
    bool cleanTxrefContainsLowercaseCharacters(const std::string & txref) {
        return std::any_of(txref.begin(), txref.end(), &::islower);
    }

    // returns true if cleaned txref contains mixed-case characters. "cleaned" means that the
    // txref has already been processed by bech32::stripUnknownChars() and only contains alpha-numeric
    // characters from the valid bech32 charset.
    bool cleanTxrefContainsMixedcaseCharacters(const std::string & txref) {
        return cleanTxrefContainsLowercaseCharacters(txref) &&
               cleanTxrefContainsUppercaseCharacters(txref);
    }

    std::string txrefEncode(
            const std::string &hrp,
            int magicCode,
            int blockHeight,
            int transactionIndex) {

        checkBlockHeightRange(blockHeight);
        checkTransactionIndexRange(transactionIndex);
        checkMagicCodeRange(magicCode);

        // ranges have been checked. make unsigned copies of params
        auto bh = static_cast<uint32_t>(blockHeight);
        auto tp = static_cast<uint32_t>(transactionIndex);

        std::vector<unsigned char> dp(DATA_SIZE);

        // set the magic code
        dp[0] = static_cast<uint8_t>(magicCode);  // sets 1-3 bits in the 1st 5 bits

        // set version bit to 0
        dp[1] &= ~(1u << 0u);                     // sets 1 bit in 2nd 5 bits

        // set block height
        dp[1] |= (bh & 0xFu) << 1u;               // sets 4 bits in 2nd 5 bits
        dp[2] |= (bh & 0x1F0u) >> 4u;             // sets 5 bits in 3rd 5 bits
        dp[3] |= (bh & 0x3E00u) >> 9u;            // sets 5 bits in 4th 5 bits
        dp[4] |= (bh & 0x7C000u) >> 14u;          // sets 5 bits in 5th 5 bits
        dp[5] |= (bh & 0xF80000u) >> 19u;         // sets 5 bits in 6th 5 bits (24 bits total for blockHeight)

        // set transaction index
        dp[6] |= (tp & 0x1Fu);                    // sets 5 bits in 7th 5 bits
        dp[7] |= (tp & 0x3E0u) >> 5u;             // sets 5 bits in 8th 5 bits
        dp[8] |= (tp & 0x7C00u) >> 10u;           // sets 5 bits in 9th 5 bits (15 bits total for transactionIndex)

        // Bech32 encode
        std::string result = bech32::encode(hrp, dp);

        // add the dashes
        std::string output = prettyPrint(result, hrp.length());

        return output;
    }

    std::string txrefExtEncode(
            const std::string &hrp,
            int magicCode,
            int blockHeight,
            int transactionIndex,
            int txoIndex) {

        checkBlockHeightRange(blockHeight);
        checkTransactionIndexRange(transactionIndex);
        checkTxoIndexRange(txoIndex);
        checkMagicCodeRange(magicCode);
        checkExtendedMagicCode(magicCode);

        // ranges have been checked. make unsigned copies of params
        auto bh = static_cast<uint32_t>(blockHeight);
        auto tp = static_cast<uint32_t>(transactionIndex);
        auto ti = static_cast<uint32_t>(txoIndex);

        std::vector<unsigned char> dp(DATA_EXTENDED_SIZE);

        // set the magic code
        dp[0] = static_cast<uint8_t>(magicCode);  // sets 1-3 bits in the 1st 5 bits

        // set version bit to 0
        dp[1] &= ~(1u << 0u);                     // sets 1 bit in 2nd 5 bits

        // set block height
        dp[1] |= (bh & 0xFu) << 1u;               // sets 4 bits in 3rd 5 bits
        dp[2] |= (bh & 0x1F0u) >> 4u;             // sets 5 bits in 4th 5 bits
        dp[3] |= (bh & 0x3E00u) >> 9u;            // sets 5 bits in 5th 5 bits
        dp[4] |= (bh & 0x7C000u) >> 14u;          // sets 5 bits in 6th 5 bits
        dp[5] |= (bh & 0xF80000u) >> 19u;         // sets 5 bits in 7th 5 bits (24 bits total for blockHeight)

        // set transaction index
        dp[6] |= (tp & 0x1Fu);                    // sets 5 bits in 8th 5 bits
        dp[7] |= (tp & 0x3E0u) >> 5u;             // sets 5 bits in 9th 5 bits
        dp[8] |= (tp & 0x7C00u) >> 10u;           // sets 5 bits in 10th 5 bits (15 bits total for transactionIndex)

        // set txo index
        dp[9] |= ti & 0x1Fu;                      // sets 5 bits in 11th 5 bits
        dp[10] |= (ti & 0x3E0u) >> 5u;            // sets 5 bits in 12th 5 bits
        dp[11] |= (ti & 0x7C00u) >> 10u;          // sets 5 bits in 13th 5 bits (15 bits total for txoIndex)

        // Bech32 encode
        std::string result = bech32::encode(hrp, dp);

        // add the dashes
        std::string output = prettyPrint(result, hrp.length());

        return output;
    }

    InputParam classifyInputStringBase(const std::string & str) {

        // before testing for various txrefs, get rid of any unknown
        // characters, ex: dashes, periods
        std::string s = bech32::stripUnknownChars(str);

        if(s.length() == TXREF_STRING_MIN_LENGTH ||
           s.length() == TXREF_STRING_MIN_LENGTH_TESTNET ||
           s.length() == TXREF_STRING_MIN_LENGTH_REGTEST)
            return InputParam::txref;

        if(s.length() == TXREF_EXT_STRING_MIN_LENGTH ||
           s.length() == TXREF_EXT_STRING_MIN_LENGTH_TESTNET ||
           s.length() == TXREF_EXT_STRING_MIN_LENGTH_REGTEST)
            return InputParam::txrefext;

        return InputParam::unknown;
    }

    InputParam classifyInputStringMissingHRP(const std::string & str) {

        // before testing for various txrefs, get rid of any unknown
        // characters, ex: dashes, periods
        std::string s = bech32::stripUnknownChars(str);

        if(s.length() == TXREF_STRING_NO_HRP_MIN_LENGTH)
            return InputParam::txref;

        if(s.length() == TXREF_EXT_STRING_NO_HRP_MIN_LENGTH)
            return InputParam::txrefext;

        return InputParam::unknown;
    }

}

namespace txref {

    std::string encode(
            int blockHeight,
            int transactionIndex,
            int txoIndex,
            bool forceExtended,
            const std::string & hrp) {

        if(txoIndex == 0 && !forceExtended)
            return txrefEncode(hrp, MAGIC_CODE_MAIN, blockHeight, transactionIndex);

        return txrefExtEncode(hrp, MAGIC_CODE_MAIN_EXTENDED, blockHeight, transactionIndex, txoIndex);

    }

    std::string encodeTestnet(
            int blockHeight,
            int transactionIndex,
            int txoIndex,
            bool forceExtended,
            const std::string & hrp) {

        if(txoIndex == 0 && !forceExtended)
            return txrefEncode(hrp, MAGIC_CODE_TEST, blockHeight, transactionIndex);

        return txrefExtEncode(hrp, MAGIC_CODE_TEST_EXTENDED, blockHeight, transactionIndex, txoIndex);

    }

    std::string encodeRegtest(
            int blockHeight,
            int transactionIndex,
            int txoIndex,
            bool forceExtended,
            const std::string & hrp) {

        if(txoIndex == 0 && !forceExtended)
            return txrefEncode(hrp, MAGIC_CODE_REGTEST, blockHeight, transactionIndex);

        return txrefExtEncode(hrp, MAGIC_CODE_REGTEST_EXTENDED, blockHeight, transactionIndex, txoIndex);

    }

    DecodedResult decode(const std::string & txref) {

        std::string runningCommentary;
        std::string txrefClean = bech32::stripUnknownChars(txref);
        if(cleanTxrefContainsMixedcaseCharacters(txrefClean)) {
            txrefClean = convertToLowercase(txrefClean);
            runningCommentary += txref + " contains mixed-case characters, which is "
                                         "forbidden by the Bech32 spec. Please use ";
            runningCommentary += convertToLowercase(txref) + " instead. ";
        }
        txrefClean = addHrpIfNeeded(txrefClean);
        bech32::DecodedResult bech32DecodedResult = bech32::decode(txrefClean);

        auto hrpLength = bech32DecodedResult.hrp.length();
        auto dataSize = bech32DecodedResult.dp.size();

        if(hrpLength == 0 && dataSize == 0) {
            throw std::runtime_error("checksum is invalid");
        }
        if(!isDataSizeValid(dataSize)) {
            throw std::runtime_error("decoded dp size is incorrect");
        }

        uint8_t magicCode;
        extractMagicCode(magicCode, bech32DecodedResult.dp);

        DecodedResult result;
        result.txref = prettyPrint(txrefClean, bech32DecodedResult.hrp.length());
        result.hrp = bech32DecodedResult.hrp;
        result.magicCode = magicCode;
        extractBlockHeight(result.blockHeight, bech32DecodedResult.dp);
        extractTransactionIndex(result.transactionIndex, bech32DecodedResult.dp);
        extractTxoIndex(result.txoIndex, bech32DecodedResult.dp);

        if(bech32DecodedResult.encoding == bech32::Encoding::Bech32m) {
            result.encoding = Encoding::Bech32m;
        }
        else if(bech32DecodedResult.encoding == bech32::Encoding::Bech32) {
            result.encoding = Encoding::Bech32;
            std::string updatedTxref;
            if(result.magicCode == MAGIC_CODE_MAIN_EXTENDED || result.magicCode == MAGIC_CODE_TEST_EXTENDED || result.magicCode == MAGIC_CODE_REGTEST_EXTENDED) {
                updatedTxref = txrefExtEncode(result.hrp, result.magicCode, result.blockHeight, result.transactionIndex, result.txoIndex);
            }
            else {
                updatedTxref = txrefEncode(result.hrp, result.magicCode, result.blockHeight, result.transactionIndex);
            }
            runningCommentary += "The txref " + result.txref +
                                 " uses an old encoding scheme and should be updated to " + updatedTxref +
                                 " See https://github.com/dcdpr/libtxref#regarding-bech32-checksums for more information.";
        }
        if(!runningCommentary.empty())
            result.commentary = runningCommentary;

        return result;
    }

    InputParam classifyInputString(const std::string & str) {

        if(str.empty())
            return InputParam::unknown;

        // if exactly 64 chars in length, it is likely a transaction id
        if(str.length() == 64)
            return InputParam::txid;

        // if it starts with certain chars, and is of a certain length, it may be a bitcoin address
        if(str[0] == '1' || str[0] == '3' || str[0] == 'm' || str[0] == 'n' || str[0] == '2')
            if(str.length() >= 26 && str.length() < 36)
                return InputParam::address;

        // check if it could be a standard txref or txrefext
        InputParam baseResult = classifyInputStringBase(str);

        // check if it could be a truncated txref or txrefext (missing the HRP)
        InputParam missingResult = classifyInputStringMissingHRP(str);

        // if one result is 'unknown' and the other isn't, then return the good one
        if(baseResult != InputParam::unknown && missingResult == InputParam::unknown)
            return baseResult;
        if(baseResult == InputParam::unknown && missingResult != InputParam::unknown)
            return missingResult;

        // special case: if baseResult is 'txref_param' and missingResult is 'txrefext' then
        // we need to dig deeper as TXREF_STRING_MIN_LENGTH == TXREF_EXT_STRING_NO_HRP_MIN_LENGTH
        if (baseResult == InputParam::txref && missingResult == InputParam::txrefext) {
            if (str[0] == txref::BECH32_HRP_MAIN[0] && // 't'
                str[1] == txref::BECH32_HRP_MAIN[1] && // 'x'
                str[2] == bech32::separator)           // '1'
                return InputParam::txref;
            else
                return InputParam::txrefext;
        }

        // otherwise, just return
        assert(baseResult == InputParam::unknown);
        return baseResult;
    }

}

// C bindings - functions

const char *txref_errordesc[] = {
        "Success",
        "Unknown error",
        "Function argument was null",
        "Function argument length was too short",
        "Max error"
};

/**
 * Returns error message string corresponding to the error code
 *
 * @param error_code the error code to convert
 *
 * @return error message string corresponding to the error code
 */
extern "C"
const char * txref_strerror(txref_error error_code) {
    const char * result = "";
    if(error_code >= E_TXREF_SUCCESS && error_code < E_TXREF_MAX_ERROR) {
        result = txref_errordesc[error_code];
    }
    else
        result = txref_errordesc[E_TXREF_UNKNOWN_ERROR];
    return result;
}

/**
 * Allocates memory for a txref_tstring and returns a pointer.
 *
 * This memory will be able to handle any size txref.
 *
 * This memory must be freed using the txref_free_tstring function.
 *
 * @return a pointer to a new txref_tstring, or NULL if error
 */
extern "C"
txref_tstring * txref_create_tstring(void) {
    auto tstring = static_cast<txref_tstring *>(malloc(sizeof(txref_tstring)));
    if(tstring == nullptr)
        return nullptr;
    // allocate enough memory for the largest possible txref string, plus 1 for '\0'
    tstring->length = txref::limits::TXREF_MAX_LENGTH;
    tstring->string = static_cast<char *> (calloc(tstring->length + 1, 1));
    if(tstring->string == nullptr) {
        txref_free_tstring(tstring);
        return nullptr;
    }
    return tstring;
}

/**
 * Frees memory for a txref_tstring
 */
 extern "C"
void txref_free_tstring(txref_tstring *tstring) {
    if(tstring == nullptr)
        return;
    free(tstring->string);
    free(tstring);
 }

/**
 * Allocates memory for a txref_DecodedResult struct and returns a pointer.
 *
 * This struct will be able to handle any size txref.
 *
 * This memory must be freed using the txref_free_DecodedResult function.
 *
 * @return a pointer to a new txref_DecodedResult struct, or NULL if error
 */
extern "C"
txref_DecodedResult * txref_create_DecodedResult(void) {
    auto decodedResult =
            static_cast<txref_DecodedResult *> (calloc(1, sizeof(txref_DecodedResult)));
    if(decodedResult == nullptr)
        return nullptr;
    // allocate enough memory for the largest possible txref string, plus 1 for '\0'
    size_t txreflen = txref::limits::TXREF_MAX_LENGTH;
    decodedResult->txref = static_cast<char *> (calloc(txreflen + 1, 1));
    if(decodedResult->txref == nullptr) {
        free(decodedResult);
        return nullptr;
    }
    // allocate enough memory for the largest possible hrp string (txtest), plus 1 for '\0'
    size_t hrplen =
            txref::limits::TXREF_EXT_STRING_MIN_LENGTH_TESTNET -
            txref::limits::TXREF_EXT_STRING_NO_HRP_MIN_LENGTH -
            1; // - 1 for the separator character '1' which is included in TXREF_EXT_STRING_MIN_LENGTH_TESTNET
    decodedResult->hrp = static_cast<char *> (calloc(hrplen + 1, 1));
    if(decodedResult->hrp == nullptr) {
        free(decodedResult->txref);
        free(decodedResult);
        return nullptr;
    }
    // note: we don't allocate memory for the commentary string as it will only be needed
    // in a few situations and can be allocated at that time.
    return decodedResult;
}

/**
 * Frees memory for a txref_DecodedResult struct.
 */
extern "C"
void txref_free_DecodedResult(txref_DecodedResult *decodedResult) {
    free(decodedResult->hrp);
    free(decodedResult->txref);
    free(decodedResult->commentary);
    free(decodedResult);
}

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
 * @param hrp the "human-readable part" for the bech32 encoding (normally "txtest")
 *
 * @return E_TXREF_SUCCESS on success, others on error
 */
extern "C"
txref_error txref_encode(
        txref_tstring * tstring,
        int blockHeight,
        int transactionIndex,
        int txoIndex,
        bool forceExtended,
        const char * hrp) {

    if(tstring == nullptr)
        return E_TXREF_NULL_ARGUMENT;
    if(tstring->string == nullptr)
        return E_TXREF_NULL_ARGUMENT;
    if(hrp == nullptr)
        return E_TXREF_NULL_ARGUMENT;

    std::string inputHrp(hrp);

    std::string outputTxref;
    try {
        outputTxref = txref::encode(blockHeight, transactionIndex, txoIndex, forceExtended, inputHrp);
    } catch (std::exception &) {
        // todo: convert exception message
        return E_TXREF_UNKNOWN_ERROR;
    }

    if(outputTxref.size() > tstring->length)
        return E_TXREF_LENGTH_TOO_SHORT;

    std::copy_n(outputTxref.begin(), outputTxref.size(), tstring->string);
    tstring->string[outputTxref.size()] = '\0';

    return E_TXREF_SUCCESS;
}

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
 * @param hrp the "human-readable part" for the bech32 encoding (normally "txtest")
 *
 * @return E_TXREF_SUCCESS on success, others on error
 */
extern "C"
txref_error txref_encodeTestnet(
        txref_tstring * tstring,
        int blockHeight,
        int transactionIndex,
        int txoIndex,
        bool forceExtended,
        const char * hrp) {

    if(tstring == nullptr)
        return E_TXREF_NULL_ARGUMENT;
    if(tstring->string == nullptr)
        return E_TXREF_NULL_ARGUMENT;
    if(hrp == nullptr)
        return E_TXREF_NULL_ARGUMENT;

    std::string inputHrp(hrp);

    std::string outputTxref;
    try {
        outputTxref = txref::encodeTestnet(blockHeight, transactionIndex, txoIndex, forceExtended, inputHrp);
    } catch (std::exception &) {
        // todo: convert exception message
        return E_TXREF_UNKNOWN_ERROR;
    }

    if(outputTxref.size() > tstring->length)
        return E_TXREF_LENGTH_TOO_SHORT;

    std::copy_n(outputTxref.begin(), outputTxref.size(), tstring->string);
    tstring->string[outputTxref.size()] = '\0';

    return E_TXREF_SUCCESS;
}

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
 * @param hrp the "human-readable part" for the bech32 encoding (normally "txrt")
 *
 * @return E_TXREF_SUCCESS on success, others on error
 */
extern "C"
txref_error txref_encodeRegtest(
        txref_tstring * tstring,
        int blockHeight,
        int transactionIndex,
        int txoIndex,
        bool forceExtended,
        const char * hrp) {

    if(tstring == nullptr)
        return E_TXREF_NULL_ARGUMENT;
    if(tstring->string == nullptr)
        return E_TXREF_NULL_ARGUMENT;
    if(hrp == nullptr)
        return E_TXREF_NULL_ARGUMENT;

    std::string inputHrp(hrp);

    std::string outputTxref;
    try {
        outputTxref = txref::encodeRegtest(blockHeight, transactionIndex, txoIndex, forceExtended, inputHrp);
    } catch (std::exception &) {
        // todo: convert exception message
        return E_TXREF_UNKNOWN_ERROR;
    }

    if(outputTxref.size() > tstring->length)
        return E_TXREF_LENGTH_TOO_SHORT;

    std::copy_n(outputTxref.begin(), outputTxref.size(), tstring->string);
    tstring->string[outputTxref.size()] = '\0';

    return E_TXREF_SUCCESS;
}

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
extern "C"
txref_error txref_decode(
        txref_DecodedResult *decodedResult,
        const char * txref) {

    if(decodedResult == nullptr)
        return E_TXREF_NULL_ARGUMENT;
    if(decodedResult->txref == nullptr)
        return E_TXREF_NULL_ARGUMENT;
    if(decodedResult->hrp == nullptr)
        return E_TXREF_NULL_ARGUMENT;
    if(txref == nullptr)
        return E_TXREF_NULL_ARGUMENT;

    std::string inputTxref(txref);

    txref::DecodedResult d;
    try {
        d = txref::decode(inputTxref);
    } catch (std::exception &) {
        // todo: convert exception message
        return E_TXREF_UNKNOWN_ERROR;
    }

    decodedResult->magicCode = d.magicCode;
    decodedResult->blockHeight = d.blockHeight;
    decodedResult->transactionIndex = d.transactionIndex;
    decodedResult->txoIndex = d.txoIndex;

    std::copy_n(d.hrp.begin(), d.hrp.size(), decodedResult->hrp);
    decodedResult->hrp[d.hrp.size()] = '\0';

    std::copy_n(d.txref.begin(), d.txref.size(), decodedResult->txref);
    decodedResult->txref[d.txref.size()] = '\0';

    decodedResult->encoding = static_cast<txref_encoding>(d.encoding);

    if(!d.commentary.empty()) {
        decodedResult->commentary = static_cast<char *> (calloc(d.commentary.size()+1, 1));
        std::copy_n(d.commentary.begin(), d.commentary.size(), decodedResult->commentary);
        decodedResult->commentary[d.commentary.size()] = '\0';
    }

    return E_TXREF_SUCCESS;
}
