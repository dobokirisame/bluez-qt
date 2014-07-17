#include "agentadaptor.h"
#include "agent.h"
#include "request.h"
#include "manager.h"
#include "adapter.h"
#include "device.h"
#include "loaddevicejob.h"

#include <QDBusConnection>
#include <QDBusObjectPath>

namespace QBluez
{

AgentAdaptor::AgentAdaptor(Agent *parent, Manager *manager)
    : QDBusAbstractAdaptor(parent)
    , m_agent(parent)
    , m_manager(manager)
{
}

QString AgentAdaptor::RequestPinCode(const QDBusObjectPath &device, const QDBusMessage &msg)
{
    msg.setDelayedReply(true);
    Request<QString> req(OrgBluezAgent, msg);

    Device *dev = m_manager->deviceForUbi(device.path());
    if (!dev) {
        req.cancel();
    } else {
        LoadDeviceJob *job = dev->load();
        job->start();
        connect(job, &LoadDeviceJob::result, [ this, req ](LoadDeviceJob *job) {
            Q_ASSERT(!job->error());
            if (job->error()) {
                req.cancel();
                return;
            }
            m_agent->requestPinCode(job->device(), req);
        });
    }

    return QString();
}

void AgentAdaptor::DisplayPinCode(const QDBusObjectPath &device, const QString &pincode)
{
    Device *dev = m_manager->deviceForUbi(device.path());
    if (dev) {
        LoadDeviceJob *job = dev->load();
        job->start();
        connect(job, &LoadDeviceJob::result, [ this, pincode ](LoadDeviceJob *job) {
            Q_ASSERT(!job->error());
            if (job->error()) {
                return;
            }
            m_agent->displayPinCode(job->device(), pincode);
        });
    }
}

quint32 AgentAdaptor::RequestPasskey(const QDBusObjectPath &device, const QDBusMessage &msg)
{
    msg.setDelayedReply(true);
    Request<quint32> req(OrgBluezAgent, msg);

    Device *dev = m_manager->deviceForUbi(device.path());
    if (!dev) {
        req.cancel();
    } else {
        LoadDeviceJob *job = dev->load();
        job->start();
        connect(job, &LoadDeviceJob::result, [ this, req ](LoadDeviceJob *job) {
            Q_ASSERT(!job->error());
            if (job->error()) {
                req.cancel();
                return;
            }
            m_agent->requestPasskey(job->device(), req);
        });
    }

    return 0;
}

void AgentAdaptor::DisplayPasskey(const QDBusObjectPath &device, quint32 passkey, quint8 entered)
{
    Device *dev = m_manager->deviceForUbi(device.path());
    if (dev) {
        LoadDeviceJob *job = dev->load();
        job->start();
        connect(job, &LoadDeviceJob::result, [ this, passkey, entered ](LoadDeviceJob *job) {
            Q_ASSERT(!job->error());
            if (job->error()) {
                return;
            }
            m_agent->displayPasskey(job->device(), passkeyToString(passkey), QString::number(entered));
        });
    }
}

void AgentAdaptor::RequestConfirmation(const QDBusObjectPath &device, quint32 passkey, const QDBusMessage &msg)
{
    msg.setDelayedReply(true);
    Request<> req(OrgBluezAgent, msg);

    Device *dev = m_manager->deviceForUbi(device.path());
    if (!dev) {
        req.cancel();
    } else {
        LoadDeviceJob *job = dev->load();
        job->start();
        connect(job, &LoadDeviceJob::result, [ this, passkey, req ](LoadDeviceJob *job) {
            Q_ASSERT(!job->error());
            if (job->error()) {
                req.cancel();
                return;
            }
            m_agent->requestConfirmation(job->device(), passkeyToString(passkey), req);
        });
    }
}

void AgentAdaptor::RequestAuthorization(const QDBusObjectPath &device, const QDBusMessage &msg)
{
    msg.setDelayedReply(true);
    Request<> req(OrgBluezAgent, msg);

    Device *dev = m_manager->deviceForUbi(device.path());
    if (!dev) {
        req.cancel();
    } else {
        LoadDeviceJob *job = dev->load();
        job->start();
        connect(job, &LoadDeviceJob::result, [ this, req ](LoadDeviceJob *job) {
            Q_ASSERT(!job->error());
            if (job->error()) {
                req.cancel();
                return;
            }
            m_agent->requestAuthorization(job->device(), req);
        });
    }
}

void AgentAdaptor::AuthorizeService(const QDBusObjectPath &device, const QString &uuid, const QDBusMessage &msg)
{
    msg.setDelayedReply(true);
    Request<> req(OrgBluezAgent, msg);

    Device *dev = m_manager->deviceForUbi(device.path());
    if (!dev) {
        req.cancel();
    } else {
        LoadDeviceJob *job = dev->load();
        job->start();
        connect(job, &LoadDeviceJob::result, [ this, uuid, req ](LoadDeviceJob *job) {
            Q_ASSERT(!job->error());
            if (job->error()) {
                req.cancel();
                return;
            }
            m_agent->authorizeService(job->device(), uuid, req);
        });
    }
}

void AgentAdaptor::Cancel()
{
    m_agent->cancel();
}

void AgentAdaptor::Release()
{
    m_agent->release();
}

QString AgentAdaptor::passkeyToString(quint32 passkey) const
{
    // Passkey will always be a 6-digit number, zero-pad it at the start
    return QString(QStringLiteral("%1")).arg(passkey, 6, 10, QLatin1Char('0'));
}

} // namespace QBluez
