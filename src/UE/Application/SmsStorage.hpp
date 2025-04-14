#pragma once

#include "Messages/PhoneNumber.hpp"
#include <string>
#include <vector>

namespace ue
{

    struct SmsMessage
    {
        common::PhoneNumber sender;
        std::string text;
        bool isRead;

        SmsMessage(common::PhoneNumber sender, const std::string& text, bool isRead = false)
            : sender(sender), text(text), isRead(isRead)
        {}
    };

    class SmsStorage
    {
    private:
        std::vector<SmsMessage> messages;

    public:
        SmsStorage() = default;

        std::size_t addReceivedSms(common::PhoneNumber sender, const std::string& text)
        {
            messages.emplace_back(sender, text, false);
            return messages.size() - 1;
        }

        std::size_t addSentSms(common::PhoneNumber recipient, const std::string& text)
        {
            messages.emplace_back(recipient, text, true);
            return messages.size() - 1;
        }

        const std::vector<SmsMessage>& getAllMessages() const
        {
            return messages;
        }

        std::size_t getUnreadCount() const
        {
            std::size_t count = 0;
            for (const auto &msg : messages)
            {
                if (!msg.isRead)
                    ++count;
            }
            return count;
        }

        bool markAsRead(std::size_t idx)
        {
            if (idx >= messages.size())
                return false;
            messages[idx].isRead = true;
            return true;
        }
    };

} // namespace ue
