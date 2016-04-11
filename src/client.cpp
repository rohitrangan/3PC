#include "../include/socket.h"
// #include <iostream>
// #include <algorithm>
// #include <cstring>

using namespace std;
int main()
{
	Socket site1(2);
	site1.connect("localhost",6666);
	
	return 0;
}