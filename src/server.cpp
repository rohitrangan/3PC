#include "../include/socket.h"
#include "../include/message.h"
// #include <iostream>
// #include <algorithm>
#include <cstdlib>
#include <cstring>
#include <ctime>
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
    srand (time (NULL));
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
            case START_VOTE:
                Socket tmp;
                int will_abort = rand () % 10;
                if (will_abort < 3)
                {
                    Message vote_abort (VOTE_ABORT, msg.transaction_id);
                    string snd_msg = vote_abort.createMessage ();
                    // Change to coordinator's port
                    tmp.connect ("localhost", 6666);
                    tmp.send (snd_msg);
                    // Change state to ABORT and remove from set.
                }
                else
                {
                    Message vote_commit (VOTE_COMMIT, msg.transaction_id);
                    string snd_msg = vote_commit.createMessage ();
                    // Change to coordinator's port
                    tmp.connect ("localhost", 6666);
                    tmp.send (snd_msg);
                    // Change state to READY.
                }
                break;
            case PRE_COMMIT:
                Socket tmp;
                Message ack (ACK, msg.transaction_id);
                string snd_msg = ack.createMessage ();
                // Change to coordinator's port
                tmp.connect ("localhost", 6666);
                tmp.send (snd_msg);
                // Change state to PRE_COMMIT
                break;
            case COMMIT:
                Socket tmp;
                Message ack (ACK, msg.transaction_id);
                string snd_msg = ack.createMessage ();
                // Change to coordinator's port
                tmp.connect ("localhost", 6666);
                tmp.send (snd_msg);
                // Change state to COMMIT and remove from set.
                break;
            case ABORT:
                Socket tmp;
                Message ack (ACK, msg.transaction_id);
                string snd_msg = ack.createMessage ();
                // Change to coordinator's port
                tmp.connect ("localhost", 6666);
                tmp.send (snd_msg);
                // Change state to ABORT and remove from set.
                break;

		}

	}
	return 0;
}
