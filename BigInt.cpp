#include "BigInt.h"
#include <fstream>

using std::ifstream;
using std::ofstream;

//определяет что это число
inline bool isDigit(int c)
{
	return(c >= (int)'0' && c <= (int)'9');
}
//оператор входа
istream &operator>>(istream &in, BigInt &n)
{
	n.size = 0;
	n.sign = 1;
	int sign = 1;
	int c;
	while ((c = in.peek()) >= 0 &&
		(c == ' ' || c == '\t' || c == '\r' || c == '\n'))
		in.get();
	if (c < 0 || (c != (int)'-' && !isDigit(c)))
	{
		in >> c;
		return in;
	}
	if (c == (int)'-') { sign = -1; in.get(); }

	while ((c = in.peek()) >= 0 && isDigit(c))
	{
		in.get();
		n = n * 10;
		n = n + (c - (int)'0');
	}
	n.sign = sign;
	n.normalize();
	return in;
}
//оператор выхода
ostream &operator<<(ostream &out, BigInt n)
{
	return out << n.toString();
}
// Создание большого числа из n с емкостью cap
BigInt::BigInt(int n, int cap)
{
	cap = mymax(cap, (int)sizeof(n)* 8);
	capacity = cap;
	sign = sig(n);
	n *= sign;
	digits = new char[cap];
	memset(digits, 0, cap);
	for (size = 0; n; size++)
	{
		digits[size] = n % 10;
		n /= 10;
	}
}
// Создание большого числа из n
BigInt::BigInt(int n)
{
	capacity = 1024;
	sign = sig(n);
	n *= sign;
	digits = new char[capacity];
	memset(digits, 0, capacity);
	size = 0;
	while (n)
	{
		digits[size++] = n % 10;
		n /= 10;
	}
}
// Создание большого числа со значением 0
BigInt::BigInt()
{
	capacity = 128;
	sign = 0;
	digits = new char[capacity];
	memset(digits, 0, capacity);
	size = 0;
}
//оператор присваивания
const BigInt &BigInt::operator=(const BigInt &n)
{
	if (&n != this)
	{
		if (capacity < n.size)
		{
			capacity = n.capacity;
			delete[] digits;
			digits = new char[capacity];
		}
		sign = n.sign;
		size = n.size;
		memcpy(digits, n.digits, size);
		memset(digits + size, 0, capacity - size);
	}
	return *this;
}
//Определение знака
int BigInt::sig(int n)
{
	return(n > 0 ? 1 : (n == 0 ? 0 : -1)); //если (n>0) возврати 1, иначе возврати( если (n=0) возврати 0, иначе -1)
}
//Создает большое число, прочитав инфу из строки типа "32141"
BigInt::BigInt(const char s[])
{
	capacity = mymax((int)strlen(s), 16);
	sign = 0;
	digits = new char[capacity];
	memset(digits, 0, capacity);

	istringstream in(s);
	in >> (*this);
}
//конструктор копирования
BigInt::BigInt(const BigInt &n)
{
	capacity = n.capacity;
	sign = n.sign;
	size = n.size;
	digits = new char[capacity];
	memcpy(digits, n.digits, capacity);
}
//деструктор
BigInt::~BigInt()
{
	delete[] digits;
}
//выкинуть 0 в старших разрядах
void BigInt::normalize()
{
	while (size && !digits[size - 1]) size--;
	if (!size) sign = 0;
}
//вывод числа
string BigInt::toString()
{
	string s = (sign >= 0 ? "" : "-");
	for (int i = size - 1; i >= 0; i--)
		s += (digits[i] + '0');
	if (size == 0) s += '0';
	return s;
}
//увеличение емкости под число, если необходимо
void BigInt::grow()
{
	char *olddigits = digits;
	int oldCap = capacity;
	capacity *= 2;
	digits = new char[capacity];
	memcpy(digits, olddigits, oldCap);
	memset(digits + oldCap, 0, oldCap);
	delete[] olddigits;
}
//арифметические операторы
BigInt BigInt::operator-()
{
	BigInt result(*this);
	result.sign *= -1;
	return result;
}

BigInt BigInt::operator+(BigInt n)
{
	int maxS = mymax(size, n.size) + 1;
	BigInt res = *this;
	while (maxS >= res.capacity) res.grow();

	if (!n.sign)
		return *this;

	if (!res.sign)
		res.sign = n.sign;

	if (res.sign == n.sign)
	{
		int carry = 0;
		int i;
		for (i = 0; i < maxS - 1 || carry; i++)
		{
			int newdig = carry;
			if (i < size)
				newdig += digits[i];
			if (i < n.size)
				newdig += n.digits[i];
			res.digits[i] = newdig % 10;
			carry = newdig / 10;
		}
		res.size = mymax(i, size);
		return res;
	}
	else
	{
		n.sign *= -1;
		operator-(n);
		n.sign *= -1;
	}
	return *this;
}

BigInt BigInt::operator-(BigInt n)
{
	int maxS = mymax(size, n.size) + 1;
	BigInt res = *this;
	while (maxS >= res.capacity) res.grow();

	if (!n.sign)
		return *this;

	if (!this->sign)
	{
		return n;
	}

	if (res.sign == n.sign)
	{
		if (res.sign >= 0 && res < n || res.sign < 0 && res > n)
		{
			// Вычитание большего числа
			BigInt tmp = n;
			tmp = tmp - res;
			res = tmp;
			res.sign = -res.sign;
			return res;
		}

		int carry = 0;
		int i;
		for (i = 0; i < maxS - 1; i++)
		{
			int newdig = carry;

			if (i < res.size)
				newdig += res.digits[i];

			if (i < n.size)
				newdig -= n.digits[i];

			if (newdig < 0)
			{
				newdig += 10;
				carry = -1;
			}
			else carry = 0;
			res.digits[i] = newdig;
		}
		if (carry)
		{// если есть заем
			if (i)
				res.digits[0] = 10 - res.digits[0];
			size = (i ? 1 : 0);
			for (int j = 1; j < i; j++)
			{
				res.digits[j] = 9 - res.digits[j];
				if (res.digits[i]) size = j + 1;
			}
			sign *= -1;
		}
		res.normalize();
		return res;
	}
	else
	{
		n.sign *= -1;
		BigInt res = *this + n;
		n.sign *= -1;
		return res;
	}
}

BigInt BigInt::operator*(int n)
{
	BigInt result(0, size + (int)sizeof(n)* 8);
	int nsign = sig(n);
	n *= nsign;
	result.sign = sign * nsign;
	if (!result.sign) return result;

	int i, j;
	for (i = 0; n; i++)
	{
		int dig = n % 10;
		if (dig)
		{
			int carry = 0;
			for (j = 0; j < size || carry; j++)
			{
				int newDig = result.digits[i + j] + (j < size ? dig * digits[j] : 0) + carry;
				result.digits[i + j] = newDig % 10;
				carry = newDig / 10;
			}
		}
		n /= 10;
	}
	result.size = i + j - 1;
	return result;
}

BigInt BigInt::operator*(BigInt n)
{
	BigInt result(0, size + n.size);

	result.sign = sign * n.sign;
	if (!result.sign) return result;

	int i, j;
	for (i = 0; i < n.size; i++)
	{
		if (n.digits[i])
		{
			int carry = 0;
			for (j = 0; j < size || carry; j++)
			{
				int newDig =
					result.digits[i + j] +
					(j < size ? n.digits[i] * digits[j] : 0) +
					carry;
				result.digits[i + j] = newDig % 10;
				carry = newDig / 10;
			}
		}
	}
	result.size = i + j - 1;

	return result;
}

BigInt BigInt::operator/(int n)
{
	if (!n) n /= n;

	BigInt result(*this);
	result /= n;
	return result;
}

BigInt BigInt::operator/(BigInt n)
{
//	if (!(n == 0)) n.size /= n.size;

	BigInt result(*this);
	result /= n;
	return result;
}

void BigInt::operator/=(int n)
{
	divide(n);
}

void BigInt::operator/=(BigInt n)
{
	divide(n);
}

BigInt BigInt::operator%(BigInt n)
{
	BigInt tmp(*this);
	return tmp.divide(n);
}

BigInt BigInt::operator^(const BigInt& n) const
{// возведение *this в степень n
	BigInt res = 1;
	BigInt base = *this;
	for (BigInt i = n; i > (int)0; i = i - 1)
		res = res * base;
	return res;
}

BigInt BigInt::divide(BigInt n)
{
//	if (!(n == 0)) n.size /= n.size;

	if (!sign) return 0;
	sign *= n.sign;

	int oldSign = n.sign;
	n.sign = 1;

	BigInt tmp(0, size);
	for (int i = size - 1; i >= 0; i--)
	{
		tmp = tmp * 10;
		tmp = tmp + (int)digits[i];
		digits[i] = 0;
		while (tmp >= n)
		{
			tmp = tmp - n;
			digits[i]++;
		}
	}
	normalize();

	n.sign = oldSign;

	return tmp;
}
//операторы сравнения
bool BigInt::operator<(BigInt n)
{
	return(compare(n) < 0);
}

bool BigInt::operator>(BigInt n)
{
	return(compare(n) > 0);
}

bool BigInt::operator==(BigInt n)
{
	return(compare(n) == 0);
}

bool BigInt::operator!=(BigInt n)
{
	return !operator==(n);
}

bool BigInt::operator<=(BigInt n)
{
	return(compare(n) <= 0);
}

bool BigInt::operator>=(BigInt n)
{
	return(compare(n) >= 0);
}

bool BigInt::operator<(int n)
{
	return(compare(BigInt(n)) < 0);
}

bool BigInt::operator>(int n)
{
	return(compare(BigInt(n)) > 0);
}

bool BigInt::operator==(int n)
{
	return(compare(BigInt(n)) == 0);
}

bool BigInt::operator<=(int n)
{
	return(compare(BigInt(n)) <= 0);
}

bool BigInt::operator>=(int n)
{
	return(compare(BigInt(n)) >= 0);
}
//сравнение
int BigInt::compare(BigInt n)
{
	if (sign < n.sign) return -1;
	if (sign > n.sign) return 1;
	if (size < n.size) return -sign;
	if (size > n.size) return sign;
	for (int i = size - 1; i >= 0; i--)
	{
		if (digits[i] < n.digits[i]) return -sign;
		else if (digits[i] > n.digits[i]) return sign;
	}
	return 0;
}

bool  BigInt::getFrom_txt(const char* filename)
{
	FILE *fp = fopen(filename, "r");
	if (!fp)
		return false;
	fseek(fp, 0, SEEK_END);
	int fileSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	char * str = new char[fileSize + 1]();
	for (int i = 0; i < fileSize; i++)
	{
		str[i] = fgetc(fp);
	}

	fclose(fp);
	BigInt number(str);
	*this = number;

	delete[]str;

	return true;

}

bool BigInt::saveTo_txt(const char* filename)
{
	FILE *fp = fopen(filename, "w");
	if (!fp)
		return false;

	string str = this->toString();
	int len = str.length();
	char* string = new char[len + 1];
	fwrite(str.c_str(), sizeof(char), len, fp);
	fclose(fp);

	return true;

}

bool BigInt::saveTo_bin(const char* filename)
{// будем считать, что в бинарный файл необходимо записать число по основанию 256
	// то есть необходимо перейти от основания 10 к 256 (256 - потому что максимальное значение байта - 255)
	// для этого надо находить остатки от деления на 256
	// пока число не уменьшиться до 0

	ofstream outBinFile(filename, std::ios::binary);
	if (outBinFile.fail())
		return false;

	BigInt temp = *this; // число, которое будем делить на 256
	temp.sign = 1; // знак числа в файл не записывается
	BigInt b256 = 256; // чтобы каждый раз не вызывать конструктор
	BigInt b0 = (int)0;

	temp.toString();

	while (temp != b0)
	{
		BigInt remainder;
		remainder = temp % b256; // делим temp на 256, в remainder - остаток
		temp /= b256;

		// то, что осталось в remainder - необходимо записать в файл
		// остаток от деления на 256 - число от 0 до 255
		// то есть максимум 3 десятичных разряда
		if (remainder != (int)0)
		{
			unsigned char ost = remainder.digits[0];
			if (remainder.size > 1)
			{
				ost += (remainder.digits[1] * 10);
				if (remainder.size == 3)
				{
					ost += (remainder.digits[2] * 100);
				}
			}
			outBinFile.write((char*)&ost, 1); // записываем байт в файл
		}
	}

	outBinFile.close();
	return true;
}

bool BigInt::getFrom_bin(const char* filename)
{// будем считать, что в бинарном файле записаны разряды числа по модулю 256.
	// таким образом, чтобы считать число из бинарного файла, необходимо каждый байт 
	// умножить на 256 ^ i, где i - позиция байта в файле и всё это сложить

	ifstream inBinFile(filename, std::ios::binary);
	if (inBinFile.fail())
		return false;

	inBinFile.seekg(0, std::ios::end);
	int SizeOfFile = inBinFile.tellg();
	inBinFile.seekg(0, std::ios::beg);

	// считываем содержимое файла
	unsigned char* fileContent = new unsigned char[SizeOfFile];
	inBinFile.read((char*)fileContent, SizeOfFile);
	inBinFile.close();

	// переведём содержимое файла к основанию 10
	BigInt res;
	BigInt b256 = 1; // чтобы каждый раз не вызывать конструктор из int


	for (int i = 0; i < SizeOfFile; i++)
	{
		unsigned int fi = fileContent[i];
		res = res + b256 * fi;
		b256 = b256 * 256;
	}
	*this = res;
	return true;
}

BigInt Pow(BigInt& A, BigInt& n, BigInt& mod)
{// возведение A в степень n по модулю modulus
	if (mod <= (int)0)
		return A ^ n;

	BigInt base = A % mod;
	BigInt res = 1;

	for (BigInt i = n; i > (int)0; i = i - 1)
		res = (res * base) % mod;

	return res;
}

