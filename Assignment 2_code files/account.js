function makeAccount(balance) {
    return function (amount) {
        balance += amount;
        return balance;
    };
}

const acct = makeAccount(100);
console.log(acct(50));      // 150
console.log(10 + "5");      // "105": weak typing coerces