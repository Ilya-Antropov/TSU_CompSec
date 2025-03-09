#include <iostream>
#include <chrono>

using namespace std;

class Array {
    int *a, n;
public:
    Array(int len = 1, int t = 1, int d = 10) {
        a = new int[len];
        n = len;
        if (t == 1) {
            for (int i = 0; i < n; i++) {
                a[i] = rand() % d;
            }
        } else if (t == 2) {
            a[0] = rand() % d;
            for (int i = 1; i < n; i++) {
                a[i] = a[i - 1] + rand() % d;
            }
        } else if (t == 3) {
            a[n - 1] = rand() % d;
            for (int i = n - 2; i >= 0; i--) {
                a[i] = a[i + 1] + rand() % d;
            }
        } else {
            exit(1);
        }
    };

    Array(int *b, int len) {
        a = new int[len];
        n = len;
        for (int i = 0; i < n; i++) {
            a[i] = b[i];
        }
    };

    Array(Array &b) {
        a = new int[b.n];
        n = b.n;
        for (int i = 0; i < n; i++) {
            a[i] = b.a[i];
        }
    };

    ~Array() {
        if (a) delete[]a;
        a = NULL;
    }

    Array &operator=(const Array &);

    bool Test();
    bool operator==(Array);

    void Shell_sort();
    void Heapsort();
    void heapify(int, int);
    void Hoar_sort();
    void Bit_sort();
    void Bit_sort(int, int, int);

    int arrLength();
    int max_bit(); // для Bit_sort
    int &operator[](int);

    friend istream &operator>>(istream &, Array &);

    friend ostream &operator<<(ostream &, Array &);
};

Array &Array::operator=(const Array &newArr) {
    if (this == &newArr)
        return *this;
    delete[] a;
    n = newArr.n;
    a = new int[n];
    for (int i = 0; i < n; i++) {
        a[i] = newArr.a[i];
    }
    return *this;
};

int &Array::operator[](int b) {
    if (b < 0 || b >= n) return a[0];
    return a[b];
}

int Array::arrLength() {
    return n;
}

bool Array::Test() {
    int test = 0;
    for (int i = 0; i < n - 1; ++i) {
        if (a[i] > a[i + 1]) {
            return 0;
        }
    }
    return 1;
}


bool Array::operator==(Array b) {
    if (n != b.n) return false;
    int len = n;
    for (int i = 0; i < n; i++) {
        bool ch = false;
        for (int j = 0; j < len; j++) {
            if (a[i] == b.a[j]) {
                ch = true;
                b[j] = b[len - 1];
                len--;
                break;
            }
        }
        if (ch == false) return false;
    }
    return true;
}

istream &operator>>(istream &in, Array &obj) {
    int len;
    cout << "len: ";
    in >> len;
    cout << "\n massiv: ";
    int *arr;
    arr = new int[len];
    for (int i = 0; i < len; i++) {
        in >> arr[i];
    }
    obj = Array(arr, len);
    delete[] arr;
    return in;
}

ostream &operator<<(ostream &out, Array &a) {
    out << " ";
    for (int i = 0; i < a.n; i++) {
        out << a[i] << " ";
    }
    out << "\n";
    return out;
}

void Array::Shell_sort() {
    for (int gap = n / 2; gap > 0; gap /= 2) {
        for (int b = 0; b < gap; b++) {
            for (int i = b + gap; i < n; i += gap) {
                int x = a[i];
                int k;
                for (k = i - gap; k >= 0 && a[k] > x; k -= gap) {
                    a[k + gap] = a[k];
                }
                a[k + gap] = x;
            }
        }
    }
}

void hoar_sort(int a[], int l, int r) {

    if (l >= r)
        return;

    int x = a[(l + r) / 2];
    int i = l - 1;
    int j = r + 1;

    while (true) {
        do {
            i++;
        } while (a[i] < x);

        do {
            j--;
        } while (a[j] > x);

        if (i >= j)
            break;

        int temp = a[i];
        a[i] = a[j];
        a[j] = temp;
    }

    hoar_sort(a, l, j);
    hoar_sort(a, j + 1, r);
}

void Array::Hoar_sort() {//сортировка Хоара
    int l = 0, r = arrLength() - 1;
    hoar_sort(a, l, r);
}

void Array::heapify(int n, int i) {
    int j = 2 * i + 1;
    int x = a[i];
    bool f = true;
    while (j < n && f) {
        if (j + 1 < n && a[j + 1] > a[j]) j++;
        if (a[j] > x) {
            a[i] = a[j];
            i = j;
            j = 2 * i + 1;
        } else f = false;
    }
    a[i] = x;
}

void Array::Heapsort() {
    for (int i = n / 2 - 1; i >= 0; i--)
        this->heapify(n, i);
    for (int i = n - 1; i > 0; i--) {
        int temp = a[0];
        a[0] = a[i];
        a[i] = temp;
        this->heapify(i, 0);
    }
}

void Array::Bit_sort() {
    Bit_sort(0,n-1,this->max_bit());
}


void Array::Bit_sort(int l, int r, int k) {/*
    if (flag) {
        flag = false;
        l = 0;
        r = n - 1;
        k = this->max_bit();
    }*/

    if (!(l >= r || k < 0)) {
        int i = l, j = r;
        int mask = 1 << k;
        while (i <= j) {
            while (i <= j && !(a[i] & mask)) i++;
            while (i <= j && a[j] & mask) j--;
            if (i < j) {
                int temp = a[i];
                a[i] = a[j];
                a[j] = temp;
                i++;
                j--;
            }
        }
        this->Bit_sort(l, j, k - 1);
        this->Bit_sort(i, r, k - 1);
    }
}


int Array::max_bit() {
    int max = a[0];
    for (int i = 0; i < n; i++) {
        if (a[i] > max) max = a[i];
    }
    int k = 0;
    while (max) {
        max >>= 1;
        k++;
    }
    return k - 1;
}

void test() {
    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::milliseconds;
    Array t(20, 1, 20);
    Array t1 = t;
    Array t2 = t;
    Array t3 = t;

    double seconds = 0;

    auto time1 = high_resolution_clock::now();
    t.Shell_sort();
    auto time2 = high_resolution_clock::now();
    duration<double, std::milli> ms_double = time2 - time1;
    cout << "\nTime sort by Shell_sort " << ms_double.count() << " ms" << endl;
    if (t.Test()) printf("    Array has been sorted\n");
    else printf("    Array hasn't been sorted\n");

    time1 = high_resolution_clock::now();
    t1.Bit_sort();
    time2 = high_resolution_clock::now();
    ms_double = time2 - time1;
    cout << "Time sort by Bit_sort " << ms_double.count() << " ms" << endl;
    if (t1.Test()) printf("    Array has been sorted\n");
    else printf("    Array hasn't been sorted\n");

    time1 = high_resolution_clock::now();
    t2.Heapsort();
    time2 = high_resolution_clock::now();
    ms_double = time2 - time1;
    cout << "Time sort by Heapsort " << ms_double.count() << " ms" << endl;
    if (t2.Test()) printf("    Array has been sorted\n");
    else printf("    Array hasn't been sorted\n");

    time1 = high_resolution_clock::now();
    t3.Hoar_sort();
    time2 = high_resolution_clock::now();
    ms_double = time2 - time1;
    std::cout << "Time sort by Hoar_sort: " << ms_double.count() << " ms" << endl;
    if (t3.Test()) printf("    Array has been sorted\n");
    else printf("    Array hasn't been sorted\n");
    
    if ((t == t1) && (t == t2) && (t == t3))
        printf("\nSame");
    else printf("\nDifferent");
}

int main() {
    srand(time(NULL));

    Array t(7, 1, 100);
    cout << "Array t: " << t << endl;
    Array t1(7, 1, 100);
    cout << "Array t1: " << t1 << endl;
    Array t2(7, 1, 100);
    cout << "Array t2: " << t2 << endl;
    Array t3(7, 1, 100);
    cout << "Array t3: " << t3 << endl;

    // Сортировка Шелла
    Array shellSorted = t; // Создаем копию исходного массива
    shellSorted.Shell_sort();
    cout << "Результат сортировки Шелла:\n" << shellSorted;

    // Сортировка Хоара
    Array hoarSorted = t1;
    hoarSorted.Hoar_sort();
    cout << "Результат сортировки Хоара:\n" << hoarSorted;

    // Пирамидальная сортировка
    Array heapSorted = t2;
    heapSorted.Heapsort();
    cout << "Результат пирамидальной сортировки:\n" << heapSorted;

    // Побитовая сортировка
    Array bitSorted = t3;
    bitSorted.Bit_sort();
    cout << "Результат побитовой сортировки:\n" << bitSorted;

    test();

    return 0;

}




