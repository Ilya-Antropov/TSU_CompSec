#include <algorithm>
#include <cstring>
#include <ctime>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>

using namespace std;

typedef unsigned int BASE;
typedef unsigned long long DBASE;
#define BASE_SIZE (sizeof(BASE) * 8)
#define DBASE_SIZE (sizeof(DBASE) * 8)

class BN {
private:
    BASE* coef;
    int len;
    int maxlen;

public:
    BN(int maxLen = 1, int parameter = 0);
    BN(const BN&);
    ~BN();

    void lenNorm();
    bool operator==(const BN&) const;
    bool operator!=(const BN&) const;
    bool operator<(const BN&) const;
    bool operator>(const BN&) const;
    bool operator<=(const BN&) const;
    bool operator>=(const BN&) const;
    BN& operator=(const BN&);
    BN operator+(const BN&) const;
    BN& operator+=(const BN&);
    BN operator-(const BN&) const;
    BN& operator-=(const BN&);
    BN operator*(const BASE&) const;
    BN& operator*=(const BASE&);
    BN operator*(const BN&) const;
    BN& operator*=(const BN&);
    BN operator/(const BASE&) const;
    BN operator%(const BASE&) const;
    friend ostream& operator << (ostream&, const BN&);
    friend istream& operator >> (istream&, BN&);
    void cout_10();
    void cin_10();
    BN operator/(const BN&) const;
    BN operator%(const BN&) const;
};
void BN::lenNorm() {
    while (len > 1 && coef[len - 1] == 0) {
        len--;
    }
}
BN::BN(int maxLen, int parameter) : len(1), maxlen(maxLen) {
    coef = new BASE[maxLen];
    if (maxLen == 1) {
        len = 1;
    }
    for (int i = 0; i < maxlen; i++) {
        coef[i] = 0;
    }
    if (parameter != 0) {
        len = maxlen;
        for (int i = 0; i < maxlen; i++) {
            coef[i] = rand();
        }
        if (sizeof(BASE) == sizeof(unsigned int)) {
            for (int i = 0; i < maxlen; ++i) {
                coef[i] <<= 16;
                coef[i] |= rand();
            }
        }
    }
}
BN::BN(const BN& other) : len(other.len), maxlen(other.maxlen) {
    coef = new BASE[maxlen];
    int i = 0;
    while (i < maxlen) {
        coef[i] = other.coef[i];
        i++;
    }
}
BN::~BN() {
    delete[] coef;
}
BN& BN::operator=(const BN& other) {
    if (this != &other) {
        delete[] coef;
        maxlen = other.maxlen;
        len = other.len;
        coef = new BASE[maxlen];
        int i = 0;
        while (i < maxlen) {
            coef[i] = other.coef[i];
            i++;
        }
    }
    return *this;
}
BN BN::operator+(const BN& other) const {
    int maxOfLengths = max(len, other.len);
    int minOfLengths = min(len, other.len);
    int sumLength = maxOfLengths + 1;
    BN sumNumber(sumLength);
    BASE carry = 0;
    int i = 0;
    while (i < minOfLengths) {
        DBASE tempSum = (DBASE)coef[i] + (DBASE)other.coef[i] + carry;
        sumNumber.coef[i] = (BASE)tempSum;
        carry = tempSum >> BASE_SIZE;
        i++;
    }

    while (i < len) {
        DBASE tempSum = (DBASE)coef[i] + carry;
        sumNumber.coef[i] = (BASE)tempSum;
        carry = tempSum >> BASE_SIZE;
        i++;
    }

    while (i < other.len) {
        DBASE tempSum = (DBASE)other.coef[i] + carry;
        sumNumber.coef[i] = (BASE)tempSum;
        carry = tempSum >> BASE_SIZE;
        i++;
    }

    sumNumber.coef[maxOfLengths] = carry;

    sumNumber.len = sumLength;
    sumNumber.lenNorm();

    return sumNumber;
}
BN& BN::operator+=(const BN& other) {
    *this = *this + other;
    return *this;
}
BN BN::operator-(const BN& other) const {
    if (*this < other) {
        throw invalid_argument("Недействительным аргумент");
    }

    int j = 0;
    int borrow = 0;
    DBASE temp;
    BN subtractionNum(len);
    while (j < other.len) {
        temp = ((DBASE)1 << BASE_SIZE) | coef[j];
        temp = temp - (DBASE)other.coef[j] - borrow;
        subtractionNum.coef[j] = (BASE)temp;
        borrow = !(temp >> BASE_SIZE);

        j++;
    }

    while (j < len) {
        temp = ((DBASE)1 << BASE_SIZE) | coef[j];
        temp -= borrow;

        // ���������� ��������
        subtractionNum.coef[j] = (BASE)temp;

        // ���������� ����� ����������� �����
        borrow = !(temp >> BASE_SIZE);

        j++;
    }
    subtractionNum.len = len;
    subtractionNum.lenNorm();

    return subtractionNum;
}
BN& BN::operator-=(const BN& other) {
    *this = *this - other;
    return *this;
}
BN BN::operator*(const BASE& multiplier) const {
    int j = 0;
    BASE carry = 0;
    BN resNumber(len + 1);
    DBASE tmp;

    while (j < len) {
        tmp = (DBASE)coef[j] * (DBASE)multiplier + (DBASE)carry;
        resNumber.coef[j] = (BASE)tmp;
        carry = (BASE)(tmp >> BASE_SIZE);
        j++;
    }
    resNumber.coef[j] = carry;
    resNumber.len = len + 1;
    resNumber.lenNorm();
    return resNumber;
}
BN& BN::operator*=(const BASE& multiplier) {
    *this = *this * multiplier;
    return *this;
}
BN BN::operator*(const BN& other) const {
    if (other.len == 1 && other.coef[0] == 0) {
        return BN();
    }
    BN resNumber(len + other.len);
    DBASE tmp;
    int j = 0;
    while (j < other.len) {
        if (other.coef[j] != 0) {
            BASE carry = 0;
            int i = 0;
            while (i < len) {
                tmp = (DBASE)coef[i] * (DBASE)other.coef[j] + (DBASE)resNumber.coef[i + j] + (DBASE)carry;
                resNumber.coef[i + j] = (BASE)tmp;
                carry = (BASE)(tmp >> BASE_SIZE);
                i++;
            }
            resNumber.coef[len + j] = carry;
        }
        j++;
    }
    resNumber.len = len + other.len;
    resNumber.lenNorm();
    return resNumber;
}
BN& BN::operator*=(const BN& other) {
    *this = *this * other;
    return *this;
}
BN BN::operator/(const BASE& number) const { //�������������� j
    int j = 0;
    DBASE tmp = 0;
    BASE left = 0;
    BN resNumber(len);
    while (j < len) {
        tmp = ((DBASE)left << BASE_SIZE) + (DBASE)coef[len - 1 - j];
        resNumber.coef[len - 1 - j] = (BASE)(tmp / (DBASE)number);
        left = (BASE)(tmp % (DBASE)number);
        j++;
    }

    resNumber.len = len;
    resNumber.lenNorm();
    return resNumber;
}
BN BN::operator%(const BASE& number) const {
    int j = 0;
    DBASE tmp = 0;
    BASE left = 0;
    BN resNumber(1);

    while (j < len) {
        tmp = ((DBASE)left << BASE_SIZE) + (DBASE)coef[len - 1 - j];
        left = (BASE)(tmp % (DBASE)number);
        j++;
    }

    resNumber.coef[0] = left;
    //resNumber.lenNorm();

    return resNumber;
}
BN BN::operator/(const BN& divisor) const {
    if (divisor.len == 1 && divisor.coef[0] == 0) {
        throw std::invalid_argument("Деление на ноль.");
    }

    if (*this < divisor) {
        BN result(1);
        return result;
    }

    if (divisor.len == 1) {
        return *this / divisor.coef[0];
    }

    DBASE base = ((DBASE)1 << BASE_SIZE);
    DBASE d = base / (DBASE)(divisor.coef[divisor.len - 1] + (BASE)1);
    int j = len - divisor.len;

    BN dividend(*this); // делимое
    dividend *= d;
    BN divisor_copy(divisor); // делитель
    divisor_copy *= d;

    BN result(j + 1);
    result.len = j + 1;

    while (j >= 0) {
        DBASE q = (DBASE)(((DBASE)((DBASE)(dividend.coef[j + divisor_copy.len]) * (DBASE)(base)) +
                (DBASE)(dividend.coef[j + divisor_copy.len - 1])) / (DBASE)(divisor_copy.coef[divisor_copy.len - 1]));
        DBASE r = (DBASE)(((DBASE)((DBASE)(dividend.coef[j + divisor_copy.len]) * (DBASE)(base)) +
                (DBASE)(dividend.coef[j + divisor_copy.len - 1])) % (DBASE)(divisor_copy.coef[divisor_copy.len - 1]));

        if (q == base || (DBASE)((DBASE)(q) * (DBASE)(divisor_copy.coef[divisor_copy.len - 2])) >
        (DBASE)(((DBASE)(base)* (DBASE)(r)) + (DBASE)(dividend.coef[j + divisor_copy.len - 2]))) {
            q--;
            r = (DBASE)(r)+(DBASE)(divisor_copy.coef[divisor_copy.len - 1]);
            if ((DBASE)(r) < base) {
                if (q == base || (DBASE)((DBASE)(q) * (DBASE)(divisor_copy.coef[divisor_copy.len - 2])) >
                (DBASE)(((DBASE)(base) * (DBASE)(r)) + (DBASE)(dividend.coef[j + divisor_copy.len - 2]))) {
                    q--;
                }
            }
        }

        BN u(divisor_copy.len + 1);
        u.len = divisor_copy.len + 1;
        for (int i = 0; i < divisor_copy.len + 1; i++) {
            u.coef[i] = dividend.coef[j + i];
        }

        if (u < divisor_copy * (BASE)(q)) { // для коррекции частного
            q--;
        }

        u = u - divisor_copy * (BASE)(q);
        result.coef[j] = (BASE)(q);

        for (int i = 0; i < divisor_copy.len + 1; i++) {
            dividend.coef[j + i] = u.coef[i];
        }

        j--;
    }

    result.lenNorm();
    return result;
}
BN BN::operator%(const BN& divisor) const {
    if (divisor.len == 1 && divisor.coef[0] == 0) {
        throw std::invalid_argument("Деление на ноль.");
    }

    if (*this < divisor) {
        return *this;
    }

    if (divisor.len == 1) {
        return *this % divisor.coef[0];
    }

    int result_len = len - divisor.len;
    int base_size = BASE_SIZE;
    DBASE base = ((DBASE)1 << base_size);
    BASE d = (BASE)((DBASE)base / (DBASE)(divisor.coef[divisor.len - 1] + 1));
    int j = result_len;

    BN dividend(*this); // делимое
    dividend *= d;
    BN divisor_copy(divisor); // делитель
    divisor_copy *= d;

    while (j >= 0) {
        DBASE q = (DBASE)(((DBASE)((DBASE)(dividend.coef[j + divisor_copy.len]) * (DBASE)(base)) +
                (DBASE)(dividend.coef[j + divisor_copy.len - 1])) / (DBASE)(divisor_copy.coef[divisor_copy.len - 1]));
        DBASE r = (DBASE)(((DBASE)((DBASE)(dividend.coef[j + divisor_copy.len]) * (DBASE)(base)) +
                (DBASE)(dividend.coef[j + divisor_copy.len - 1])) % (DBASE)(divisor_copy.coef[divisor_copy.len - 1]));

        if (q == base || (DBASE)((DBASE)(q) * (DBASE)(divisor_copy.coef[divisor_copy.len - 2])) >
        (DBASE)(((DBASE)(base) * (DBASE)(r)) + (DBASE)(dividend.coef[j + divisor_copy.len - 2]))) {
            q--;
            r = (DBASE)(r)+(DBASE)(divisor_copy.coef[divisor_copy.len - 1]);
            if ((DBASE)(r) < base) {
                if (q == base || (DBASE)((DBASE)(q) * (DBASE)(divisor_copy.coef[divisor_copy.len - 2])) >
                (DBASE)(((DBASE)(base) * (DBASE)(r)) + (DBASE)(dividend.coef[j + divisor_copy.len - 2]))) {
                    q--;
                }
            }
        }

        BN u(divisor_copy.len + 1);
        u.len = divisor_copy.len + 1;
        for (int i = 0; i < divisor_copy.len + 1; i++) {
            u.coef[i] = dividend.coef[j + i];
        }

        if (u < divisor_copy * (BASE)(q)) {
            q--;
        }

        u = u - (divisor_copy * (BASE)(q));

        for (int i = 0; i < divisor_copy.len + 1; i++) {
            dividend.coef[j + i] = u.coef[i];
        }

        j--;
    }

    dividend.lenNorm();

    return dividend / d;
}
bool BN::operator==(const BN& other) const {
    if (len != other.len) {
        return false;
    }
    for (int i = 0; i < len; ++i) {
        if (coef[i] != other.coef[i]) {
            return false;
        }
    }
    return true;
}
bool BN::operator!=(const BN& other) const {
    if (len != other.len) {
        return true;
    }
    for (int i = 0; i < len; ++i) {
        if (coef[i] != other.coef[i]) {
            return true;
        }
    }
    return false;
}
bool BN::operator<(const BN& other) const {
    if (len < other.len) {
        return true;
    }
    if (len > other.len) {
        return false;
    }
    for (int i = len - 1; i >= 0; --i) {
        if (coef[i] < other.coef[i]) {
            return true;
        }
        if (coef[i] > other.coef[i]) {
            return false;
        }
    }
    return false;
}
bool BN::operator>(const BN& other) const {
    if (len > other.len) {
        return true;
    }

    if (len < other.len) {
        return false;
    }
    for (int i = len - 1; i >= 0; --i) {
        if (coef[i] > other.coef[i]) {
            return true;
        }
        if (coef[i] < other.coef[i]) {
            return false;
        }
    }
    return false;
}
bool BN::operator<=(const BN& other) const {
    if (len > other.len) {
        return false;
    }
    if (len < other.len) {
        return true;
    }
    for (int i = len - 1; i >= 0; --i) {
        if (coef[i] < other.coef[i]) {
            return true;
        }
        if (coef[i] > other.coef[i]) {
            return false;
        }
    }
    return true;
}
bool BN::operator>=(const BN& other) const {
    if (len < other.len) {
        return false;
    }
    if (len > other.len) {
        return true;
    }
    for (int i = len - 1; i >= 0; --i) {
        if (coef[i] > other.coef[i]) {
            return true;
        }
        if (coef[i] < other.coef[i]) {
            return false;
        }
    }
    return true;
}
istream& operator >> (istream& in, BN& bNum) {
    char* s = new char[1000];
    int j = 0;
    int k = 0;
    unsigned int tmp = 0;

    in.getline(s, 1000);

    bNum.len = (strlen(s) - 1) / (BASE_SIZE / 4) + 1;
    bNum.maxlen = bNum.len;
    bNum.coef = new BASE[bNum.maxlen];

    for (int i = 0; i < bNum.len; i++) {
        bNum.coef[i] = 0;
    }
    for (int i = strlen(s) - 1; i > -1; i--) {
        if ('0' <= s[i] && s[i] <= '9') {
            tmp = s[i] - '0';
        }
        else if ('a' <= s[i] && s[i] <= 'f') {
            tmp = s[i] - 'a' + 10;
        }
        else if ('A' <= s[i] && s[i] <= 'F') {
            tmp = s[i] - 'A' + 10;
        }
        else {
            throw invalid_argument("Invalid arguments.");
        }

        bNum.coef[j] |= tmp << (k * 4);
        k++;
        if (k >= BASE_SIZE / 4) {
            k = 0;
            j++;
        }
    }
    return in;
}
ostream& operator << (ostream& st, const BN& num)
{
    for (int j = num.len - 1; j >= 0; j--)
    {
        st.width(BASE_SIZE / 4);
        st.fill('0');
        st << hex << (int)num.coef[j];
    }
    return st;
}
void BN::cout_10() {
    BN newNum = *this;
    BN zero(newNum.len);
    string s;
    zero.len = newNum.len;
    while (newNum != 0) {
        BN t = newNum % 10;
        s.push_back(t.coef[0] + '0');
        newNum = newNum / 10;
        zero.len = newNum.len;
    }
    reverse(s.begin(), s.end());
    cout << "Base10= " << s << endl;
}
void BN::cin_10() {
    int j = 0;
    string s;
    cout << "Base10: ";
    getline(cin, s);
    int k = s.length();
    BASE t = 0;

    BN bNum((k - 1) / (BASE_SIZE / 4) + 1);

    while (j < k) {
        if ('0' > s[j] || s[j] > '9') {
            throw invalid_argument("Неверные аргументы");
        }
        t = s[j] - '0';
        bNum = bNum * ((BASE)10);

        BN newNum;
        newNum.coef[0] = (BASE)t;
        bNum += newNum;
        j++;
    }

    bNum.len = bNum.maxlen;
    bNum.lenNorm();
    *this = bNum;
}

int main() {
//    BN slave;
//    slave.cin_10();
//    slave.cout_10();
    srand(time(NULL));
    BN A(2, 1), B(2, 1);
//   cin >> B;
    cout << "A: " << A << endl;
    cout << "B: " << B << endl;

    if (A == B) {
        cout << "A == B" << endl;
    } else {
        cout << "A != B" << endl;
    }

    if (A < B) {
        cout << "A < B" << endl;
    } else {
        cout << "A /< B" << endl;
    }

    if (A <= B) {
        cout << "A <= B" << endl;
    } else if(A >= B){
        cout << "A >= B" << endl;
    }

    BN C = A + B;
    cout << "Сложение: " << C << endl;

    BN S = (A + B) - B;
    cout << "Вычитание: " << S << endl;


    if (S == A) {
        cout << "S == A" << endl;
    } else {
        cout << "S /== A" << endl;
    }

    int intNum;
    cout << "Введите целое число: ";
    cin >> intNum;

    BN numD = A * intNum;
    cout << "Умножение на целое число: " << numD << endl;

    BN numE = A * B;
    cout << "Умножение на BN: " << numE << endl;

    BN numF = A / intNum;
    cout << "Деление на целое число: " << numF << endl;

    BN numG = A % intNum;
    cout << "Деление с остатком на целое число: " << numG << endl;

    BN numK = numE / B;
    cout << "Деление на BN " << numK << endl;

    BN numN = numE % B;
    cout << "Деление с остатком на BN " << numN << endl;

    return 0;
}
 // f
