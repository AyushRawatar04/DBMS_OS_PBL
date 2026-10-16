#ifndef PAYMENT_H
#define PAYMENT_H

class Payment {
public:
    virtual void pay(int amount) = 0;
    virtual ~Payment() = default;
};

class UPI : public Payment {
public:
    void pay(int amount) override;
};

class CreditCard : public Payment {
public:
    void pay(int amount) override;
};

class CashPayment : public Payment {
public:
    void pay(int amount) override;
};

#endif
