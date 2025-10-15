#include <iostream>
#include "BigNum.h"
#include "map"
#include "cmath"

int Gelfond(BN g,BN p,int n, BN a){
    int h = sqrt(n)+1; // 1
    BN b = g.pow(h) % p; // 2 g^h
    map<BN, int> big_step;
    for(int u = 1; u <= h; u++){
        BN key = b.pow(u) % p;
        if(big_step[key] == 0)
            big_step[key] = u;
    }

    map<int, BN> small_step;
    for(int v = 1; v <= h; v++){
        small_step[v] = (a * g.pow(v)) % p;
        if(big_step.find(small_step[v]) != big_step.end())
            return (h * big_step[small_step[v]] - v) % n;
    }
    cout << "No result.";
    return 0;
}

bool tests() {
    vector<int> g_list = { 22, 22, 70, 70, 52, 52, 1003, 1003, 77, 77, 8900, 8900 };
    vector<int> p_list = { 599, 599, 599, 599, 1571, 1571, 1571, 1571, 10069, 10069, 10069, 10069 };
    vector<int> a_list = { 418, 183, 21, 269, 647, 190, 912, 25, 625, 7128, 9458, 7330 };
    vector<int> expected_results = { 185, 321, 221, 436, 368, 1023, 146, 86, 156, 835, 1024, 2356 };
    vector<int> n_list = { 598, 598, 598, 598, 1570, 1570, 1570, 1570, 10068, 10068, 10068, 10068 };
    bool all_tests_passed = true;

    for (size_t i = 0; i < g_list.size(); ++i) {
        BN g; BN p; BN a;
        g = g_list[i];
        p = p_list[i];
        a = a_list[i];
        int n = n_list[i];

        int result = Gelfond(g, p, n, a);
        if (result != expected_results[i]) {
            cout << "Test " << i + 1 << " failed. Expected " << expected_results[i] << ", but got " << result << "\n";
            all_tests_passed = false;
        }
        else
            cout << "Test " << i + 1 << " passed. Expected " << expected_results[i] << ", and got " << result << "\n";

    }

    return all_tests_passed;
}

int main() {
    if (tests()) {
        std::cout << "all tests completed successfully!\n";
    }
    else {
        std::cout << "tests failed.\n";
    }
    /*
    BN g, p, a;
    int n, temp;
    std::cout << "Enter g:\n";
    g.cin_base10();
    std::cout << "Enter a:\n";
    a.cin_base10();
    std::cout << "Enter p: ";
    std::cin >> temp;
    n = temp - 1;
    p = temp;
    std::cout << Gelfonds_algorithm(g, p, n, a) << "\n";
    */
    return 0;
}