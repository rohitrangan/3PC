#include "../include/server.h"

#include <csignal>

using namespace std;

Socket site;

void server_shutdown (int s)
{
    (void) s;
    cout << "\n";
    cout << "Initiating Server Shutdown...\n";
    cout << "Closing All Connections... ";
    site.close ();
    cout << "\nAll Connections Closed. Exiting...\n\n";
    exit (EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
    /* Signal handling to shutdown the ports and the server. */
    struct sigaction sigint_handler;
    sigint_handler.sa_handler = server_shutdown;
    sigemptyset (&sigint_handler.sa_mask);
    sigint_handler.sa_flags = 0;
    if (sigaction(SIGINT, &sigint_handler, NULL) < 0)
    {
        cerr << "ERROR! Cannot Start Signal Handler\n\n";
        return 1;
    }

    if (argc != 4)
    {
        cout << "Not enough parameters.\n";
        return 1;
    }

    int site_id = std::stoi (string (argv[1]));
    int tid_fail = std::stoi (string (argv[2]));
    int fail_loc = std::stoi (string (argv[3]));
    srand (time (NULL));
	//Socket site;
	std::map<int, int> sites;
	sites[0] = 6666;
	sites[1] = 6667;
	sites[2] = 6668;
	site.bind (sites[site_id]);
	set<int> txLive;
	map<int,transaction_info> trans_table;
	site.listen(100);
	cout << "Server " << site_id << " Up" << endl;
	while(1)
	{
		Socket inc = site.accept();
		string str;
        str = inc.recv ((size_t) 1024);
        //cout << "Received str:- " << str << endl;
		Message msg(str);
        string tempStr, snd_msg;
        transaction_info new_t;
		switch(msg.message)
		{
			case TRANSACTION:
            {
                new_t.numVotes = 1;
                new_t.site_st = S_INIT;
                new_t.coordinator = site_id;
				trans_table[msg.transaction_id] = new_t;
                cout << "Site " << site_id << " accepted transaction " <<
                    msg.transaction_id << endl;
				Message tempMsg(START_VOTE,msg.transaction_id, site_id);
				tempStr = tempMsg.createMessage();
				txLive.insert(msg.transaction_id);
				for(int i = 0; i < sites.size();i++)
				{
                    Socket temp;
                    if (i != site_id)
                    {
					    if (temp.connect("localhost", sites[i]) < 0)
                        {
                            cout << "Site " << site_id << " timed out in State "
                                << "INITIAL" << endl;
                            termination_protocol (site_id, i, site, true, sites,
                                    txLive, trans_table);
                            cout << "Site " << site_id << " ended TERMINATION"
                                << endl;
                            sites.erase (i);
                            break;
                        }
					    temp.send(tempStr);
                        cout << "[" << msg.transaction_id << "]Site " << site_id
                            << " sent START_VOTE to Site " << i << endl;
                    }
				}
                trans_table[msg.transaction_id].site_st = S_WAIT;
                if ((fail_loc == 2) && (msg.transaction_id > tid_fail))
                {
                    cout << "[" << msg.transaction_id << "]Site " << site_id
                        << " is FAILING" << endl;
                    exit (1);
                }
				break;
            }
			case VOTE_COMMIT:
            {
				trans_table[msg.transaction_id].numVotes++;
                cout << "[" << msg.transaction_id << "]Site " << site_id
                    << " got VOTE_COMMIT from Site " << msg.site_id << endl;
				if(trans_table[msg.transaction_id].numVotes==sites.size())
				{
                    cout << "[" << msg.transaction_id << "]Site " << site_id <<
                        " has all vote commits." << endl;
					Message tempMsg(PRE_COMMIT,msg.transaction_id, site_id);
					tempStr = tempMsg.createMessage();
					for(int i = 0; i < sites.size();i++)
					{
                        Socket temp;
                        if (i != site_id)
                        {
						    if (temp.connect("localhost", sites[i]) < 0)
                            {
                                cout << "Site " << site_id << " timed out in "
                                    << "State WAIT" << endl;
                                termination_protocol (site_id, i, site, true,
                                        sites, txLive, trans_table);
                                cout << "Site " << site_id << " ended "
                                    "TERMINATION" << endl;
                                sites.erase (i);
                                break;
                            }
                            temp.send(tempStr);
                            cout << "[" << msg.transaction_id << "]Site " <<
                                site_id << " sent PRE_COMMIT to Site " << i <<
                                endl;
                        }
					}
					trans_table[msg.transaction_id].site_st = S_PRE_COMMIT;
					trans_table[msg.transaction_id].numVotes = 1;
				}
				break;
            }
			case VOTE_ABORT:
            {
                if(txLive.find(msg.transaction_id) != txLive.end())
                {
                    Message tempMsg(ABORT,msg.transaction_id, site_id);
                    tempStr = tempMsg.createMessage();
                    cout << "[" << msg.transaction_id << "]Site " << site_id
                        << " got VOTE_ABORT from Site " << msg.site_id << endl;
                    for(int i = 0; i < sites.size();i++)
                    {
                        Socket temp;
                        if (i != site_id)
                        {
                            if (temp.connect("localhost", sites[i]) < 0)
                            {
                                cout << "Site " << site_id << " timed out in "
                                    << "State WAIT" << endl;
                                termination_protocol (site_id, i, site, true,
                                        sites, txLive, trans_table);
                                cout << "Site " << site_id << " ended "
                                    << "TERMINATION" << endl;
                                sites.erase (i);
                                break;
                            }
                            temp.send(tempStr);
                            cout << "[" << msg.transaction_id << "]Site " <<
                                site_id << " sent ABORT to Site " << i <<
                                endl;
                        }
                    }
                    txLive.erase(msg.transaction_id);
                    trans_table[msg.transaction_id].site_st = S_ABORT;
                    cout << "[" << msg.transaction_id << "]Site " << site_id
                            << " ABORTED" << "\n\n\n";
                }
				break;
            }
			case ACK:
            {
                cout << "[" << msg.transaction_id << "]Site " << site_id
                    << " got ACK from Site " << msg.site_id << endl;
				trans_table[msg.transaction_id].numVotes++;
				if(trans_table[msg.transaction_id].numVotes==sites.size())
				{
					Message tempMsg(COMMIT, msg.transaction_id, site_id);
					string tempStr = tempMsg.createMessage();
					for(int i = 0; i < sites.size();i++)
					{
                        Socket temp;
                        if (i != site_id)
                        {
                            if (temp.connect("localhost", sites[i]) < 0)
                            {
                                cout << "Site " << site_id << " timed out in "
                                    << "State PRE_COMMIT" << endl;
                                termination_protocol (site_id, i, site, true,
                                        sites, txLive, trans_table);
                                cout << "Site " << site_id << " ended "
                                    "TERMINATION" << endl;
                                sites.erase (i);
                                break;
                            }
                            temp.send(tempStr);
                            cout << "[" << msg.transaction_id << "]Site " <<
                            site_id << " sent COMMIT to Site " << i <<
                            endl;
                        }
					}
					trans_table[msg.transaction_id].site_st = S_COMMIT;
                    cout << "[" << msg.transaction_id << "]Site " << site_id
                        << " COMMITTED" << "\n\n\n";
                    txLive.erase (msg.transaction_id);
				}
				break;
            }
            case START_VOTE:
            {
                cout << "[" << msg.transaction_id << "]Site " << site_id
                    << " got START_VOTE from Site " << msg.site_id << endl;
                if ((fail_loc == 0) && (msg.transaction_id > tid_fail))
                {
                    cout << "[" << msg.transaction_id << "]Site " << site_id
                        << " is FAILING" << endl;
                    exit (1);
                }
                txLive.insert (msg.transaction_id);
                new_t.numVotes = -1;
                new_t.site_st = S_INIT;
                new_t.coordinator = msg.site_id;
				trans_table[msg.transaction_id] = new_t;
                Socket tmp;
                int will_abort = rand () % ABORT_DENOM;
                if (will_abort < ABORT_NUM)
                {
                    Message vote_abort (VOTE_ABORT, msg.transaction_id,
                                site_id);
                    snd_msg = vote_abort.createMessage ();
                    if (tmp.connect("localhost", sites[msg.site_id]) < 0)
                    {
                        cout << "Site " << site_id << " timed out in "
                            << "State INITIAL" << endl;
                        termination_protocol (site_id, msg.site_id, site, true,
                                sites, txLive, trans_table);
                        cout << "Site " << site_id << " ended "
                            "TERMINATION" << endl;
                        sites.erase (msg.site_id);
                        break;
                    }
                    tmp.send (snd_msg);
                    trans_table[msg.transaction_id].site_st = S_ABORT;
                    cout << "[" << msg.transaction_id << "]Site " << site_id
                        << " sent VOTE_ABORT to Site " << msg.site_id << endl;
                    cout << "[" << msg.transaction_id << "]Site " << site_id
                        << " ABORTED." << "\n\n\n";
                    txLive.erase (msg.transaction_id);
                }
                else
                {
                    Message vote_commit (VOTE_COMMIT, msg.transaction_id,
                                site_id);
                    snd_msg = vote_commit.createMessage ();
                    if (tmp.connect("localhost", sites[msg.site_id]) < 0)
                    {
                        cout << "Site " << site_id << " timed out in "
                            << "State INITIAL" << endl;
                        termination_protocol (site_id, msg.site_id, site, true,
                                sites, txLive, trans_table);
                        cout << "Site " << site_id << " ended "
                            "TERMINATION" << endl;
                        sites.erase (msg.site_id);
                        break;
                    }
                    tmp.send (snd_msg);
                    cout << "[" << msg.transaction_id << "]Site " << site_id
                        << " sent VOTE_COMMIT to Site " << msg.site_id << endl;
                    trans_table[msg.transaction_id].site_st = S_READY;
                }
                break;
            }
            case PRE_COMMIT:
            {
                Socket tmp;
                cout << "[" << msg.transaction_id << "]Site " << site_id
                    << " got PRE_COMMIT from Site " << msg.site_id << endl;
                if ((fail_loc == 1) && (msg.transaction_id > tid_fail))
                {
                    cout << "[" << msg.transaction_id << "]Site " << site_id
                        << " is FAILING" << endl;
                    exit (1);
                }
                Message ack (ACK, msg.transaction_id, site_id);
                snd_msg = ack.createMessage ();
                if (tmp.connect("localhost", sites[msg.site_id]) < 0)
                {
                    cout << "Site " << site_id << " timed out in "
                        << "State READY" << endl;
                    termination_protocol (site_id, msg.site_id, site, true,
                            sites, txLive, trans_table);
                    cout << "Site " << site_id << " ended "
                        "TERMINATION" << endl;
                    sites.erase (msg.site_id);
                    break;
                }
                tmp.send (snd_msg);
                cout << "[" << msg.transaction_id << "]Site " << site_id
                    << " sent ACK to Site " << msg.site_id << endl;
                trans_table[msg.transaction_id].site_st = S_PRE_COMMIT;
                break;
            }
            case COMMIT:
            {
                if(txLive.find(msg.transaction_id) != txLive.end())
                {
                    cout << "[" << msg.transaction_id << "]Site " << site_id
                        << " got COMMIT from Site " << msg.site_id << endl;
                    trans_table[msg.transaction_id].site_st = S_COMMIT;
                    cout << "[" << msg.transaction_id << "]Site " << site_id
                        << " COMMITTED" << "\n\n\n";
                    txLive.erase (msg.transaction_id);
                }
                break;
            }
            case ABORT:
            {
                if(txLive.find(msg.transaction_id) != txLive.end())
                {
                    cout << "[" << msg.transaction_id << "]Site " << site_id
                        << " got ABORT from Site " << msg.site_id << endl;
                    trans_table[msg.transaction_id].site_st = S_ABORT;
                    cout << "[" << msg.transaction_id << "]Site " << site_id
                        << " ABORTED" << "\n\n\n";
                    txLive.erase (msg.transaction_id);
                }
                break;
            }
            case TERMINATION:
            {
                cout << "Site " << site_id << " got TERMINATION" << endl;
                Socket tmp;
                Message ack (ACK, msg.transaction_id, site_id);
                snd_msg = ack.createMessage ();
                if (tmp.connect ("localhost", sites[msg.site_id]) < 0)
                {
                    cout << "COULD NOT CONNECT!!!!\n";
                    cout << "Error number = " << errno << endl;
                }
                tmp.send (snd_msg);
                termination_protocol (site_id, msg.failed_id, site, false,
                        sites, txLive, trans_table);
                cout << "Site " << site_id << " finished TERMINATION" << endl;
                sites.erase (msg.failed_id);
                break;
            }
            default:
                return -1;

		}

	}
	return 0;
}
