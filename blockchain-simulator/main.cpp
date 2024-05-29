// BLOCKCHAIN SIMULATOR
//
// This project is a simple blockchain simulator.
// It tries to cover the basics of what a blockchain does behind the scenes using OOP principles.
//
// It has several missing features (like transaction signing) since it tries not to dive into technicalities too much.
// User inputs are not fully sanitized, just some simple checks are performed.
// Large numbers might break the program.
// 
// It somewhat follows the model of Ethereum (in terms of funds handling)
//
// The menu was tested in Windows Command Prompt
//
// Note: Inside the menu, the slow printing can be skipped by pressing enter. The exe can also be ran with --fast parameter.

#include <iostream>
#include <cstring>
#include <sstream>
#include <list>
#include <cmath>
#include <unordered_map>
#include <chrono>
#include <thread>
#include <conio.h>
#include <variant>
#include <iomanip>

// define colours for the console
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_RED     "\x1b[91m"
#define ANSI_COLOR_RESET   "\x1b[0m"

using namespace std;
class Block;

// some utility functions useful at different points in the code
void hashFunc(unsigned long long &h, long data){
    // this is a simple hashing function (I don't expect it to be cryptographically strong)
    auto rotateRight = [](unsigned long long x, int n) {
        return (x >> n) | (x << (64 - n));
    };

    // perform 11 rounds of mixing (arbitrary value)
    for (int i = 0; i < 11; i++){
        h ^= data + 0x9e3779b97f4a7c15 + (h << 6) + (h >> 2);   // the constant is commonly used for having better dispersion

        h += (h << 3);
        h ^= rotateRight(h, 11);
        h += (h << 15);
    }
}

bool isProperHex(string str){
    // check if all characters in a string are hex digits
    for (int i = 0; i < str.length(); i++)
        if (!isxdigit(str[i]))          // we check if all characters are hex digits
            return false;
    return true;
}

bool isAddress(string addr){
    // checks if a hex string is a proper address
    if (addr.length() != 42)            // we check if the address has proper length
        return false;
    if (!isProperHex(addr.substr(2)))   // check if it's a hex string (we exclude the '0x' prefix)
        return false;
    return true;
}

string generateRandomHex(){
    // generates a random address (a random hex string)
    // since it has length 40, we can consider it an address too
    stringstream ss;
    ss << "0x";
    for (int i = 0; i < 40; i++)
        ss << hex << rand() % 16;
    return ss.str();
}

// functions for different categories of console messages

void info(string msg){
    cout << ANSI_COLOR_CYAN << "INFO: " << ANSI_COLOR_RESET << msg << endl;
}

void warning(string msg){
    cout << ANSI_COLOR_YELLOW << "WARNING: " << ANSI_COLOR_RESET << msg << endl;
}

void sysMessage(string msg){
    cout << ANSI_COLOR_RED << "SYS: " << ANSI_COLOR_RESET << msg << endl;
}

// ----------------- TRANSACTION -----------------

class Transaction{
    string hash;        // hash of the other fields (id of transaction)
    string from, to;
    int amount, fee;    // !!! amount and fee are expressed as 1/100 of a unit of coin (amount = 102 <=> user has 1.02 coins)
                        // in some places they are represented as floats for UX but are stored as integers
    int nonce;          // nonce of the transaction (used to prevent double spending)
    bool isMined;       // under normal circumstances only the blockchain changes this value to true
    static const string godAddress;  // special address

    public:
        // CONSTRUCTORS
        Transaction();
        Transaction(string from, string to, int);
        Transaction(string from, string to, int amount, int nonce, int fee);
        Transaction(string from, string to, int amount, int fee, int nonce, bool isMined);
        Transaction(string hash, string from, string to, int amount, int fee, int nonce, bool isMined);
        Transaction(const Transaction&);

        // utility functions
        string calculateHash() const;
        void updateHash();
        bool isMineable() const;

        // OPERATORS
        Transaction& operator=(const Transaction&);
        variant<string, int, bool> operator[](int);
        Transaction& operator++();
        Transaction operator++(int);
        Transaction operator+(float);
        Transaction operator-(float);
        bool operator<(const Transaction&);
        bool operator>(const Transaction &obj);
        bool operator==(const Transaction &obj);
        operator string();
        operator string() const;
        operator int() const;

        // GETTERS
        string getHash() const;
        string getFrom() const;
        string getTo() const;
        long getAmount() const;
        long getFee() const;
        int getNonce() const;
        bool getIsMined() const;
        static const string getGodAddress();

        //SETTERS
        void setFrom(string from);
        void setTo(string to);
        void setAmount(int amount);
        void setFee(int fee);
        void setNonce(int nonce);
        void setIsMined(bool isMined);

        // DESTRUCTOR
        ~Transaction();
};

const string Transaction::godAddress = "0x0000000000000000000000000000000000000000";

// CONSTRUCTORS
Transaction::Transaction():from(""), to(""), amount(0), fee(0), nonce(0), hash(""), isMined(false) {}

Transaction::Transaction(string from, string to, int amount):fee(0), nonce(0), hash(""), isMined(false){
    this -> setFrom(from);
    this -> setTo(to);
    this -> setAmount(amount);
}

Transaction::Transaction(string from, string to, int amount, int nonce, int fee = 100):isMined(false){
    this -> setFrom(from);
    this -> setTo(to);
    this -> setAmount(amount);
    this -> setFee(fee);
    this -> setNonce(nonce);
    this -> updateHash();
}

Transaction::Transaction(string from, string to, int amount, int fee, int nonce, bool isMined){
    this -> setFrom(from);
    this -> setTo(to);
    this -> setAmount(amount);
    this -> setFee(fee);
    this -> setNonce(nonce);
    this -> isMined = isMined;
    
    this -> hash = this -> calculateHash();
}

Transaction::Transaction(string hash, string from, string to, int amount, int fee, int nonce, bool isMined){
    this -> setFrom(from);
    this -> setTo(to);
    this -> setAmount(amount);
    this -> setFee(fee);
    this -> setNonce(nonce);
    this -> isMined = isMined;

    if (hash != "0x" + this -> calculateHash()){
        sysMessage("The hash provided does not match the hash calculated from the other fields. The hash was not set.");
        this -> hash = "";
    }
    else this -> updateHash();
}

Transaction::Transaction(const Transaction &obj):from(obj.from), to(obj.to), amount(obj.amount), 
                                                 fee(obj.fee), nonce(obj.nonce), isMined(obj.isMined){
    this -> hash = this -> calculateHash();
}

// GETTERS
string Transaction::getHash() const{
    return this -> hash;
}

string Transaction::getFrom() const{
    return this -> from;
}

string Transaction::getTo() const{
    return this -> to;
}

long Transaction::getAmount() const{
    return this -> amount;
}

long Transaction::getFee() const{
    return this -> fee;
}

int Transaction::getNonce() const{
    return this -> nonce;
}

bool Transaction::getIsMined() const{
    return this -> isMined;
}

const string Transaction::getGodAddress(){
    return godAddress;
}

//SETTERS
void Transaction::setFrom(string from){
    if (!isAddress(from)){
        sysMessage("The string is not an address.");
        this -> from = "";
        return;
    }
    this -> from = from;
}

void Transaction::setTo(string to){
    if (!isAddress(to)){
        sysMessage("The string is not an address.");
        this -> from = "";
        return;
    }
    this -> to = to;
}

void Transaction::setAmount(int amount){
    if (amount < 0){
        sysMessage("Amount can not be negative. Zero has been filled by default.");
        this -> amount = 0;
        return;
    }
    this -> amount = amount;
}

void Transaction::setFee(int fee){
    if (fee <= 0){
        sysMessage("Fee needs to be greater than 0. The default was set (1 coin)");
        this -> fee = 100;
        return;
    }
    this -> fee = fee;
}

void Transaction::setNonce(int nonce){
    if (nonce < 0){
        sysMessage("Nonce needs to be greater than 0.");
        this -> nonce = 0;
        return;
    }
    this -> nonce = nonce;
}

void Transaction::setIsMined(bool isMined){
    this -> isMined = isMined;
}

// DESTRUCTOR
Transaction::~Transaction(){
    // no dynamic memory allocated
}

// OPERATORS
istream& operator>>(istream& in, Transaction &obj){
    // we will use the setters to read into variables (since they perform checks on the input data)
    string hash;
    string from, to;
    double amount;
    string fee, nonce;

    cout << "Enter the address of the sender (\"god\" - for god address): ";
    cin >> from;

    cout << "Enter the address of the receiver (\"god\" - for god address, empty - for a random one): ";
    in.ignore();
    getline(in,to);
    if (to == "")
        to = generateRandomHex();

    cout << "Enter the amount of coins to be sent: ";
    in >> amount;

    cout << "Enter the fee for the transaction (Enter for default: 1 coin): ";
    in.ignore();
    getline(in, fee);
    if (fee != "")
        obj.setFee(int(floor(stof(fee) * 100)));
    else obj.setFee(100);            // default fee value

    cout << "Enter the nonce for the transaction (Enter for default): ";
    getline(in, nonce);     // we don't use ignore since the previous getline consumes any delimiter
    if (nonce != ""){
        int intNonce;
        try{                        // handle large input
            intNonce = stoi(nonce);
        }
        catch (out_of_range){
            sysMessage("The nonce provided is too large.");
            intNonce = 0;
        }
        obj.setNonce(intNonce);
    }
    else obj.setNonce(0);

    // set the fields of the transaction
    if (from == "god")
        from = Transaction::getGodAddress();
    obj.setFrom(from);
    if (to == "god")
        to = Transaction::getGodAddress();
    obj.setTo(to);
    obj.setAmount(int(amount * 100));
    obj.setIsMined(false);

    obj.updateHash();
    
    return in;
}

ostream& operator<<(ostream& out, const Transaction &obj){
    out << ANSI_COLOR_GREEN << "~ Transaction " << obj.getHash() << " ~" << ANSI_COLOR_RESET << endl;
    out << "From: " << obj.getFrom() << endl;
    out << "To: " << obj.getTo() << endl;
    out << "Amount: " << double(obj.getAmount()) / 100 << endl;
    out << "Fee: " << float(obj.getFee()) / 100 << endl;
    out << "Nonce: " << obj.getNonce() << endl;
    out << "Is Mined: " << (obj.getIsMined() ? "true" : "false") << endl;
    return out;
}

Transaction& Transaction::operator=(const Transaction &obj){
    if (this == &obj)
        return *this;

    this -> from = obj.from;
    this -> to = obj.to;
    this -> amount = obj.amount;
    this -> fee = obj.fee;
    this -> nonce = obj.nonce;
    this -> isMined = obj.isMined;

    this -> hash = this -> calculateHash();
    return *this;
}

bool operator<(const Transaction &obj1, const Transaction &obj2){
    // this is also helpful for mempool sorting later on
    return obj1.getNonce() < obj2.getNonce();
}

variant<string, int, bool> Transaction::operator[](int index){
    // this is a simple way to access the fields of the transaction
    // it's not the best way to do it, but it's a proof of concept
    switch (index){
        case 0:
            return this -> hash;
        case 1:
            return this -> from;
        case 2:
            return this -> to;
        case 3:
            return this -> amount;
        case 4:
            return this -> fee;
        case 5:
            return this -> nonce;
        case 6:
            return this -> isMined;
        default:
            throw invalid_argument("Index out of bounds.");
    }
}

Transaction& Transaction::operator++(){
    // increment the nonce of the transaction
    this -> nonce++;
    this -> updateHash();
    return *this;
}

Transaction Transaction::operator++(int){
    // increment the nonce of the transaction
    Transaction copy = *this;
    ++(*this);
    return copy;
}

Transaction Transaction::operator+(float amount){
    // add coins to the transaction
    Transaction copy = *this;
    copy.setAmount(copy.getAmount() + int(amount * 100));    // avoids overflow
    copy.updateHash();
    return copy;
}

Transaction operator+(float amount, const Transaction &tx){
    // adds an integer amount of coins to the transaction
    Transaction copy = tx;
    return copy + amount;
}

Transaction Transaction::operator-(float amount){
    // subtract coins from the transaction
    Transaction copy = *this;
    copy.setAmount(copy.getAmount() - int(amount * 100));
    copy.updateHash();
    return copy;
}

Transaction operator-(float amount, const Transaction &tx){
    // subtracts an integer amount of coins from the transaction
    Transaction copy = tx;
    copy.setAmount(int(amount * 100) - copy.getAmount());
    return copy;
}

bool Transaction::operator<(const Transaction &obj){
    // compare two transactions based on their amount transacted
    return this -> amount < obj.amount;
}

bool Transaction::operator>(const Transaction &obj){
    return obj < *this;
}

bool Transaction::operator==(const Transaction &obj){
    // check if two transactions are the same
    return this -> hash == obj.hash;
}

Transaction::operator string(){
    return this -> hash;
}

Transaction::operator string() const{
    return this -> hash;
}

Transaction::operator int() const{
    return this -> amount;
}

// utility functions
string Transaction::calculateHash() const{
    // calculates a hash based on all fields of the transaction (except the isMined field)
    // maybe hashVal should start from some special value for cryptographic reasons
    // but it's good enough for a proof of concept
    unsigned long long hashVal = 0;
    for (int i = 0; i < this -> from.length(); i++){
        hashFunc(hashVal, int(from[i]));           // length can't differ from 42 (address length)
        hashFunc(hashVal, int(to[i]));
    }
    hashFunc(hashVal, amount);
    hashFunc(hashVal, int(fee));
    hashFunc(hashVal, int(nonce));

    stringstream ss;
    ss << hex << hashVal;
    return "0x" + ss.str();
}

void Transaction::updateHash(){
    this -> hash = this -> calculateHash();
}

bool Transaction::isMineable() const{
    // check if a transaction can be included in a block
    // this is more of a syntax check, the blockchain needs to check balances and nonces itself!
    if (!isAddress(this -> from) || !isAddress(this -> to))
        return false;
    if (this -> amount < 0)
        return false;
    if (this -> fee <= 0)
        return false;
    if (this -> nonce < 0)
        return false;
    if (this -> hash != this -> calculateHash())
        return false;
    return true;
}

// ----------------- MEMPOOL -----------------

class Mempool{
    // The mempool is a list of transactions that are waiting to be mined

    list<Transaction> txList;   // list of transactions not yet included in a block
    int maxSize;                // maximum number of transactions that can be stored in the mempool (DDoS securtity measure)
    int minFee;                 // minimum fee for a transaction to be included in the mempool
    float averageFee;           // average fee of transactions in the mempool

    public:
        // CONSTRUCTORS
        Mempool();
        Mempool(list<Transaction> txList);
        Mempool(list<Transaction> txList, int maxSize);
        Mempool(list<Transaction> txList, int maxSize, int minFee);
        Mempool(list<Transaction> txList, int maxSize, int minFee, float averageFee);
        Mempool(const Mempool &obj);

        // utility functions
        void updateAverageFee();
        void addTx(Transaction&);
        void deleteTx(string);

        // OPERATORS
        Mempool& operator=(const Mempool&);
        Transaction operator[](string);
        Mempool& operator--();
        Mempool operator--(int);
        Mempool operator+(Mempool&);
        Mempool operator-(Mempool&);
        bool operator<(const Mempool &obj) const;
        bool operator<=(const Mempool &obj) const;
        bool operator>(const Mempool &obj);
        bool operator>=(const Mempool &obj);
        bool operator==(const Mempool &obj) const;
        operator list<Transaction>() const;

        // GETTERS
        const list<Transaction>& getTxList() const;
        int getMaxSize() const;
        int getMinFee() const;
        float getAverageFee() const;

        // SETTERS
        void setTxList(list<Transaction>);
        void setMaxSize(int);
        void setMinFee(int);
        void setAverageFee(float);

        // DESTRUCTOR
        ~Mempool();
};

// CONSTRUCTORS
Mempool::Mempool():maxSize(1024), minFee(25), averageFee(0) {} // txList is empty by default

Mempool::Mempool(list<Transaction> txList):maxSize(1024), minFee(25){
    this -> setTxList(txList);
    this -> updateAverageFee();
}

Mempool::Mempool(list<Transaction> txList, int maxSize):minFee(25){
    this -> setMaxSize(maxSize);
    this -> setTxList(txList);
    this -> updateAverageFee();
}

Mempool::Mempool(list<Transaction> txList, int maxSize, int minFee){
    this -> setMaxSize(maxSize);
    this -> setTxList(txList);
    this -> setMinFee(minFee);
    this -> updateAverageFee();
}

Mempool::Mempool(list<Transaction> txList, int maxSize, int minFee, float averageFee){
    this -> setMaxSize(maxSize);
    this -> setTxList(txList);
    this -> setMinFee(minFee);
    this -> setAverageFee(averageFee);
}

Mempool::Mempool(const Mempool &obj):maxSize(obj.maxSize), txList(obj.txList), 
                                     minFee(obj.minFee), averageFee(obj.averageFee) {}


// GETTERS
const list<Transaction>& Mempool::getTxList() const{
    return this -> txList;
}

int Mempool::getMaxSize() const{
    return this -> maxSize;
}

int Mempool::getMinFee() const{
    return this -> minFee;
}

float Mempool::getAverageFee() const{
    return this -> averageFee;
}

// SETTERS
void Mempool::setTxList(list<Transaction> txList){
    if (txList.size() > this -> maxSize){
        sysMessage("The number of transactions in the list exceeds the maximum size of the mempool. The list was not set.");
        return;
    }
    this -> txList = txList;    // deep copy
    this -> updateAverageFee();
}

void Mempool::setMaxSize(int maxSize){
    if (maxSize < 1){
        sysMessage("Maximum size of the mempool can not be less than 1. Default value (1024) set.");
        this -> maxSize = 1024;
        return;
    }
    this -> maxSize = maxSize;
}

void Mempool::setMinFee(int minFee){
    if (minFee < 0){
        sysMessage("Minimum fee can not be less than 0. Default value (0.25 coins) set.");
        this -> minFee = 25;
        return;
    }
    this -> minFee = minFee;
}

void Mempool::setAverageFee(float averageFee){
    if (averageFee < 0){
        sysMessage("Average fee can not be less than 0. Default value (0) set.");
        this -> averageFee = 0;
        return;
    }
    this -> averageFee = averageFee;
}

// DESTRUCTOR
Mempool::~Mempool(){
    // since the txList holds objects and not pointers, there's nothing to delete manually
}

// OPERATORS
istream& operator>>(istream& in, Mempool &obj){
    // we will use the setters as they have checks and deep copy implemented
    list<Transaction> txList;
    int maxSize;
    float minFee;

    cout << "Enter the maximum size of the mempool: ";
    in >> maxSize;

    cout << "Enter the minimum fee for a transaction to be included in the mempool: ";
    in >> minFee;

    cout << "Do you want to add any tx in the mempool? (Y/n): ";
    char choice;
    in >> choice;
    while (choice == 'Y' || choice == 'y'){
        if (txList.size() >= maxSize){
            sysMessage("The mempool is full. No more transactions can be added.");
            break;
        }
        Transaction tx;
        in >> tx;
        if (!tx.isMineable()){
            sysMessage("The transaction is not mineable. It will not be added to the mempool.");
        }
        else txList.push_back(tx);

        cout << "Do you want to add another tx in the mempool? (Y/n): ";
        in >> choice;
    }

    // set the fields of the mempool
    obj.setTxList(txList);
    obj.setMaxSize(maxSize);
    obj.setMinFee(round(minFee* 100));
    obj.updateAverageFee();

    return in;
}

ostream& operator<<(ostream& out, const Mempool &obj){
    cout << ANSI_COLOR_GREEN << "~=~=~=~=~=~=~=~=~=~=~= MEMPOOL =~=~=~=~=~=~=~=~=~=~\n" << ANSI_COLOR_RESET;
    out << "Maximum size of the mempool: " << obj.getMaxSize() << endl;
    out << "Minimum fee for a transaction to be included in the mempool: " << float(obj.getMinFee()) / 100 << endl;
    out << "Average fee of transactions in the mempool: " << obj.getAverageFee() << endl;

    if (obj.getTxList().empty()){
        out << "The mempool is empty.\n";
        return out;
    }

    out << "Transactions in the mempool: \n\n";
    int i = 0;
    for (auto it = obj.getTxList().begin(); it != obj.getTxList().end(); it++)
        out << *it << endl;
    return out;
}

Mempool& Mempool::operator=(const Mempool &obj){
    if (this == &obj)
        return *this;

    this -> setTxList(obj.txList);
    this -> setMaxSize(obj.maxSize);
    this -> setMinFee(obj.minFee);

    return *this;
}

Transaction Mempool::operator[](string hash){
    for (auto it = this -> txList.begin(); it != this -> txList.end(); it++)
        if ((*it).getHash() == hash)
            return *it;
    sysMessage("The transaction with the hash provided was not found in the mempool.");
    return Transaction(); 
}

Mempool& Mempool::operator--(){
    // remove the last transaction from the mempool
    if (this -> txList.empty()){
        sysMessage("The mempool is empty. No transactions to remove.");
        return *this;
    }
    this -> txList.pop_back();
    return *this;
}

Mempool Mempool::operator--(int){
    // remove the last transaction from the mempool
    Mempool copy = *this;
    --(*this);
    return copy;
}

Mempool Mempool::operator+(Mempool &obj){
    // merge two mempools
    // the mempools need to be compatible (same minFee and maxSize)
    if (this -> minFee != obj.minFee){
        sysMessage("The minimum fees of the two mempools are different. The operation can not be performed.");
        return *this;
    }
    if (this -> maxSize != obj.maxSize){
        sysMessage("The maximum sizes of the two mempools are different. The operation can not be performed.");
        return *this;
    }

    // merge the tx lists
    Mempool copy = *this;
    for (auto it = obj.txList.begin(); it != obj.txList.end(); it++)
        copy.addTx(*it);

    return copy;
}

Mempool Mempool::operator-(Mempool &obj){
    // remove transactions from the first mempool that are also in the second mempool
    // the mempools need to be compatible (same minFee and maxSize)
    if (this -> minFee != obj.minFee){
        sysMessage("The minimum fees of the two mempools are different. The operation can not be performed.");
        return *this;
    }
    if (this -> maxSize != obj.maxSize){
        sysMessage("The maximum sizes of the two mempools are different. The operation can not be performed.");
        return *this;
    }

    Mempool copy = *this;
    for (auto it = obj.txList.begin(); it != obj.txList.end(); it++)
        copy.deleteTx((*it).getHash());

    return copy;
}

bool Mempool::operator<(const Mempool &obj) const{
    // compare based on the number of transactions in the mempool
    return this -> txList.size() < obj.txList.size();
}

bool Mempool::operator<=(const Mempool &obj)const{
    return *this < obj || *this == obj;
}

bool Mempool::operator>(const Mempool &obj){
    return obj < *this;
}

bool Mempool::operator>=(const Mempool &obj){
    return obj <= *this;
}

bool Mempool::operator==(const Mempool &obj) const{
    // check if both mempools hold the same txs
    if (this -> maxSize != obj.maxSize)
        return false;
    if (this -> minFee != obj.minFee)
        return false;
    if (this -> txList.size() != obj.txList.size())
        return false;
    
    for (auto it = this -> txList.begin(); it != this -> txList.end(); it++){
        bool found = false;
        for (auto it2 = obj.txList.begin(); it2 != obj.txList.end(); it2++){
            if ((*it) == (*it2)){
                found = true;
                break;
            }
        }
        if (!found)
            return false;
    }
    return true;
}

Mempool::operator list<Transaction>() const{
    return this -> txList;
}

// utility functions
void Mempool::updateAverageFee(){
    // updates the average fee of transactions from the mempool
    float aux = 0;
    int ct = 0;
    for (auto it = txList.begin(); it != txList.end(); it++){
        aux += (*it).getFee();
        ct++;
    }

    // handle nan float case
    if (aux == 0)
        this -> setAverageFee(0);
    else this -> setAverageFee((aux / ct) / 100.0);
}

void Mempool::addTx(Transaction &tx){
    // adds a transaction in the mempool
    if (!tx.isMineable()){
        sysMessage("The transaction is not mineable. It will not be added to the mempool.");
        return;
    }
    if (txList.size() >= maxSize){
        sysMessage("The mempool is full. No more transactions can be added.");
        return;
    }
    if (tx.getFee() < this -> minFee){
        sysMessage("The fee of the transaction is less than the minimum fee required for a transaction to be included in the mempool. The transaction will not be added.");
        return;
    }
    this -> txList.push_back(tx);
    this -> updateAverageFee();
}

void Mempool::deleteTx(string hash){
    // removes a transaction from the mempool given its hash
    for (auto it = txList.begin(); it != txList.end(); it++){
        if ((*it).getHash() == hash){
            txList.erase(it);
            this -> updateAverageFee();
            return;
        }
    }
    warning("The transaction with the hash provided was not found in the mempool.");

}

// ----------------- WALLET -----------------

class Wallet{
    // the wallet keeps track of an address' balance and nonce
    // the nonce is a value that needs to be incremented for every transaction (to prevent someone from speding it twice)
    // since this is a simulator, the wallet also keeps track of some other statistics for UX
    // txList and averageSpent are not actually required for the blockchain to function, but they are useful for the user
    string address;
    int balance;
    int nonce;
    list<const Transaction*> txList;    // pointers to mined txs (in blocks) or txs from the mempool
    float averageSpent;                 // average amount of coins spent in transactions

    public:
        // CONSTRUCTORS
        Wallet();
        Wallet(string address);
        Wallet(string address, int balance);
        Wallet(string address, int balance, int nonce, list<const Transaction*> txList, float averageSpent);
        Wallet(const Wallet &obj);

        // utility functions
        void updateTx(string, const Transaction*);
        void deleteTx(string);
        void addTx(const Transaction*);
        void updateAverageSpent();

        // OPERATORS
        Wallet& operator=(const Wallet&);
        Transaction operator[](string);
        Wallet& operator++();
        Wallet operator++(int);
        Wallet operator+(int);
        Wallet operator-(int);
        bool operator<(const Wallet&) const;
        bool operator>(const Wallet&);
        bool operator==(const Wallet&);
        operator pair<string, int>() const;

        // GETTERS
        string getAddress() const;
        int getBalance() const;
        int getNonce() const;
        const list<const Transaction*>& getTxList() const;
        float getAverageSpent() const;

        // SETTERS
        void setAddress(string);
        void setBalance(int);
        void setNonce(int);
        void setTxList(list<const Transaction*>);
        void setAverageSpent(float);

        // DESTRUCTOR
        ~Wallet();
    
};

// CONSTRUCTORS
Wallet::Wallet():balance(0), nonce(0), averageSpent(0) {
    this -> address = generateRandomHex();
    // txList is empty by default
}

Wallet::Wallet(string address):balance(0), nonce(0), averageSpent(0){
    if (!isAddress(address)){
        sysMessage("The string is not an address. A random address has been generated.");
        this -> address = generateRandomHex();
    }
    else this -> address = address;
}

Wallet::Wallet(string address, int balance):nonce(0), averageSpent(0){
    if (!isAddress(address)){
        sysMessage("The string is not an address. A random address has been generated.");
        this -> address = generateRandomHex();
    }
    else this -> address = address;

    this -> setBalance(balance);
}

Wallet::Wallet(string address, int balance, int nonce, list<const Transaction*> txList, float averageSpent){
    if (!isAddress(address)){
        sysMessage("The string is not an address. A random address has been generated.");
        this -> address = generateRandomHex();
    }
    else this -> address = address;

    this -> setBalance(balance);
    this -> nonce = nonce;
    this -> txList = txList;
    this -> averageSpent = averageSpent;
}

Wallet::Wallet(const Wallet &obj):address(obj.address), balance(obj.balance), nonce(obj.nonce), 
                                  txList(obj.txList), averageSpent(obj.averageSpent) {}

// GETTERS
string Wallet::getAddress() const{
    return this -> address;
}

int Wallet::getBalance() const{
    return this -> balance;
}

int Wallet::getNonce() const{
    return this -> nonce;
}

const list<const Transaction*>& Wallet::getTxList() const{
    return this -> txList;
}

float Wallet::getAverageSpent() const{
    return this -> averageSpent;
}

// SETTERS
void Wallet::setAddress(string address){
    if (!isAddress(address)){
        sysMessage("The string is not an address. A random address has been generated.");
        this -> address = generateRandomHex();
        return;
    }
    this -> address = address;
}

void Wallet::setBalance(int balance){
    if (balance < 0){
        sysMessage("Balance can not be negative. Zero has been filled by default.");
        this -> balance = 0;
        return;
    }
    this -> balance = balance;
}

void Wallet::setNonce(int nonce){
    if (nonce < 0){
        sysMessage("Nonce can not be negative. Zero has been filled by default.");
        this -> nonce = 0;
        return;
    }
    this -> nonce = nonce;
}

void Wallet::setTxList(list<const Transaction*> txList){
    this -> txList = txList;
    this -> updateAverageSpent();
}

void Wallet::setAverageSpent(float averageSpent){
    if (averageSpent < 0){
        sysMessage("Average spent can not be negative. Zero has been filled by default.");
        this -> averageSpent = 0;
        return;
    }
    this -> averageSpent = averageSpent;
}

// DESTRUCTOR
Wallet::~Wallet(){
    // we specifically don't want to delete transactions from txList, as those are also kept in blocks/mempool
    // the mempool will modify these pointers if it hands off a transaction to a block
    // (or delete the list entry if it dropps the tx from the mempool)
    // nothing else is dynamically allocated
}

// OPERATORS
istream& operator>>(istream &in, Wallet &obj){
    // this should only be called when creating the blockchain (not after the blockchain was setup) so it assumes some default values
    string address;
    double balance;
    string nonce;
    list<const Transaction*> txList;
    float averageSpent;

    cout << "Enter the balance of the wallet: ";
    in >> balance;

    cout << "Enter a hex address of 42 characters (including 0x prefix) for the wallet (Leave blank for random address; \"god\" for god address): ";
    in.ignore();
    getline(in, address);
    if (address == "god")
        address = Transaction::getGodAddress();
    else if (address.substr(0,3) != "0x")
        address = generateRandomHex();
    else if (!isAddress(address)){
        sysMessage("The string is not an address. A random address has been generated.");
        address = generateRandomHex();
    }

    // set the fields of the wallet
    obj.setAddress(address);
    obj.setBalance(round(balance * 100));
    obj.setNonce(0);
    obj.setTxList(txList);
    obj.setAverageSpent(0);

    return in;
}

ostream& operator<<(ostream& out, const Wallet &obj){
    out << "Address: " << obj.getAddress() << endl;
    out << "Balance: " << float(obj.getBalance()) / 100 << endl;
    out << "Nonce: " << obj.getNonce() << endl;
    out << "Average spent: " << float(obj.getAverageSpent()) / 100 << endl;

    if (obj.getTxList().empty()){
        out << "The wallet has no transactions.\n";
        return out;
    }
    out << "Transactions in the wallet: \n\n";
    for (auto it = obj.getTxList().begin(); it != obj.getTxList().end(); it++){
        out << **it << endl;
    }
    return out;
}

Wallet& Wallet::operator=(const Wallet &obj){
    if (this == &obj)
        return *this;

    this -> address = obj.address;
    this -> balance = obj.balance;
    this -> nonce = obj.nonce;
    this -> txList = obj.txList;
    this -> averageSpent = obj.averageSpent;

    return *this;
}

Transaction Wallet::operator[](string hash){
    for (auto it = this -> txList.begin(); it != this -> txList.end(); it++)
        if ((*it) -> getHash() == hash)
            return **it;
    sysMessage("The transaction with the hash provided was not found in the wallet.");
    return Transaction();
}

Wallet& Wallet::operator++(){
    // increment the nonce of the wallet
    this -> nonce++;
    return *this;
}

Wallet Wallet::operator++(int){
    // increment the nonce of the wallet
    Wallet copy = *this;
    ++(*this);
    return copy;
}

Wallet Wallet::operator+(int nonce){
    // add nonce to the wallet
    Wallet copy = *this;
    copy.setNonce(copy.getNonce() + nonce);
    return copy;
}

Wallet operator+(int nonce, const Wallet &w){
    // adds an integer nonce to the wallet
    Wallet copy = w;
    return copy + nonce;
}

Wallet Wallet::operator-(int nonce){
    // subtract nonce from the wallet
    Wallet copy = *this;
    copy.setNonce(copy.getNonce() - nonce);
    return copy;
}

Wallet operator-(int nonce, const Wallet &w){
    // subtracts an integer nonce from the wallet
    Wallet copy = w;
    copy.setNonce(nonce - copy.getNonce());
    return copy;
}

bool Wallet::operator<(const Wallet &obj) const{
    // compare based on the balance of the wallet
    return this -> balance < obj.balance;
}

bool Wallet::operator>(const Wallet &obj){
    return obj < *this;
}

bool Wallet::operator==(const Wallet &obj){
    // check if two wallets are the same
    return this -> address == obj.address;
}

Wallet::operator pair<string, int>() const{
    return make_pair(this -> address, this -> balance);
}

// utility functions
void Wallet::addTx(const Transaction *tx){
    // adds a transaction to the wallet, keeps the list sorted by nonce
    auto it = this -> txList.begin();
    for (; it != this -> txList.end(); it++)
        if ((*it) -> getNonce() > tx -> getNonce())
            // search where to insert the new tx
            break;
    this -> txList.insert(it, tx);
    this -> updateAverageSpent();
}

void Wallet::updateTx(string hash, const Transaction* tx){
    // updates the pointer of a tx from the wallet
    // e.g. if a tx was included in a block, the pointer needs to be updated (the mempool deletes the tx object)
    for (auto it = this -> txList.begin(); it != this -> txList.end(); it++){
        if ((**it).getHash() == hash){
            *it = tx;
            return;
        }
    }
    sysMessage("Potential memory leak! The transaction with the hash provided was not found in the wallet when trying to update a pointer. Hash: " + hash);
}

void Wallet::deleteTx(string hash){
    // removes a transaction from the wallet given its hash
    for (auto it = this -> txList.begin(); it != this -> txList.end(); it++){
        Transaction tx = **it;
        if ((**it).getHash() == hash){
            txList.erase(it);
            return;
        }
    }
    sysMessage("The transaction with the hash provided was not found in the wallet.");
}

void Wallet::updateAverageSpent(){
    // updates the average spent by the user (also includes not mined txs)
    float aux = 0;
    int ct = 0;
    for (auto it = this -> txList.begin(); it != this -> txList.end(); it++){
        if ((*it) -> getFrom() == this -> getAddress()){    
            aux += (*it) -> getAmount();
            ct++;
        }
    }

    // handle nan float case
    if (aux == 0)
        this -> setAverageSpent(0);
    else this -> setAverageSpent(aux / ct);
}

// ----------------- BLOCK -----------------

class Block{
    string hash;                      // hash of the other fields (id of block)
    string parentHash;                // hash of the previous block
    int height;                       // height of the block in the blockchain
    list<Transaction> transactions;   // list of transactions included in the block

    public:
        // CONSTRUCTORS
        Block();
        Block(string parentHash, int height);
        Block(string parentHash, int height, list<Transaction> transactions);
        Block(string hash, string parentHash, int height, list<Transaction> transactions);
        Block(const Block &obj);

        // utility functions
        string calculateHash();
        void updateHash();
        void addTx(const Transaction &tx);

        // OPERATORS
        Block& operator=(const Block&);
        Block operator+(const Transaction&);
        Block operator+(const Block&);
        Transaction operator[](string);
        Block& operator--();
        Block operator--(int);
        Block operator-(const Transaction&);
        bool operator<(const Block&) const;
        bool operator>(const Block&);
        bool operator==(const Block&);
        operator list<Transaction>() const;

        // GETTERS
        string getHash() const;
        string getParentHash() const;
        int getHeight() const;
        const list<Transaction>& getTransactions() const;

        // SETTERS
        void setHash(string);
        void setParentHash(string);
        void setHeight(int);
        void setTransactions(list<Transaction>);

        // DESTRUCTOR
        ~Block();
};

// CONSTRUCTORS
Block::Block():hash(""), parentHash(""), height(0){}

Block::Block(string parentHash, int height){
    this -> setParentHash(parentHash);
    this -> setHeight(height);
    this -> updateHash();
}

Block::Block(string parentHash, int height, list<Transaction> transactions){
    this -> setParentHash(parentHash);
    this -> setHeight(height);
    this -> setTransactions(transactions);
    this -> updateHash();
}

Block::Block(string hash, string parentHash, int height, list<Transaction> transactions){
    this -> setHash(hash);
    this -> setParentHash(parentHash);
    this -> setHeight(height);
    this -> setTransactions(transactions);
}

Block::Block(const Block &obj):hash(obj.hash), parentHash(obj.parentHash), height(obj.height), 
                               transactions(obj.transactions) {}

// GETTERS
string Block::getHash() const{
    return this -> hash;
}

string Block::getParentHash() const{
    return this -> parentHash;
}

int Block::getHeight() const{
    return this -> height;
}

const list<Transaction>& Block::getTransactions() const{
    return this -> transactions;
}

// SETTERS
void Block::setHash(string hash){
    if (!isProperHex(hash.substr(2))){
        sysMessage("The string is not a proper hex string. Field left blank.");
        this -> hash = "";
        return;
    }
    if (this -> calculateHash() != hash){
        sysMessage("The hash provided does not match the hash calculated from the other fields. The hash was not set.");
        this -> hash = "";
        return;
    }
    this -> hash = hash;
}

void Block::setParentHash(string parentHash){
    if (!isProperHex(parentHash.substr(2))){
        sysMessage("The string is not a proper hex string. Field left blank.");
        this -> parentHash = "";
        return;
    }
    this -> parentHash = parentHash;
}

void Block::setHeight(int height){
    if (height < 0){
        sysMessage("Height can not be negative. Zero has been filled by default.");
        this -> height = 0;
        return;
    }
    this -> height = height;
}

void Block::setTransactions(list<Transaction> transactions){
    this -> transactions = transactions;
}

// DESTRUCTOR
Block::~Block(){
    // no dynamic memory allocated
}

// OPERATORS
istream& operator>>(istream& in, Block &obj){
    string hash;
    string parentHash;
    int height;
    list<Transaction> transactions;

    cout << "Enter the hash of the parent block: ";
    in >> parentHash;

    cout << "Enter the height of the block: ";
    in >> height;

    cout << "Do you want to add any tx in the block? (Y/n): ";
    char choice;
    in >> choice;
    while (choice == 'Y' || choice == 'y'){
        cout << endl << "Transaction details: \n";
        Transaction tx;
        in >> tx;
        if (!tx.isMineable()){
            sysMessage("The transaction is not mineable. It will not be added to the block.");
        }
        else transactions.push_back(tx);

        cout << "Do you want to add another tx in the block? (Y/n): ";
        in >> choice;
    }

    // set the fields of the block
    obj.setParentHash(parentHash);
    obj.setHeight(height);
    obj.setTransactions(transactions);

    obj.updateHash();

    return in;
}

ostream& operator<<(ostream &out, const Block &obj){
    out << ANSI_COLOR_GREEN << "+-+-+ BLOCK " << obj.getHeight() << " +-+-+\n" << ANSI_COLOR_RESET;
    out << "Hash: " << obj.getHash() << endl;
    out << "Parent Hash: " << obj.getParentHash() << endl;

    if (obj.getTransactions().empty()){
        out << "The block has no transactions.\n";
        return out;
    }
    out << "Transactions in the block: " << endl << endl;
    int i = 0;
    for (auto it = obj.getTransactions().begin(); it != obj.getTransactions().end(); it++)
        out << *it << endl;
    return out;
}

Block& Block::operator=(const Block &obj){
    if (this == &obj)
        return *this;

    this -> hash = obj.hash;
    this -> parentHash = obj.parentHash;
    this -> height = obj.height;
    this -> transactions = obj.transactions;

    return *this;
}

// + operator between 2 classes respecting comutativity
Block Block::operator+(const Transaction &tx){
    // adds a transaction to the block
    Block newBlock = *this;
    newBlock.addTx(tx);
    return newBlock;
}

Block operator+(Transaction tx, const Block &block){
    // adds a transaction to the block
    Block newBlock = block;
    newBlock.addTx(tx);
    return newBlock;
}

Block Block::operator-(const Transaction &tx){
    // removes a transaction from the block
    Block newBlock = *this;
    for (auto it = newBlock.transactions.begin(); it != newBlock.transactions.end(); it++){
        if ((*it) == tx){
            newBlock.transactions.erase(it);
            newBlock.updateHash();
            return newBlock;
        }
    }
    return newBlock;
}

Block Block::operator+(const Block &block){
    // adds all transactions from a block to the current block
    Block newBlock = *this;
    for (auto it = block.transactions.begin(); it != block.transactions.end(); it++)
        newBlock.addTx(*it);
    return newBlock;
}

Transaction Block::operator[](string hash){
    for (auto it = this -> transactions.begin(); it != this -> transactions.end(); it++)
        if ((*it).getHash() == hash)
            return *it;
    sysMessage("The transaction with the hash provided was not found in the block.");
    return Transaction();
}

Block& Block::operator--(){
    // remove the last transaction from the block
    if (this -> transactions.empty()){
        sysMessage("The block is empty. No transactions to remove.");
        return *this;
    }
    this -> transactions.pop_back();
    this -> updateHash();
    return *this;
}

Block Block::operator--(int){
    // remove the last transaction from the block
    Block copy = *this;
    --(*this);
    return copy;
}

bool Block::operator<(const Block &obj) const{
    // compare based on the height of the block
    return this -> height < obj.height;
}

bool Block::operator>(const Block &obj){
    return obj < *this;
}

bool Block::operator==(const Block &obj){
    // check if two blocks are the same
    return this -> hash == obj.hash;
}

Block::operator list<Transaction>() const{
    return this -> transactions;
}

// utility functions
string Block::calculateHash(){
    // calculates a hash based on all fields of the block
    unsigned long long hashVal = 0;

    for (int i = 0; i < this -> parentHash.length(); i++){
        hashFunc(hashVal, int(parentHash[i]));
    }
    hashFunc(hashVal, height);

    // we consider the hashes of the transactions for the block hash
    for (auto it = this -> transactions.begin(); it != this -> transactions.end(); it++){
        for (int i = 0; i < (*it).getHash().length(); i++)
            hashFunc(hashVal, int((*it).getHash()[i]));
    }

    stringstream ss;
    ss << hex << hashVal;
    return "0x" + ss.str();
}

void Block::updateHash(){
    this -> hash = this -> calculateHash();
}

void Block::addTx(const Transaction &tx){
    // adds a transaction to the block
    if (!tx.isMineable()){
        sysMessage("The transaction is not mineable. It will not be added to the block.");
        return;
    }
    this -> transactions.push_back(tx);
    this -> updateHash();
}

// ----------------- BLOCKCHAIN -----------------

class Blockchain{
    int currentHeight;                      // height of the last block mined
    char *currentHash;                      // hash of the last block mined
    Mempool mempool;                        // mempory pool of transactions
    list<Block> blocks;                     // list of blocks proccessed
    unordered_map<string, Wallet> wallets;  // map of wallets (address -> wallet)
    char status;                            // status of the blockchain (I - initializing, A - active)

    // statistics variables
    float *txStats;                          // array of average coins transacted per block
    double averageTransacted;                // average amount of coins transacted in all blocks

    public:
        // CONSTRUCTORS
        Blockchain();
        Blockchain(int currentHeight, char *currentHash, unordered_map<string, Wallet> wallets);
        Blockchain(int currentHeight, char *currentHash, list<Block> blocks, unordered_map<string, Wallet> wallets);
        Blockchain(int currentHeight, char *currentHash, Mempool mempool, list<Block> blocks, 
                   unordered_map<string, Wallet> wallets, char status, float *txStats, double averageTransacted);
        Blockchain(const Blockchain &obj);

        // utility functions
        bool validateTx(Transaction&, unordered_map<string, Wallet>&);
        bool validateBlockTransactions(Block&);
        void processBlock(Block&);
        void applyBlockOnState(Block&);
        void updateStatistics(Block&);
        void generateGenesis();
        void sendTx(Transaction&);
        Block proposeBlock();
        int getAccountNonce(string);
        Transaction readTx();
        void cleanMempool();
        void cleanWallets();

        // OPERATORS
        friend istream& operator>>(istream&, Blockchain&);
        Blockchain& operator=(const Blockchain&);
        Transaction operator[](string);
        Blockchain& operator--();
        Blockchain operator--(int);
        Blockchain operator+(Block);
        Blockchain operator-(const Transaction&);
        bool operator<(const Blockchain&) const;
        bool operator>(const Blockchain&);
        bool operator==(const Blockchain&);
        operator string() const;

        // GETTERS
        int getCurrentHeight() const;
        const char* getCurrentHash() const;
        const Mempool& getMempool() const;
        const list<Block>& getBlocks() const;
        const unordered_map<string, Wallet>& getWallets() const;
        char getStatus() const;
        const float* getTxStats() const;
        double getAverageTransacted() const;

        // SETTERS
        void setCurrentHeight(int);
        void setCurrentHash(char*);
        void setStatus(char);
        void setMempool(Mempool);
        void setTxStats(float*);
        void setAverageTransacted(double);
        void setBlocks(list<Block>&);

        // DESTRUCTOR
        ~Blockchain();
};

 // CONSTRUCTORS
Blockchain::Blockchain():currentHeight(0), currentHash(NULL), status('I'), txStats(NULL), averageTransacted(0) {}

Blockchain::Blockchain(int currentHeight, char *currentHash, 
                       unordered_map<string, Wallet> wallets):txStats(NULL), averageTransacted(0), status('A'){
    this -> setCurrentHeight(currentHeight);
    this -> setCurrentHash(currentHash);
    this -> wallets = wallets;
}

Blockchain::Blockchain(int currentHeight, char *currentHash, list<Block> blocks, 
                       unordered_map<string, Wallet> wallets):status('A'), txStats(NULL), averageTransacted(0){
    this -> setCurrentHeight(currentHeight);
    this -> setCurrentHash(currentHash);
    this -> blocks.clear();
    this -> wallets.clear();
    this -> wallets = wallets;
    this -> setBlocks(blocks);
}

Blockchain::Blockchain(int currentHeight, char *currentHash, Mempool mempool, list<Block> blocks, 
            unordered_map<string, Wallet> wallets, char status, float *txStats, double averageTransacted){
    this -> currentHeight = currentHeight;
    this -> setCurrentHash(currentHash);
    this -> mempool = mempool;
    this -> blocks = blocks;
    this -> wallets = wallets;
    this -> status = status;
    this -> setTxStats(txStats);
    this -> averageTransacted = averageTransacted;
}

Blockchain::Blockchain(const Blockchain &obj):currentHeight(obj.currentHeight), 
                                              mempool(obj.mempool), blocks(obj.blocks), wallets(obj.wallets), 
                                              status(obj.status), averageTransacted(obj.averageTransacted)
{
    this -> setCurrentHash(obj.currentHash);
    this -> setTxStats(obj.txStats);
}

// GETTERS
int Blockchain::getCurrentHeight() const{
    return this -> currentHeight;
}

const char* Blockchain::getCurrentHash() const{
    return this -> currentHash;
}

const Mempool& Blockchain::getMempool() const{
    return this -> mempool;
}

const list<Block>& Blockchain::getBlocks() const{
    return this -> blocks;
}

const unordered_map<string, Wallet>& Blockchain::getWallets() const{
    return this -> wallets;
}

char Blockchain::getStatus() const{
    return this -> status;
}

const float* Blockchain::getTxStats() const{
    return this -> txStats;
}

double Blockchain::getAverageTransacted() const{
    return this -> averageTransacted;
}

// SETTERS
void Blockchain::setCurrentHeight(int currentHeight){
    if (this -> currentHeight + 1 != currentHeight && this -> status == 'A'){
        sysMessage("The height of the new block is not consistent with the current height. The height was not modified.");
        return;
    }
    this -> currentHeight = currentHeight;
}

void Blockchain::setCurrentHash(char *currentHash){
    if (this -> currentHash)
        delete[] this -> currentHash;
    this -> currentHash = new char[strlen(currentHash) + 1];
    strcpy(this -> currentHash, currentHash);
}

void Blockchain::setStatus(char status){
    if (status != 'A' && status != 'I'){
        sysMessage("The status of the blockchain can only be A (active) or I (initializing). The status was not modified.");
        return;
    }
    if (this -> status == 'A'){
        sysMessage("The blockchain is already active. It can no longer be modified.");
        return;
    }
    this -> status = status;
}

void Blockchain::setMempool(Mempool mempool){
    this -> mempool = mempool;
}

void Blockchain::setTxStats(float *txStats){
    if (txStats == NULL){
        this -> txStats = NULL;
        return;
    }

    if (this -> txStats)
        delete[] this -> txStats;
    this -> txStats = new float[this -> currentHeight];
    for (int i = 0; i < this -> currentHeight; i++)
        this -> txStats[i] = txStats[i];
}

void Blockchain::setAverageTransacted(double averageTransacted){
    if (averageTransacted < 0){
        sysMessage("The average transacted can not be negative. The value was not modified.");
        return;
    }
    this -> averageTransacted = averageTransacted;
}

void Blockchain::setBlocks(list<Block> &blocks){
    // note: we don't delete old blocks!
    for (auto it = blocks.begin(); it != blocks.end(); it++)
        this -> processBlock(*it);
}

// DESTRUCTOR
Blockchain::~Blockchain(){
    if (currentHash)
        delete[] currentHash;
    if (txStats)
        delete[] txStats;
}

// OPERATORS
istream& operator>>(istream& in, Blockchain &obj){
    if (obj.getStatus() == 'A'){
        sysMessage("The blockchain is already active. It can not be modified like this.");
        return in;
    }
    int currentHeight;
    string buffer;
    unordered_map<string, Wallet> wallets;
    char choice;

    cout << "Enter the current height of the blockchain (how many blocks have been mined before): ";
    in >> currentHeight;
    if (currentHeight > 0){
        // read current hash
        cout << "Enter the hash of the last block (or leave blank for a random value): ";
        buffer = new char[65];
        in.ignore();
        getline(in, buffer);
        if (buffer == "")
            buffer = generateRandomHex(); 

        obj.setCurrentHeight(currentHeight);

        // read wallets
        cout << "Do you want to add wallets? (Y/n): ";
        in >> choice;
        while (choice == 'Y' || choice == 'y'){
            Wallet w;
            in >> w;
            wallets[w.getAddress()] = w;
            cout << "Do you want to add another wallet? (Y/n): ";
            in >> choice;
        }
        obj.wallets = wallets;

        // consider no stats since we won't read blocks
        obj.txStats = NULL;
        obj.averageTransacted = 0;
        if (obj.txStats)
            delete[] obj.txStats;
        obj.txStats = new float[currentHeight + 1];
        for (int i = 1; i <= currentHeight; i++)
            obj.txStats[i] = 0;

        // generate dummy block with the current hash provided (or randomly generated)
        Block bl(buffer, currentHeight);
        obj.blocks.push_back(bl);

        // set the current hash from the dummy block
        obj.setCurrentHash((char*)bl.getHash().c_str());

        // blocks will not be read, the mempool will be empty
        // we can consider this blockchain as a truncated one, where it starts living based
        // on the current state (formed of the current height, current hash, mempool and wallets)
        // some blockchains utilize this feature since it allows deleting really old blocks
    }
    else if (currentHeight == 0)
        obj.generateGenesis();
    else{
        sysMessage("The height of the blockchain can not be negative. A fresh blockchain was generated.");
        obj.generateGenesis();
    }

    // set the status of the blockchain
    obj.status = 'A';

    return in;
}

ostream& operator<<(ostream& out, const Blockchain &obj){
    out << ANSI_COLOR_GREEN << "== BLOCKCHAIN ==\n" << ANSI_COLOR_RESET;
    out << "Current height: " << obj.getCurrentHeight() << endl;
    out << "Current hash: " << obj.getCurrentHash() << endl;
    string status = obj.getStatus() == 'A' ? " (Active)" : " (Inactive)";
    out << "Status: " << obj.getStatus() << status << endl;
    out << "Average transacted: " << float(obj.getAverageTransacted()) / 100 << endl;
    if (obj.getCurrentHeight() >= 3){
        out << "The average for the last 3 blocks was: "
            << float(obj.getTxStats()[obj.getCurrentHeight() - 2])  / 100 << " " 
            << float (obj.getTxStats()[obj.getCurrentHeight() - 1]) / 100 << " " 
            << float(obj.getTxStats()[obj.getCurrentHeight() - 0])  / 100 << endl;
    }

    out << "There are " << obj.getWallets().size() << " wallets in the blockchain.\n";

    return out;
}

Blockchain& Blockchain::operator=(const Blockchain &obj){
    if (this == &obj)
        return *this;

    this -> currentHeight = obj.currentHeight;
    this -> setCurrentHash(obj.currentHash);
    this -> mempool = obj.mempool;
    this -> blocks = obj.blocks;    // deep copy
    this -> wallets = obj.wallets;
    this -> status = obj.status;
    this -> setTxStats(obj.txStats);
    this -> averageTransacted = obj.averageTransacted;

    return *this;
}

Transaction Blockchain::operator[](string hash){
    // search for tx in the mempool
    for (auto it = this -> mempool.getTxList().begin(); it != this -> mempool.getTxList().end(); it++)
        if ((*it).getHash() == hash)
            return *it;

    // search for tx in blocks
    for (auto it = this -> blocks.begin(); it != this -> blocks.end(); it++)
        for (auto it2 = (*it).getTransactions().begin(); it2 != (*it).getTransactions().end(); it2++)
            if ((*it2).getHash() == hash)
                return *it2;

    sysMessage("The transaction with the hash provided was not found in the blockchain.");
    return Transaction();
}

Blockchain& Blockchain::operator--(){
    // remove the last tx from the mempool
    if (this -> mempool.getTxList().empty()){
        sysMessage("The mempool is empty. No transactions to remove.");
        return *this;
    }
    --(this -> mempool);
    return *this;
}

Blockchain Blockchain::operator--(int){
    // remove the last tx from the mempool
    Blockchain copy = *this;
    --(*this);
    return copy;
}

Blockchain Blockchain::operator+(Block bl){
    // process a block and add it to the blockchain
    // not using const Block& due to conflict with processBlock function
    Blockchain copy = *this;
    copy.processBlock(bl);
    return copy;
}

Blockchain Blockchain::operator-(const Transaction &tx){
    // remove a transaction from the mempool
    Blockchain copy = *this;
    copy.mempool.deleteTx(tx.getHash());
    return copy;
}

bool Blockchain::operator<(const Blockchain &obj) const{
    // compare based on the height of the blockchain
    return this -> currentHeight < obj.currentHeight;
}

bool Blockchain::operator>(const Blockchain &obj){
    return obj < *this;
}

bool Blockchain::operator==(const Blockchain &obj){
    // check if two blockchains are the same
    
    return this -> currentHeight ==  obj.currentHeight && 
           strcmp(this -> currentHash, obj.currentHash) == 0 &&
           this -> mempool       ==  obj.mempool;
}

Blockchain::operator string() const{
    // returns the hash as a string
    string aux = this -> currentHash;
    return aux;
}

// utility functions
bool Blockchain::validateTx(Transaction &tx, unordered_map<string, Wallet> &wallets){
    // checks if a transaction is valid (considering correct amounts and nonces)
    if (!tx.isMineable())
        return false;

    // check if wallet exists
    if (wallets.find(tx.getFrom()) == wallets.end())
        return false;

    // check for overflow and verify if enough funds are available
    if (tx.getAmount() + tx.getFee() < 0 || tx.getAmount() + tx.getFee() > wallets[tx.getFrom()].getBalance())
        return false;
        
    // check nonce
    if (tx.getNonce() < wallets[tx.getFrom()].getNonce() + 1 && tx.getNonce() != 0)
        return false;
    return true;
}

bool Blockchain::validateBlockTransactions(Block &bl){
    // checks if the transactions from a block are valid
    unordered_map<string, Wallet> walletsCopy = this -> wallets;
    for (auto it = bl.getTransactions().begin(); it != bl.getTransactions().end(); it++){
        Transaction tx = *it;
        if (!validateTx(tx, walletsCopy) || tx.getNonce() != walletsCopy[tx.getFrom()].getNonce() + 1)
            return false;
        if (walletsCopy.find(tx.getTo()) == walletsCopy.end())
            walletsCopy[tx.getTo()] = Wallet(tx.getTo(), 0);  // create a new wallet if the address is not in the map
        
        // simulate balance and nonce updates (this helps proposing valid blocks)
        walletsCopy[tx.getFrom()].setBalance(walletsCopy[tx.getFrom()].getBalance() - tx.getAmount());
        walletsCopy[tx.getTo()].setBalance(walletsCopy[tx.getTo()].getBalance() + tx.getAmount());
        walletsCopy[tx.getFrom()].setNonce(walletsCopy[tx.getFrom()].getNonce() + 1);
    }
    return true;

}

void Blockchain::applyBlockOnState(Block &bl){
    // applies the transactions from a block on the wallets
    // the transactions are validated before calling this function
    for (auto it = bl.getTransactions().begin(); it != bl.getTransactions().end(); it++){
        Transaction tx = *it;
        if (wallets.find(tx.getTo()) == wallets.end())
            this -> wallets[tx.getTo()] = Wallet(tx.getTo(), 0);  // create a new wallet if the address is not in the map
        
        // set balances and nonces
        this -> wallets[tx.getFrom()].setBalance(wallets[tx.getFrom()].getBalance() - tx.getAmount() - tx.getFee());
        this -> wallets[tx.getTo()].setBalance(wallets[tx.getTo()].getBalance() + tx.getAmount());
        this -> wallets[tx.getFrom()]++;    // increments nonce

        // we modify the transaction pointer held in both wallets because we are going
        // to move the tx from the mempool to a block (and the address will be modified)
        this -> wallets[tx.getFrom()].updateTx(tx.getHash(), &*it);
        if (tx.getTo() != tx.getFrom())     // avoid tx to self
            this -> wallets[tx.getTo()].updateTx(tx.getHash(), &*it);

        // the tx has been mined
        this -> mempool.deleteTx(tx.getHash());
    }
}

void Blockchain::processBlock(Block &bl){
    // processes a block and adds it to the blockchain
    if (bl.getHeight() != this -> currentHeight + 1){
        sysMessage("The height of the new block is not consistent with the current height. The block was not processed.");
        return;
    }
    if (bl.getParentHash() != this -> currentHash){
        sysMessage("The parent hash of the new block does not match the hash of the current block. The block was not processed.");
        return;
    }
    if (!this -> validateBlockTransactions(bl)){
        sysMessage("Block contains invalid transactions and will not be processed.");
        return;
    }

    this -> blocks.push_back(bl);
    this -> applyBlockOnState(this -> blocks.back());   // we apply the block which was copied into the blockchain
                                                        // for proper references to the transactions
    this -> setCurrentHeight(this -> currentHeight + 1);
    this -> setCurrentHash((char*)bl.getHash().c_str());
    this -> updateStatistics(bl);
    this -> cleanMempool();
    this -> cleanWallets();
}

void Blockchain::updateStatistics(Block& bl){
    // updates the statistics of the blockchain after a new block is added
    // does an arithmetic mean of all spends in all blocks

    // copy the old array
    float *aux = new float[this -> currentHeight + 1];
    if (this -> txStats)
        for (int i = 1; i <= this -> currentHeight - 1; i++)
            aux[i] = this -> txStats[i];
    
    // calculate the sum of the transactions in the new block
    aux[this -> currentHeight] = 0;
    for (auto it = bl.getTransactions().begin(); it != bl.getTransactions().end(); it++){
        aux[this -> currentHeight] += (*it).getAmount();
    }
    if (aux[this -> currentHeight] != 0)
        aux[this -> currentHeight] /= bl.getTransactions().size();

    // delete old array
    if (this -> txStats)
        delete[] this -> txStats;
    this -> txStats = aux;

    double sum = 0;
    for (int i = 1; i <= this -> currentHeight; i++)
        sum += this -> txStats[i];
    this -> setAverageTransacted(sum / this -> currentHeight);
}

void Blockchain::generateGenesis(){
    // generates the genesis block of the blockchain
    // this block is special 
    if (this -> status != 'I'){
        sysMessage("The blockchain is not in initializing state. The genesis block was not generated.");
        return;
    }

    // generate the first block
    Block genesisBlock("0xdeadbeef", 0);    // parent hash is a special value (usually random)
    this -> blocks.push_back(genesisBlock);

    // generate the god wallet
    Wallet godWallet(Transaction::getGodAddress(), 100000);
    this -> wallets[Transaction::getGodAddress()] = godWallet;

    // set the remaining fields for the blockchain
    this -> currentHeight = 0;
    this -> setCurrentHash((char*)genesisBlock.getHash().c_str());
    this -> setStatus('A');
}

void Blockchain::sendTx(Transaction &tx){
    // sends a transaction to the mempool
    if (!validateTx(tx, this -> wallets)){
        sysMessage("The transaction is invalid. It will not be added to the mempool.");
        return;
    }
    this -> mempool.addTx(tx);

    // we also register the tx in the respective wallets
    this -> wallets[tx.getFrom()].addTx(&this -> mempool.getTxList().back());

    // if the "to" wallet does not exist, create it
    if (this -> wallets.find(tx.getTo()) == this -> wallets.end())
        this -> wallets[tx.getTo()] = Wallet(tx.getTo(), 0);

    if (tx.getTo() != tx.getFrom())     // avoid tx to self
        this -> wallets[tx.getTo()].addTx(&this -> mempool.getTxList().back());

    this -> mempool.updateAverageFee();
}

Block Blockchain::proposeBlock(){
    // proposes a new block to be added to the blockchain
    // the block is proposed based on the transactions in the mempool
    if (this -> mempool.getTxList().empty()){
        info("The mempool is empty. Empty block was generated.");
        return Block(this -> currentHash, this -> currentHeight + 1);
    }

    list<Transaction> txList = this -> mempool.getTxList();
    // we sort the mempool such that we can process the transaction in the order of their nonce
    // otherwise, some transactions would end up being mined separately
    // some more complex logic could be added to process transactions based on money flow to further optimize this process
    txList.sort();
    
    unordered_map<string, Wallet> walletsCopy = this -> wallets;
    Block newBlock(this -> currentHash, this -> currentHeight + 1);
    for (auto it = txList.begin(); it != txList.end(); it++){
        Transaction tx = *it;
        if (validateTx(tx, walletsCopy) && tx.getNonce() == walletsCopy[tx.getFrom()].getNonce() + 1){
            tx.setIsMined(true);
            newBlock.addTx(tx);

            // we simulate balance updates to produce a valid block
            walletsCopy[tx.getFrom()].setBalance(walletsCopy[tx.getFrom()].getBalance() - tx.getAmount());
            walletsCopy[tx.getTo()].setBalance(walletsCopy[tx.getTo()].getBalance() + tx.getAmount());
            walletsCopy[tx.getFrom()].setNonce(walletsCopy[tx.getFrom()].getNonce() + 1);
        }
        else warning("Transaction skipped.");
    }

    return newBlock;
}

int Blockchain::getAccountNonce(string addr){
    // returns the nonce of an account
    return this -> wallets[addr].getNonce();
}

Transaction Blockchain::readTx(){
    // reads a tx from the console and sets the nonce if necessary
    Transaction tx;
    cin >> tx;

    if (tx.getNonce() != 0)
        return tx;

    if (this -> wallets.find(tx.getFrom()) == this -> wallets.end()){
        warning("The sender of the transaction does not exist.");
        tx.setNonce(1);
        return tx;
    }

    tx.setNonce(this -> wallets[tx.getFrom()].getNonce() + 1);
    tx.updateHash();

    return tx;
}

void Blockchain::cleanMempool(){
    // this functions drops old transactions from the mempool
    // if the nonce of an account is higher than a transaction in the mempool, it is dropped
    list<Transaction> txList = this -> mempool.getTxList();
    for (auto it = txList.begin(); it != txList.end(); it++){
        if ((*it).getNonce() <= this -> getAccountNonce((*it).getFrom())){
            // delete from wallets
            this -> wallets[(*it).getFrom()].deleteTx((*it).getHash());
            this -> wallets[(*it).getTo()].deleteTx((*it).getHash());

            // delete from the mempool
            this -> mempool.deleteTx((*it).getHash());
        }
    }
}

void Blockchain::cleanWallets(){
    // removes wallets that have no transactions (god wallet is excluded)
    for (auto it = this -> wallets.begin(); it != this -> wallets.end();){
        if ((*it).second.getTxList().empty() && (*it).first != Transaction::getGodAddress())
            it = this -> wallets.erase(it);
        else it++;
    }
}

// ----------------- MAIN -----------------

int normalMenuSpeed = 35;
int fastMenuSpeed = 15;
bool skipMenu = false;
void slowPrint(string s, int ms = normalMenuSpeed, bool reset_menu_skip = false){
    // slowly prints characters to the console
    // to give the impression of someone typing
    // it also contains logic for skipping over the slow print

    if (skipMenu)  // skip the slow print
        ms = 0;

    for (char c : s){
        if (_kbhit() && _getch() == '\r')   // if the user presses enter, skip the slow print (until the next menu is displayed)
            ms = 0, skipMenu = true;

        cout << c << flush;
        if (ms < 3 && c != '\n')
            continue;
        this_thread::sleep_for(chrono::milliseconds(ms));
    }

    // reset the skip_menu flag
    if (reset_menu_skip)
        skipMenu = false;
}

void continuePrompt(){
    // waits for the user to press any key to continue
    _getch();
}

// This function refreshes the console
// and displays a visual element
void refreshConsole(){
    system("cls");    // no remote access to this program so it should be safe enough
    cout << ANSI_COLOR_GREEN;
    cout << "+--------------------------+\n";
    cout << "|   Blockchain Simulator   |\n";
    cout << "| Created by Andrei Stefan |\n";
    cout << "+--------------------------+\n\n";
    cout << ANSI_COLOR_RESET;
}

int main(int argc, char* argv[]){
    cout << fixed << setprecision(2);   // set cout fp precision

    Blockchain bc;   // our blockchain
    int choice;      // some auxiliary variables
    char char_choice;

    // take --fast as an argument to speed up the menu
    if (argc >= 2 && strcmp(argv[1], "--fast") == 0)
        normalMenuSpeed = 3, fastMenuSpeed = 2;

    cout << ANSI_COLOR_RESET;
    refreshConsole();

    // welcome menu
    slowPrint("Welcome to the blockchain simulator!\n");
    slowPrint("This program simulates the main features of a blockchain: sending transactions, mining blocks, and managing wallets.\n\n");

    slowPrint("How would you like to start the simulation?\n");
    slowPrint("1. Create a fresh blockchain (Recommended)\n", fastMenuSpeed);
    slowPrint("2. Introduce the details of a blockchain manually\n", fastMenuSpeed);
    slowPrint("3. Read a short introduction about blockchains\n", fastMenuSpeed, true);
    char_choice = _getch();
    
    refreshConsole();
    // short introduction to blockchain basics
    if (char_choice == '3'){
        // transactions info
        slowPrint(" -=-=-=- Transactions -=-=-=- \n\n", fastMenuSpeed);
        slowPrint("A transaction represents the transfer of coins from one wallet to another.\n");
        slowPrint("This is how a transaction looks like:\n\n");
        Transaction tx(Transaction::getGodAddress(), Transaction::getGodAddress(), 0, 0);
        cout << tx << endl;
        slowPrint("Each transaction has a unique hash, a sender, a receiver, an amount, a fee, and a nonce.\n\n");
        slowPrint("The hash is like an ID for the transaction and is usually expressed as a hex string.\n\n");
        slowPrint("The nonce is a number that is incremented each time a transaction is sent from an address.\n\n");
        slowPrint("This helps prevent sending the same transaction twice or spending the same coins twice (you would have a different tx with a different nonce).\n\n");
        slowPrint("Our simulator will also keep track if a transaction is mined into a block, but this is not necessary for the blockchain (and is excluded from the hash calculation).\n\n");
        slowPrint("Press any key to continue.\n", normalMenuSpeed, true);
        continuePrompt();
        refreshConsole();

        // blocks info
        slowPrint(" -=-=-=- Blocks -=-=-=- \n\n", fastMenuSpeed);
        slowPrint("A block is a collection of transactions that are processed by the blockchain.\n\n");
        slowPrint("If a transaction is not in a mined block, it is not considered processed. In some networks a transaction is considered processed after multiple blocks have been mined on top of the block containing the transaction.\n\n");
        slowPrint("This is how a block looks like:\n\n");
        Block bl("0x123beef", 420);
        cout << bl << endl;
        slowPrint("Like transactions, blocks also have hases (IDs).\n\n");
        slowPrint("Additionally, to create a link between blocks, each one contains the hash of the previous block.\n");
        slowPrint("This means the entire history of a blockchain is reflected in the hash of the last block.\n");
        slowPrint("If you modify a hash in an old block, all blocks that come after it will have to be modified as well.\n\n");
        slowPrint("Press any key to continue.\n", normalMenuSpeed, true);
        continuePrompt();
        refreshConsole();

        // wallets info
        slowPrint(" -=-=-=- Wallets -=-=-=- \n\n", fastMenuSpeed);
        slowPrint("The blockchain needs quick access to the details of all wallets in the network to process transactions.\n");
        slowPrint("So it stores some relevant information for each active wallet.\n\n");
        slowPrint("This collection of wallets, along with other information, may be called the state of the blockchain.\n");
        slowPrint("This state can be reconstructed by processing each block.\n\n");
        slowPrint("This is how a wallet looks like:\n\n");
        Wallet w(Transaction::getGodAddress(), 1000);
        cout << w << endl;
        slowPrint("Notice that we also keep track of the nonce of each wallet so we can validate their transactions.\n\n");
        slowPrint("Press any key to continue.\n", normalMenuSpeed, true);
        continuePrompt();
        refreshConsole();

        // mempool info
        slowPrint(" -=-=-=- Mempool -=-=-=- \n\n", fastMenuSpeed);
        slowPrint("The mempool is a waiting room for transactions that are yet to be mined in a block.\n\n");
        slowPrint("Mempools don't usually have rigid rules. Depending on the network, minimal fees may be required for a transaction to be held in the mempool.\n");
        slowPrint("Mempools may also have a maximum size to prevent bad actors from spamming the network with transactions.\n\n");
        slowPrint("Press any key to continue.\n", normalMenuSpeed, true);
        continuePrompt();
        refreshConsole();

        // blockchain info
        slowPrint(" -=-=-=- Blockchain -=-=-=- \n\n", fastMenuSpeed);
        slowPrint("The blockchain can be thought of as a set of rules (the protocol) used to come to a consensus about what transactions are valid and which should be processed.\n\n");
        slowPrint("All this is done in a decentralized manner, meaning that the network needs to be able to work without participants \"trusting\" each other.\n");
        slowPrint("Everything needs to be verifiable: transactions are cryptographically signed and blocks are not proposed by the same entity.\n\n");
        slowPrint("Press any key to continue.\n", normalMenuSpeed, true);
        continuePrompt();
        refreshConsole();  

        // show the options again after tutorial
        slowPrint("How would you like to start the simulation?\n");
        slowPrint("1. Create a fresh blockchain (Recommended)\n", fastMenuSpeed);
        slowPrint("2. Introduce the details of a blockchain manually\n", fastMenuSpeed, true);

        do{     // handle invalid options
            char_choice = _getch();
        } while (char_choice != '1' && char_choice != '2');
        refreshConsole();
    }

    // blockchain creation menu
    if (char_choice == '1'){
        // option 1: create a fresh blockchain
        bc.generateGenesis();
        slowPrint("A fresh blockchain has been created.\n\n");
        slowPrint("A new blockchain usually starts with a genesis block.\n");
        slowPrint("This block is a special one since it is not linked to any other block (like the others must be).\n");
        slowPrint("In this case, the field that would link it to the previous block contains a special value (0xdeafbeef).\n");
        slowPrint("Here are the details of this block:\n\n");
        cout << bc.getBlocks().front() << endl << endl;
        slowPrint("In our case, along with the generation of this block, a wallet has been created for the \"God\" address.\n");
        slowPrint("This wallet contains 1000 coins:\n\n");
        cout << ANSI_COLOR_GREEN << "~=~=~=~=~=~=~=~=~=~=~= WALLET ~=~=~=~=~=~=~=~=~=~=~\n" << ANSI_COLOR_RESET;
        cout << bc.getWallets().at(Transaction::getGodAddress()) << endl;
        slowPrint("You can now start sending transactions and mining blocks. Press any key to continue.\n", normalMenuSpeed, true);
        continuePrompt();
        refreshConsole();
    }
    else if (char_choice == '2'){
        // option 2: introduce the details of the blockchain manually
        slowPrint("Please introduce the details of the blockchain.\n\n");
        cin >> bc;
        slowPrint("\nThe blockchain has been created:\n\n");
        cout << bc << endl << endl;

        slowPrint("You can now start sending transactions and mining blocks. Press any key to continue.\n", normalMenuSpeed, true);
        continuePrompt();
        refreshConsole();
    }

    int menuEntries = 0;  // auxiliary variables for UX
    bool case6 = false;
    while (true){
        if (menuEntries == 2)
            normalMenuSpeed = 3, fastMenuSpeed = 2;  // speed up the menu after the first 2 entries
        menuEntries++;

        // main menu
        slowPrint("What would you like to do?\n\n", fastMenuSpeed);
        slowPrint("1. View the mempool\n", fastMenuSpeed);
        slowPrint("2. View the wallets\n", fastMenuSpeed);
        slowPrint("3. View the details of the last block\n", fastMenuSpeed);
        slowPrint("4. View the details of the blockchain\n", fastMenuSpeed);
        slowPrint("5. Send a transaction\n", fastMenuSpeed);
        slowPrint("6. Mine a block\n", fastMenuSpeed);
        slowPrint("7. Exit the application\n", fastMenuSpeed, true);

        char_choice = _getch();

        // menu based on user choice
        refreshConsole();
        int i = 0;              // auxiliary variables for UX and menu logic
        Transaction tx;
        Block bl;
        switch (char_choice){
            case '1':
                // view the mempool
                cout << bc.getMempool() << endl;
                break;
            case '2':
                // view the wallets
                slowPrint("The wallets in the blockchain are:\n\n", fastMenuSpeed);
                for (pair<string, Wallet> pair : bc.getWallets()){
                    cout << ANSI_COLOR_GREEN << " =-=-=-=-=-=-=-=-=-=- Wallet " << ++i << " -=-=-=-=-=-=-=-=-=-= \n" << ANSI_COLOR_RESET;
                    cout << pair.second << endl;
                }
                break;
            case '3':
                // view the details of the last block
                slowPrint("The details of the last block are:\n\n", fastMenuSpeed);
                cout << bc.getBlocks().back() << endl;
                break;
            case '4':
                // view the details of the blockchain
                cout << bc << endl;
                break;
            case '5':
                // send a transaction
                slowPrint("Enter the details of the transaction:\n\n", fastMenuSpeed);
                tx = bc.readTx();
                bc.sendTx(tx);
                slowPrint("The transaction has been submitted to the mempool.\n", normalMenuSpeed, true);
                break;
            case '6':
                // mine a block
                if (case6 == false){
                    // if first time, display some info about mining

                    // speed up the menu only if --fast was given as an argument
                    int aux_normal_speed = 35;
                    if (argc >= 2 && strcmp(argv[1], "--fast") == 0)
                        aux_normal_speed = 3;

                    slowPrint("In a blockchain, miners around the world propose new blocks to be added to the blockchain.\n", aux_normal_speed);
                    slowPrint("If they are valid, the rest of the network will accept them and add them to the blockchain.\n", aux_normal_speed);
                    slowPrint("In this context, the blockchain will contain blocks proposed by us.\n\n", aux_normal_speed);
                    slowPrint("This is the block we have crafted using the transactions from the mempool:\n\n", aux_normal_speed);
                    bl = bc.proposeBlock();
                    cout << bl << endl;
                    slowPrint("Press any key to add the block to the blockchain.\n", normalMenuSpeed, true);
                    continuePrompt();
                    bc.processBlock(bl);
                    refreshConsole();

                    case6 = true;
                }
                else {  
                    // do not display the info about mining again
                    bl = bc.proposeBlock();
                    bc.processBlock(bl);
                    slowPrint("Details of the crafted block:\n\n");
                    cout << bl << endl;
                    slowPrint("Block has been added to the blockchain. Press any key to continue.\n", normalMenuSpeed, true);
                    char_choice = _getch();
                    refreshConsole();
                }
                continue;
            case '7':
                // exit the application
                slowPrint("Are you sure you want to exit the application (nothing is saved)? (Y/n)\n", fastMenuSpeed);
                do{
                    char_choice = _getch();
                } while (char_choice != 'Y' && char_choice != 'y' && char_choice != 'N' && char_choice != 'n');

                if (char_choice == 'Y' || char_choice == 'y'){
                    system("cls");
                    return 0;
                }
                
                refreshConsole();
                continue;
            default:
                slowPrint("Invalid choice. Please try again...\n", normalMenuSpeed, true);
                break;
        }

        slowPrint("Press any key to continue.\n", normalMenuSpeed, true);
        continuePrompt();
        refreshConsole();
    }

    return 0;
}