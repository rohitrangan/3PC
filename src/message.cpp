/* message.cpp
 *
 * Functions to parse incoming messages and to create outgoing messages.
 */

#include "../include/message.h"

#include <iostream>
#include <cstring>
std::string message_strings[NUM_MESSAGES] = {"TRANSACTION", "START_VOTE",
    "VOTE_COMMIT", "VOTE_ABORT", "PRE_COMMIT", "COMMIT", "ABORT", "ACK",
    "TERMINATION", "ERROR"
};

void split(const std::string& line, char delim, std::vector<std::string>& val) {
    std::string::size_type i = 0;
    std::string::size_type j = line.find (delim);

    while (j != std::string::npos)
    {
        val.push_back (line.substr (i, j-i));
        i = ++j;
        j = line.find (delim, j);

        if (j == std::string::npos)
            val.push_back (line.substr (i, line.length ()));
    }
}

Message::Message (message_id m_id, int t_id, int s_id) : message (m_id),
            transaction_id (t_id), site_id (s_id), failed_id (0)
{
}

Message::Message (message_id m_id, int t_id, int s_id, int f_id) :
            message (m_id), transaction_id (t_id), site_id (s_id),
            failed_id (f_id)
{
}

Message::Message (std::string m_str) : message_str (m_str)
{
    parseMessage ();
}

std::string Message::createMessage ()
{
    std::stringstream s;
    s << message_strings[message] << DELIM << transaction_id;
    s << DELIM << site_id << DELIM << failed_id;
    return s.str ();
}

message_id Message::messageIndex (std::string message_str)
{
    for (int i = 0; i < NUM_MESSAGES; ++i)
    {
        if (message_strings[i] == message_str)
            return (message_id) i;
    }
    return ERROR;
}

void Message::parseMessage ()
{
    std::vector<std::string> m_parsed;
    split (message_str, DELIM, m_parsed);
    this->message = messageIndex (m_parsed[0]);
    this->transaction_id = std::stoi (m_parsed[1]);
    this->site_id = std::stoi (m_parsed[2]);
    this->failed_id = std::stoi (m_parsed[3]);
}
