#pragma once
#include "Heap.h"
#include <assert.h>

template<class T>
struct HuffmanTreeNode
{
	T _weight;
	HuffmanTreeNode *_left;
	HuffmanTreeNode *_right;
	HuffmanTreeNode *_parent;
	HuffmanTreeNode(T x)
		:_weight(x)
		, _left(NULL)
		, _right(NULL)
		, _parent(NULL)
	{}
};


template<class T>
class HuffmanTree
{
	typedef HuffmanTreeNode<T> Node;
public:
	
	HuffmanTree(T*arr, int len,T &invalid)
	{
		assert(arr);
		_CreatHuffmanTree(arr, len,invalid);
	}
	~HuffmanTree()
	{
		_DestroyTree(_root);
	}
	Node* retRoot()
	{
		return _root;
	}
	template<class T>
	struct compareNode		//比较节点的仿函数
	{
		bool operator()(Node *l, Node *r)
		{
			return l->_weight < r->_weight;
		}
	};
protected:
	void _CreatHuffmanTree(T*arr, int len, T &invalid)
	{
		Node *left, *right, *parent;
		heap<Node*, compareNode<T>> hp;//将该仿函数传给堆；则可以在内部比较
		//heap<Node*, Less<T>> hp;
		for (int i = 0; i < len; i++)
		{
			if (arr[i] != invalid)
			{
				Node* node = new Node(arr[i]);
				hp.Push(node);
			}
		}
		while (hp.Size()>1)
		{
			left = hp.Top();
			hp.Pop();
			right = hp.Top();
			hp.Pop();

			Node* parent = new Node(left->_weight + right->_weight);
			parent->_left = left;
			parent->_right = right;
			left->_parent = parent;
			right->_parent = parent;
			hp.Push(parent);
		}
		_root = hp.Top();
	}
	void _DestroyTree(Node *&_root)
	{
		if (_root != NULL)
		{
			_DestroyTree(_root->_left);
			_DestroyTree(_root->_right);
			delete _root;
		}
		return;
	}
private:
	Node *_root;
};


