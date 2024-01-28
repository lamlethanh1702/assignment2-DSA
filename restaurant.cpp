#include "main.h"

int MAXSIZE = 0;
int check = 0;

bool compare(const pair<char, int> &a, const pair<char, int> &b)
{
	if (a.second != b.second)
	{
		return a.second > b.second;
	}
	else if (islower(a.first) && isupper(b.first))
	{
		return false;
	}
	else if (isupper(a.first) && islower(b.first))
	{
		return true;
	}
	else
	{
		return a.first > b.first;
	}
}

class huffman_tree
{
	class Node;

private:
	Node *root = nullptr;

public:
	~huffman_tree() { clear(root); }
	void clear(Node *node)
	{
		if (node)
		{
			clear(node->left);
			clear(node->right);
			delete node;
		}
	}

	vector<pair<char, int>> process_name_to_caesar(string &name_input)
	{
		//*bước 1
		unordered_map<char, int> char_freq;
		vector<char> orderOfInsertion;

		for (char ch : name_input)
		{
			if (char_freq.find(ch) == char_freq.end())
			{
				orderOfInsertion.push_back(ch);
			}
			char_freq[ch]++;
		}

		vector<pair<char, int>> list_freq;

		for (char ch : orderOfInsertion)
		{
			list_freq.emplace_back(ch, char_freq[ch]);
		}

		if (list_freq.size() < 3)
		{
			return {};
		}

		//*bước 2, 3, 4
		for (pair<char, int> &iter : list_freq)
		{
			if (islower(iter.first))
			{
				iter.first = ((iter.first - 'a' + iter.second) % 26 + 'a');
			}
			else
			{
				iter.first = ((iter.first - 'A' + iter.second) % 26 + 'A');
			}
		}

		string name_caesar = "";

		for (char ch : name_input)
		{
			if (islower(ch))
			{
				char r = (ch - 'a' + char_freq[ch]) % 26 + 'a';
				name_caesar += r;
			}
			else
			{
				char r = (ch - 'A' + char_freq[ch]) % 26 + 'A';
				name_caesar += r;
			}
		}

		name_input = name_caesar;

		unordered_map<char, int> caesar_freq;

		for (pair<char, int> iter : list_freq)
		{
			caesar_freq[iter.first] += iter.second;
		}

		vector<pair<char, int>> combinedlist_freq;

		for (const auto iter : caesar_freq)
		{
			combinedlist_freq.emplace_back(iter.first, iter.second);
		}

		sort(combinedlist_freq.begin(), combinedlist_freq.end(), compare);

		return combinedlist_freq;
	}

	int getHeight(Node *node)
	{
		if (node == nullptr)
			return 0;
		return 1 + max(getHeight(node->left), getHeight(node->right));
	}
	int balance_factor(Node *node)
	{
		if (node == nullptr)
			return 0;
		return getHeight(node->left) - getHeight(node->right);
	}
	Node *leftRotate(Node *node)
	{
		if (!node || !node->right)
			return node;

		Node *temp1 = node->right;
		Node *temp2 = temp1->left;

		temp1->left = node;
		node->right = temp2;

		return temp1;
	}

	Node *rightRotate(Node *node)
	{
		if (!node || !node->left)
			return node;
		Node *temp1 = node->left;
		Node *temp2 = temp1->right;

		temp1->right = node;
		node->left = temp2;

		return temp1;
	}

	Node *balanceNode(Node *node, int &count)
	{
		if (node == nullptr)
			return node;

		int balance = balance_factor(node);

		// Left of Left
		if (balance > 1 && balance_factor(node->left) >= 0)
		{
			count += 1;
			return rightRotate(node);
		}

		// Right of Right
		if (balance < -1 && balance_factor(node->right) <= 0)
		{
			count += 1;
			return leftRotate(node);
		}

		// Left of Right
		if (balance > 1 && balance_factor(node->left) < 0)
		{
			node->left = leftRotate(node->left);
			count += 1;

			return rightRotate(node);
		}

		// Right of Left
		if (balance < -1 && balance_factor(node->right) > 0)
		{
			node->right = rightRotate(node->right);
			count += 1;

			return leftRotate(node);
		}

		return node;
	}

	Node *balanceTree(Node *node, int &count)
	{
		if (node == nullptr || count == 3)
			return node;

		node = balanceNode(node, count);

		node->left = balanceTree(node->left, count);
		node->right = balanceTree(node->right, count);

		return node;
	}
	Node *buildHuff(vector<pair<char, int>> freq)
	{

		vector<Node *> build;

		// Build the initial vector of leaf nodes
		for (const auto &entry : freq)
		{
			build.push_back(new Node(entry.second, entry.first));
		}

		while (build.size() > 1)
		{
			Node *temp1 = build.back();
			build.pop_back();
			Node *smallest2 = build.back();
			build.pop_back();

			Node *newNode = new Node(temp1->weight + smallest2->weight, '\0', temp1, smallest2);

			int count = 0;
			newNode = balanceTree(newNode, count);
			newNode = balanceTree(newNode, count);
			newNode = balanceTree(newNode, count);

			vector<Node *>::iterator it = std::lower_bound(build.begin(), build.end(), newNode, [](Node *a, Node *b)
														   {
        if (a->weight != b->weight) {
            return a->weight > b->weight;
        }
        else if (a->val_char == '\0' && b->val_char != '\0') {
            return true; 
        } else if (a->val_char != '\0' && b->val_char == '\0') {
            return false; 
        } else {
            return false; 
        } });

			build.insert(it, newNode);
		}

		return build[0];
	}

	void encodingHuffman_rec(vector<string> &encoding, Node *node, string s = "")
	{
		if (node == nullptr)
			return;

		if (node->isChar())
		{
			encoding[node->val_char] = s;
			return;
		}

		if (node->left)
			encodingHuffman_rec(encoding, node->left, s + "0");

		if (node->right)
			encodingHuffman_rec(encoding, node->right, s + "1");
	}
	int BIN_TO_DEC(const string &binary)
	{
		int result = 0;
		int power = 1; // 2^0

		for (int i = binary.size() - 1; i >= 0; --i)
		{
			if (binary[i] == '1')
				result += power;

			power <<= 1; // Tăng giá trị của power lên 2 (tương đương với power *= 2)
		}

		return result;
	}
	//* đầu vào là 1 cây và name đã được mã hóa Caesar -> đầu ra là result kết quả cần tìm.
	int encodingHuffman(Node *root, string nameCaesar)
	{
		if (root->left == nullptr && root->right == nullptr)
			return 0;

		// Step 1: Get encoding for each character
		vector<string> encoding(256, "");
		encodingHuffman_rec(encoding, root, "");

		// Step 2: Get the last 10 binary characters from nameCaesar
		string binary = "";

		for (auto it = nameCaesar.begin(); it != nameCaesar.end(); ++it)
		{
			binary += encoding[*it];
		}
		int len = binary.size();
		binary = (len >= 10) ? binary.substr(len - 10) : binary;
		reverse(binary.begin(), binary.end());
		// Step 3: Convert binary to decimal
		int result = BIN_TO_DEC(binary);
		;

		return result;
	}

	int encode(string name)
	{

		//* bước 1 xử lí chuỗi thu được list để tới phần sau
		vector<pair<char, int>> freq = this->process_name_to_caesar(name);
		if (freq.size() == 0)
			return -1;

		this->clear(root);
		root = this->buildHuff(freq);

		if (root->left == nullptr && root->right == nullptr)
		{

			return 0;
		}
		int result = this->encodingHuffman(root, name);

		return result;
	}

	void print_node_weight(Node *node)
	{
		if (node == nullptr)
			return;
		print_node_weight(node->left);
		if (node->val_char == '\0')
			solution << node->weight << "\n";
		else
			solution << node->val_char << "\n";
		print_node_weight(node->right);
	}
	void hand_print() { print_node_weight(root); }

private:
	class Node
	{
	public:
		int weight;
		char val_char;
		Node *left;
		Node *right;
		friend class huffman_tree;

	public:
		Node(int weight, char val_char = '\0', Node *left = nullptr, Node *right = nullptr) : weight(weight), val_char(val_char), left(left), right(right) {}
		bool isChar() const { return val_char != '\0'; }
	};
};

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

class Gojo_res
{
	class Tree_BST;

private:
	vector<Tree_BST> table_area;

public:
	Gojo_res() : table_area(MAXSIZE + 1)
	{
		for (int i = 0; i <= MAXSIZE + 1; i++)
		{
			table_area.push_back(Tree_BST());
		}
	}

	void add_cus_area_table(int result)
	{
		int ID = result % MAXSIZE + 1;
		table_area[ID].insert(result);
	}

	void remove_KOKUSEN()
	{

		//---------------------------------------------------------------------
		for (int i = 1; i < MAXSIZE + 1; i++)
			table_area[i].remove();
		//---------------------------------------------------------------------
	}

	void limitless_print(int number)
	{
		if (number <= 0 || number > MAXSIZE)
			return; //! quá kí tự
		table_area[number].print();
	}

private:
	class Tree_BST
	{
		class Node;

	private:
		Node *root;
		queue<int> time_customer;

	public:
		Tree_BST() : root(NULL), time_customer()
		{
		}
		~Tree_BST()
		{
			while (!time_customer.empty())
			{
				int temp = time_customer.front();
				time_customer.pop();
				root = delete_node(root, temp);
			}
		}
		int size() { return time_customer.size(); }

		Node *insert_recursive(Node *node, int result)
		{
			if (node == nullptr)
			{
				Node *new_node = new Node(result);
				time_customer.push(result);
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
			if (r > n)
				return 0;

			vector<vector<unsigned long long>> dp;

			dp.resize(n + 1);

			for (int i = 0; i <= n; i++)
			{
				dp[i].resize(n + 1);
			}

			for (int i = 0; i <= n; i++)
			{
				for (int j = 0; j <= i; j++)
				{
					if (j == 0 || j == i)
						dp[i][j] = 1;
					else
						dp[i][j] = dp[i - 1][j - 1] % MAXSIZE + dp[i - 1][j] % MAXSIZE;
				}
			}

			return dp[n][r] % MAXSIZE;
		}

		unsigned long long count_spy_in_G(Node *node)
		{
			if (node == nullptr)
				return 1;
			unsigned long long numleft = count_node(node->left);
			unsigned long long numright = count_node(node->right);

			unsigned long long nCr = count_nCr(numleft + numright, numleft) % MAXSIZE * count_spy_in_G(node->left) % MAXSIZE * count_spy_in_G(node->right) % MAXSIZE;
			return nCr % MAXSIZE;
		}
		void remove()
		{
			if (this->size() == 0)
				return;
			// dem so luong can xoa
			unsigned long long number = count_spy_in_G(root);

			// if (number == 1 && this->size() == 1)
			// 	return;
			while (number != 0 && !time_customer.empty())
			{
				int tmp = time_customer.front();
				time_customer.pop();
				root = delete_node(root, tmp);
				number--;
			}
		}

		void print_node_weight(Node *node)
		{
			if (node == nullptr)
				return;
			print_node_weight(node->left);
			solution << node->result << endl;
			print_node_weight(node->right);
		}
		void print() { print_node_weight(root); }

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

	public:
	};
};

class Su_ku_na_res
{
	class Node;

private:
	vector<Node *> table_area;
	list<Node *> add_recently;

private:
	void Down_heap(int index)
	{
		int child_min = -1;
		int child_left = 2 * index + 1;
		int child_right = 2 * index + 2;

		if (child_left < (int)table_area.size() && child_right >= (int)table_area.size())
		{
			child_min = child_left;
		}
		if (child_right < (int)table_area.size() && table_area[child_left]->size() < table_area[child_right]->size())
		{
			child_min = child_left;
		}

		if (child_right < (int)table_area.size() && table_area[child_right]->size() < table_area[child_left]->size())
		{
			child_min = child_right;
		}
		if (child_left < (int)table_area.size() && child_right < (int)table_area.size() && table_area[child_right]->size() == table_area[child_left]->size())
		{
			list<Node *>::iterator it3 = find(add_recently.begin(), add_recently.end(), table_area[child_left]);
			list<Node *>::iterator it4 = find(add_recently.begin(), add_recently.end(), table_area[child_right]);
			int distance3 = distance(add_recently.begin(), it3);
			int distance4 = distance(add_recently.begin(), it4);
			if (distance3 > distance4)
			{
				child_min = child_left;
			}
			else
			{
				child_min = child_right;
			}
		}
		if (child_min != -1)
		{
			if (table_area[index]->size() > table_area[child_min]->size())
			{
				swap(table_area[index], table_area[child_min]);
				Down_heap(child_min);
			}
			else if (table_area[index]->size() == table_area[child_min]->size())
			{
				// Sizes are equal, handle based on add_recently distance
				list<Node *>::iterator it1 = find(add_recently.begin(), add_recently.end(), table_area[index]);
				list<Node *>::iterator it2 = find(add_recently.begin(), add_recently.end(), table_area[child_min]);

				if (it1 != add_recently.end() && it2 != add_recently.end())
				{
					int distance1 = distance(add_recently.begin(), it1);
					int distance2 = distance(add_recently.begin(), it2);

					if (distance1 < distance2)
					{
						swap(table_area[index], table_area[child_min]);
						Down_heap(child_min);
					}
					else
					{
						return;
					}
				}
			}
			else
				return;
		}
	}

	void Up_heap(int index)
	{
		int parent = (index - 1) / 2;

		if (index == 0 || table_area[parent]->size() < table_area[index]->size())
			return;
		else if (table_area[index]->size() == table_area[parent]->size())
		{
			list<Node *>::iterator it2 = find(add_recently.begin(), add_recently.end(), table_area[parent]);
			list<Node *>::iterator it1 = find(add_recently.begin(), add_recently.end(), table_area[index]);

			// Thêm một số điều kiện để tránh sự nhầm lẫn
			if (it1 != add_recently.end() && it2 != add_recently.end())
			{
				int distance2 = distance(it2, add_recently.end());
				int distance1 = distance(it1, add_recently.end());

				if (distance1 < distance2)
				{
					swap(table_area[index], table_area[parent]);
					Up_heap(parent);
				}
				else if (distance1 > distance2)
				{
					return;
				}
			}
		}

		swap(table_area[index], table_area[parent]);
		Up_heap(parent);
	}

	void moveTop(Node *node)
	{
		list<Node *>::iterator it = find(add_recently.begin(), add_recently.end(), node);

		if (it != add_recently.end())
		{
			add_recently.erase(it);
		}

		add_recently.push_front(node);
	}

	void removeNode(Node *node)
	{
		auto it = std::find(add_recently.begin(), add_recently.end(), node);

		if (it != add_recently.end())
		{
			add_recently.erase(it);
		}
	}

public:
	Su_ku_na_res() {}
	~Su_ku_na_res()
	{
		for (int i = 0; i < (int)table_area.size(); i++)
		{
			delete table_area[i];
		}
	}

	void add_cus_area_table(int result)
	{
		int ID = result % MAXSIZE + 1;

		int index = -1;

		for (int i = 0; i < (int)table_area.size(); i++)
		{
			if (table_area[i]->ID == ID)
			{
				index = i;
				break;
			}
		}
		if (index == -1)
		{
			table_area.push_back(new Node(ID));
			index = (int)table_area.size() - 1;
			table_area[index]->insert(result);
			this->moveTop(table_area[index]);
			this->Up_heap(index);
		}

		else
		{
			table_area[index]->insert(result);
			this->moveTop(table_area[index]);
			this->Down_heap(index);
		}
	}

	void remove_KEITEIKEN(int number)
	{
		if ((int)table_area.size() <= 0)
			return;
		vector<Node *> area_new_table(table_area.begin(), table_area.end());
		queue<Node *> listDelete;
		for (int i = 0; (int)table_area.size() && i < number; i++)
		{

			Node *nodeDelete = table_area[0];
			swap(table_area[0], table_area[(int)table_area.size() - 1]);
			table_area.pop_back();
			this->Down_heap(0);
			listDelete.push(nodeDelete);
		}

		table_area = area_new_table;

		while (listDelete.size())
		{

			Node *nodeDelete = listDelete.front();
			listDelete.pop();

			nodeDelete->remove(number);

			int index = 0;
			while (table_area[index] != nodeDelete)
				index++;

			if (nodeDelete->size() == 0)
			{
				swap(table_area[index], table_area[(int)table_area.size() - 1]);

				this->removeNode(table_area[(int)table_area.size() - 1]);
				delete table_area[(int)table_area.size() - 1];

				//! xóa trong heap nữa
				table_area.pop_back();
			}
			this->Down_heap(index);
		}
	}

	void pre_ordPrint(int index, int number)
	{
		if (index >= (int)this->table_area.size() || number <= 0)
			return;
		this->table_area[index]->print(number);
		pre_ordPrint(index * 2 + 1, number);
		pre_ordPrint(index * 2 + 2, number);
	}
	void CLEAVE(int number) { pre_ordPrint(0, number); }

private:
	class Node
	{
	private:
		int ID;
		list<int> head;
		friend class Su_ku_na_res;

	public:
		Node(int ID) : ID(ID) {}
		int size() const { return head.size(); }
		void insert(int result) { head.push_front(result); }
		void remove(int number)
		{
			while (number != 0 && !head.empty())
			{
				solution << head.back() << "-" << ID << "\n";
				head.pop_back();
				number--;
			}
		}
		void print(int number)
		{
			for (list<int>::iterator it = head.begin(); number > 0 && it != head.end(); ++it, --number)
			{
				solution << ID << "-" << *it << "\n";
			}
		}
	};
};

void simulate(string filename)
{
	ifstream ss(filename);
	string str, maxsize, name, num;
	int line = 0;

	ss >> str;
	if (str == "MAXSIZE")
	{
		line++;
		ss >> maxsize;
		MAXSIZE = stoi(maxsize);
	}
	Gojo_res go_jo_restaurant;
	Su_ku_na_res su_ku_na_restaurant;
	huffman_tree huffmanTree;
	while (ss >> str)
	{
		line++;
		if (str == "LAPSE")
		{
			ss >> name;

			if (name[0] >= '0' && name[0] <= '9')
			{
				int result = stoi(name);
				if (result % 2 == 1)
					go_jo_restaurant.add_cus_area_table(result);
				else
					su_ku_na_restaurant.add_cus_area_table(result);
				continue;
			}
			int result = huffmanTree.encode(name);
			if (result == -1)
			{
				continue;
			}

			if (result % 2 == 1)
				go_jo_restaurant.add_cus_area_table(result);
			else
				su_ku_na_restaurant.add_cus_area_table(result);
		}
		else if (str == "HAND")
		{
			huffmanTree.hand_print();
		}
		else if (str == "KEITEIKEN")
		{
			ss >> num;
			su_ku_na_restaurant.remove_KEITEIKEN(stoi(num));
		}
		else if (str == "KOKUSEN")
		{
			go_jo_restaurant.remove_KOKUSEN();
		}
		else if (str == "CLEAVE")
		{
			ss >> num;
			su_ku_na_restaurant.CLEAVE(stoi(num));
		}
		else if (str == "LIMITLESS")
		{
			ss >> num;
			go_jo_restaurant.limitless_print(stoi(num));
		}
	}
	return;
}
