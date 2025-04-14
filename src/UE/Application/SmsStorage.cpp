#include "SmsStorage.hpp"

namespace ue
{

    std::size_t SmsStorage::addReceivedSms(common::PhoneNumber sender, const std::string &text)
    {
        messages.emplace_back(sender, text, false);
        return messages.size() - 1;
    }

    std::size_t SmsStorage::addSentSms(common::PhoneNumber recipient, const std::string &text)
    {
        messages.emplace_back(recipient, text, true);
        return messages.size() - 1;
    }

    const std::vector<SmsMessage>& SmsStorage::getAllMessages() const
    {
        return messages;
    }

    std::size_t SmsStorage::getUnreadCount() const
    {
        std::size_t count = 0;
        for (const auto &msg : messages)
        {
            if (!msg.isRead)
                ++count;
        }
        return count;
    }

    bool SmsStorage::markAsRead(std::size_t idx)
    {
        if (idx >= messages.size())
            return false;
        messages[idx].isRead = true;
        return true;
    }


} // namespace ue
