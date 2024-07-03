
#include <iostream>
#include <queue>

class Node{
public:
    int key;
    Node* left;
    Node* right;

    Node(int key) {
        this->key = key;
        left = nullptr;
        right = nullptr;
    }
};

class BST {
public:
    Node* root;

    BST() {
        root = nullptr;
    }
    BST(int key) {
        root = new Node(key);
    }
    BST(Node* node) {
        root = node;
    }
    BST(const BST& bst) { // Конструктор копирования
        root = copy(bst.root);
    }
    ~BST() {
        destroy(root);
    }
    BST& operator=(const BST& bst) { // Оператор присваивания
        if (this != &bst) {
            destroy(root);
            root = copy(bst.root);
        }
        return *this;
    }
    Node* search(int key) {
        return search(root, key);
    }
    void insert(int key) {
        root = insert(root, key);
    }
    void remove(int key) {
        root = remove(root, key);
    }
    Node* findMin() {
        if (root == nullptr) {
            return nullptr;
        }
        Node* current = root;
        while (current->left != nullptr) {
            current = current->left;
        }
        return current;
    }
    Node* findMax() {
        if (root == nullptr) {
            return nullptr;
        }
        Node* current = root;
        while (current->right != nullptr) {
            current = current->right;
        }
        return current;
    }
    void printTree() const {
        std::cout << "Дерево:" << std::endl;
        printTree(root, 0);
    }
    void printLevelOrder() {
        if (root == nullptr) {
            return;
        }
        std::queue<Node*> q;
        q.push(root);

        while (!q.empty()) {
            int levelSize = q.size();
            for (int i = 0; i < levelSize; ++i) {
                Node* currentNode = q.front();
                q.pop();
                std::cout << currentNode->key << " ";

                if (currentNode->left != nullptr) {
                    q.push(currentNode->left);
                }
                if (currentNode->right != nullptr) {
                    q.push(currentNode->right);
                }
            }
        }
    }

    void inorderTraversal(Node* node) {
        if (node == nullptr) {
            return;
        }
        inorderTraversal(node->left);
        std::cout << node->key << " ";
        inorderTraversal(node->right);
    }
private:
    void destroy(Node* node) {
        if (node != nullptr) {
            destroy(node->left);
            destroy(node->right);
            delete node;
        }
    }

    Node* copy(Node* node) {
        if (node == nullptr) {
            return nullptr;
        }
        Node* newNode = new Node(node->key);
        newNode->left = copy(node->left);
        newNode->right = copy(node->right);
        return newNode;
    }

    Node* search(Node* node, int key) {
        if (node == nullptr || node->key == key) {
            return node;
        }
        if (node->key > key) {
            return search(node->left, key);
        }
        return search(node->right, key);
    }

    Node* insert(Node* node, int key) {
        if (node == nullptr) {
            return new Node(key);
        }
        if (key < node->key) {
            node->left = insert(node->left, key);
        } else if (key > node->key) {
            node->right = insert(node->right, key);
        }
        return node;
    }

    Node* remove(Node* node, int key) {
        if (node == nullptr) {
            return nullptr;
        }
        if (key < node->key) {
            node->left = remove(node->left, key);
        } else if (key > node->key) {
            node->right = remove(node->right, key);
        } else {
            if (node->left == nullptr) {
                Node* temp = node->right;
                delete node;
                return temp;
            } else if (node->right == nullptr) {
                Node* temp = node->left;
                delete node;
                return temp;
            }
            Node* temp = minValueNode(node->right);
            node->key = temp->key;
            node->right = remove(node->right, temp->key);
        }
        return node;
    }

    Node* minValueNode(Node* node) {
        Node* current = node;
        while (current->left != nullptr) {
            current = current->left;
        }
        return current;
    }

    void printTree(const Node* node, int level) const {
        if (node == nullptr) {
            return;
        }
        printTree(node->right, level + 1);
        for (int i = 0; i < level; i++) {
            std::cout << "   ";
        }
        std::cout << node->key << std::endl;
        printTree(node->left, level + 1);
    }
};

int main(){
    BST bst(10);    // Создаем новое дерево

    bst.insert(5);  // Добавляем узлы
    bst.insert(15);
    bst.insert(3);
    bst.insert(7);
    bst.insert(13);
    bst.insert(17);
    bst.insert(14);


    bst.printTree();    // Выводим дерево на экран

    std::cout << "обход: " << std::endl;    // Обходим дерево в порядке возрастания ключей
    bst.inorderTraversal(bst.root);    //обход

    std::cout << "\nОбход по уровням: " << std::endl;    //обход по уровням
    bst.printLevelOrder();
    std::cout << std::endl;

    Node* node = bst.search(17);    // Проверяем поиск узлов
    if (node != nullptr){
        std::cout << "Узел с данным ключом найден!" << std::endl;
    }
    else {
        std::cout << "Узел с данным ключом не найден :(" << std::endl;
    }
    bst.remove(10);    // Удаляем узел
    bst.printTree();
    Node* minNode = bst.findMin();    // Проверяем минимальный и максимальный узлы
    Node* maxNode = bst.findMax();
    std::cout << "Минимальный ключ: " << minNode->key << std::endl;
    std::cout << "Максимальный ключ: " << maxNode->key << std::endl;

    return 0;
}