#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "BigInt.h"
using namespace std;

//менюшка при входе в программу
void PrintStart()
{
	printf("Program TCHMK.\n");
	printf("Enter the input parameters:\n");
	printf("\n<name of program> <filename A> <operation>\n<filename B> <result filename C> [b] [filename with module]\n\n");
	printf("Parametr: \n");
	printf("\"-b or 0\" - binfile\n\n");
	printf("Operations: \n");
	printf("\n +,-,*,/,%%,^\n\n");
}
//проверяем на косяки при вводе параметров
bool checkParam(int argc, char* argv[]) 
{
	
	if (argc < 2)

		return false;

	if (argc > 7)
	{
		printf("\nToo many parameters.\n");
		PrintStart();
		return false;
	}

	if ((argc > 1) && (argc < 5))
	{
		printf("\nIntroduced not all parameters.\n");
		PrintStart();
		return false;
	}

	if (strlen(argv[2]) > 1)
	{
		printf("Invalid input operation.\n");
		PrintStart();
		return false;
	}

	return true;
}
//чтение из файла
bool getFromFiles(char* fileA, char* fileB, char* fileMod, char operation, bool bin, BigInt& a, BigInt& b, BigInt& mod) 
{
	if (bin)
	{
		if (!a.getFrom_bin(fileA))
		{
			cout << "Can't get number from " << fileA << endl;
			return false;
		}
		if (!b.getFrom_bin(fileB))
		{
			cout << "Can't get number from " << fileB << endl;
			return false;
		}
		if (fileMod)
		{
			if (!mod.getFrom_bin(fileMod))
			{
				cout << "Can't get number from " << fileMod << endl;
				return false;
			}
		}
	}
	else
	{
		if (!a.getFrom_txt(fileA))
		{
			cout << "Can't get number from " << fileA << endl;
			return false;
		}
		if (!b.getFrom_txt(fileB))
		{
			cout << "Can't get number from " << fileB << endl;
			return false;
		}
		if (fileMod)
		{
			if (!mod.getFrom_txt(fileMod))
			{
				cout << "Can't get number from " << fileMod << endl;
				return false;
			}
		}
	}
	return true;
}
//действия при определенных операциях
bool actions(BigInt& a, char operation, BigInt& b, BigInt& c, BigInt mod)
{
	if (mod < 0)
	{
		cout << "Negative module" << endl;
		return false;
	}

	if (operation == '^')
	{
		c = Pow(a, b, mod);
		return true;
	}

	if (operation == '+')
	{
		c = a + b;
		return true;
	}

	else if (operation == '-')
	{
		c = a - b;
		return true;
	}

	else if (operation == '*')
	{
		c = a * b;
		return true;
	}

	else if (operation == '/')
	{
		if (b == 0)
		{
			cout << "Division by zero" << endl;
			return false;
		}
		c = a / b;
		return true;
	}

	else if (operation == '%')
	{
		if (b == 0)
		{
			cout << "Division by zero" << endl;
			return false;
		}
		c = a % b;
		return true;
	}
	//else if (operation == '^')
	//{
	//	cout << "No pow!" << endl;
	//}

}
//запись в файл
bool saveFileOut(char* fileC, BigInt c, bool bin)
{
	if (bin)
	{
		if (!c.saveTo_bin(fileC))
		{
			cout << "Can't save number to " << fileC << endl;
			return false;
		}
	}
	else
	{
		if (!c.saveTo_txt(fileC))
		{
			cout << "Can't save number to " << fileC << endl;
			return false;
		}

	}
	return true;
}
//самое оно
int main(int argc, char* argv[])
{
	/*BigInt a, b;
	cout << "Enter A: ";
	cin >> a;
	cout << "Enter B: ";
	cin >> b;
	
	cout << "A ^ B = " << (BigInt(a) ^ BigInt(b)) << endl;
	BigInt mod;
	cout << endl << "Enter modulus for pow: ";
	cin >> mod;
	cout << "A ^ B mod N = " << a << " ^ " << b << " mod " << mod << " = " << Pow(a, b, mod) << endl;
	
	
	
	
	BigInt d = "11";
	BigInt e = "3";
	cout << d - e;
	return 0;
	*/
	cout << "Program TCHMK." << endl;

	if (!checkParam(argc, argv))
		return 0;

	char *fname_in1 = argv[1];
	char operation = argv[2][0];
	char *fname_in2 = argv[3];
	char *fname_out = argv[4];
	bool  bin = false;
	char *fmodule = NULL;

	if (argc == 6)
	{
		if (!strcmp(argv[5], "-b"))
			bin = true;
	}

	if (argc == 7)
		fmodule = argv[6];

	BigInt a, b, c, mod;

	if (!getFromFiles(fname_in1, fname_in2, fmodule, operation, bin, a, b, mod))
		return 0;

	if (!actions(a, operation, b, c, mod))
		return 0;

	c.toString();

	if (!saveFileOut(fname_out, c, bin))
		return 0;
	return 0;
}

