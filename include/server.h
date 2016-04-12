#ifndef PC_SERVER_H_
#define PC_SERVER_H_

#include "../include/socket.h"
#include "../include/message.h"

#include <cstdlib>
#include <cstring>
#include <vector>
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

bool termination_protocol (int, int, Socket, bool, vector<int>&, set<int>&,
        map<int, transaction_info>&);

#endif  /* PC_SERVER_H_ */
