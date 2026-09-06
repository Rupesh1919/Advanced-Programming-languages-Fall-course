#include <iostream>
#include <functional>
using namespace std;

function<int(int)> makeAccount(int balance) {
    return [balance](int amount) mutable {
        balance += amount;
        return balance;
    };
}

int main() {
    auto acct = makeAccount(100);
    cout << acct(50) << endl;   // 150
    // int v = "text";  // won't compile: static typing
}