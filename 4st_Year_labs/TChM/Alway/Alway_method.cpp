#include <iostream>
#include <vector>
#include "BigNum.h"

using namespace std;

BN Alway_method(BN n, BN d) {
    BN zero, two, one, four;
    two = 2;
    one = 1;
    four = 4;
    if(n % 2 != zero){
        n.root(3).cout_base10();
        cout << " root 3\n";
        if((d >= n.root(3)*2+one) && (d % 2 != zero)){ // d ≥ 2∛n + 1
            BN r_1 = n % d;
            BN r_2 = n % (d - two);
            BN q = ((n / (d - two)) - (n / d)) * 4;
            BN s = n.root(2); // верхняя граница поиска делителей
            while(d < s){
                d += two;
                BN r;
                if((r_1 * 2) + q < r_2){
                    r = ((r_1 * 2) + q + d) - r_2;
                    r_2 = r_1;
                    r_1 = r;
                    q += four; // 3,4
                } else {
                    r = ((r_1 * 2) + q) - r_2;
                    r_2 = r_1;
                    r_1 = r; // 3
                }
                while(r_1 >= d){
                    r_1 -= d;
                    q -= four; // 5
                }
                if(r_1 == zero) // 6
                    return d;
            }
            cout << "нету делителя\n";
            return zero;
        }
        else{
            cout << "d неправильное\n";
            return zero;
        }
    }
    else{
        cout << "n неправильное\n";
        return zero;
    }
}

int main() {

    BN num, d;
    BN one;
    one = 1;
    num.cin_base10();
    BN result;
    d = num.root(3)*2+one; // d = 2∛num + 1
    result = Alway_method(num, d);
    result.cout_base10();

}
