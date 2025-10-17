#ifndef _POLINOMIAL_H
#define _POLINOMIAL_H

#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

using namespace std;

class Polynomial {
private:
    vector<int> coefficients;
    int mod;

public:
    static int InverseMod(int a, int mod) { // Евклид расш обратный
        int m0 = mod, q, t;
        int x0 = 0, x1 = 1;

        if (m0 == 1) {
            return 0;
        }

        while (a > 1) {
            q = a / mod;
            t = mod;

            mod = a % mod;
            a = t;
            t = x0;

            x0 = x1 - q * x0;
            x1 = t;
        }

        if (x1 < 0) {
            x1 += m0;
        }

        return x1;
    }

    void normalize() {
        while (!coefficients.empty() && coefficients.back() == 0) {
            coefficients.pop_back();
        }
        if (coefficients.empty())
            coefficients.push_back(0);
    }

    Polynomial(const vector<int> &coeffs, int mod) : coefficients(coeffs), mod(mod) {
        normalize();
    }

    void print() const {
        bool first = true;
        for (int i = degree(); i >= 0; --i) {
            if (coefficients[i] != 0) {
                if (!first)
                    cout << " + ";
                if (i == 0) {
                    cout << coefficients[i];
                } else if (i == 1) {
                    cout << coefficients[i] << "*x";
                } else {
                    cout << coefficients[i] << "*x^" << i;
                }
                first = false;
            }
        }
        if (first)
            cout << "0";
        cout << endl;
    }

    Polynomial operator+(const Polynomial &other) const {
        vector<int> result(max(coefficients.size(), other.coefficients.size()), 0);

        for (int i = 0; i < result.size(); ++i) {
            int a = (i < coefficients.size()) ? coefficients[i] : 0;
            int b = (i < other.coefficients.size()) ? other.coefficients[i] : 0;
            result[i] = (a + b) % mod;
        }
        return Polynomial(result, mod);
    }

    Polynomial operator-(const Polynomial &other) const {
        vector<int> result(max(coefficients.size(), other.coefficients.size()), 0);

        for (int i = 0; i < result.size(); ++i) {
            int a = (i < coefficients.size()) ? coefficients[i] : 0;
            int b = (i < other.coefficients.size()) ? other.coefficients[i] : 0;
            result[i] = (a - b + mod) % mod;
        }
        return Polynomial(result, mod);
    }

    Polynomial operator*(const Polynomial &other) const {
        vector<int> result(coefficients.size() + other.coefficients.size() - 1, 0);

        for (int i = 0; i < coefficients.size(); ++i) {
            for (int j = 0; j < other.coefficients.size(); ++j) {
                result[i + j] = (result[i + j] + (coefficients[i] * other.coefficients[j]) % mod) % mod;
            }
        }
        return Polynomial(result, mod);
    }

    Polynomial operator%(const Polynomial &divisor) const {
        Polynomial dividend = *this;
        Polynomial result({0}, mod);

        if (divisor.degree() == 0) {
            if (divisor.coefficients[0] == 0) {
                throw std::invalid_argument("Division by zero polynomial");
            }
            return Polynomial({0}, mod);
        }

        while (dividend.degree() >= divisor.degree()) {
            int DegreeDiff = dividend.degree() - divisor.degree();
            vector<int> quotientTerm(DegreeDiff + 1, 0);
            quotientTerm[DegreeDiff] =
                    InverseMod(divisor.coefficients.back(), mod) * dividend.coefficients.back() % mod;

            Polynomial term(quotientTerm, mod);
            result = result + term;
            dividend = dividend - (term * divisor);
        }
        return dividend;
    }

    bool operator!=(const Polynomial &x) const {
        if (this->coefficients.size() != x.coefficients.size()) {
            return true;
        }
        for (int i = 0; i < this->coefficients.size(); i++) {
            if (this->coefficients[i] != x.coefficients[i]) {
                return true;
            }
        }
        return false;
    }

    bool operator==(const Polynomial &x) const {
        if (this->coefficients.size() != x.coefficients.size()) {
            return false;
        }
        for (int i = 0; i < this->coefficients.size(); i++) {
            if (this->coefficients[i] != x.coefficients[i]) {
                return false;
            }
        }
        return true;
    }

    int degree() const {
        return coefficients.size() - 1;
    }

    Polynomial pow(int deg) {
        Polynomial f = *this;
        Polynomial result = f;

        while (deg != 1) {
            result = result * f;
            deg--;
        }
        return result;
    }

    Polynomial gcd(const Polynomial &b) const {
        Polynomial f = *this;
        Polynomial g = b;

        if (f.degree() == 0 && f.coefficients[0] == 1)
            return f;
        if (g.degree() == 0 && g.coefficients[0] == 1)
            return g;

        while (g.degree() >= 0 && !(g.degree() == 0 && g.coefficients[0] == 0))
        {
            Polynomial remainder = f % g;
            f = g;
            g = remainder;
        }

        int leadingCoeff = f.coefficients.back();
        int inverseLeadingCoeff = InverseMod(leadingCoeff, f.mod);

        for (int &coeff: f.coefficients) {
            coeff = (coeff * inverseLeadingCoeff) % f.mod;
            if (coeff < 0)
                coeff += f.mod;
        }

        return f;
    }

    vector<int> PrimeFactorization(int n) {
        vector<int> primeNumber;
        while (n % 2 == 0) {
            primeNumber.push_back(2);
            n = n / 2;
        }

        for (int i = 3; i <= sqrt(n); i++) {
            while (n % i == 0) {
                primeNumber.push_back(i);
                n = n / i;
            }
        }

        if (n > 2) {
            primeNumber.push_back(n);
        }
        return primeNumber;
    }

    bool irreducibilitypol1();

    bool irreducibilitypol2();
};


#endif //TCHM_POLINOMIAL_H
