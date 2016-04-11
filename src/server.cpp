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
    S_INIT,
    S_READY,
    S_WAIT,
    S_PRE_COMMIT,
    S_COMMIT,
    S_ABORT    
};

struct transaction_info
{
    int numVotes;
    state site_st;
    int coordinator;
};

int main(int argc, char *argv[])
{
    int site_id = std::stoi (string (argv[1]));
    srand (time (NULL));
	Socket site;
	site.bind(6666);
	set<int> txLive;
	map<int,transaction_info> trans_table;
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
                transaction_info new_t;
                new_t.numVotes = 1;
                new_t.site_st = S_INIT;
                new_t.coordinator = site_id;
				trans_table[msg.transaction_id] = new_t;
                cout << "Site " << site_id << " accepted transaction " <<
                    msg.transaction_id << endl;
				Socket temp;
				Message tempMsg(START_VOTE,msg.transaction_id, site_id);
				string tempStr = tempMsg.createMessage();
				for(int i = 0; i < sites.size();i++)
				{
                    if (i != site_id)
                    {
					    temp.connect("localhost", sites[i]);
					    temp.send(str);
                        cout << "[" << msg.transaction_id << "]Site " << site_id
                            << " sent START_VOTE to Site " << i << endl;
                    }
				}
                trans_table[msg.transaction_id].site_st = S_WAIT;
				txLive.insert(msg.transaction_id);
				break;
			case VOTE_COMMIT:
				trans_table[msg.transaction_id].numVotes++;
                cout << "[" << msg.transaction_id << "]Site " << site_id
                    << " got VOTE_COMMIT from Site " << msg.site_id << endl;
				if(trans_table[msg.transaction_id].numVotes==sites.size())
				{
                    cout << "[" << msg.transaction_id << "]Site " << site_id <<
                        " has all vote commits." << endl;
					Socket temp;
					Message tempMsg(PRE_COMMIT,msg.transaction_id, site_id);
					string tempStr = tempMsg.createMessage();
					for(int i = 0; i < sites.size();i++)
					{
						temp.connect("localhost", sites[i]);
						temp.send(str);
                        cout << "[" << msg.transaction_id << "]Site " << site_id
                            << " sent PRE_COMMIT to Site " << i << endl;
					}
					trans_table[msg.transaction_id].site_st = S_PRE_COMMIT;
					trans_table[msg.transaction_id].numVotes = 1;
				}
				break;
			case VOTE_ABORT:
				Socket temp;
				Message tempMsg(ABORT,msg.transaction_id, site_id);
				string tempStr = tempMsg.createMessage();
				for(int i = 0; i < sites.size();i++)
				{
					temp.connect("localhost",sites[i]);
					temp.send(str);
				}
				txLive.erase(msg.transaction_id);
				trans_table[msg.transaction_id].site_st = S_ABORT;
				break;
			case ACK:
				trans_table[msg.transaction_id].numVotes++;
				if(trans_table[msg.transaction_id].numVotes==sites.size())
				{
					Socket temp;
					Message tempMsg(COMMIT, msg.transaction_id, site_id);
					string tempStr = tempMsg.createMessage();
					for(int i = 0; i < sites.size();i++)
					{
						temp.connect("localhost",sites[i]);
						temp.send(str);
					}
					trans_table[msg.transaction_id].site_st = S_COMMIT;
				}
				break;
            case START_VOTE:
                txLive.insert (msg.transaction_id);
                transaction_info new_t;
                new_t.numVotes = -1;
                new_t.site_st = S_INIT;
                new_t.coordinator = msg.site_id;
				trans_table[msg.transaction_id] = new_t;
                Socket tmp;
                int will_abort = rand () % 10;
                if (will_abort < 3)
                {
                    Message vote_abort (VOTE_ABORT, msg.transaction_id,
                                site_id);
                    string snd_msg = vote_abort.createMessage ();
                    tmp.connect ("localhost", sites[msg.site_id]);
                    tmp.send (snd_msg);
                    trans_table[msg.transaction_id].site_st = S_ABORT;
                    txLive.erase (msg.transaction_id);
                }
                else
                {
                    Message vote_commit (VOTE_COMMIT, msg.transaction_id,
                                site_id);
                    string snd_msg = vote_commit.createMessage ();
                    tmp.connect ("localhost", sites[msg.site_id]);
                    tmp.send (snd_msg);
                    trans_table[msg.transaction_id].site_st = S_READY;
                }
                break;
            case PRE_COMMIT:
                Socket tmp;
                Message ack (ACK, msg.transaction_id, site_id);
                string snd_msg = ack.createMessage ();
                tmp.connect ("localhost", sites[msg.site_id]);
                tmp.send (snd_msg);
                trans_table[msg.transaction_id].site_st = S_PRE_COMMIT;
                break;
            case COMMIT:
                trans_table[msg.transaction_id].site_st = S_COMMIT;
                txLive.erase (msg.transaction_id);
                break;
            case ABORT:
                trans_table[msg.transaction_id].site_st = S_ABORT;
                txLive.erase (msg.transaction_id);
                break;

		}

	}
	return 0;
}
