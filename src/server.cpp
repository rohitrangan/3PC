#include "../include/socket.h"
#include "../include/message.h"
// #include <iostream>
// #include <algorithm>
#include <cstring>
#include <map>
#include <set>

using namespace std;
enum state
{
    INIT,
    READY,
    WAIT,
    PRE_COMMIT,
    COMMIT,
    ABORT    
};

int main()
{
	Socket site;
	site.bind(6666);
	map<int,state> txState;
	set<int> txLive;
	map<int,nVotes> numVotes;
	site.listen(100);
	std::vector<int> sites(3);
	sites[1] =6666;
	sites[2] =6666;
	sites[3] =6666;
	cout<<"Server -1 Up"<<endl;
	while(1)
	{
		site.accept();
		string str;
		str = site.recv((size_t)256);
		cout<<msg<<endl;
		Message msg(str);
		switch(msg.message)
		{
			case TRANSACTION:
				Socket temp;
				for(int)
				temp.connect('localhost',)
		}



	}
	
	return 0;
}