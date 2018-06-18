#ifndef TXREF_TXID2TXREF_H
#define TXREF_TXID2TXREF_H

namespace t2t {

    struct Config {
        std::string rpcuser = "";
        std::string rpcpassword ="";
        std::string rpchost = "127.0.0.1";
        std::string query ="";
        int rpcport = 0;
        int txoIndex = 0;
        bool forceExtended = false;
    };

    struct Transaction {
        std::string txid = "";
        std::string txref = "";
        std::string network = "";
        int blockHeight = 0;
        int position = 0;
        int txoIndex = 0;
        std::string query = "";
    };

}

#endif //TXREF_TXID2TXREF_H
