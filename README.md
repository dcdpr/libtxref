# libtxref

This is a library for converting between bitcoin transaction IDs (TxIDs), bitcoin transaction
coordinates (blockHeight, transactionIndex, and txoIndex),
and "Bech32 Encoded Tx Position References" (TxRefs). TxRefs are
described in [BIP 0136](https://github.com/bitcoin/bips/blob/master/bip-0136.mediawiki).

## Usage Examples

### C++ Encoding Examples

See [the full code for the following examples](examples/cpp_other_examples.cpp).

#### Create a txref for a mainnet transaction, with only a blockHeight and transactionIndex:

```cpp
    int blockHeight = 10000;
    int transactionIndex = 2;

    std::string txref = txref::encode(blockHeight, transactionIndex);

    assert(txref == "tx1:rq3n-qqzq-qk8k-mzd");
```

#### Create a txref for a testnet transaction, with only a blockHeight and transactionIndex:

```cpp
    int blockHeight = 10000;
    int transactionIndex = 4;

    std:string txref = txref::encodeTestnet(blockHeight, transactionIndex);

    assert(txref == "txtest1:xq3n-qqyq-qhrg-gy3");
```

#### Create an extended txref for a mainnet transaction, with a blockHeight, transactionIndex, and txoIndex:

```cpp
    int blockHeight = 10000;
    int transactionIndex = 2;
    int txoIndex = 3;

    std::string txref = txref::encode(blockHeight, transactionIndex, txoIndex);

    assert(txref == "tx1:yq3n-qqzq-qrqq-9z4d-2n");
```

#### Create an extended txref for a testnet transaction, with a blockHeight, transactionIndex, and txoIndex

```cpp
    int blockHeight = 10000;
    int transactionIndex  = 4;
    int txoIndex = 6;

    std::string txref = txref::encodeTestnet(blockHeight, transactionIndex, txoIndex);

    assert(txref == "txtest1:8q3n-qqyq-qxqq-v3x4-ze");
```

### C++ Decoding Examples

See [the full code for the following examples](examples/cpp_other_examples.cpp).

#### Decode a txref

```cpp
    txref::DecodedResult decodedResult = txref::decode("tx1:yq3n-qqzq-qrqq-9z4d-2n");

    assert(decodedResult.hrp == "tx");
    assert(decodedResult.magicCode == txref::MAGIC_CODE_MAIN_EXTENDED);
    assert(decodedResult.blockHeight == 10000);
    assert(decodedResult.transactionIndex == 2);
    assert(decodedResult.txoIndex == 3);
    assert(decodedResult.encoding == txref::Encoding::Bech32m);
```

#### Decode an "original" txref

This txref has been encoded with the original bech32 internal constant. The
decoding will still succeed, but the library will also return a commentary
string that contains an explanatory message along with the txref that should
be used instead.

```cpp
    txref::DecodedResult decodedResult = txref::decode("txtest1:xjk0-uqay-zat0-dz8");

    assert(decodedResult.hrp == "txtest");
    assert(decodedResult.magicCode == txref::MAGIC_CODE_TEST);
    assert(decodedResult.blockHeight == 466793);
    assert(decodedResult.transactionIndex == 2205);
    assert(decodedResult.txoIndex == 0);
    assert(decodedResult.encoding == txref::Encoding::Bech32); // note: Bech32 rather than Bech32m

    std::cout << decodedResult.commentary << "\n";
    // prints:
    // "The txref txtest1:xjk0-uqay-zat0-dz8 uses an old encoding scheme and should be updated to txtest1:xjk0-uqay-zghl-p89 See https://github.com/dcdpr/libtxref#regarding-bech32-checksums for more information."
```

### C Encoding Example

See [the full code for the following example](examples/c_usage_encoding_example.c).

#### Create a txref for a mainnet transaction

```C
    int blockHeight = 10000;
    int transactionIndex = 2;
    int txoIndex = 3;
    char main_hrp[] = "tx";
    
    txref_tstring * tstring = txref_create_tstring();
    if(!tstring)
        return E_TXREF_NO_MEMORY;

    txref_error err = txref_encode(tstring, blockHeight, transactionIndex, txoIndex, false, main_hrp);
    if(err != E_TXREF_SUCCESS) {
        txref_free_tstring(tstring);
        return err;
    }
    
    assert(strcmp(tstring->string, "tx1:yq3n-qqzq-qrqq-9z4d-2n") == 0);
    
    txref_free_tstring(tstring);
```

### C Decoding Example

See [the full code for the following example](examples/c_usage_decoding_example.c).

#### Decode a txref

```C
    char txref[] = "tx1:yq3n-qqzq-qrqq-9z4d-2n";

    txref_DecodedResult *decodedResult = txref_create_DecodedResult();
    if(!decodedResult)
        return E_TXREF_NO_MEMORY;
    
    txref_error err = txref_decode(decodedResult, txref);
    if(err != E_TXREF_SUCCESS) {
        txref_free_DecodedResult(decodedResult);
        return err;
    }
    
    assert(decodedResult->blockHeight == 10000);
    assert(decodedResult->transactionIndex == 2);
    assert(decodedResult->txoIndex == 3);

    txref_free_DecodedResult(decodedResult);
```

## Building libtxref

To build libtxref, you will need:

* cmake
* g++, clang or Visual Studio (community edition)

libtxref uses a pretty standard cmake build system:

```
mkdir build
cd build
cmake ..
make
```

You can also run all the tests:

```
make test
```

### Installing prerequisites

If the above doesn't work, you probably need to install some
prerequisites. For example, on a fresh Debian 10 ("buster") system:

```
sudo apt-get update
sudo apt-get install make gcc g++
```

It is worth getting the latest cmake (check [cmake.org](https://cmake.org/download/) for
the latest release), so install that the hard way:

```
sudo apt-get install libssl-dev # recent cmake needs ssl
wget https://github.com/Kitware/CMake/releases/download/v3.20.1/cmake-3.20.1.tar.gz
wget https://github.com/Kitware/CMake/releases/download/v3.20.1/cmake-3.20.1-SHA-256.txt
shasum -a 256 --ignore-missing -c cmake-3.20.1-SHA-256.txt # make sure this says "OK"
tar xzf cmake-3.20.1.tar.gz
cd cmake-3.20.1
./configure
make 
sudo make install
```

Now you can again try to build libtxref.

### libbech32

If you got this code by cloning the github repo:

```
git clone https://github.com/dcdpr/libtxref.git
```

then you will need to do a few extra steps to make sure you have everything:

```
cd libbech32
git submodule init
git submodule update
```

There is another way to do this which is a little simpler, however. If
you pass --recurse-submodules to the git clone command, it will
automatically initialize and update each submodule in the repository:

```
git clone --recurse-submodules https://github.com/dcdpr/libtxref.git
```

## Regarding bech32 checksums

The Bech32 data encoding format was first proposed by Pieter Wuille in early 2017 in
[BIP 0173](https://github.com/bitcoin/bips/blob/master/bip-0173.mediawiki). Later, in November 2019, Pieter published
some research that a constant used in the bech32 checksum algorithm (value = 1) may not be
optimal for the error detecting properties of bech32. In February 2021, Pieter published
[BIP 0350](https://github.com/bitcoin/bips/blob/master/bip-0350.mediawiki) reporting that "exhaustive analysis" showed the best possible constant value is
0x2bc830a3. This improved variant of Bech32 is called "Bech32m".

When decoding a txref, libtxref returns an enum value showing whether bech32m or bech32
was used to encode. If the original bech32 variant is detected, libtxref also returns a
commentary string that can be shown to the user. This commentary will contain a new txref that represents
the same transaction data, but using the new bech32m variant. This can be seen in the examples above.

When encoding data, libtxref will only use the new constant value of 0x2bc830a3.

