# libtxref

This is a library for converting between bitcoin transaction IDs (txid)
and "Bech32 Encoded Tx Position References" (txref). Txrefs are
described in [BIP 0136](https://github.com/veleslavs/bips/blob/txrev_v2/bip-0136.mediawiki).

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
$ sudo apt-get install make gcc g++
```

It is worth getting the latest cmake, so install that the hard way:

```
wget https://cmake.org/files/v3.13/cmake-3.13.2.tar.gz
tar xzf cmake-3.13.2.tar.gz
cd cmake-3.13.2
./configure
make 
sudo make install
```

Now you can again try to build libtxref.

### libbech32

If you got this code by cloning the github repo:

```
git clone git@github.com:dcdpr/libtxref.git
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
git clone --recurse-submodules git@github.com:dcdpr/libtxref.git
```

### C++ Usage Example

```cpp
#include "libtxref.h"
#include <string>
#include <iostream>
#include <cassert>

int main() {

    // encode: mainnet, extended txref example

    int blockHeight = 10000;
    int transactionPosition = 2;
    int txoIndex = 3;

    std::string txref = txref::encode(blockHeight, transactionPosition, txoIndex);

    std::cout << "mainnet, extended txref for (blockHeight = 10000, transactionPosition=2, txoIndex=3):" << std::endl;
    std::cout << txref << "\n\n";
    assert(txref == "tx1:yq3n-qqzq-qrqq-0p67-s0");

    // decode

    txref::LocationData loc = txref::decode(txref);

    assert(loc.hrp == "tx");
    assert(loc.magicCode == txref::MAGIC_BTC_MAIN_EXTENDED);
    assert(loc.blockHeight == 10000);
    assert(loc.transactionPosition == 2);
    assert(loc.txoIndex == 3);
}
```

### C Usage Example

```C
#include "libtxref.h"
#include <string.h>
#include <stdio.h>
#include <assert.h>

int main() {
    char main_hrp[] = "tx";

    // encode: mainnet, extended txref example

    int blockHeight = 10000;
    int transactionPosition = 2;
    int txoIndex = 3;

    char *txref = create_Txref_storage();

    assert(txref_encode(txref, max_Txref_length(), blockHeight, transactionPosition, txoIndex,
                        false, main_hrp, sizeof(main_hrp)) == E_TXREF_SUCCESS);

    printf("mainnet, extended txref for (blockHeight = 10000, transactionPosition=2, txoIndex=3):\n%s\n\n", txref);
    assert(strcmp(txref, "tx1:yq3n-qqzq-qrqq-0p67-s0") == 0);

    // decode

    txref_LocationData *locationData = create_LocationData_storage();

    assert(txref_decode(locationData, txref, strlen(txref) + 1) == E_TXREF_SUCCESS);
    assert(locationData->blockHeight == 10000);
    assert(locationData->transactionPosition == 2);
    assert(locationData->txoIndex == 3);

    free_LocationData_storage(locationData);
    free_Txref_storage(txref);
}
```
