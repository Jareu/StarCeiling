#pragma once
#pragma once
#include <type_traits>

/*
	Balance of a node is the difference between the height of its left and right children
	balance = height(left) - height(right)
	4 cases that should be checked every time an item is inserted or removed from a tree
	1) Left heavy when balance > 1
		A) Right child is not null (solved using a single right rotation)
		B) Right child is null (left-right situation; solved with left rotation on left child of the root node of the sub-tree to be fixed, followed by a right rotation)
	2) Right heavy when balance < -1
		A) Left child is not null (solved using a single left rotation)
		B) Left child is null (right-left situation; solved with right rotation on right child of the root node of the sub-tree to be fixed, followed by a left rotation)

	balanced when |balance| <= 1
	heights of the nodes should be updated with every rotation

*/

template <typename TKey, typename TData>
class AVLTree
{
private:
	/*
		Node Class Definition
	*/
	class Node
	{
	private:

		const TKey key_;
		TData data_;
		uint16_t height_;
		std::unique_ptr<Node> leftchild_;
		std::unique_ptr<Node> rightchild_;
		// Node* parent_; (TODO: determine if parent is really needed)

	public:
		// constructor
		Node(const TKey& key, const TData& data) : key_{ key }, data_{ data }, height_{ 1 } {
			static_assert(std::is_integral_v<TKey>, "Integral type required for key."); // key must be integral type
		}

		/*
				-- Getters --
		*/

		inline uint16_t GetHeight() const
		{
			return height_;
		}

		inline std::unique_ptr<Node>& GetLeftChild()
		{
			return leftchild_;
		}

		inline std::unique_ptr<Node>& GetRightChild()
		{
			return rightchild_;
		}

		inline const TKey& GetKey() const
		{
			return key_;
		}

		inline TData& GetData()
		{
			return data_;
		}

		/*
				-- Setters --
		*/

		inline void SetHeight(uint16_t height)
		{
			this->height_ = height;
		}

		inline void SetLeftChild(std::unique_ptr<Node>& data)
		{
			this->leftchild_ = std::move(data);
		}

		inline void SetRightChild(std::unique_ptr<Node>& data)
		{
			this->rightchild_ = std::move(data);
		}

		inline void SetData(const TData& data)
		{
			this->data_ = data;
		}
	};

	/*
		AVLTree private member variables
	*/

	std::unique_ptr<Node> root_;
	bool bOverwriteExistingKeys = true;

	template<class T>
	inline const int GetMax(const T a, const T b) {
		static_assert(std::is_integral_v<T>, "Integral type required.");
		return (a > b ? a : b);
	}

	template<class T>
	inline const int GetMin(const T a, const T b) {
		static_assert(std::is_integral_v<T>, "Integral type required.");
		return (a < b ? a : b);
	}

	inline uint16_t GetNodeHeight(const Node* pNode) const {
		return pNode ? pNode->GetHeight() : 0;
	}

	inline int GetBalance(Node* node) const {
		return (node ? GetNodeHeight(node->GetLeftChild().get()) - GetNodeHeight(node->GetRightChild().get()) : 0);
	}

	inline void UpdateHeight(Node* node)
	{
		if (node) node->SetHeight(GetMax(GetNodeHeight(node->GetLeftChild().get()), GetNodeHeight(node->GetRightChild().get())) + 1);
	}

	std::unique_ptr<Node>& RotateLeft(std::unique_ptr<Node>& node) {
		Node* rightNode = node->GetRightChild().release();
		Node* node_ = node.release();
		Node* centerNode = rightNode->GetLeftChild().release();

		rightNode->GetLeftChild().reset(node_);
		node_->GetRightChild().reset(centerNode);
		node.reset(rightNode);

		UpdateHeight(node_);
		UpdateHeight(rightNode);

		return node;
	}

	std::unique_ptr<Node>& RotateRight(std::unique_ptr<Node>& node) {
		Node* leftNode = node->GetLeftChild().release();
		Node* node_ = node.release();
		Node* centerNode = leftNode->GetRightChild().release();

		leftNode->GetRightChild().reset(node_);
		node_->GetLeftChild().reset(centerNode);
		node.reset(leftNode);

		UpdateHeight(node_);
		UpdateHeight(leftNode);

		return node;
	}

	std::unique_ptr<Node>& ApplyRotation(std::unique_ptr<Node>& node) {
		int balance = GetBalance(node.get());

		if (balance > 1) {
			// left heavy
			if (GetBalance(node->GetLeftChild().get()) < 0) {
				node->SetLeftChild(RotateLeft(node->GetLeftChild()));
			}
			return RotateRight(node);
		}
		else if (balance < -1) {
			// right heavy
			if (GetBalance(node->GetRightChild().get()) > 0) {
				node->SetRightChild(RotateRight(node->GetRightChild()));
			}
			return RotateLeft(node);
		}

		return node;
	}

	std::unique_ptr<Node>& Insert(const TKey& key, const TData& data, std::unique_ptr<Node>& node)
	{
		static_assert(std::is_integral_v<TKey>, "Integral type required for key."); // key must be integral type

		if (node == nullptr) {
			node = std::make_unique<Node>(key, data);
			return node;
		}

		if (key < node->GetKey()) {
			node->SetLeftChild(Insert(key, data, node->GetLeftChild()));
		}
		else if (key > node->GetKey()) {
			node->SetRightChild(Insert(key, data, node->GetRightChild()));
		}
		else {
			// existing key. Overwrite?
			if (bOverwriteExistingKeys) {
				node->SetData(data);
			}
		}

		/*
		if (key < node->GetKey()) {
			// left sub-tree
			if (node->GetLeftChild()) {
				Insert(key, data, node->GetLeftChild());
			}
			else {
				// left child is null
				auto newNodePtr = std::make_unique<Node>(key, data);
				node->SetLeftChild(newNodePtr);
			}
		}
		else if (key > node->GetKey()) {
			// right sub-tree
			if (node->GetRightChild()) {
				Insert(key, data, node->GetRightChild());
			}
			else {
				// right child is null
				auto newNodePtr = std::make_unique<Node>(key, data);
				node->SetRightChild(newNodePtr);
			}
		}
		else {
			// existing key. Overwrite?
			if (bOverwriteExistingKeys) {
				node->SetData(data);
			}
		}
		*/


		UpdateHeight(node.get());
		return ApplyRotation(node);
	}

	inline std::unique_ptr<Node>& GetMin(std::unique_ptr<Node>& node) const
	{
		if (node->GetLeftChild()) {
			return GetMin(node->GetLeftChild());
		}
		return node;
	}

	inline std::unique_ptr<Node>& GetMax(std::unique_ptr<Node>& node) const
	{
		if (node->GetRightChild()) {
			return GetMax(node->GetRightChild());
		}
		return node;
	}

	inline void TraverseInOrder(Node* node)
	{
		if (node) {
			TraverseInOrder(node->GetLeftChild().get());
			// node->doSomething() do something with node
			TraverseInOrder(node->GetRightChild().get());
		}
	}

	std::unique_ptr<Node>& Delete(TKey key, std::unique_ptr<Node>& node) {
		if (!node) return;

		if (key < node->GetKey()) {
			node->SetLeftChild(Delete(key, node->GetLeftChild()));
		}
		else if (key > node->GetKey()) {
			node->SetRightChild(Delete(key, node->GetRightChild()));
		}
		else {
			// delete 
			// One child or leaf node (no children)
			if (node->GetLeftChild() == nullptr) {
				return node->GetRightChild();
			}
			else if (node->GetRightChild() == nullptr) {
				return node->GetLeftChild();
			}

			//Two children
			node = std::move(GetMax(node->GetLeftChild()));
			node->SetLeftChild(Delete(node->GetKey(), node->GetLeftChild()));
		}

		return node;
	}

	inline Node* Search(const TKey key, Node* node) {
		static_assert(std::is_integral_v<TKey>, "Integral type required for key."); // key must be integral type

		if (!node) return nullptr;

		if (node->GetKey() == key) {
			// found it
			return node;
		}
		else if (key < node->GetKey()) {
			// Search left
			return Search(key, node->GetLeftChild().get());
		}
		else if (node->GetKey() < key) {
			// Search right
			return Search(key, node->GetRightChild().get());
		}
	}

public:
	AVLTree() = default;

	inline bool IsEmpty() const
	{
		return (root_ == nullptr);
	}

	inline TData GetMin() const
	{
		if (IsEmpty()) return TData(); // Todo: make function return reference to data and figure out how to handle returning references to null or default data
		return GetMin(root_);
	}

	inline TData GetMax() const
	{
		if (IsEmpty()) return TData(); // Todo: same as in GetMin()
		return GetMax(root_);
	}

	inline void Traverse()
	{
		TraverseInOrder(root_.get());
	}

	void Insert(const TKey& key, const TData& data)
	{
		static_assert(std::is_integral_v<TKey>, "Integral type required for key."); // key must be integral type
		if (IsEmpty()) {
			root_ = std::make_unique<Node>(key, data);
		}
		else {
			Insert(key, data, root_);
		}
	}

	TData* Search(TKey key) {
		return &Search(key, root_.get())->GetData();
	}

	void Delete(TKey key) {
		root_ = std::move(Delete(key, root_));
	}

	inline void SetOverrideExistingKeys(bool val)
	{
		// how to handle existing keys when inserting a new key/value pair
		bOverwriteExistingKeys = val;
	}

	/****************************************
	 *			Debug functions				*
	/****************************************/

	void PrintDebug()
	{
		TraverseDebug(root_.get());
	}

};
