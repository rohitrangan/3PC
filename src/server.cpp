#include "../include/socket.h"
// #include <iostream>
// #include <algorithm>
// #include <cstring>

using namespace std;
int main()
{
	Socket site1;
	site1.bind(6666);
	

		cout<<"Server bound"<<endl;
		
			site1.listen(100);
		while(1)
		{
			site1.accept();

		}
	
	return 0;
}