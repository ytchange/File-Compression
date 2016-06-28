#pragma once
#include "HuffmanTree.h"
typedef long long longtype;
struct FileInfo  
{
	unsigned char _ch;//�ַ�,����Ϊunsigned���ͣ���Ϊ��_ch���±�ʱ����ΧӦ����0~255
	string _code;//����
	longtype _count;//���ֵĴ��������ļ�ĳ���ַ����ֵĴ������ܻᳬ�����εķ�Χ
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
	//�ļ�ѹ��
	void Compress(const char *filename)
	{
		assert(filename);
		FILE *fout = fopen(filename, "r");
		assert(fout);
		char ch = fgetc(fout);
		long long filesize = 0;//ͳ���ļ����ܹ����ֵ��ַ���
		while (ch != EOF)  //�����ַ����ֵĴ���ͳ����_count��
		{
			_array[(unsigned char)ch]._count++;
			ch = fgetc(fout);
			filesize++;
		}

		//����
		FileInfo invalid;
		HuffmanTree<FileInfo> hft(_array, 256, invalid);//������������
		//�������������ɹ���ʼ���б���
		string code;
		_generateHuffCode(hft.retRoot(), code);

		//���봦��ѹ��
		string compressFile = filename;
		compressFile += ".compress";
		FILE *finCompressFile = fopen(compressFile.c_str(), "wb");//�Զ����Ƶķ�ʽѹ���ļ�

		int size = 0;
		char infile = 0;
		fseek(fout, 0, SEEK_SET);  //��chָ���ļ��Ŀ�ͷ
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
		if (size!=0)  //������һ���ֽڵĲ���һ���ֽڣ����油0��
		{
			infile <<= (8 - size);
			fputc(infile, finCompressFile);
		}
		//��д�����ļ��������ַ����ֵĴ�������ѹ�Ĺ����п���ͨ���������huffmantree��
		string configFile = filename;
		configFile += ".config";
		FILE *finconfig = fopen(configFile.c_str(), "w");
		assert(finconfig);
		//������countת��Ϊ�ַ����洢�������ļ���
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
	//�ļ���ѹ��
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

		//��ȡ��ÿ���ַ����ֵĴ������������_count��
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
		//���¹�����������
		FileInfo invalid;
		HuffmanTree<FileInfo> hft(_array, 256, invalid);//������������
		//��ȡѹ���ļ�
		string compressfile = filename;
		compressfile += ".compress";
		FILE *filecom = fopen(compressfile.c_str(), "rb");//�Զ����Ƶķ�ʽ��ѹ���ļ�
		assert(filecom);

		string uncompressfile = filename;
		uncompressfile += ".uncompress";
		FILE *fileuncom = fopen(uncompressfile.c_str(), "w");//�򿪽�ѹ���ļ�
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
				cur = root; //ÿ�ҵ�һ���ͷ��ص����ڵ����²���
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
		_generateHuffCode(root->_left, code + '0');//����Ϊ0������Ϊ1
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
