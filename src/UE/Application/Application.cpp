#include "Application.hpp"
#include "States/NotConnectedState.hpp"
#include "States/ConnectedState.hpp"

namespace ue
{

Application::Application(common::PhoneNumber phoneNumber,
                         common::ILogger &iLogger,
                         IBtsPort &bts,
                         IUserPort &user,
                         ITimerPort &timer)
    : context{iLogger, bts, user, timer},
      logger(iLogger, "[APP] ")
{
    logger.logInfo("Application started with phone number: ", phoneNumber);
    context.setState<NotConnectedState>();
}

Application::~Application()
{
    logger.logInfo("Application stopped");
}

void Application::handleTimeout()
{
    if (context.state)
        context.state->handleTimeout();
}

void Application::handleSib(common::BtsId btsId)
{
    if (context.state)
        context.state->handleSib(btsId);
}

void Application::handleAttachAccept()
{
    if (context.state)
        context.state->handleAttachAccept();
}

void Application::handleAttachReject()
{
    if (context.state)
        context.state->handleAttachReject();
}

void Application::handleDisconnected()
{
    logger.logInfo("Transport disconnected");
    if (context.state)
        context.state->handleDisconnected();
}

void Application::handleSmsReceive(common::PhoneNumber sender, std::string text)
{
    logger.logInfo("SMS received from: ", sender);
    if (context.state)
        context.state->handleSmsReceive(sender, text);
}

void Application::handleSmsSentResult(common::PhoneNumber to, bool success)
{
    logger.logInfo("SMS sent result for: ", to, ", success: ", success);
    if (context.state)
        context.state->handleSmsSentResult(to, success);
}

void Application::handleSmsComposeResult(common::PhoneNumber recipient, const std::string &text)
{
    context.smsStorage.addMessage(recipient, text);
    context.bts.sendSms(recipient, text);
    context.setState<ConnectedState>();
}

void Application::handleUserAction(std::optional<std::size_t> selectedIndex)
{
    if (context.state)
        context.state->handleUserAction(selectedIndex);
}

void Application::handleUserBack()
{
    if (context.state)
        context.state->handleUserBack();
}


}
