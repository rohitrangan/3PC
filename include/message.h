/* message.h
 *
 * Functions to parse incoming messages and to create outgoing messages.
 * Our messages are of the form - "message_id,transaction_id,...".
 * The message_id field is defined as an enum below. transaction_id is an
 * integer with the ID of the transaction.
 */

#ifndef PC_MESSAGE_H_
#define PC_MESSAGE_H_

#include <string>

enum message_id
{
    TRANSACTION,
    START_VOTE,
    VOTE_COMMIT,
    VOTE_ABORT,
    PRE_COMMIT,
    COMMIT,
    ABORT,
    ACK
};

class Message
{
  public:
    message_id message;
    int transaction_id;
    string 

    /* Use this constructor to create a new message. */
    Message (message_id, int);
    /* Use this constructor to parse a message. */
    Message (string);
    /* Returns a message which you can send. */
    string createMessage ();

  private:
    void parseMessage ();
};

#endif  /* 3PC_MESSAGE_H_ */
