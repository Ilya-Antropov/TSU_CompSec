#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <list>

using namespace std;

class Uzel {
public:
    int key;
    char s;
    Uzel* left, * right;
    Uzel() { left = NULL; right = NULL; }
    Uzel(Uzel* l, Uzel* r) {
        left = l;
        right = r;
        key = l->key + r->key;
    }

    ~Uzel() {
        while (left)
            delete left;
        while (right)
            delete right;
        left = NULL;
        right = NULL;
    }
};

class Hafman {
    Uzel* root;
    map<char, int> m;
    map<char, int> ::iterator ii;

    vector<bool> code;
    map<char, vector<bool> > buf;

public:
    Hafman();
    ~Hafman();
    void buildForEncode(ifstream& f);
    void buildForDecode(ifstream& fg);
    void TreeHaf(Uzel* r);
    double encode(ifstream& f, ofstream& g);
    bool decode(ifstream& fg,ofstream& gf);
    struct Sort {
        bool operator() (const Uzel* l, const Uzel* r) {
            return l->key < r->key;
        }
    };
};

Hafman::Hafman() {
    root = NULL;
}

Hafman::~Hafman() {
    delete root;
    root = NULL;
}

void Hafman::buildForEncode(ifstream& f) {
    if (root != NULL) {
        delete root;
        root = NULL;
    }
    while (!f.eof()) {
        char c = f.get();
        m[c]++;
    }
    list<Uzel*> L;
    for (ii = m.begin(); ii != m.end(); ii++) {
        Uzel* p = new Uzel;
        p->s = ii->first;
        p->key = ii->second;
        L.push_back(p);
    }
    while (L.size() != 1) {
        L.sort(Sort());
        Uzel* Left = L.front();
        L.pop_front();
        Uzel* Right = L.front();
        L.pop_front();
        Uzel* pr = new Uzel(Left, Right);
        L.push_back(pr);
    }
    root = L.front();
}

void Hafman::buildForDecode(ifstream& f) {
    if (root != NULL){
        delete root;
        root = NULL;
    }
    if (m.size() != 0)
        m.clear();
    if (code.size() != 0)
        code.clear();
    if (buf.size() != 0)
        buf.clear();

    int x1,x2;
    char s;
    f.read((char*)&x1, sizeof (x1));
    while (x1 > 0) {
        f.read((char*)&s, sizeof(s));
        f.read((char*)&x2, sizeof(x2));
        x1 -= 40;
        m[s] = x2;
    }
    list<Uzel*> L;
    for (ii = m.begin(); ii != m.end(); ii++) {
        Uzel* p = new Uzel;
        p->s = ii->first;
        p->key = ii->second;
        L.push_back(p);
    }
    while (L.size() != 1) {
        L.sort(Sort());
        Uzel* Left = L.front();
        L.pop_front();
        Uzel* Right = L.front();
        L.pop_front();
        Uzel* pr = new Uzel(Left, Right);
        L.push_back(pr);
    }
    root = L.front();
}

double Hafman::encode(ifstream& f, ofstream& g) {
    buildForEncode(f);
    TreeHaf(root);
    int count = 0;
    for (ii = m.begin(); ii != m.end(); ii++) {
        if (ii->second != 0) count += 40;
    }
    g.write((char*)(&count), sizeof(count));
    for (int i = 0; i < 256; i++) {
        if (m[char(i)] > 0) {
            char c = char(i);
            g.write((char*)(&c), sizeof(c));
            g.write((char*)(&m[char(i)]), sizeof(m[char(i)]));
        }
    }
    f.clear();
    f.seekg(0);
    count = 0;

    char tx = 0;
    while (!f.eof()) {
        char c = f.get();
        vector<bool> x = buf[c];
        for (int j = 0; j < x.size(); j++) {
            tx = tx | x[j] << (7 - count);
            count++;
            if (count == 8)
            {
                count = 0;
                g << tx;
                tx = 0;
            }
        }
    }
    f.clear();
    f.seekg(0, std::ios::end);
    g.seekp(0, std::ios::end);
    double sizeF = f.tellg();
    double sizeG = g.tellp();
    f.close();
    g.close();
    return sizeG / sizeF;
}

bool Hafman::decode(ifstream& fg, ofstream& gf){
    if (fg.is_open()) {
        buildForDecode(fg);
    }
    else
        return false;
    TreeHaf(root);

    char byte;
    int count = 0;
    Uzel* p = root;
    byte = fg.get();
    while (!fg.eof()) {
        bool b = byte & (1 << (7 - count));
        if (b)
            p = p->right;
        else
            p = p->left;
        if (p->right == NULL && p->left == NULL) {
            gf << p->s;
            p = root;
        }
        count++;
        if (count == 8) {
            count = 0;
            byte = fg.get();
        }
    }
    return true;
}

void Hafman::TreeHaf(Uzel* r) {
    if (r->left != NULL) {
        code.push_back(0);
        TreeHaf(r->left);
    }
    if (r->right != NULL) {
        code.push_back(1);
        TreeHaf(r->right);
    }
    if (r->right == NULL && r->left == NULL) {
        buf[r->s] = code;
    }
    if (!code.empty())
        code.pop_back();
}

int main() {
    ifstream f("text.txt", ios::in | ios::binary);
    ofstream g("code.txt", ios::out | ios::binary);
    Hafman haf;
    double coef = haf.encode(f, g);
    std::cout << coef << std::endl;
    ifstream fg("code.txt", ios::out | ios::binary);
    ofstream gf("output.txt", ios::out | ios::binary);
    Hafman decodeHaf;
    if (decodeHaf.decode(fg,gf))
        std::cout << "decode" << std::endl;
    else
        std::cout << "didnt decode" << std::endl;
    fg.close();
    gf.close();
    return 0;
}