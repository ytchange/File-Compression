#include <iostream>
#include <vector>
using namespace std;
//Ð¡¶Ñ
template<class T>
struct Less
{
	bool operator()(const T&l, const T&r)
	{
		return l < r;
	}
};
//´ó¶Ñ
template<class T>
struct Greater
{
	bool operator()(const T&l, const T&r)
	{
		return l > r;
	}
};
template<class T,class compare=Greater<T>>
class heap
{
public:
	heap()
	{}
	heap(const T*array, size_t size)
	{
		for (size_t i = 0; i < size; i++)
		{
			_array.push_back(array[i]);
		}
		int begin = _array.size() / 2 - 1;
		for (; begin >= 0; begin--)
		{
			AdjustToDown(begin);
		}
	}
	/*heap(const vector<T> &array)
	{
		_array.swap(array);
		int begin = _array.size() / 2 - 1;
		for (; begin >= 0; begin--)
		{
			AdjustToDown(begin);
		}
	}*/
	void Push(T num)
	{
		_array.push_back(num);
		int begin = _array.size() - 1;
		AdgustToUp(begin);

	}
	T& Top()
	{
		return _array[0];
	}
	void Pop()
	{
		if (_array.size() <= 0)
		{
			return;
		}
		swap(_array[0], _array[_array.size() - 1]);
		_array.pop_back();
		AdjustToDown(0);
	}
	bool Empty()
	{
		return _array.empty();
	}
	size_t Size()
	{
		return _array.size();
	}
	void print()
	{
		for (size_t i = 0; i < _array.size(); i++)
		{
			cout << _array[i] << " ";
		}
		cout << endl;
	}

protected:
	void AdjustToDown(int root)
	{
		compare com;
		while (1)
		{
			size_t left = root * 2 + 1;
			size_t right = left + 1;
			size_t tmp = left;
			if (right < _array.size() && com(_array[right], _array[left]))
			{
				tmp = right;
			}
			if (left < _array.size() && com(_array[tmp], _array[root]))
			{
				swap(_array[root], _array[tmp]);
				root = tmp;
			}
			else
			{
				break;
			}
		}
	}
	void AdgustToUp(int child)
	{
		compare com;
		int root = 1;
		while (child)
		{
			root = (child -1)/2;
			if (com(_array[child], _array[root]))
			{
				swap(_array[root], _array[child]);
				child = root;
			}
			else
			{
				break;
			}
		}
	}
private:
	vector<T> _array;
};

