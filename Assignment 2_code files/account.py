def make_account(balance):
    def deposit(amount):
        nonlocal balance
        balance += amount
        return balance
    return deposit

acct = make_account(100)
print(acct(50))            # 150
value = 10
value = "text"             # dynamic typing: rebinds freely
print(value)