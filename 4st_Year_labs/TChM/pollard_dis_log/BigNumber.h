#ifndef _BIGNUM_H_
#define _BIGNUM_H_

#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <string>
#include <vector>
#include <cstdint>
#include <cstring>
#include <ctime>
#include <cstdlib>
#include <cmath>

typedef uint32_t BASE;
typedef uint64_t DBASE;
#define BASE_SIZE (sizeof(BASE) * 8)

class BN {
public:
    int len;
    int maxlen;
    BASE* coef;

    BN();
    explicit BN(int ml, int t = 0);
    BN(long long value);
    BN(const BN& bNum);

    ~BN();

    BN& operator=(const BN& bNum);
    BN& operator=(long long value);

    bool operator==(const BN& bNum) const;
    bool operator!=(const BN& bNum) const;
    bool operator<(const BN& bNum) const;
    bool operator<=(const BN& bNum) const;
    bool operator>(const BN& bNum) const;
    bool operator>=(const BN& bNum) const;

    bool operator==(long long value) const;
    bool operator!=(long long value) const;
    bool operator<(long long value) const;
    bool operator<=(long long value) const;
    bool operator>(long long value) const;
    bool operator>=(long long value) const;

    BN operator+(const BN& bNum) const;
    BN& operator+=(const BN& bNum);
    BN operator-(const BN& bNum) const;
    BN& operator-=(const BN& bNum);
    BN operator*(const BN& bNum) const;
    BN& operator*=(const BN& bNum);
    BN operator/(const BN& bNum) const;
    BN operator%(const BN& bNum) const;

    BN operator+(long long value) const;
    BN& operator+=(long long value);
    BN operator-(long long value) const;
    BN& operator-=(long long value);
    BN operator*(long long value) const;
    BN& operator*=(long long value);
    BN operator/(long long value) const;
    BN operator%(long long value) const;

    BN pow_mod(const BN& exponent, const BN& mod) const;
    BN gcd(const BN& num) const;
    double to_double() const;
    BN sqrt() const;
    BN square() const;
    BN root(int n) const;
    BN pow(int exponent) const;
    BN pow(const BN& exponent) const;
    bool isPrime(int t = 5) const;
    int log(int a) const;
    BN fastPow(const BN& b, int n, const BN& m) const;

    void cout_base10() const;
    void cin_base10();

    friend BN operator*(BASE num, const BN& bNum);
    friend std::ostream& operator<<(std::ostream& out, const BN& bNum);
    friend std::istream& operator>>(std::istream& in, BN& bNum);
};

BN::BN() : len(1), maxlen(1) {
    coef = new BASE[maxlen];
    coef[0] = 0;
}

BN::BN(int ml, int t) : len(1), maxlen(ml) {
    coef = new BASE[maxlen];
    std::fill(coef, coef + maxlen, 0);

    if (t == 1) {
        for (int i = 0; i < len; i++) {
            coef[i] = rand();
        }
        while (len > 1 && coef[len - 1] == 0) {
            len--;
        }
    }
    else if (t != 0) {
        throw std::invalid_argument("Invalid constructor argument.");
    }
}

BN::BN(long long value) {
    if (value < 0) {
        throw std::invalid_argument("Negative value not supported.");
    }

    maxlen = 1;
    long long temp = value;
    while (temp >= (1LL << BASE_SIZE)) {
        temp >>= BASE_SIZE;
        maxlen++;
    }

    coef = new BASE[maxlen];
    std::fill(coef, coef + maxlen, 0);
    len = 0;
    temp = value;
    while (temp > 0) {
        coef[len++] = static_cast<BASE>(temp & ((1ULL << BASE_SIZE) - 1));
        temp >>= BASE_SIZE;
    }
    if (len == 0) {
        len = 1;
        coef[0] = 0;
    }
}

BN::BN(const BN& bNum) : len(bNum.len), maxlen(bNum.maxlen) {
    coef = new BASE[maxlen];
    std::copy(bNum.coef, bNum.coef + len, coef);
}

BN::~BN() {
    delete[] coef;
}

BN& BN::operator=(const BN& bNum) {
    if (this != &bNum) {
        BASE* new_coef = new BASE[bNum.maxlen];
        std::copy(bNum.coef, bNum.coef + bNum.len, new_coef);
        delete[] coef;
        coef = new_coef;
        len = bNum.len;
        maxlen = bNum.maxlen;
    }
    return *this;
}

BN& BN::operator=(long long value) {
    if (value < 0) {
        throw std::invalid_argument("Negative value not supported.");
    }

    delete[] coef;

    maxlen = 1;
    long long temp = value;
    while (temp >= (1LL << BASE_SIZE)) {
        temp >>= BASE_SIZE;
        maxlen++;
    }

    coef = new BASE[maxlen];
    std::fill(coef, coef + maxlen, 0);
    len = 0;
    temp = value;
    while (temp > 0) {
        coef[len++] = static_cast<BASE>(temp & ((1ULL << BASE_SIZE) - 1));
        temp >>= BASE_SIZE;
    }
    if (len == 0) {
        len = 1;
        coef[0] = 0;
    }
    return *this;
}

bool BN::operator==(const BN& bNum) const {
    if (len != bNum.len) return false;
    for (int i = 0; i < len; ++i) {
        if (coef[i] != bNum.coef[i]) return false;
    }
    return true;
}

bool BN::operator!=(const BN& bNum) const {
    return !(*this == bNum);
}

bool BN::operator<(const BN& bNum) const {
    if (len != bNum.len)
        return len < bNum.len;
    for (int i = len - 1; i >= 0; --i) {
        if (coef[i] != bNum.coef[i])
            return coef[i] < bNum.coef[i];
    }
    return false;
}

bool BN::operator<=(const BN& bNum) const {
    return (*this < bNum) || (*this == bNum);
}

bool BN::operator>(const BN& bNum) const {
    return !(*this <= bNum);
}

bool BN::operator>=(const BN& bNum) const {
    return !(*this < bNum);
}

bool BN::operator==(long long value) const {
    BN temp(value);
    return (*this == temp);
}

bool BN::operator!=(long long value) const {
    return !(*this == value);
}

bool BN::operator<(long long value) const {
    BN temp(value);
    return (*this < temp);
}

bool BN::operator<=(long long value) const {
    BN temp(value);
    return (*this <= temp);
}

bool BN::operator>(long long value) const {
    BN temp(value);
    return (*this > temp);
}

bool BN::operator>=(long long value) const {
    BN temp(value);
    return (*this >= temp);
}

BN BN::operator+(const BN& bNum) const {
    int l = std::max(len, bNum.len) + 1;
    BN result(l, 0);
    DBASE carry = 0;
    int i = 0;
    for (; i < std::max(len, bNum.len); ++i) {
        DBASE sum = carry;
        if (i < len) sum += coef[i];
        if (i < bNum.len) sum += bNum.coef[i];
        result.coef[i] = static_cast<BASE>(sum & ((1ULL << BASE_SIZE) - 1));
        carry = sum >> BASE_SIZE;
    }
    if (carry) {
        result.coef[i++] = static_cast<BASE>(carry);
    }
    result.len = i;
    while (result.len > 1 && result.coef[result.len - 1] == 0) {
        result.len--;
    }
    return result;
}

BN& BN::operator+=(const BN& bNum) {
    *this = *this + bNum;
    return *this;
}

BN BN::operator-(const BN& bNum) const {
    if (*this < bNum) {
        throw std::invalid_argument("Subtraction results in negative value, which is not supported.");
    }

    BN result(maxlen, 0);
    DBASE borrow = 0;
    int i = 0;
    for (; i < bNum.len; ++i) {
        DBASE diff = static_cast<DBASE>(coef[i]) - static_cast<DBASE>(bNum.coef[i]) - borrow;
        if (diff >= (1ULL << BASE_SIZE)) {
            diff += (1ULL << BASE_SIZE);
            borrow = 1;
        }
        else {
            borrow = 0;
        }
        result.coef[i] = static_cast<BASE>(diff & ((1ULL << BASE_SIZE) - 1));
    }
    for (; i < len; ++i) {
        if (coef[i] < borrow) {
            result.coef[i] = static_cast<BASE>(coef[i] + (1ULL << BASE_SIZE) - borrow);
            borrow = 1;
        }
        else {
            result.coef[i] = static_cast<BASE>(coef[i] - borrow);
            borrow = 0;
        }
    }
    result.len = len;
    while (result.len > 1 && result.coef[result.len - 1] == 0) {
        result.len--;
    }
    return result;
}

BN& BN::operator-=(const BN& bNum) {
    *this = *this - bNum;
    return *this;
}

BN BN::operator*(const BN& bNum) const {
    BN result(len + bNum.len, 0);
    for (int i = 0; i < len; ++i) {
        DBASE carry = 0;
        for (int j = 0; j < bNum.len; ++j) {
            DBASE temp = static_cast<DBASE>(coef[i]) * static_cast<DBASE>(bNum.coef[j]) +
                          result.coef[i + j] + carry;
            result.coef[i + j] = static_cast<BASE>(temp & ((1ULL << BASE_SIZE) - 1));
            carry = temp >> BASE_SIZE;
        }
        result.coef[i + bNum.len] += static_cast<BASE>(carry);
    }
    result.len = len + bNum.len;
    while (result.len > 1 && result.coef[result.len - 1] == 0) {
        result.len--;
    }
    return result;
}

BN& BN::operator*=(const BN& bNum) {
    *this = *this * bNum;
    return *this;
}

BN BN::operator/(const BN& bNum) const {
    if (bNum.len == 1 && bNum.coef[0] == 0) {
        throw std::invalid_argument("Division by zero.");
    }
    if (*this < bNum) {
        return BN(1, 0); // 0
    }

    BN dividend(*this);
    BN divisor(bNum);
    BN quotient;
    BN zero;
    while (dividend >= divisor) {
        dividend = dividend - divisor;
        quotient = quotient + 1;
    }
    return quotient;
}

BN BN::operator%(const BN& bNum) const {
    if (bNum.len == 1 && bNum.coef[0] == 0) {
        throw std::invalid_argument("Modulo by zero.");
    }
    if (*this < bNum) {
        return *this;
    }

    BN dividend(*this);
    BN divisor(bNum);
    BN zero;
    while (dividend >= divisor) {
        dividend = dividend - divisor;
    }
    return dividend;
}

BN BN::operator+(long long value) const {
    BN temp(value);
    return *this + temp;
}

BN& BN::operator+=(long long value) {
    *this = *this + value;
    return *this;
}

BN BN::operator-(long long value) const {
    BN temp(value);
    return *this - temp;
}

BN& BN::operator-=(long long value) {
    *this = *this - value;
    return *this;
}

BN BN::operator*(long long value) const {
    BN temp(value);
    return *this * temp;
}

BN& BN::operator*=(long long value) {
    *this = *this * value;
    return *this;
}

BN BN::operator/(long long value) const {
    if (value == 0) {
        throw std::invalid_argument("Division by zero.");
    }
    BN divisor(value);
    return *this / divisor;
}

BN BN::operator%(long long value) const {
    if (value == 0) {
        throw std::invalid_argument("Modulo by zero.");
    }
    BN divisor(value);
    return *this % divisor;
}

BN BN::pow_mod(const BN& exponent, const BN& mod) const {
    if (mod == BN(1)) return BN(0);

    BN result = 1;
    BN base = *this % mod;
    BN exp = exponent;

    while (exp > BN(0)) {
        if (exp.coef[0] & 1) {
            result = (result * base) % mod;
        }
        base = (base * base) % mod;
        bool carry = false;
        for (int i = exp.len - 1; i >= 0; --i) {
            BASE new_val = (exp.coef[i] >> 1) | (carry ? (1 << (BASE_SIZE - 1)) : 0);
            carry = (exp.coef[i] & 1) != 0;
            exp.coef[i] = new_val;
        }
        while (exp.len > 1 && exp.coef[exp.len - 1] == 0) {
            exp.len--;
        }
    }
    return result;
}

BN BN::gcd(const BN& num) const {
    BN a = *this;
    BN b = num;
    BN zero;
    zero = 0;

    while (b != zero) {
        BN temp = b;
        b = a % b;
        a = temp;
    }

    return a;
}

double BN::to_double() const {
    double result = 0;
    double base = 1;
    for (int i = 0; i < len; i++) {
        result += static_cast<double>(coef[i]) * base;
        base *= static_cast<double>(1ULL << BASE_SIZE);
    }
    return result;
}

BN BN::square() const {
    return (*this) * (*this);
}

BN BN::sqrt() const {
    if (*this < BN(0)) {
        throw std::invalid_argument("Square root of negative number is not supported.");
    }
    BN a = *this;
    BN x0 = a;
    BN one = 1;
    BN x1 = (x0 + a / x0) / 2;
    while (x1 < x0) {
        x0 = x1;
        x1 = (x0 + a / x0) / 2;
    }
    return x0;
}

BN BN::root(int n) const {
    if (*this == 0) return *this;
    if (n <= 0) throw std::invalid_argument("Root exponent must be positive.");
    
    BN a = *this;
    BN x0, x1 = a;
    BN one = 1;
    
    do {
        x0 = x1;
        x1 = ((x0 * (n - 1)) + (a / x0.pow(n - 1))) / n;
    } while (x0 > x1);
    
    return x0;
}

BN BN::pow(int exponent) const {
    if (exponent < 0) throw std::invalid_argument("Exponent must be non-negative.");
    BN result = 1;
    BN base = *this;
    
    while (exponent > 0) {
        if (exponent % 2 == 1) {
            result = result * base;
        }
        base = base * base;
        exponent /= 2;
    }
    
    return result;
}

BN BN::pow(const BN& exponent) const {
    BN result = 1;
    BN base = *this;
    BN exp = exponent;
    BN zero = 0;
    BN one = 1;
    
    while (exp > zero) {
        if (exp.coef[0] % 2 == 1) {
            result = result * base;
        }
        base = base * base;
        exp = exp / 2;
    }
    
    return result;
}

BN pseudorandom(BN lower, BN higher) {
    BN random(higher.maxlen, 1);
    if (higher > lower) {
        random = random % (higher - lower);
        random = random + lower;
    }
    else {
        random = lower;
    }
    return random;
}

bool BN::isPrime(int t) const {
    BN n = *this;
    BN one = 1;
    BN two = 2;
    BN zero = 0;
    
    if (n == two) return true;
    if (n < two || n % two == zero) return false;

    BN d = n - one;
    int s = 0;
    while (d % two == zero) {
        d = d / two;
        s++;
    }

    for (int i = 0; i < t; i++) {
        BN a = pseudorandom(two, n - two);
        BN x = a.pow_mod(d, n);

        if (x == one || x == n - one) continue;

        bool is_composite = true;
        for (int j = 1; j < s; j++) {
            x = x.square() % n;
            if (x == n - one) {
                is_composite = false;
                break;
            }
            if (x == one) return false;
        }

        if (is_composite) return false;
    }

    return true;
}

int BN::log(int a) const {
    if (a <= 1) throw std::invalid_argument("Base of log must be greater than 1.");
    
    BN b = *this;
    BN result = 1;
    int exponent = 0;
    
    while (result <= b) {
        result = result * a;
        exponent++;
    }
    
    return exponent - 1;
}

BN BN::fastPow(const BN& b, int n, const BN& m) const {
    BN result = 1;
    BN base = b;
    BN mod = m;
    
    while (n > 0) {
        if (n % 2 == 1) {
            result = (result * base) % mod;
        }
        base = (base * base) % mod;
        n /= 2;
    }
    
    return result;
}

void BN::cout_base10() const {
    if (len == 1 && coef[0] == 0) {
        std::cout << "0";
        return;
    }
    BN temp = *this;
    std::string result;
    BN zero = 0;
    BN ten = 10;
    while (temp > zero) {
        BN digit = temp % ten;
        result += '0' + digit.coef[0];
        temp = temp / 10;
    }
    std::reverse(result.begin(), result.end());
    std::cout << result;
}

void BN::cin_base10() {
    std::string s;
    std::cin >> s;
    *this = 0;
    BN ten = 10;
    for (char c : s) {
        if (c >= '0' && c <= '9') {
            *this = (*this * ten) + (c - '0');
        }
        else {
            throw std::invalid_argument("Invalid character in input.");
        }
    }
}

BN operator*(BASE num, const BN& bNum) {
    BN result(bNum.len + 1, 0);
    DBASE carry = 0;
    int i = 0;
    for (; i < bNum.len; ++i) {
        DBASE temp = static_cast<DBASE>(bNum.coef[i]) * static_cast<DBASE>(num) + carry;
        result.coef[i] = static_cast<BASE>(temp & ((1ULL << BASE_SIZE) - 1));
        carry = temp >> BASE_SIZE;
    }
    if (carry) {
        result.coef[i++] = static_cast<BASE>(carry);
    }
    result.len = i;
    while (result.len > 1 && result.coef[result.len - 1] == 0) {
        result.len--;
    }
    return result;
}

std::ostream& operator<<(std::ostream& out, const BN& bNum) {
    bNum.cout_base10();
    return out;
}

std::istream& operator>>(std::istream& in, BN& bNum) {
    bNum.cin_base10();
    return in;
}


#endif // _BIGNUM_H_
