#include <iostream>
#include <vector>
#include "BigNum.h"

vector<BN> Ferma(BN n){

    BN x = n.sqrt(); // 1
    vector<BN> d(2);
    BN one, y, z, zero;
    one = 1;
    zero = 0;

    if (n % 2 != zero){
        if (x.square() == n){ // 1
            d[0] = x;
            d[1] = x;
            return d;
        }
        while (y.square() != z){ // 4
            x = x + one; // 2
            if (x == (n + one) / 2){ // 3
                cout << "n is prime\n";
                return d;
            }
            z = x.square() - n;
            y = z.sqrt();
        }
        d[0] = x + y;
        d[1] = x - y;
        return d;
    }
    else{
        cout << "n is wrong\n";
        return d;
    }
}

int main(){
    BN num;

    while (true){
        num.cin_base10();

        vector<BN> d = Ferma(num);

        num.cout_base10();
        std::cout << " = ";

        for (size_t i = 0; i < d.size(); i++){
            if (i != 0){
                std::cout << " * ";
            }
            d[i].cout_base10();
        }
        std::cout << endl << endl;
    }
}