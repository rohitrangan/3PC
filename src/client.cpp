#include "../include/socket.h"
#include "../include/message.h"

#include <map>
#include <ctime>
#include <string>
#include <thread>
#include <chrono> 
#include <vector>
#include <cstring>
#include <cstdlib>

using namespace std;

int main(int argc, char* argv[])
{
    if (argc != 5)
    {
        cout << "Not enough parameters.\n";
        return 1;
    }

    int transactionId = stoi (string (argv[1]));
    int add_v = stoi (string (argv[2]));
    int sleep_time = stoi (string (argv[3]));
    double send_till = stod (string (argv[4]));
    int site_id = 1231;
    double time_elapsed = 0.0;
    int mod_sites = 3;
    map<int, int> sites;
    sites[0] = 6666;
    sites[1] = 6667;
    sites[2] = 6668;

    while ((int)time_elapsed < (int)send_till)
    {
        auto t1 = chrono::steady_clock::now ();
        Socket transaction_gen;
        Message msg (TRANSACTION,transactionId, site_id);
        string str = msg.createMessage();
        srand (time(NULL));
        if (mod_sites == 0)
        {
            cout << "Cannot connect to any server. Exiting.." << endl;
            exit (0);
        }
        int cur_site = rand() % mod_sites;
        cout << "Transaction ID: " << transactionId << ", Site ID :" <<cur_site;
        cout << ", message: " << str << endl;
        if (transaction_gen.connect("localhost", sites[cur_site]) < 0)
        {
            sites.erase (cur_site);
            --mod_sites;
            continue;
        }
        transaction_gen.send(str);
        transactionId += add_v;
        transaction_gen.close ();
        this_thread::sleep_for (chrono::milliseconds(sleep_time));
        auto t2 = chrono::steady_clock::now ();
        chrono::duration<double> time_span = chrono::\
                        duration_cast<chrono::duration<double>>(t2 - t1);
        time_elapsed += time_span.count ();
    }
    return 0;
}
