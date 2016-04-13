#include "../include/server.h"

using namespace std;

bool termination_protocol (int site_id, int failed_site, Socket& site, bool b,
        vector<int>& sites, set<int>& txLive,
        map<int, transaction_info>& trans_table)
{
    int new_coordinator = (failed_site != 0) ? 0 : 1;
    bool flag = false;
    int numAcks = 0;
    cout << "Site " << site_id << " entered termination protocol" << endl;

    if (b)
    {
        Message start_term (TERMINATION, 0, site_id, failed_site);
        string msg_start_term = start_term.createMessage ();
        for (int i = 0; i < sites.size (); ++i)
        {
            Socket tmp;
            if (i != site_id && i != failed_site)
            {
                tmp.connect ("localhost", sites[i]);
                tmp.send (msg_start_term);
                cout << "Site " << site_id
                    << " sent TERMINATION to Site " << i << endl;
            }
        }
        while (1)
        {
            Socket inc = site.accept ();
            string str;
            str = inc.recv ((size_t) 1024);
            //cout << "Received str:- " << str << endl;
            Message msg (str);
            string tempStr, snd_msg;
            switch (msg.message)
            {
                case TERMINATION:
                {
                    cout << "Site " << site_id << " received TERMINATION from "
                        "Site " << msg.site_id << endl;
                    failed_site = msg.failed_id;
                    Message tempMsg(ACK, msg.transaction_id, site_id);
                    tempStr = tempMsg.createMessage();
                    Socket temp;
                    temp.connect ("localhost", sites[msg.site_id]);
                    temp.send (tempStr);
                    cout << "Site " << site_id << " sent ACK to Site " <<
                        msg.site_id << endl;
                    break;
                }
                case ACK:
                {
                    cout << "Site " << site_id << " got ACK from Site " <<
                        msg.site_id << endl;
                    ++numAcks;
                    if (numAcks == (sites.size () - 2))
                    {
                        flag = true;
                    }
                    break;
                }
                default:
                    break;
            }
            if (flag)
                break;
        }
    }

    for (set<int>::iterator it = txLive.begin (); it != txLive.end (); )
    {
        int t_chkid = *it;
        /* Handling the case for transactions being its own and if it is the new
         * coordinator.
         */
        if ((trans_table[t_chkid].coordinator == site_id) ||
                ((trans_table[t_chkid].coordinator == failed_site) &&
                 (new_coordinator == site_id)))
        {
            cout << "handling transaction: " << t_chkid << endl;
            switch (trans_table[t_chkid].site_st)
            {
                case S_INIT:

                case S_READY:

                case S_WAIT:
                {
                    cout << "[" << t_chkid << "]Site " << site_id <<
                        " is aborting transaction" << endl;
                    Message snd_msg_all (ABORT, t_chkid, site_id);
                    string tmp_msg = snd_msg_all.createMessage ();
                    for (int i = 0; i < sites.size (); ++i)
                    {
                        Socket tmp;
                        if (i != site_id && i != failed_site)
                        {
                            tmp.connect ("localhost", sites[i]);
                            tmp.send (tmp_msg);
                            cout << "[" << t_chkid << "]Site " << site_id
                                << " sent ABORT to Site " << i << endl;
                        }
                    }
                    trans_table[t_chkid].site_st = S_ABORT;
                    cout << "[" << t_chkid << "]Site " << site_id
                        << " ABORTED" << "\n\n\n";
                    it = txLive.erase (it);
                    break;
                }
                case S_PRE_COMMIT:
                {
                    cout << "[" << t_chkid << "]Site " << site_id <<
                        " is going to commit transaction" << endl;
                    Message snd_msg_all (COMMIT, t_chkid, site_id);
                    string tmp_msg = snd_msg_all.createMessage ();
                    for (int i = 0; i < sites.size (); ++i)
                    {
                        Socket tmp;
                        if (i != site_id && i != failed_site)
                        {
                            tmp.connect ("localhost", sites[i]);
                            tmp.send (tmp_msg);
                            cout << "[" << t_chkid << "]Site " << site_id
                                << " sent COMMIT to Site " << i << endl;
                        }
                    }
                    trans_table[t_chkid].site_st = S_COMMIT;
                    cout << "[" << t_chkid << "]Site " << site_id
                        << " COMMITTED" << "\n\n\n";
                    it = txLive.erase (it);
                    break;
                }
                case S_ABORT:
                {
                    cout << "[" << t_chkid << "]Site " << site_id <<
                        " is aborting transaction" << endl;
                    Message snd_msg_all (ABORT, t_chkid, site_id);
                    string tmp_msg = snd_msg_all.createMessage ();
                    for (int i = 0; i < sites.size (); ++i)
                    {
                        Socket tmp;
                        if (i != site_id && i != failed_site)
                        {
                            tmp.connect ("localhost", sites[i]);
                            tmp.send (tmp_msg);
                            cout << "[" << t_chkid << "]Site " << site_id
                                << " sent ABORT to Site " << i << endl;
                        }
                    }
                    trans_table[t_chkid].site_st = S_ABORT;
                    cout << "[" << t_chkid << "]Site " << site_id
                        << " ABORTED" << "\n\n\n";
                    it = txLive.erase (it);
                    break;
                }
                default:
                    cout << "State is:- " << trans_table[t_chkid].site_st << endl;
                    ++it;
                    break;
            }
        }
        else
            ++it;
    }

    flag = false;
    while (1)
    {
        Socket inc = site.accept ();
		string str;
        str = inc.recv ((size_t) 1024);
        //cout << "Received str:- " << str << endl;
		Message msg (str);
        string tempStr, snd_msg;
		switch (msg.message)
		{
			case COMMIT:
            {
                cout << "[" << msg.transaction_id << "]Site " << site_id <<
                    " received COMMIT from Site " << msg.site_id << endl;
                trans_table[msg.transaction_id].site_st = S_COMMIT;
                cout << "[" << msg.transaction_id << "]Site " << site_id
                    << " COMMITTED" << "\n\n\n";
                txLive.erase (msg.transaction_id);
                if (txLive.empty ())
                    flag = true;
				break;
            }
			case ABORT:
            {
                cout << "[" << msg.transaction_id << "]Site " << site_id <<
                    " received ABORT from Site " << msg.site_id << endl;
                trans_table[msg.transaction_id].site_st = S_ABORT;
                cout << "[" << msg.transaction_id << "]Site " << site_id
                    << " ABORTED" << "\n\n\n";
                txLive.erase (msg.transaction_id);
                if (txLive.empty ())
                    flag = true;
				break;
            }
            default:
                break;
		}
        if (flag)
            break;
    }

	return false;
}
