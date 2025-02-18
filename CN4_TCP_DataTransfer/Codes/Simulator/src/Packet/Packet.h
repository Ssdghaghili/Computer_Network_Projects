#ifndef PACKET_H
#define PACKET_H

#include "../Header/DataLinkHeader.h"
#include "../Header/TCPHeader.h"
#include "IP/IP.h"

#include <QSharedPointer>
#include <QString>
#include <QVector>

enum class PacketType
{
    Data,
    Control,
    RIPUpdate,
    OSPFHello,
    OSPFLSA,
    DHCPRequest,
    DHCPOffer,
    Custom
};

class Packet
{
public:
    explicit Packet(PacketType type = PacketType::Data, const QByteArray &payload = "");
    Packet(PacketType type, const QByteArray &payload, int ttl);    // New constructor

    void             setPayload(const QByteArray &payload);
    QByteArray       getPayload() const;

    void             addToPath(const QString &routerIP);
    QVector<QString> getPath() const;

    void             incrementWaitCycles();
    size_t           getWaitCycles() const;

    void             incrementQueueWaitCycles();
    size_t           getQueueWaitCycles() const;

    void             setSequenceNumber(int sequenceNumber);
    int              getSequenceNumber() const;

    void             markDropped(bool isDropped);
    bool             isDropped() const;

    size_t
    getWaitingCycle()
    {
        return m_waitingCycle;
    }

    size_t
    getTotalCycle()
    {
        return m_totalCycle;
    }

    QString
    getPathTaken()
    {
        return m_pathTaken;
    }

    void
    increamentTotalCycle()
    {
        m_totalCycle++;
    }

    void
    increamentWaitCycle()
    {
        m_waitingCycle++;
    }

    void
    addToPathTaken(QString path)
    {
        m_pathTaken.append("-->" + path);
    }

    bool
    isIPv6()
    {
        return m_isWantedIpV6;
    }

    PacketType     getType() const;
    size_t        *getWaitCyclesPtr();

    // TTL methods
    void           setTTL(int ttl);
    int            getTTL() const;
    void           decrementTTL();

    // DataLinkHeader Methods
    void           setDataLinkHeader(const DataLinkHeader &header);
    DataLinkHeader getDataLinkHeader() const;

    // TCPHeader Methods
    void           setTCPHeader(const TCPHeader &header);
    TCPHeader      getTCPHeader() const;

    qint64         getId() const;

    QSharedPointer<IP> destinationIP() const;
    void               setDestinationIP(QSharedPointer<IP> newDestinationIP);

    QSharedPointer<IP> sourceIP() const;
    void               setSourceIP(QSharedPointer<IP> newSourceIP);

private:
    static qint64    s_nextId;
    PacketType       m_type;
    QByteArray       m_payload;
    QVector<QString> m_path;
    size_t           m_waitCycles;
    size_t           m_queueWaitCycles;
    int              m_sequenceNumber;
    bool             m_isDropped;
    DataLinkHeader   m_dataLinkHeader;
    TCPHeader        m_tcpHeader;
    int              m_ttl;
    qint64           m_id;
    size_t           m_waitingCycle;
    size_t           m_totalCycle;
    QString          m_pathTaken;
    bool             m_isWantedIpV6;

    QSharedPointer<IP> m_destinationIP;
    QSharedPointer<IP> m_sourceIP;
};

typedef QSharedPointer<Packet> PacketPtr_t;

#endif    // PACKET_H
