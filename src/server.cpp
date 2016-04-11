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
	sites[0] =6666;
	sites[1] =6666;
	sites[2] =6666;
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
				txState[msg.transaction_id] = INIT;
				numVotes[msg.transaction_id] = 1;
				Socket temp;
				Message tempMsg(START_VOTE,msg.transaction_id);
				string tempStr = tempMsg.createMessage();
				for(int i = 0; i < sites.size();i++)
				{
					temp.connect('localhost',sites[i]);
					temp.send(str);
				}
				txState[msg.transaction_id] = WAIT;
				txLive.insert(msg.transaction_id);
				break;
			case VOTE_COMMIT:
				numVotes[msg.transaction_id] ++;
				if(numVotes[msg.transaction_id]==sites.size())
				{
					Socket temp;
					Message tempMsg(PRE_COMMIT,msg.transaction_id);
					string tempStr = tempMsg.createMessage();
					for(int i = 0; i < sites.size();i++)
					{
						temp.connect('localhost',sites[i]);
						temp.send(str);
					}
					txState[msg.transaction_id] = PRE_COMMIT;
					numVotes[msg.transaction_id] = 1;
				}
				break;
			case VOTE_ABORT:
				Socket temp;
				Message tempMsg(ABORT,msg.transaction_id);
				string tempStr = tempMsg.createMessage();
				for(int i = 0; i < sites.size();i++)
				{
					temp.connect('localhost',sites[i]);
					temp.send(str);
				}
				txLive.erase(msg.transaction_id);
				txState[msg.transaction_id] = ABORT;
				break;
			case ACK:
				numVotes[msg.transaction_id] ++;
				if(numVotes[msg.transaction_id]==sites.size())
				{
					Socket temp;
					Message tempMsg(COMMIT,msg.transaction_id);
					string tempStr = tempMsg.createMessage();
					for(int i = 0; i < sites.size();i++)
					{
						temp.connect('localhost',sites[i]);
						temp.send(str);
					}
					txState[msg.transaction_id] = COMMIT;
				}
				break;

		}

		}



	}
	
	return 0;
}