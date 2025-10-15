#include <iostream>
#include <tuple>
#include "BigNumber.h"

BN mod_sub(const BN& a, const BN& b, const BN& n) {
    if (a >= b) {
        return (a - b) % n;
    } else {
        return (n - (b - a)) % n;
    }
}

bool in_S1(const BN& x) {
    return x % 3 == 1;
}

bool in_S2(const BN& x) {
    return x % 3 == 2;
}

bool in_S3(const BN& x) {
    return x % 3 == 0;
}

// 2
std::tuple<BN, BN, BN> F(const BN& x, const BN& u, const BN& v, const BN& g, const BN& a, const BN& p, const BN& n) {
    BN new_x, new_u, new_v;
    BN one = 1;
    BN two = 2;

    if (in_S1(x)) {
        new_x = (a * x) % p;
        new_u = u;
        new_v = (v + one) % n;
    } else if (in_S2(x)) {
        new_x = (x * x) % p;
        new_u = (two * u) % n;
        new_v = (two * v) % n;
    } else { // in_S3(x)
        new_x = (g * x) % p;
        new_u = (u + one) % n;
        new_v = v;
    }

    return { new_x, new_u, new_v };
}

BN mod_inverse(const BN& a, const BN& m) {
    BN m0 = m;
    BN y = 0, x = 1;
    BN zero = 0;
    BN one = 1;

    if (m == one)
        return zero;

    BN a_temp = a;
    BN m_temp = m;

    while (a_temp > one) {
        BN q = a_temp / m_temp;
        BN t = m_temp;

        m_temp = a_temp % m_temp;
        a_temp = t;
        t = y;

        y = x - q * y;
        x = t;
    }

    if (x < zero)
        x += m0;

    return x;
}

// Решение уравнения вида r*z ≡ diff (mod n) с использованием расширенного алгоритма Евклида
BN solve_mod_equation(const BN& r, const BN& diff, const BN& n) {
    BN zero = 0;
    BN one = 1;

    BN d = r.gcd(n);
    if (diff % d != zero) {
        throw std::invalid_argument("No solutions");
    }

    // Приводим уравнение к виду: (r/d) * z ≡ (diff/d) (mod n/d)
    BN n_d = n / d;
    BN diff_d = diff / d;
    BN r_d = r / d;

    BN r_d_inv = mod_inverse(r_d, n_d);

    BN z0 = (diff_d * r_d_inv) % n_d;

    return z0;
}

// Основной алгоритм Полларда (ρ-метод)
BN pollard_rho(const BN& g, const BN& p, const BN& n, const BN& a) {
    BN x1 = 1, x2 = 1; // 1
    BN u1 = 0, u2 = 0;
    BN v1 = 0, v2 = 0;
    BN zero = 0, one = 1;

    std::cout << "x1\tu1\tv1\tx2\tu2\tv2\n";
    std::cout << "-----------------------\n";
    std::cout << x1 << "\t" << u1 << "\t" << v1 << "\t" << x2 << "\t" << u2 << "\t" << v2 << "\n";

    int max_iterations = 20000;
    int iterations = 0;

    while (iterations < max_iterations) {
        std::tie(x1, u1, v1) = F(x1, u1, v1, g, a, p, n);

        std::tie(x2, u2, v2) = F(x2, u2, v2, g, a, p, n);
        std::tie(x2, u2, v2) = F(x2, u2, v2, g, a, p, n);

        iterations++;

        std::cout << x1 << "\t" << u1 << "\t" << v1 << "\t" << x2 << "\t" << u2 << "\t" << v2 << "\n";

        if (x1 == x2) { // 3
            std::cout << "Collision found!" << std::endl;
            BN r = mod_sub(v1, v2, n); // 4

            if (r == zero) {
                std::cout << "r is zero, cannot solve equation. Continuing..." << std::endl;
                continue;
            }
            // Решение уравнения r*z ≡ diff (mod n)
            BN diff = mod_sub(u2, u1, n); // 5 diff = u2 - u1 mod n

            try {
                BN z0 = solve_mod_equation(r, diff, n); // 1 решение
                BN d = r.gcd(n);
                BN i = zero; // 6
                std::cout << "Testing " << d << " possible solutions..." << std::endl;

                while (i < d) {
                    BN z = (z0 + i * (n / d)) % n;

                    BN verification = g.pow_mod(z, p); // // Проверка: g^z ≡ a (mod p)
                    if (verification == a) {
                        std::cout << "Found solution: " << z << std::endl;
                        return z;
                    }
                    i = i + one;
                }

                std::cout << "No valid solution found for this collision." << std::endl;
            } catch (const std::exception& e) {
                std::cout << "Error solving equation: " << e.what() << std::endl;
            }
        }
    }

    std::cout << "Max iterations reached without finding solution." << std::endl;
    throw std::runtime_error("Solution not found");
}

int main() {
    BN g = 8900;
    BN p = 10069;
    BN a = 7330;
    BN n = p - 1;

    std::cout << "Solving: " << g << "^x ≡ " << a << " mod " << p << std::endl;
    std::cout << "Order of group: " << n << std::endl << std::endl;

    try {
        BN result = pollard_rho(g, p, n, a);
        std::cout << std::endl << "Discrete logarithm found: " << result << std::endl;
        std::cout << "Verification: " << g << "^" << result << " mod " << p << " = " << g.pow_mod(result, p) << std::endl;
        std::cout << "Expected: " << a << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "An error occurred: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
