#include "polinomial.h"
#include <iostream>
#include <cmath>
#include <vector>

using namespace std;

bool Polynomial::irreducibilitypol1() {
    Polynomial f = *this;
    Polynomial one({1}, f.mod);
    Polynomial x({0, 1}, f.mod);
    int deg = f.degree();

    Polynomial u = x; // 1

    for (int i = 0; i < int(deg / 2); i++) { // 2
        u = (u.pow(f.mod)) % f;

        Polynomial tmp = u - x;
        Polynomial d = f.gcd(tmp);

        if (d != one)
            return true;
    }
    return false;
}

bool Polynomial::irreducibilitypol2() {
    Polynomial f = *this;
    Polynomial one({1}, f.mod);
    Polynomial x({0, 1}, f.mod);

    // 1.
    int deg = std::pow(f.mod, f.degree()); // p^n
    Polynomial u = x.pow(deg) % f;

    if (u != x)
        return true;

    vector<int> factorization = PrimeFactorization(f.degree()); // 2

    for (int k = 0; k < factorization.size(); k++) {
        int deg_x = std::pow(f.mod, (f.degree() / factorization[k]));
        u = x.pow(deg_x) % f;

        Polynomial tmp = u - x;
        Polynomial d = f.gcd(tmp);

        if (d != one)
            return true;
    }
    return false;
}

void run_test1(int selection) {
    vector<vector<int>> irreducibilitypol = {
            {1, 0, 0, 0, 1, 1, 1, 0, 1},
            {1, 0, 0, 1, 0, 1, 0, 1, 1},
            {1, 0, 0, 1, 0, 1, 1, 0, 1},
            {1, 0, 0, 0, 1, 1, 0, 1, 1},
            {1, 0, 0, 1, 1, 1, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1},
            {1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1},
            {1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1},
            {1, 0, 0, 0, 0, 1, 1, 0, 1, 0, 1}
    };

    vector<vector<int>> reduciblepol = {
            {1, 0, 0, 1, 1, 1, 0, 1, 1},
            {1, 0, 0, 0, 1, 1, 1, 1, 1},
            {1, 0, 0, 0, 1, 0, 0, 1, 1},
            {1, 0, 0, 1, 1, 1, 1, 0, 1},
            {1, 0, 0, 1, 0, 0, 0, 1, 1},
            {1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1},
            {1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1},
            {1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1},
            {1, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1},
            {1, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1}
    };

    int p = 2;

    cout << "Test irreducibilitypol z = 2:\n ";
    for (const auto &poly: irreducibilitypol) {
        Polynomial test_poly(poly, p);
        if (selection == 1) {
            if (test_poly.irreducibilitypol1()) {
                cout << "We reduce the polynomial (error)\n";
            } else {
                cout << "Polynomial irreducibe\n";
            }
        } else if (selection == 2) {
            if (test_poly.irreducibilitypol2()) {
                cout << "We reduce the polynomial (error)\n";
            } else {
                cout << "Polynomial irreducibe\n";
            }
        }
    }

    cout << "Testing reducible polynomials z = 2:\n ";
    for (const auto &poly: reduciblepol) {
        Polynomial test_poly(poly, p);
        if (selection == 1) {
            if (test_poly.irreducibilitypol1()) {
                cout << "We reduce the polynomial\n";
            } else {
                cout << "Polynomial irreducibe (error)\n";
            }
        } else if (selection == 2) {
            if (test_poly.irreducibilitypol2()) {
                cout << "We reduce the polynomial\n";
            } else {
                cout << "Polynomial irreducibe (error)\n";
            }
        }
    }
}

void run_test2(int selection) {
    vector<vector<int>> irreducibilitypol = {
            {1, 0, 0, 0, 2, 1},
            {1, 0, 0, 2, 1, 1},
            {1, 0, 0, 0, 2, 2},
            {1, 0, 0, 1, 1, 2},
            {1, 0, 2, 1, 2, 2},
            {1, 0, 0, 0, 0, 1, 2},
            {1, 0, 0, 0, 0, 2, 2},
            {1, 0, 0, 0, 1, 1, 1},
            {1, 0, 0, 0, 1, 2, 1},
            {1, 0, 0, 0, 2, 0, 1}
    };

    vector<vector<int>> reduciblepol = {
            {1, 0, 0, 0, 1, 2},
            {1, 0, 0, 2, 1, 2},
            {1, 0, 0, 2, 2, 1},
            {1, 0, 1, 2, 0, 2},
            {1, 0, 2, 1, 0, 2},
            {1, 0, 0, 0, 0, 1, 1},
            {1, 0, 0, 0, 1, 2, 2},
            {1, 0, 0, 0, 1, 1, 2},
            {1, 0, 0, 0, 2, 1, 1},
            {1, 0, 0, 0, 2, 1, 2}
    };

    int p = 3;

    cout << "Test irreducibilitypol z = 3:\n ";
    for (const auto &poly: irreducibilitypol) {
        Polynomial test_poly(poly, p);
        if (selection == 1) {
            if (test_poly.irreducibilitypol1()) {
                cout << "We reduce the polynomial (error)\n";
            } else {
                cout << "Polynomial irreducibe\n";
            }
        } else if (selection == 2) {
            if (test_poly.irreducibilitypol2()) {
                cout << "We reduce the polynomial (error)\n";
            } else {
                cout << "Polynomial irreducibe\n";
            }
        }
    }

    cout << "Testing reducible polynomials z = 3:\n ";
    for (const auto &poly: reduciblepol) {
        Polynomial test_poly(poly, p);
        if (selection == 1) {
            if (test_poly.irreducibilitypol1()) {
                cout << "We reduce the polynomial\n";
            } else {
                cout << "Polynomial irreducibe (error)\n";
            }
        } else if (selection == 2) {
            if (test_poly.irreducibilitypol2()) {
                cout << "We reduce the polynomial\n";
            } else {
                cout << "Polynomial irreducibe (error)\n";
            }
        }
    }
}

void run_test3(int selection) {
    vector<vector<int>> irreducibilitypol = {
            {1, 0, 1, 2, 2},
            {1, 0, 1, 3, 2},
            {1, 0, 0, 0, 2},
            {1, 0, 0, 1, 4},
            {1, 0, 1, 0, 2},
            {1, 0, 0, 0, 4, 2},
            {1, 0, 0, 0, 4, 3},
            {1, 0, 0, 0, 4, 1},
            {1, 0, 0, 0, 4, 4},
            {1, 0, 0, 1, 2, 4}
    };

    vector<vector<int>> reduciblepol = {
            {1, 0, 0, 0, 4},
            {1, 0, 0, 1, 2},
            {1, 0, 0, 1, 3},
            {1, 0, 1, 1, 2},
            {1, 0, 1, 2, 4},
            {1, 0, 0, 0, 3, 1},
            {1, 0, 0, 0, 3, 4},
            {1, 0, 0, 0, 2, 1},
            {1, 0, 0, 1, 1, 2},
            {1, 0, 0, 1, 2, 3}
    };

    int p = 5;

    cout << "Test irreducibilitypol z = 5:\n ";
    for (const auto &poly: irreducibilitypol) {
        Polynomial test_poly(poly, p);
        if (selection == 1) {
            if (test_poly.irreducibilitypol1()) {
                cout << "We reduce the polynomial (error)\n";
            } else {
                cout << "Polynomial irreducibe\n";
            }
        } else if (selection == 2) {
            if (test_poly.irreducibilitypol2()) {
                cout << "We reduce the polynomial (error)\n";
            } else {
                cout << "Polynomial irreducibe\n";
            }
        }
    }

    cout << "Testing reducible polynomials z = 5:\n ";
    for (const auto &poly: reduciblepol) {
        Polynomial test_poly(poly, p);
        if (selection == 1) {
            if (test_poly.irreducibilitypol1()) {
                cout << "We reduce the polynomial\n";
            } else {
                cout << "Polynomial irreducibe (error)\n";
            }
        } else if (selection == 2) {
            if (test_poly.irreducibilitypol2()) {
                cout << "We reduce the polynomial\n";
            } else {
                cout << "Polynomial irreducibe (error)\n";
            }
        }
    }
}

int main() {
    setlocale(LC_ALL, "Russian");

    short int selection = 0;
    cout << "Введите число\n(1 - тест 1 на неприводимость полиномов\n2 - тест 2 на неприводимость полиномов): ";
    cin >> selection;

    if (selection > 0 && selection <= 2) {
        run_test1(selection);
        cout << endl;
        run_test2(selection);
        cout << endl;
        run_test3(selection);
        cout << endl;

    }
}