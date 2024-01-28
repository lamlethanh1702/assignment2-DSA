#include "main.h"

int MAXSIZE = 0;
int check = 0;

// Huffman tree node abstract base class
template <typename E>
class HuffNode
{
public:
    virtual ~HuffNode() {}     // Base destructor
    virtual int weight() = 0;  // Return frequency
    virtual bool isLeaf() = 0; // Determine type
    virtual HuffNode<E> *left() const = 0;
    virtual HuffNode<E> *right() const = 0;
    virtual void setLeft(HuffNode<E> *node) = 0;
    virtual void setRight(HuffNode<E> *node) = 0;
};
template <typename E> // Leaf node subclass
class LeafNode : public HuffNode<E>
{
private:
    E it;    // Value
    int wgt; // Weight
public:
    LeafNode(const E &val, int freq) // Constructor
    {
        it = val;
        wgt = freq;
    }
    int weight() { return wgt; }
    E val() const { return it; }
    bool isLeaf() { return true; }
    HuffNode<E> *left() const override { return nullptr; }
    HuffNode<E> *right() const override { return nullptr; }
    void setLeft(HuffNode<E> *node) override {}
    void setRight(HuffNode<E> *node) override {}
};

template <typename E> // Internal node subclass
class IntlNode : public HuffNode<E>
{
private:
    HuffNode<E> *lc; // Left child
    HuffNode<E> *rc; // Right child
    int wgt;         // Subtree weight
public:
    IntlNode(HuffNode<E> *l, HuffNode<E> *r)
    {
        wgt = l->weight() + r->weight();
        lc = l;
        rc = r;
    }
    int weight() { return wgt; }
    bool isLeaf() { return false; }
    HuffNode<E> *left() const override { return lc; }
    HuffNode<E> *right() const override { return rc; }
    void setLeft(HuffNode<E> *node) override { lc = node; }
    void setRight(HuffNode<E> *node) override { rc = node; }
};

// HuffTree is a template of two parameters: the element
// type being coded and a comparator for two such elements.

template <typename E>
int getHeight(HuffNode<E> *node)
{
    if (!node)
        return 0;
    return 1 + max(getHeight(node->left()), getHeight(node->right()));
}

template <typename E>
int getBalanceFactor(HuffNode<E> *node)
{
    if (!node)
        return 0;

    int leftHeight = 0;
    int rightHeight = 0;

    if (auto intlNode = dynamic_cast<IntlNode<E> *>(node))
    {
        leftHeight = getHeight(intlNode->left());
        rightHeight = getHeight(intlNode->right());
    }

    return leftHeight - rightHeight;
}

template <typename E>
class HuffTree
{
private:
    HuffNode<E> *Root; // Tree root

    void generateCodes(HuffNode<E> *node, unordered_map<E, string> &codes, string path, unordered_map<E, int> &char_freq)
    {
        if (node)
        {
            if (node->isLeaf())
            {
                codes[dynamic_cast<LeafNode<E> *>(node)->val()] = path;
                char_freq[dynamic_cast<LeafNode<E> *>(node)->val()] = node->weight();
            }
            else
            {
                generateCodes(dynamic_cast<IntlNode<E> *>(node)->left(), codes, path + "0", char_freq);
                generateCodes(dynamic_cast<IntlNode<E> *>(node)->right(), codes, path + "1", char_freq);
            }
        }
    }

    void printTreeInOrder(HuffNode<E> *node)
    {
        if (node)
        {
            if (auto leafNode = dynamic_cast<LeafNode<E> *>(node))
            {
                cout << leafNode->val() << ": " << leafNode->weight() << endl;
            }
            else if (auto intlNode = dynamic_cast<IntlNode<E> *>(node))
            {
                printTreeInOrder(intlNode->left());
                cout << "Weight: " << intlNode->weight() << endl;
                printTreeInOrder(intlNode->right());
            }
        }
    }

public:
    HuffTree(E &val, int freq) // Leaf constructor
    {
        Root = new LeafNode<E>(val, freq);
    }

    void setRoot(HuffNode<E> *newRoot) { Root = newRoot; }
    // Internal node constructor
    HuffTree(HuffTree<E> *l, HuffTree<E> *r)
    {
        Root = new IntlNode<E>(l->root(), r->root());
    }
    ~HuffTree() {}                                // Destructor
    HuffNode<E> *root() const { return Root; }    // Get root
    int weight() const { return Root->weight(); } // Root weight

    void printTree(HuffNode<E> *node, int indent = 0)
    {
        if (node)
        {
            if (!node->isLeaf())
                cout << "Weight: " << node->weight() << std::endl;

            for (int i = 0; i < indent; ++i)
                cout << "  ";

            if (node->isLeaf())
                cout << "Leaf: " << dynamic_cast<LeafNode<E> *>(node)->val() << " (" << node->weight() << ")" << std::endl;
            else
            {
                cout << "Internal Node:" << endl;
                printTree(dynamic_cast<IntlNode<E> *>(node)->left(), indent + 1);
                printTree(dynamic_cast<IntlNode<E> *>(node)->right(), indent + 1);
            }
        }
    }

    void getHuffmanCodes(unordered_map<E, string> &codes)
    {
        unordered_map<E, int> char_freq;
        string path = "";
        generateCodes(Root, codes, path, char_freq);
    }
};

template <typename E>
class minTreeComp
{
public:
    bool operator()(const HuffTree<E> *tree1, const HuffTree<E> *tree2) const
    {
        // Compare based on frequency
        if (tree1->weight() == tree2->weight())
        {
            // If frequencies are equal, compare based on character
            const LeafNode<E> *leaf1 = dynamic_cast<const LeafNode<E> *>(tree1->root());
            const LeafNode<E> *leaf2 = dynamic_cast<const LeafNode<E> *>(tree2->root());

            if (leaf1 && leaf2)
            {
                return leaf1->val() >= leaf2->val();
            }
        }
        return tree1->weight() > tree2->weight();
    }
};

// Build a Huffman tree from a collection of frequencies
template <typename E>
HuffTree<E> *
buildHuff(HuffTree<E> **TreeArray, int count)
{
    priority_queue<HuffTree<E> *, vector<HuffTree<E> *>, minTreeComp<E>> forest(TreeArray, TreeArray + count, minTreeComp<E>());
    HuffTree<E> *temp1, *temp2, *temp3 = nullptr;
    while (forest.size() > 1)
    {
        temp1 = forest.top();
        forest.pop();
        temp2 = forest.top();

        forest.pop();
        temp3 = new HuffTree<E>(temp1, temp2);
        forest.push(temp3);
    }
    return temp3;
}

// template <typename E>

template <typename E>
HuffNode<E> *rotateLeft(HuffNode<E> *node)
{
    if (!node || !node->right())
        return node;

    HuffNode<E> *newRoot = node->right();
    node->setRight(newRoot->right());
    HuffNode<E> *temp = newRoot->left();
    newRoot->setLeft(node);
    newRoot->left()->setRight(temp);
    return newRoot;
}

template <typename E>
HuffNode<E> *rotateRight(HuffNode<E> *node)
{
    if (!node || !node->left())
        return node;

    HuffNode<E> *newRoot = node->left();
    node->setLeft(newRoot->left());
    HuffNode<E> *temp = newRoot->left();
    newRoot->setRight(node);
    newRoot->right()->setLeft(temp);
    return newRoot;
}

template <typename E>
HuffNode<E> *balanceTree(HuffNode<E> *root)
{
    int rotation = 0;
    while (rotation < 3)
    {
        int balanceFactor = getBalanceFactor(root);
        if (balanceFactor > 1)
        {
            rotation++;
            root = rotateRight(root);
        }
        else if (balanceFactor < -1)
        {
            rotation++;
            // Right-heavy, perform LL or RL rotation
            root = rotateLeft(root);
        }
        else
            break;
    }
    return root;
}

char encryptCaesar(char character, int shift)
{
    // Mật mã Caesar
    const int alphabetSize = 26;
    if (character >= 'a' && character <= 'z')
    {
        return 'a' + (character - 'a' + shift) % alphabetSize;
    }
    return 'A' + (character - 'A' + shift) % alphabetSize;
}

HuffTree<char> *huffmanTree;
int LAPSE(const string &queueTimeName)
{
    map<char, int> name_frequency;
    for (char c : queueTimeName)
    {
        name_frequency[c]++;
    }
    int n = name_frequency.size();
    if (n < 3)
        return -1;
    vector<pair<char, int>> char_frequency(name_frequency.begin(), name_frequency.end());
    for (int i = 0; i < char_frequency.size(); i++)
    {
        char encryptedChar = encryptCaesar(char_frequency[i].first, char_frequency[i].second);
        char_frequency[i].first = encryptedChar;
    }
    sort(char_frequency.begin(), char_frequency.end());
    int pivot = 0;
    int i = 1;
    while (i < char_frequency.size())
    {
        if (char_frequency[i].first == char_frequency[pivot].first)
        {
            char_frequency[pivot].second += char_frequency[i].second;
            char_frequency.erase(char_frequency.begin() + i);
        }
        else
        {
            pivot = i;
            i++;
        }
    }

    HuffTree<char> *TreeArray[n];
    for (int i = 0; i < n; ++i)
    {
        TreeArray[i] = new HuffTree<char>(char_frequency[i].first, char_frequency[i].second);
    }

    // Build Huffman tree
    HuffTree<char> *huffmanTree = buildHuff(TreeArray, n);
    // huffmanTree->preOrderTraversal(huffmanTree->root());

    cout << endl;

    huffmanTree->setRoot(balanceTree(huffmanTree->root()));

    // Huffman code
    unordered_map<char, string> huffmanCodes;
    huffmanTree->getHuffmanCodes(huffmanCodes);

    string encoded_name = "";

    for (char c : queueTimeName)
    {
        char d = c;
        encoded_name += huffmanCodes[d + name_frequency[d]];
    }

    // cout << "Encoded_name part 1: " << encoded_name << endl;

    if (encoded_name.size() > 10)
    {
        encoded_name = encoded_name.substr(encoded_name.size() - 10, 10);
    }
    reverse(encoded_name.begin(), encoded_name.end());
    // cout << "Encoded_name: " << encoded_name << endl;

    int indexCounter = 0;
    int result = 0;
    for (int i = encoded_name.length() - 1; i >= 0; i--)
    {

        if (encoded_name[i] == '1')
        {
            result += pow(2, indexCounter);
        }
        indexCounter++;
    }
    return result;
}

class RESTAURANT_Gojo
{
    class Tree_BST;

private:
    vector<Tree_BST> areaTable;

public:
    RESTAURANT_Gojo() : areaTable(MAXSIZE + 1)
    {
        for (int i = 0; i <= MAXSIZE + 1; i++)
        {
            areaTable.push_back(Tree_BST());
        }
    }

    void insertAreaTable(int result)
    {
        int ID = result % MAXSIZE + 1;
        areaTable[ID].insert(result);
    }

    void remove_KOKUSEN()
    {
        for (int i = 1; i < MAXSIZE + 1; i++)
            areaTable[i].remove();
    }

    void print_LIMITLESS(int number)
    {
        if (number <= 0 || number > MAXSIZE)
            return; //! quá kí tự
        areaTable[number].print();
    }

private:
    class Tree_BST
    {
        class Node;

    private:
        Node *root;
        queue<int> queueTime;

    public:
        Tree_BST() : root(NULL), queueTime()
        {
        }

        int size() { return queueTime.size(); }

        Node *insert_recursive(Node *node, int result)
        {
            if (node == nullptr)
            {
                Node *new_node = new Node(result);
                queueTime.push(result);
                return new_node;
            }
            if (result < node->result)
            {
                node->left = insert_recursive(node->left, result);
            }
            else
            {
                node->right = insert_recursive(node->right, result);
            }
            return node;
        }

        void insert(int result)
        {
            root = insert_recursive(root, result);
        }
        Node *pos_min(Node *node)
        {
            while (node->left != nullptr)
                node = node->left;
            return node;
        }
        Node *delete_node(Node *node, int result)
        {
            if (node == nullptr)
                return node;
            if (result < node->result)
            {
                node->left = delete_node(node->left, result);
            }
            else if (result > node->result)
            {
                node->right = delete_node(node->right, result);
            }
            else
            {
                if (node->left == nullptr)
                {
                    Node *tmp = node->right;
                    delete node;
                    return tmp;
                }
                if (node->right == nullptr)
                {
                    Node *tmp = node->left;
                    delete node;
                    return tmp;
                }
                Node *tmp = pos_min(node->right);
                node->result = tmp->result;
                node->right = delete_node(node->right, tmp->result);
            }
            return node;
        }
        int count_node(Node *node)
        {
            return node == NULL ? 0 : 1 + count_node(node->left) + count_node(node->right);
        }
        unsigned long long count_nCr(int n, int r)
        {
            long long p = 1, k = 1;

            if (n - r < r)
                r = n - r;

            if (r != 0)
            {
                while (r)
                {
                    p *= n;
                    k *= r;

                    long long m = __gcd(p, k);

                    p /= m;
                    k /= m;

                    n--;
                    r--;
                }
            }

            else
                p = 1;
            return p;
        }

        unsigned long long count_spy_in_G(Node *node)
        {
            if (node == nullptr)
                return 1;
            unsigned long long numleft = count_node(node->left);
            unsigned long long numright = count_node(node->right);

            unsigned long long nCr = count_nCr(numleft + numright, numleft) * count_spy_in_G(node->left) * count_spy_in_G(node->right);
            return nCr % MAXSIZE;
        }
        void remove()
        {
            if (this->size() == 0)
                return;
            // dem so luong can xoa
            unsigned long long number = count_spy_in_G(root);
            if (number == 1 && this->size() == 1)
                return;
            while (number != 0 && !queueTime.empty())
            {
                int tmp = queueTime.front();
                queueTime.pop();
                root = delete_node(root, tmp);
                number--;
            }
        }
        string print_recursive(Node *node)
        {
            if (node == nullptr)
                return "NULL"; //! trường hợp dừng print
            string left = print_recursive(node->left);
            string right = print_recursive(node->right);
            if (node->left == nullptr && node->right == nullptr)
                return to_string(node->result); //! tr
            return to_string(node->result) + "(" + left + "," + right + ")";
        }
        void print()
        {
            if (this->size() == 0)
            {
                solution << "Tree: EMPTY\n";
                return;
            }
            //! trường hợp rỗng bỏ qua
            string s = print_recursive(root);
            solution << "Tree: " << s << endl;
            queue<int> tempQueue = queueTime;
            while (!tempQueue.empty())
            {
                tempQueue.pop();
            }
        }

        // void printTopCustomers(int NUM, int areaID)
    private:
        class Node
        {
        private:
            int result;
            Node *left;
            Node *right;
            friend class Tree_BST;

        public:
            Node(int result) : result(result), left(NULL), right(NULL) {}
        };

        // void printTopCustomersPreOrder(Node *node, priority_queue<int, vector<int>, greater<int>> &minHeap, int &NUM)
        // {
        // 	if (node != nullptr && NUM > 0)
        // 	{
        // 		minHeap.push(node->result);
        // 		--NUM;

        // 		// Traverse left and right in pre-order
        // 		printTopCustomersPreOrder(node->left, minHeap, NUM);
        // 		printTopCustomersPreOrder(node->right, minHeap, NUM);
        // 	}
        // }
    };
};

class RESTAURANT_Sukuna
{
    class Link_List;

private:
    vector<Link_List> areaTable;

private:
    void ReHeap_up(int index)
    {
    }

    void ReHeap_down(int index)
    {
    }

public:
    RESTAURANT_Sukuna() {}
    void insertAreaTable(int result)
    {
        int ID = result % MAXSIZE + 1;
        //* bước 1 kiểm tra khu vực ID có khách hay chưa
        bool ischeckCustomer = false;
        // TODO: kiểm tra thử khu vực ID đã có khách hay chưa dùng hàm size() của Link_List

        //* bước 2 thêm khách vào khu vực đó
        if (ischeckCustomer)
        {
            //* trường hợp chưa có bàn thì phải tạo bàn mới
            areaTable.push_back(Link_List(result, ID));
            this->ReHeap_up(areaTable.size());
        }
        else
        {
            //* trường hợp đã có khách trong bàn tìm bàn đó thôi
            for (int i = 0; i < areaTable.size(); i++)
            {
                if (areaTable[i].ID == ID)
                {
                    areaTable[i].insert(result);
                    return;
                }
            }
        }
    }

    void remove_KEITEIKEN()
    {
    }

    void print_LIMITLESS(int number)
    {
        if (number < 0 || number > MAXSIZE)
            return; //! quá kí tự
        areaTable[number].print(number);
    }

private:
    class Link_List
    {
        class Node;

    private:
        Node *head;
        int ID;
        queue<Node *> queueTime;
        friend class RESTAURANT_Sukuna;

    public:
        Link_List(int result, int ID)
        {
            this->ID = ID;
            head = new Node(result);
            queueTime.push(head);
        }
        int size() { return queueTime.size(); }

        void insert(int result)
        {
            // TODO: tự code đi các bạn: khi new nhớ push vào queueTime
            // TODO: Trường hợp phần tử cha có hai con, thì phần tử cha sẽ hoán đổi với phần tử con có giá trị lớn hơn
            // TODO: Nếu các phần tử có cùng giá trị NUM thì phần tử lớn hơn được quy ước là phần tử được thêm vào heap sớm hơn
        }

        void remove(int number)
        {
        }

        void print(int number) {}

    private:
        class Node
        {
        private:
            int result;
            Node *next;
            friend class Link_List;

        public:
            Node(int result) : result(result), next(NULL) {}
        };
    };
};

class JJK_RESTAURANT_OPERATIONS
{
private:
    // HuffTree_AVL New_customers_arrive;
    RESTAURANT_Gojo hash;
    RESTAURANT_Sukuna heap;

public:
    void LAPSE(string name)
    {

        int result = stoi(name);
        //* CODE CODE CODE BỎ QUẢ

        if (result % 2 == 1)
            hash.insertAreaTable(result);
        else
        {
        }
    }

    //* xử lí nhà hàng gojo
    void KOKUSEN() { hash.remove_KOKUSEN(); }
    void LIMITLESS(int num) { hash.print_LIMITLESS(num); }

    //* xử lí nhà hàng Sukuna
    void KEITEIKEN(int num) {}
    void CLEAVE(int num) {}

    void HAND() {}
};

void simulate(string filename)
{
    ifstream ss(filename);
    string str, maxsize, name, num;
    int line = 0;
    int num_delete = 0;
    ss >> str;
    if (str == "MAXSIZE")
    {
        ss >> maxsize;
        MAXSIZE = stoi(maxsize);
        solution << "MAXSIZE : " << MAXSIZE << "\n";
        // cout << "MAXSIZE : " << MAXSIZE << "\n";
    }
    RESTAURANT_Gojo go_jo_restaurant;
    while (ss >> str)
    {
        line++;
        if (str == "LAPSE")
        {
            ss >> name;
            solution << "LAPSE : LINE " << line << "\n";
            // cout << "LAPSE : LINE " << line << "\n";
            if (name[0] >= '0' && name[0] <= '9')
            {
                int result = stoi(name);
                if (result % 2 == 1)
                    go_jo_restaurant.insertAreaTable(result);
            }
            // // int result = LAPSE(name);
        }
        else if (str == "HAND")
        {
            solution << "HAND : LINE " << line << "\n";
            // cout << "HAND : LINE " << line << "\n";
            // printHuffmanTreeInOrder(huffmanTree);
        }
        else if (str == "KEITEIKEN")
        {
            solution << "KEITEIKEN : LINE " << line << "\n";
            // cout << "KEITEIKEN : LINE " << line << "\n";
            ss >> num;
            // int num_delete = go_jo_restaurant.count_spy_in_G();
        }
        else if (str == "KOKUSEN")
        {
            solution << "KOKUSEN : LINE " << line << "\n";
            // cout << "KOKUSEN : LINE " << line << "\n";
            go_jo_restaurant.remove_KOKUSEN();
        }
        else if (str == "CLEAVE")
        {
            solution << "CLEAVE : LINE " << line << "\n";
            // cout << "CLEAVE : LINE " << line << "\n";
            ss >> num;
            // go_jo_restaurant.printTopCustomers(stoi(num));
        }
        else if (str == "LIMITLESS")
        {
            solution << "LIMITLESS : LINE " << line << "\n";
            // cout << "LIMITNESS : LINE " << line << "\n";
            ss >> num;
            go_jo_restaurant.print_LIMITLESS(stoi(num));
        }
    }

    // cout
    // 	<< "Good Luck";
    return;
}
