#include "../include/socket.h"
#include "../include/message.h"

#include <thread>
#include <chrono> 
#include <cstring>
#include <vector>
#include <ctime>
#include <cstdlib>

using namespace std;

int main()
{
    int site_id = 1231;
    std::vector<int> sites(3);
    sites[0] = 6666;
    sites[1] = 6667;
    sites[2] = 6668;
    int transactionId =1;
    while(transactionId!=10)
    {
        Socket transaction_gen;
        Message msg (TRANSACTION,transactionId, site_id);
        string str = msg.createMessage();
        srand (time(NULL));
        int cur_site = rand() % 3;
        cout << "Transaction ID: " << transactionId << ", Site ID :" <<cur_site;
        cout << ", message: " << str << endl;
        transaction_gen.connect("localhost", sites[cur_site]);
        transaction_gen.send(str);
        ++transactionId;
        transaction_gen.close ();
        std::this_thread::sleep_for (std::chrono::seconds(1));
    }
    return 0;
}
