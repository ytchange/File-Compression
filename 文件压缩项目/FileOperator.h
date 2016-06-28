#pragma once
#include "HuffmanTree.h"
typedef long long longtype;
struct FileInfo  
{
	unsigned char _ch;//字符,必须为unsigned类型，因为用_ch做下标时，范围应该是0~255
	string _code;//编码
	longtype _count;//出现的次数，大文件某个字符出现的次数可能会超过整形的范围
	FileInfo(unsigned char ch=0)
		:_ch(ch)
		, _count(0)
	{}
	FileInfo operator+(FileInfo &info)
	{
		FileInfo tmp;
		tmp._count = _count + info._count;
		return tmp;
	}
	bool operator!=(const FileInfo &info)const
	{
		return _count != info._count;
	}
	bool operator<(FileInfo &info)
	{
		return _count < info._count;
	}

};
class FileCompress
{
public:
	FileCompress()
	{
		for (int i = 0; i < 256; i++)
		{
			_array[i]._ch = i;
		}
	}
	//文件压缩
	void Compress(const char *filename)
	{
		assert(filename);
		FILE *fout = fopen(filename, "r");
		assert(fout);
		char ch = fgetc(fout);
		long long filesize = 0;//统计文件中总共出现的字符数
		while (ch != EOF)  //将个字符出现的次数统计在_count中
		{
			_array[(unsigned char)ch]._count++;
			ch = fgetc(fout);
			filesize++;
		}

		//编码
		FileInfo invalid;
		HuffmanTree<FileInfo> hft(_array, 256, invalid);//构建哈夫曼树
		//哈夫曼树构建成功后开始进行编码
		string code;
		_generateHuffCode(hft.retRoot(), code);

		//编码处理压缩
		string compressFile = filename;
		compressFile += ".compress";
		FILE *finCompressFile = fopen(compressFile.c_str(), "wb");//以二进制的方式压缩文件

		int size = 0;
		char infile = 0;
		fseek(fout, 0, SEEK_SET);  //将ch指向文件的开头
		ch = fgetc(fout);
		//string &chcode = _array[unsigned char(ch)]._code;
		while(ch!=EOF)
		{
			string &chcode = _array[unsigned char(ch)]._code;
			for (int j = 0; j < chcode.size(); j++)
			{
				infile <<= 1;
				if (chcode[j] == '1')
				{
					infile |= 1;
				}
				if (++size == 8)
				{
					fputc(infile, finCompressFile);
					size = 0;
					infile = 0;
				}
			}
			ch = fgetc(fout);
		}
		if (size!=0)  //将不够一个字节的补够一个字节，后面补0；
		{
			infile <<= (8 - size);
			fputc(infile, finCompressFile);
		}
		//编写配置文件，保存字符出现的次数，解压的过程中可以通过这个来建huffmantree；
		string configFile = filename;
		configFile += ".config";
		FILE *finconfig = fopen(configFile.c_str(), "w");
		assert(finconfig);
		//将整形count转化为字符串存储在配置文件中
		char countToString[100];
		itoa(filesize, countToString, 10);
		fputs(countToString, finconfig);
		fputc('\n', finconfig);

		for (int i = 0; i < 256; i++)
		{
			if (_array[i] != invalid)
			{
				itoa(_array[i]._count, countToString, 10);
				fputc(_array[i]._ch, finconfig);
				fputc(',', finconfig);
				fputs(countToString, finconfig);
				fputc('\n', finconfig);
			}
		}
		fclose(fout);
		fclose(finCompressFile);
		fclose(finconfig);
	}
	//文件解压缩
	void unCompress(const char *filename)
	{
		string ConfigFile = filename;
		ConfigFile += ".config";
		FILE *fOutConfig = fopen(ConfigFile.c_str(), "r");
		assert(fOutConfig);

		string line;
		long long filesize = 0;
		ReadLine(fOutConfig, line);
		filesize = atoi(line.c_str());
		line.clear();

		//获取到每个字符出现的次数，将其存入_count中
		while (ReadLine(fOutConfig, line))
		{
			if (!line.empty())
			{
				unsigned char ch = line[0];
				_array[ch]._count = atoi(line.substr(2).c_str());
				line.clear();
			}
			else
			{
				line += '\n';
			}
		}
		//重新构建哈夫曼树
		FileInfo invalid;
		HuffmanTree<FileInfo> hft(_array, 256, invalid);//构建哈夫曼树
		//读取压缩文件
		string compressfile = filename;
		compressfile += ".compress";
		FILE *filecom = fopen(compressfile.c_str(), "rb");//以二进制的方式打开压缩文件
		assert(filecom);

		string uncompressfile = filename;
		uncompressfile += ".uncompress";
		FILE *fileuncom = fopen(uncompressfile.c_str(), "w");//打开解压缩文件
		assert(fileuncom);

		
		HuffmanTreeNode<FileInfo>* root = hft.retRoot();
		HuffmanTreeNode<FileInfo>* cur = root;

		int pos = 8;
		unsigned char ch = fgetc(filecom);
		while (ch!=EOF)
		{
			if (cur->_left == NULL && cur->_right == NULL)
			{
				fputc(cur->_weight._ch, fileuncom);
				cur = root; //每找到一个就返回到根节点重新查找
				if (--filesize == 0)
				{
					break;
				}
			}
			--pos;
			if ((unsigned)ch & (1 << pos))
				cur = cur->_right;
			else
				cur = cur->_left;
			if (pos == 0)
			{
				ch = fgetc(filecom);
				pos = 8;
			}
		}
		fclose(fOutConfig);
		fclose(filecom);
		fclose(fileuncom);
	}
protected:
	void _generateHuffCode(HuffmanTreeNode<FileInfo>*root, string code)
	{
		if (root == NULL)
		{
			return;
		}
		if (root->_left == NULL&&root->_right == NULL)
		{
			_array[root->_weight._ch]._code = code;
		}
		_generateHuffCode(root->_left, code + '0');//左走为0，右走为1
		_generateHuffCode(root->_right, code + '1');
	}
	bool ReadLine(FILE* file, string &line)
	{
		char ch = fgetc(file);
		if (ch == EOF)
		{
			return false;
		}
		while (ch != EOF&&ch != '\n')
		{
			line += ch;
			ch = fgetc(file);
		}
		return true;
	}
private:
	FileInfo _array[256];
};
