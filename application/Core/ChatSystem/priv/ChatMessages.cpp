#include <priv/ChatMessages.h>
using namespace CS;

#include <QSet>

#include <Common/Constants.h>
#include <Common/PersistentData.h>
#include <Common/Settings.h>

#include <priv/Log.h>

ChatMessages::ChatMessages() :
   MAX_NUMBER_OF_STORED_CHAT_MESSAGES(SETTINGS.get<quint32>("max_number_of_stored_chat_messages")),
   changed(false)
{}

QSharedPointer<ChatMessage> ChatMessages::add(const QString& message, const Common::Hash& ownerID, const QString& ownerNick)
{
   QSharedPointer<ChatMessage> mess = QSharedPointer<ChatMessage>(new ChatMessage(message, ownerID, ownerNick));

   this->insert(QList<QSharedPointer<ChatMessage>> { mess });

   return mess;
}

QList<QSharedPointer<ChatMessage> > ChatMessages::add(const Protos::Common::ChatMessages& chatMessages)
{
   QList<QSharedPointer<ChatMessage>> messages;

   for (int i = 0; i < chatMessages.message_size(); i++)
      messages << QSharedPointer<ChatMessage>(new ChatMessage(chatMessages.message(i)));

   return this->insert(messages);
}

QList<quint64> ChatMessages::getLastMessageIDs(int n) const
{
   QList<quint64> result;

   QListIterator<QSharedPointer<ChatMessage>> i(messages);
   i.toBack();
   while (i.hasPrevious())
      result << i.previous()->getID();

   return result;
}

/**
  * The returned messages are sorted from oldest to youngest.
  */
QList<QSharedPointer<ChatMessage>> ChatMessages::getUnknownMessage(const Protos::Core::GetLastChatMessages& getLastChatMessage)
{
   QSet<quint64> knownIDs;
   for (int i = 0; i < getLastChatMessage.message_id_size(); i++)
      knownIDs.insert(getLastChatMessage.message_id(i));

   QList<QSharedPointer<ChatMessage>> result;
   for (int i = this->messages.size() - 1; i >= 0 && this->messages.size() - i <= int(getLastChatMessage.number()); i--)
   {
      if (!knownIDs.contains(this->messages[i]->getID()))
         result.prepend(this->messages[i]);
   }

   return result;
}

void ChatMessages::fillProtoChatMessages(Protos::Common::ChatMessages& chatMessages, int number) const
{
   int i = number > this->messages.size() ? 0 : this->messages.size() - number;
   while (i < this->messages.size())
      this->messages[i++]->fillProtoChatMessage(*chatMessages.add_message());
}

void ChatMessages::fillProtoChatMessages(Protos::Common::ChatMessages& chatMessages, const QList<QSharedPointer<ChatMessage>>& messages)
{
   for (QListIterator<QSharedPointer<ChatMessage>> i(messages); i.hasNext();)
      i.next()->fillProtoChatMessage(*chatMessages.add_message());
}


/**
  * Load the chat messages from the file and return it.
  */
void ChatMessages::loadFromFile()
{
   try
   {
      Protos::Common::ChatMessages chatMessages;
      Common::PersistentData::getValue(Common::Constants::FILE_CHAT_MESSAGES, chatMessages, Common::Global::DataFolderType::LOCAL);
      this->add(chatMessages);
      this->changed = false;
   }
   catch (Common::UnknownValueException& e)
   {
      L_WARN(QString("The saved chat messages cannot be retrived (the file doesn't exist) : %1").arg(Common::Constants::FILE_CHAT_MESSAGES));
   }
   catch (...)
   {
      L_WARN(QString("The saved chat messages cannot be retrived (Unkown exception) : %1").arg(Common::Constants::FILE_CHAT_MESSAGES));
   }
}

void ChatMessages::saveToFile() const
{
   if (!this->changed)
      return;

   try
   {
      Protos::Common::ChatMessages chatMessages;
      this->fillProtoChatMessages(chatMessages);
      Common::PersistentData::setValue(Common::Constants::FILE_CHAT_MESSAGES, chatMessages, Common::Global::DataFolderType::LOCAL);
      this->changed = false;
   }
   catch (Common::PersistentDataIOException& err)
   {
      L_ERRO(err.message);
   }
}

/**
  * We return the inserted messages. A message will not be inserted if:
  *  - The messages size is equal to 'MAX_NUMBER_OF_STORED_CHAT_MESSAGES' and the message to insert is older than the oldest message in the list.
  *  - The message is already in the list.
  */
QList<QSharedPointer<ChatMessage>> ChatMessages::insert(const QList<QSharedPointer<ChatMessage>>& messages)
{
   QList<QSharedPointer<ChatMessage>> insertedMessages;

   QListIterator<QSharedPointer<ChatMessage>> i(messages);
   i.toBack();

   int j = this->messages.size();
   while (i.hasPrevious())
   {
      const QSharedPointer<ChatMessage>& mess = i.previous();

      if (this->messageIDs.contains(mess->getID()))
         continue;

      while (j > 0 && this->messages[j-1]->getTime() > mess->getTime())
         j--;      

      if (this->messages.size() != MAX_NUMBER_OF_STORED_CHAT_MESSAGES || j != 0) // We avoid to insert a message which will ne deleted right after.
      {
         insertedMessages.prepend(mess);
         this->messageIDs.remove(mess->getID());
         this->messages.insert(j, mess);
      }
   }

   if (this->messages.size() > MAX_NUMBER_OF_STORED_CHAT_MESSAGES)
   {
      const auto begin = this->messages.begin();
      const auto end = this->messages.begin() + (this->messages.size() - MAX_NUMBER_OF_STORED_CHAT_MESSAGES);
      for (auto m = begin; m != end; m++)
         this->messageIDs.remove((*m)->getID());
      this->messages.erase(begin, end);
   }

   this->changed = !insertedMessages.isEmpty();

   return insertedMessages;
}