/* message.h
 *
 * Functions to parse incoming messages and to create outgoing messages.
 * Our messages are of the form - "message_id,transaction_id,site_id,...".
 * The message_id field is defined as an enum below. transaction_id is an
 * integer with the ID of the transaction.
 */

#ifndef PC_MESSAGE_H_
#define PC_MESSAGE_H_

#include <vector>
#include <string>
#include <sstream>

#define DELIM   ','
#define NUM_MESSAGES    9

enum message_id
{
    TRANSACTION,
    START_VOTE,
    VOTE_COMMIT,
    VOTE_ABORT,
    PRE_COMMIT,
    COMMIT,
    ABORT,
    ACK,
    ERROR
};

extern std::string message_strings[NUM_MESSAGES];

/* Function to split strings. */
void split (const std::string&, char, std::vector<std::string>&);

/* Creates a message object. This can be used to send as well as create new
 * messages. Parsing messages is done automatically. All you need to do is
 * check the message field and the transaction field in order to obtain the
 * message.
 */
class Message
{
  public:
    message_id message;
    int transaction_id;
    int site_id;
    std::string message_str;

    /* Use this constructor to create a new message. */
    Message (message_id, int, int);
    /* Use this constructor to parse a message. */
    Message (std::string);
    /* Returns a message which you can send. */
    std::string createMessage ();

  private:
    void parseMessage ();
    message_id messageIndex (std::string);
};

#endif  /* 3PC_MESSAGE_H_ */
