#include <iostream>
#include <cmath>
#include "BigNum.h"

BN f(BN x, BN n){
    BN result = x;
    BN one;
    one = 1;
    result = (result*result + one) % n;
    return result;
}

BN P0_Pollard_method(BN n){
    BN two, zero;
    two = 2;

    if (n % 2 == zero)
        return two;

    BN a, b, d, one;
    a = 2;
    b = 2;
    one = 1;
    d = 1;
    while (d == one){
        a = f(a, n);
        b = f(f(b, n), n);

        if (a == b){
            std::cout << "No result\n";
            return zero;
        }
        if (a > b){
            d = n.gcd(a - b);
        }
        else{
            d = n.gcd(b - a);
        }
    }
    return d;
}

BN P1_Pollard_method(BN n){
    int B = 100;
    BN a, two, one, zero;
    one = 1;
    two = 2;
    zero = 0;

    a = random_bound(two, n - two); // 2
    BN d = a.gcd(n);
    if (d > one){
        return d;
    }

    // 3
    for (int q = 2; q < B; q++){
        BN q_bn(q);
        if (!q_bn.isPrime(3)){
            continue;
        }

        // 3.1
        int e = 0;
        BN temp;
        temp = 1;
        while (temp <= n){
            temp = temp * q;
            e++;
        }
        e = e - 1;

        // 3.2
        BN exponent = q_bn.pow(e);
        a = a.pow_mod(exponent, n);
    }

    // 4
    if (a == one){
        std::cout << "Denied soft\n";
        return zero;
    } else {
        d = n.gcd(a - one);
        if (d == one) {
            std::cout << "Denied hard\n";
            return zero;
        } else {
            return d;
        }
    }
}

int main(){
    BN num;

    while (true){
        num.cin_base10();

        BN p1, p2;

         p1 = P0_Pollard_method(num);
         cout << "p0   Pollard: ";
         p1.cout_base10();
         std::cout << endl;

        p2 = P1_Pollard_method(num);
        cout << "p-1 Pollard: ";
        p2.cout_base10();
        std::cout << endl;
    }
}