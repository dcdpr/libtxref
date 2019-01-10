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

