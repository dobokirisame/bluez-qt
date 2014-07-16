#include "obexmanager.h"
#include "obexmanager_p.h"
#include "debug_p.h"
#include "pendingcall.h"
#include "obexagent.h"
#include "obexagentadaptor.h"

#include <QDBusServiceWatcher>

namespace QBluez
{

ObexManager::ObexManager(QObject *parent)
    : QObject(parent)
    , d(new ObexManagerPrivate(this))
{
}

ObexManager::~ObexManager()
{
    delete d;
}

InitObexManagerJob *ObexManager::init()
{
    return 0;
}

bool ObexManager::isInitialized() const
{
    return d->m_initialized;
}

bool ObexManager::isOperational() const
{
    return d->m_initialized && d->m_obexRunning && d->m_loaded;
}

PendingCall *ObexManager::registerAgent(ObexAgent *agent)
{
    if (!d->m_obexAgentManager) {
        return 0;
    }

    new ObexAgentAdaptor(agent, this);

    if (!QDBusConnection::sessionBus().registerObject(agent->objectPath().path(), agent)) {
        qCWarning(QBLUEZ) << "Cannot register object" << agent->objectPath().path();
    }

    return new PendingCall(d->m_obexAgentManager->RegisterAgent(agent->objectPath()),
                           PendingCall::ReturnVoid, this);
}

PendingCall *ObexManager::unregisterAgent(ObexAgent *agent)
{
    if (!d->m_obexAgentManager) {
        return 0;
    }

    return new PendingCall(d->m_obexAgentManager->UnregisterAgent(agent->objectPath()),
                           PendingCall::ReturnVoid, this);
}

PendingCall *ObexManager::createSession(const QString &destination, const QVariantMap &args)
{
    if (!d->m_obexClient) {
        return 0;
    }

    return new PendingCall(d->m_obexClient->CreateSession(destination, args),
                           PendingCall::ReturnObjectPath, this);
}

PendingCall *ObexManager::removeSession(const QDBusObjectPath &session)
{
    if (!d->m_obexClient) {
        return 0;
    }

    return new PendingCall(d->m_obexClient->RemoveSession(session),
                           PendingCall::ReturnVoid, this);
}

} // namespace QBluez

#include "obexmanager.moc"