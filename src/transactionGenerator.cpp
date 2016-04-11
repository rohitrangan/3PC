#include "../include/socket.h"
#include "../include/message.h"
// #include <iostream>
// #include <algorithm>
// #include <thread>         // std::this_thread::sleep_for
// #include <chrono> 
#include <cstring>
#include <vector>
#include <ctime>
#include <cstdlib>
using namespace std;
int main()
{
	Socket transaction_gen(5);
	std::vector<int> sites(3);
	sites[1] =6666;
	sites[2] =6666;
	sites[3] =6666;
	int transactionId =1;
    while(transactionId!=10)
    {
    	Message msg(TRANSACTION,transactionId);
    	string str = msg.createMessage();
    	srand (time(NULL));
    	int cur_site = rand() % 3 + 1;
    	cout<<"Transaction ID:"<<transactionId<<" Site ID :"<<cur_site<<endl;
		transaction_gen.connect("localhost",sites[cur_site]);
		transaction_gen.send(str);
		// std::this_thread::sleep_for (std::chrono::seconds(1));
		

	}
		
	return 0;
}