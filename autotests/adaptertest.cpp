#include "adaptertest.h"
#include "autotests.h"
#include "pendingcall.h"
#include "initmanagerjob.h"

#include <QtTest/QTest>
#include <QtTest/QSignalSpy>
#include <QDebug>

using namespace QBluez;

void AdapterTest::initTestCase()
{
    m_manager = new Manager();
    InitManagerJob *initJob = m_manager->init();
    initJob->exec();
    QVERIFY(!initJob->error());

    Q_FOREACH (Adapter *adapter, m_manager->adapters()) {
        QVERIFY(!adapter->ubi().isEmpty());

        AdapterUnit u;
        u.adapter = adapter;
        u.dbusAdapter = new org::bluez::Adapter1(QStringLiteral("org.bluez"),
                                                 adapter->ubi(),
                                                 QDBusConnection::systemBus(),
                                                 this);
        u.dbusProperties = new org::freedesktop::DBus::Properties(QStringLiteral("org.bluez"),
                                                                  adapter->ubi(),
                                                                  QDBusConnection::systemBus(),
                                                                  this);
        m_units.append(u);
    }

    qDebug() << "Got" << m_units.count() << "adapters.";
}

void AdapterTest::cleanupTestCase()
{
    Q_FOREACH (const AdapterUnit &unit, m_units) {
        delete unit.dbusAdapter;
        delete unit.dbusProperties;
    }

    delete m_manager;
}

static void compareUuids(const QStringList &actual, const QStringList &expected)
{
    QCOMPARE(actual.size(), expected.size());

    for (int i = 0; i < actual.size(); ++i) {
        QCOMPARE(actual.at(i).toUpper(), expected.at(i).toUpper());
    }
}

void AdapterTest::getPropertiesTest()
{
    Q_FOREACH (const AdapterUnit &unit, m_units) {
        QCOMPARE(unit.adapter->ubi(), unit.dbusAdapter->path());
        QCOMPARE(unit.adapter->address(), unit.dbusAdapter->address());
        QCOMPARE(unit.adapter->name(), unit.dbusAdapter->name());
        QCOMPARE(unit.adapter->alias(), unit.dbusAdapter->alias());
        QCOMPARE(unit.adapter->adapterClass(), unit.dbusAdapter->adapterClass());
        QCOMPARE(unit.adapter->isPowered(), unit.dbusAdapter->powered());
        QCOMPARE(unit.adapter->isDiscoverable(), unit.dbusAdapter->discoverable());
        QCOMPARE(unit.adapter->discoverableTimeout(), unit.dbusAdapter->discoverableTimeout());
        QCOMPARE(unit.adapter->isPairable(), unit.dbusAdapter->pairable());
        QCOMPARE(unit.adapter->pairableTimeout(), unit.dbusAdapter->pairableTimeout());
        QCOMPARE(unit.adapter->isDiscovering(), unit.dbusAdapter->discovering());
        QCOMPARE(unit.adapter->modalias(), unit.dbusAdapter->modalias());

        compareUuids(unit.adapter->uuids(), unit.dbusAdapter->uUIDs());
    }
}

void AdapterTest::setAliasTest()
{
    Q_FOREACH (const AdapterUnit &unit, m_units) {
        QSignalSpy adapterSpy(unit.adapter, SIGNAL(aliasChanged(QString)));
        QSignalSpy dbusSpy(unit.dbusProperties, SIGNAL(PropertiesChanged(QString,QVariantMap,QStringList)));

        QString originalValue = unit.adapter->alias();
        QString value = originalValue + QLatin1String("_tst_alias");

        unit.adapter->setAlias(value);
        adapterSpy.wait();
        QCOMPARE(adapterSpy.count(), 1);

        QList<QVariant> adapterArguments = adapterSpy.takeFirst();
        QCOMPARE(adapterArguments.at(0).toString(), value);
        verifyPropertiesChangedSignal(dbusSpy, QStringLiteral("Alias"), value);

        QCOMPARE(unit.adapter->alias(), value);
        QCOMPARE(unit.dbusAdapter->alias(), value);

        unit.adapter->setAlias(originalValue)->waitForFinished();
    }
}

void AdapterTest::setPoweredTest()
{
    Q_FOREACH (const AdapterUnit &unit, m_units) {
        QSignalSpy adapterSpy(unit.adapter, SIGNAL(poweredChanged(bool)));
        QSignalSpy dbusSpy(unit.dbusProperties, SIGNAL(PropertiesChanged(QString,QVariantMap,QStringList)));

        bool originalValue = unit.adapter->isPowered();
        bool value = !originalValue;

        unit.adapter->setPowered(value);
        adapterSpy.wait();
        QCOMPARE(adapterSpy.count(), 1);

        QVERIFY(dbusSpy.count() >= 1);
        verifyPropertiesChangedSignal(dbusSpy, QStringLiteral("Powered"), value);

        QList<QVariant> adapterArguments = adapterSpy.takeFirst();
        QCOMPARE(adapterArguments.at(0).toBool(), value);

        QCOMPARE(unit.adapter->isPowered(), value);
        QCOMPARE(unit.dbusAdapter->powered(), value);

        unit.adapter->setPowered(originalValue)->waitForFinished();
    }
}

void AdapterTest::setDiscoverableTest()
{
    // Discoverable cannot be changed when Adapter is off

    Q_FOREACH (const AdapterUnit &unit, m_units) {
        bool wasPowered = unit.adapter->isPowered();
        unit.adapter->setPowered(true)->waitForFinished();

        QSignalSpy adapterSpy(unit.adapter, SIGNAL(discoverableChanged(bool)));
        QSignalSpy dbusSpy(unit.dbusProperties, SIGNAL(PropertiesChanged(QString,QVariantMap,QStringList)));

        bool originalValue = unit.adapter->isDiscoverable();
        bool value = !originalValue;

        unit.adapter->setDiscoverable(value);
        adapterSpy.wait();
        QCOMPARE(adapterSpy.count(), 1);

        QList<QVariant> adapterArguments = adapterSpy.takeFirst();
        QCOMPARE(adapterArguments.at(0).toBool(), value);
        verifyPropertiesChangedSignal(dbusSpy, QStringLiteral("Discoverable"), value);

        QCOMPARE(unit.adapter->isDiscoverable(), value);
        QCOMPARE(unit.dbusAdapter->discoverable(), value);

        unit.adapter->setDiscoverable(originalValue)->waitForFinished();
        unit.adapter->setPowered(wasPowered)->waitForFinished();
    }
}

void AdapterTest::setDiscoverableTimeoutTest()
{
    Q_FOREACH (const AdapterUnit &unit, m_units) {
        QSignalSpy adapterSpy(unit.adapter, SIGNAL(discoverableTimeoutChanged(quint32)));
        QSignalSpy dbusSpy(unit.dbusProperties, SIGNAL(PropertiesChanged(QString,QVariantMap,QStringList)));

        quint32 originalValue = unit.adapter->discoverableTimeout();
        quint32 value = originalValue + 1;

        unit.adapter->setDiscoverableTimeout(value);
        adapterSpy.wait();
        QCOMPARE(adapterSpy.count(), 1);

        QVERIFY(dbusSpy.count() >= 1);
        verifyPropertiesChangedSignal(dbusSpy, QStringLiteral("DiscoverableTimeout"), value);

        QList<QVariant> adapterArguments = adapterSpy.takeFirst();
        QCOMPARE(adapterArguments.at(0).toUInt(), value);

        QCOMPARE(unit.adapter->discoverableTimeout(), value);
        QCOMPARE(unit.dbusAdapter->discoverableTimeout(), value);

        unit.adapter->setDiscoverableTimeout(originalValue)->waitForFinished();
    }
}

void AdapterTest::setPairableTest()
{
    Q_FOREACH (const AdapterUnit &unit, m_units) {
        QSignalSpy adapterSpy(unit.adapter, SIGNAL(pairableChanged(bool)));
        QSignalSpy dbusSpy(unit.dbusProperties, SIGNAL(PropertiesChanged(QString,QVariantMap,QStringList)));

        bool originalValue = unit.adapter->isPairable();
        bool value = !originalValue;

        unit.adapter->setPairable(value);
        adapterSpy.wait();
        QCOMPARE(adapterSpy.count(), 1);

        QList<QVariant> adapterArguments = adapterSpy.takeFirst();
        QCOMPARE(adapterArguments.at(0).toBool(), value);
        verifyPropertiesChangedSignal(dbusSpy, QStringLiteral("Pairable"), value);

        QCOMPARE(unit.adapter->isPairable(), value);
        QCOMPARE(unit.dbusAdapter->pairable(), value);

        unit.adapter->setPairable(originalValue)->waitForFinished();
    }
}

void AdapterTest::setPairableTimeoutTest()
{
    Q_FOREACH (const AdapterUnit &unit, m_units) {
        QSignalSpy adapterSpy(unit.adapter, SIGNAL(pairableTimeoutChanged(quint32)));
        QSignalSpy dbusSpy(unit.dbusProperties, SIGNAL(PropertiesChanged(QString,QVariantMap,QStringList)));

        quint32 originalValue = unit.adapter->pairableTimeout();
        quint32 value = originalValue + 1;

        unit.adapter->setPairableTimeout(value);
        adapterSpy.wait();
        QCOMPARE(adapterSpy.count(), 1);

        QVERIFY(dbusSpy.count() >= 1);
        verifyPropertiesChangedSignal(dbusSpy, QStringLiteral("PairableTimeout"), value);

        QList<QVariant> adapterArguments = adapterSpy.takeFirst();
        QCOMPARE(adapterArguments.at(0).toUInt(), value);

        QCOMPARE(unit.adapter->pairableTimeout(), value);
        QCOMPARE(unit.dbusAdapter->pairableTimeout(), value);

        unit.adapter->setPairableTimeout(originalValue)->waitForFinished();
    }
}

void AdapterTest::discoveryTest()
{
    // Discovery needs Adapter powered on

    Q_FOREACH (const AdapterUnit &unit, m_units) {
        // Make sure the Adapter is powered on and not discovering
        bool wasPowered = unit.adapter->isPowered();
        unit.adapter->setPowered(true)->waitForFinished();
        if (unit.adapter->isDiscovering()) {
            unit.adapter->stopDiscovery()->waitForFinished();
        }

        QSignalSpy adapterSpy(unit.adapter, SIGNAL(discoveringChanged(bool)));
        QSignalSpy dbusSpy(unit.dbusProperties, SIGNAL(PropertiesChanged(QString,QVariantMap,QStringList)));

        // Start Discovery
        unit.adapter->startDiscovery();
        adapterSpy.wait();
        QCOMPARE(adapterSpy.count(), 1);

        QVERIFY(dbusSpy.count() >= 1);
        verifyPropertiesChangedSignal(dbusSpy, QStringLiteral("Discovering"), true);

        QList<QVariant> adapterArguments = adapterSpy.takeFirst();
        QCOMPARE(adapterArguments.at(0).toBool(), true);

        QCOMPARE(unit.adapter->isDiscovering(), true);
        QCOMPARE(unit.dbusAdapter->discovering(), true);

        adapterSpy.clear();
        dbusSpy.clear();

        // Stop Discovery
        unit.adapter->stopDiscovery();
        adapterSpy.wait();
        QCOMPARE(adapterSpy.count(), 1);

        QVERIFY(dbusSpy.count() >= 1);
        verifyPropertiesChangedSignal(dbusSpy, QStringLiteral("Discovering"), false);

        adapterArguments = adapterSpy.takeFirst();
        QCOMPARE(adapterArguments.at(0).toBool(), false);

        QCOMPARE(unit.adapter->isDiscovering(), false);
        QCOMPARE(unit.dbusAdapter->discovering(), false);

        unit.adapter->setPowered(wasPowered)->waitForFinished();
    }
}

QTEST_MAIN(AdapterTest)

