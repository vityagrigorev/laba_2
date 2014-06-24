linuxdyn: main.cpp BigInt.cpp
	g++ -c -fPIC BigInt.cpp
	g++ -shared -o libBigInt.so BigInt.o
	LD_LIBRARY_PATH=./ g++ main.cpp -lBigInt -I. -Wl,-rpath,. -L. -o laba
	
windowsdyn: main.cpp BigInt.cpp
	g++ -c BigInt.cpp
	g++ -shared -o BigInt.dll BigInt.o
	g++ main.cpp BigInt.dll -I. -Wl,-rpath,. -L. -o laba.exe
	del BigInt.o