#ifndef PC_H
#define PC_H

#include "../MetricsCollector/MetricsCollector.h"
#include "../Port/Port.h"
#include "Node.h"

#include <QSharedPointer>

class PC : public Node
{
    Q_OBJECT

public:
    explicit PC(int id, const QString &ipAddress, QObject *parent = nullptr);
    ~PC() override;

    PortPtr_t getPort();

    QString   getIpAddress() const;

    void      setMetricsCollector(QSharedPointer<MetricsCollector> collector);

    void      initDataGeneratorListener();

signals:
    void packetSent(const PacketPtr_t &data);
    void thisIsTheEnd();

public Q_SLOTS:
    void initialize();
    void generatePacket();
    void requestIPFromDHCP();
    void processPacket(const PacketPtr_t &packet);
    void processDataPacket(const PacketPtr_t &packet);

private:
    void fillStorage(const QList<PacketPtr_t> &packets);

private:
    PortPtr_t                        m_port;
    QSharedPointer<MetricsCollector> m_metricsCollector;
    QList<PacketPtr_t>               m_packetStorage;
    bool                             m_workingWithDataPackets = false;
};

#endif    // PC_H
