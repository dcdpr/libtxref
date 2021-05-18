# libtxref

This is a library for converting between bitcoin transaction IDs (TxIDs)
and "Bech32 Encoded Tx Position References" (TxRefs). TxRefs are
described in [BIP 0136](https://github.com/bitcoin/bips/blob/master/bip-0136.mediawiki).

## Usage Examples

### C++ Encoding Examples

See [the full code for the following examples](https://raw.githubusercontent.com/dcdpr/libtxref/master/examples/cpp_example.cpp).

#### Create a txref for a mainnet transaction, with only a blockHeight and transactionPosition:

```cpp
    int blockHeight = 10000;
    int transactionPosition = 2;

    std::string txref = txref::encode(blockHeight, transactionPosition);

    assert(txref == "tx1:rq3n-qqzq-qk8k-mzd");
```

#### Create a txref for a testnet transaction, with only a blockHeight and transactionPosition:

```cpp
    int blockHeight = 10000;
    int transactionPosition = 4;

    std:string txref = txref::encodeTestnet(blockHeight, transactionPosition);

    assert(txref == "txtest1:xq3n-qqyq-qhrg-gy3");
```

#### Create an extended txref for a mainnet transaction, with a blockHeight and transactionPosition and a specific txoIndex:

```cpp
    int blockHeight = 10000;
    int transactionPosition = 2;
    int txoIndex = 3;

    std::string txref = txref::encode(blockHeight, transactionPosition, txoIndex);

    assert(txref == "tx1:yq3n-qqzq-qrqq-9z4d-2n");
```

#### Create an extended txref for a testnet transaction, with a blockHeight and transactionPosition and a specific txoIndex

```cpp
    int blockHeight = 10000;
    int transactionPosition  = 4;
    int txoIndex = 6;

    std::string txref = txref::encodeTestnet(blockHeight, transactionPosition, txoIndex);

    assert(txref == "txtest1:8q3n-qqyq-qxqq-v3x4-ze");
```

### C++ Decoding Examples

See [the full code for the following examples](https://raw.githubusercontent.com/dcdpr/libtxref/master/examples/cpp_example.cpp).

#### Decode a txref

```cpp
    txref::DecodedResult decodedResult = txref::decode("tx1:yq3n-qqzq-qrqq-9z4d-2n");

    assert(decodedResult.hrp == "tx");
    assert(decodedResult.magicCode == txref::MAGIC_BTC_MAIN_EXTENDED);
    assert(decodedResult.blockHeight == 10000);
    assert(decodedResult.transactionPosition == 2);
    assert(decodedResult.txoIndex == 3);
    assert(decodedResult.encoding == txref::Bech32m);
```

#### Decode an "original" txref

This txref has been encoded with the original bech32 internal constant. The
decoding will still succeed, but the library will also return a commentary
string that contains an explanatory message along with the txref that should
be used instead.

```cpp
    txref::DecodedResult decodedResult = txref::decode("txtest1:xjk0-uqay-zat0-dz8");

    assert(decodedResult.hrp == "txtest");
    assert(decodedResult.magicCode == txref::MAGIC_BTC_TEST);
    assert(decodedResult.blockHeight == 466793);
    assert(decodedResult.transactionPosition == 2205);
    assert(decodedResult.txoIndex == 0);
    assert(decodedResult.encoding == txref::Bech32);

    std::cout << decodedResult.getCommentary() << "\n";
    // prints:
    // "The txref txtest1:xjk0-uqay-zat0-dz8 uses an old encoding scheme and should be updated to txtest1:xjk0-uqay-zghl-p89 See https://github.com/dcdpr/libtxref#regarding-bech32-checksums for more information."
```

### C Encoding Example

See [the full code for the following example](https://raw.githubusercontent.com/dcdpr/libtxref/master/examples/c_example.cpp).

#### Create a txref for a mainnet transaction, with only a blockHeight and transactionPosition:

```C
    char main_hrp[] = "tx";
    int blockHeight = 10000;
    int transactionPosition = 2;
    int txoIndex = 3;
    char *txref = create_Txref_storage();
    assert(txref_encode(txref, max_Txref_length(), blockHeight, transactionPosition, txoIndex,
                        false, main_hrp, sizeof(main_hrp)) == E_TXREF_SUCCESS);
    free_Txref_storage(txref);
```

### C Decoding Example

See [the full code for the following example](https://raw.githubusercontent.com/dcdpr/libtxref/master/examples/c_example.cpp).

#### Decode a txref

```C
    char *txref = create_Txref_storage();
    strcpy(txref, "tx1:rq3n-qqzq-qk8k-mzd");
    assert(txref_decode(decodedResult, txref, strlen(txref) + 1) == E_TXREF_SUCCESS);
    assert(decodedResult->blockHeight == 10000);
    assert(decodedResult->transactionPosition == 2);
    assert(decodedResult->txoIndex == 0);
    free_Txref_storage(txref);
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

### Installing prerequirements

If the above doesn't work, you probably need to install some
prerequirements. For example, on a fresh Debian Stretch system:

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

