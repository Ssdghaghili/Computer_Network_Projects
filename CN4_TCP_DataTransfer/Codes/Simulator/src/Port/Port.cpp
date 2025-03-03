#include <QDebug>

#include "Port.h"
#include "../Network/PC.h"
#include "../Network/Router.h"
#include "../NetworkSimulator/Simulator.h"
#include "../NetworkSimulator/ApplicationContext.h"

Port::Port(QObject *parent) :
    QObject {parent},
    m_number(0),
    m_numberOfPacketsSent(0),
    m_numberOfPacketsReceived(0),
    m_routerIP(""),
    m_isConnected(false),
    m_connectedPC(nullptr),
    m_connectedRouterIP(" ")
{}

Port::~Port() {}

void Port::setPortNumber(uint8_t number)
{
    QMutexLocker locker(&m_mutex);
    m_number = number;
}

uint8_t Port::getPortNumber() const
{
    QMutexLocker locker(&m_mutex);
    return m_number;
}

void Port::setRouterIP(const QString &ip)
{
    QMutexLocker locker(&m_mutex);
    m_routerIP = ip;
}

QString Port::getRouterIP() const
{
    QMutexLocker locker(&m_mutex);
    return m_routerIP;
}

bool Port::isConnected() const
{
    QMutexLocker locker(&m_mutex);
    return m_isConnected;
}

void Port::setConnected(bool connected)
{
    QMutexLocker locker(&m_mutex);
    m_isConnected = connected;
}

uint64_t Port::getNumberOfPacketsSent() const
{
    QMutexLocker locker(&m_mutex);
    return m_numberOfPacketsSent;
}

uint64_t Port::getNumberOfPacketsReceived() const
{
    QMutexLocker locker(&m_mutex);
    return m_numberOfPacketsReceived;
}

void Port::sendPacket(const PacketPtr_t &data) {
    {
        QMutexLocker locker(&m_mutex);
        ++m_numberOfPacketsSent;
    }
    emit packetSent(data);
    // qDebug() << "Port::sendPacket() emitted packetSent.";
}

void
Port::sendPackets(const QList<PacketPtr_t> &packets)
{
    for(const auto &packet : packets)
    {
        sendPacket(packet);
    }
}

void Port::receivePacket(const PacketPtr_t &data) {
    {
        QMutexLocker locker(&m_mutex);
        ++m_numberOfPacketsReceived;
    }
    emit packetReceived(data);
    // qDebug() << "Port::receivePacket() emitted packetReceived.";
}

void Port::setConnectedRouterId(int routerId) {
    QMutexLocker locker(&m_mutex);
    m_connectedRouterId = routerId;
}

int Port::getConnectedRouterId() const {
    QMutexLocker locker(&m_mutex);
    return m_connectedRouterId;
}

void Port::connectToPC(QSharedPointer<PC> pc)
{
    QMutexLocker locker(&m_mutex);
    if (m_connectedPC) {
        qWarning() << "Port" << m_number << "is already connected to PC with IP" << m_connectedPC->getIpAddress();
        return;
    }

    m_connectedPC = pc;
    m_isConnected = true;

    qDebug() << "Port" << m_number << "connected to PC with IP" << pc->getIpAddress();

    connect(this, &Port::packetReceived, pc.data(), &PC::processPacket);
    connect(pc.data(), &PC::packetSent, this, &Port::sendPacket);
}

QSharedPointer<PC> Port::getConnectedPC() const
{
    QMutexLocker locker(&m_mutex);
    return m_connectedPC;
}

QString Port::getConnectedRouterIP() const
{
    QMutexLocker locker(&m_mutex);

    if (m_connectedRouterIP != " ") {
        return m_connectedRouterIP;
    }

    if (m_connectedPC)
        return QString();

    if (m_connectedRouterId == -1)
        return QString();

    QSharedPointer<Simulator> simulator = ApplicationContext::instance().getSimulator();
    if (!simulator) {
        qWarning() << "Port::getConnectedRouterIP() - Simulator instance not found.";
        return QString();
    }

    QSharedPointer<Network> network = simulator->getNetwork();
    if (!network) {
        qWarning() << "Port::getConnectedRouterIP() - Network instance not found.";
        return QString();
    }

    const auto &autonomousSystems = network->getAutonomousSystems();
    for (const auto &asInstance : autonomousSystems) {
        const auto &routers = asInstance->getRouters();
        for (const auto &router : routers) {
            if (router->getId() == m_connectedRouterId) {
                return router->getIPAddress();
            }
        }
    }

    qWarning() << "Port::getConnectedRouterIP() - Router with ID" << m_connectedRouterId << "not found.";
    return QString();
}
