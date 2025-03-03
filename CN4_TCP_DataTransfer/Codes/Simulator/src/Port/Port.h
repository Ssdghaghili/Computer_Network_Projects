#ifndef PORT_H
#define PORT_H

#include <QMutex>
#include <QObject>

#include "../Packet/Packet.h"

class PC;

class Port : public QObject
{
    Q_OBJECT

public:
    explicit Port(QObject *parent = nullptr);
    ~Port() override;

    void setPortNumber(uint8_t number);
    uint8_t getPortNumber() const;

    void setRouterIP(const QString &ip);
    QString getRouterIP() const;

    bool isConnected() const;
    void setConnected(bool connected);

    uint64_t getNumberOfPacketsSent() const;
    uint64_t getNumberOfPacketsReceived() const;

    void setConnectedRouterId(int routerId);
    int getConnectedRouterId() const;

    void connectToPC(QSharedPointer<PC> pc);
    QSharedPointer<PC> getConnectedPC() const;

    QString getConnectedRouterIP() const;
    void setConnectedRouterIP(const QString &ip) { m_connectedRouterIP = ip; }

Q_SIGNALS:
    void packetSent(const PacketPtr_t &data);
    void packetReceived(const PacketPtr_t &data);

public Q_SLOTS:
    void sendPacket(const PacketPtr_t &data);
    void sendPackets(const QList<PacketPtr_t> &packets);
    void receivePacket(const PacketPtr_t &data);

private:
    uint8_t  m_number;
    uint64_t m_numberOfPacketsSent;
    uint64_t m_numberOfPacketsReceived;
    QString  m_routerIP;
    bool     m_isConnected;

    QSharedPointer<PC> m_connectedPC;
    QString m_connectedRouterIP;
    mutable QMutex m_mutex;
    int m_connectedRouterId = -1;
};

typedef QSharedPointer<Port> PortPtr_t;

#endif    // PORT_H
