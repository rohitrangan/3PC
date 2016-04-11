/* message.cpp
 *
 * Functions to parse incoming messages and to create outgoing messages.
 */

Message::Message (message_id m_id, int t_id) : message (m_id),
            transaction_id (t_id)
{
}

Message::Message (string m_str) : message_str (m_str)
{
    parseMessage ();
}

string Message::createMessage ()
{

}
