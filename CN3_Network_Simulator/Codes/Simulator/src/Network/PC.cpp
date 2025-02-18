#include "PC.h"

#include "../DataGenerator/DataGenerator.h"
#include "../EventsCoordinator/EventsCoordinator.h"
#include "../MACAddress/MACADdressGenerator.h"
#include "../Packet/Packet.h"

#include <QDebug>
#include <QThread>

PC::PC(int id, const QString &ipAddress, QObject *parent) :
    Node(id, ipAddress, NodeType::PC, parent)
{
    m_port = PortPtr_t::create(this);
    m_port->setPortNumber(1);
    m_port->setRouterIP(m_ipAddress->getIp());

    connect(m_port.get(), &Port::packetReceived, this, &PC::processPacket);

    QSharedPointer<MACAddressGenerator> generator = QSharedPointer<MACAddressGenerator>::create();
    m_macAddress                                  = generator->generate();

    qDebug() << "PC initialized: ID =" << m_id << ", IP =" << m_ipAddress->getIp();

    auto eventsCoordinator = EventsCoordinator::instance();

    connect(eventsCoordinator, &EventsCoordinator::nextTickForPCs, this, &PC::generatePacket);

    connect(this, &PC::thisIsTheEnd, eventsCoordinator, &EventsCoordinator::thisIsTheEnd);

    setObjectName(QString::number(m_id));
}

PC::~PC()
{
    qDebug() << "PC destroyed: ID =" << m_id;
}

PortPtr_t
PC::getPort()
{
    return m_port;
}

void
PC::initialize()
{
    qDebug() << "PC initialized: ID =" << m_id << ", IP =" << m_ipAddress->getIp()
             << ", running in thread" << (quintptr)QThread::currentThreadId();
}

void
PC::generatePacket()
{
    m_workingWithDataPackets = true;
    /**
     * ======================================================
     * ======================================================
     * ======================================================
     * ======================================================
     * @attention Send Packets from buffer to the destination.
     * @attention Be aware that you must follow the TCP congestion control algorithms here.
     * ======================================================
     * ======================================================
     * ======================================================
     * ======================================================
     **/

    if(m_packetStorage.isEmpty())
    {
        // qWarning() << "PC" << m_id << "has no packets to send.";
        return;
    }

    // qInfo() << Q_FUNC_INFO << "PC" << m_id << "ip:" << m_ipAddress->getIp() << "has"
    //         << m_packetStorage.size() << "packets to send.";


    static const QString destination   = "192.168.100.24";
    auto                 destinationIP = QSharedPointer<IP>::create(destination);

    auto                 packet        = m_packetStorage.takeFirst();

    packet->addToPath(m_ipAddress->getIp());
    packet->addToPathTaken(m_ipAddress->getIp());
    packet->addToPath(destination);
    packet->setDestinationIP(destinationIP);
    packet->setSourceIP(m_ipAddress);

    if(m_packetStorage.length() == 0)
    {
        qDebug() << "PC" << m_id << "sent the last packet.";
    }

    m_port->sendPacket(packet);
}

void
PC::processDataPacket(const PacketPtr_t &packet)
{
    if(!packet) return;

    static size_t packet_counter = 0;

    if(packet->sourceIP()->getIp() == m_ipAddress->getIp())
    {
        // qDebug() << "PC" << m_id << "received packet from itself. Dropping.";
        return;
    }

    if(m_id == 24)
    {
        packet_counter++;
        qDebug() << "PC" << m_id << "received" << packet_counter << "th packet.";

        /**
         * ======================================================
         * ======================================================
         * ======================================================
         * ======================================================
         * @attention Handle the received packet and remove this return.
         * ======================================================
         * ======================================================
         * ======================================================
         * ======================================================
         **/
        return;
    }

    QString payload = packet->getPayload();

    if(m_ipAddress->getIp() != packet->destinationIP()->getIp())
    {
        // qWarning() << Q_FUNC_INFO << "PC" << m_id << "ip:" << m_ipAddress->getIp()
        //            << "received data packet! IT IS NOT FOR ME! Dropping."
        //            << "destination:" << packet->getPath()[1];

        // Record packet drop
        if(m_metricsCollector)
        {
            m_metricsCollector->recordPacketDropped();
        }

        return;
    }
    else
    {
        qInfo() << Q_FUNC_INFO << "PC" << m_id << "ip:" << m_ipAddress->getIp()
                << "received packet with payload size:" << packet->getPayload().size();

        if(m_metricsCollector)
        {
            m_metricsCollector->increamentHops();
            m_metricsCollector->recordPacketReceived(packet->getPath());
            m_metricsCollector->increamentHops();
            m_metricsCollector->recordWaitCycle(packet->getWaitingCycle());
        }
    }



    if(packet->getType() == PacketType::Data)
    {
        if(m_ipAddress->getIp() == "192.168.100.24")
        {
            qInfo() << Q_FUNC_INFO << "PC" << m_id << "ip:" << m_ipAddress->getIp()
                    << "received packet with payload size:" << packet->getPayload().size();
        }

        QString destinationIP = packet->destinationIP()->getIp();
        QString sourceIP      = packet->sourceIP()->getIp();
        QString payload       = packet->getPayload();

        packet->addToPathTaken(destinationIP);

        if(payload == "TCP_ACK")
        {
            /**
                 * ======================================================
                 * ======================================================
                 * ======================================================
                 * ======================================================
                 * @attention Handle the received ACK packet.
                 * ======================================================
                 * ======================================================
                 * ======================================================
                 * ======================================================
                 */
        }

            /**
             * ======================================================
             * ======================================================
             * ======================================================
             * ======================================================
             * @attention Parse the payload and process it.
             * @attention store it in a buffer and at the end, flush the buffer in a file.
             * @attention Send ACK to the source. (On Next Tick!)
             * @attention put ACK at the queue heade and send it on next cycle
             * @attention process the next data packet from the buffer.
             * ======================================================
             * ======================================================
             * ======================================================
             * ======================================================
             **/
    }

    /**
     * ======================================================
     * ======================================================
     * ======================================================
     * ======================================================
     * @attention When you received all packets (You Know This By Hard Coding :)
     * @attention emit thisIsTheEnd() signal to flush buffer to file
     * @attention and terminate the application
     * ======================================================
     * ======================================================
     * ======================================================
     * ======================================================
     **/
}

void
PC::requestIPFromDHCP()
{
    qDebug() << "PC" << m_id << "requesting IP via DHCP.";
    auto packet = QSharedPointer<Packet>::create(PacketType::Control,
                                                 QString("DHCP_REQUEST:%1").arg(m_id).toUtf8());
    m_port->sendPacket(packet);

    emit packetSent(packet);
}

void
PC::processPacket(const PacketPtr_t &packet)
{
    if(!packet) return;

    if(m_workingWithDataPackets)
    {
        return processDataPacket(packet);
    }

    QString payload = packet->getPayload();
    // qDebug() << "PC" << m_id << "received packet with payload:" << payload;

    if(payload.contains("DHCP_OFFER"))
    {
        QStringList parts = payload.split(":");
        if(parts.size() == 3)
        {
            QString offeredIP = parts.at(1);
            int     clientId  = parts.at(2).toInt();

            if(clientId == m_id)
            {
                qDebug() << "PC" << m_id << "received DHCP offer:" << offeredIP << "Assigning IP.";
                m_ipAddress->setIp(offeredIP);
                qDebug() << "PC" << m_id << "assigned IP:" << m_ipAddress;
            }
        }
        else
        {
            qWarning() << "Malformed DHCP_OFFER packet on PC" << m_id << "payload:" << payload;
        }
    }
    else
    {
        qDebug() << "PC" << m_id << "received unknown/unsupported packet, dropping it.";
    }

    // emit thisIsTheEnd();
}

void
PC::fillStorage(const QList<PacketPtr_t> &packets)
{
    m_packetStorage.append(packets);
}

QString
PC::getIpAddress() const
{
    return m_ipAddress->getIp();
}

void
PC::setMetricsCollector(QSharedPointer<MetricsCollector> collector)
{
    m_metricsCollector = collector;
}

void
PC::initDataGeneratorListener()
{
    auto eventsCoordinator = EventsCoordinator::instance();
    connect(eventsCoordinator->dataGenerator().get(), &DataGenerator::packetsGeneratedForPC,
            [this](int id, const QList<QSharedPointer<Packet>> &packets) {
                /**
                 * ======================================================
                 * ======================================================
                 * ======================================================
                 * ======================================================
                 * @attention Here i gave the audio packets to the PC.
                 * @attention Store the packets in a buffer.
                 * @attention On every tick, send the packets to the destination.
                 * 
                 * @attention THERE IS NO NEED TO DO ANYTHING ELSE HERE.
                 * 
                 * ======================================================
                 * ======================================================
                 * ======================================================
                 * ======================================================
                 **/

                qInfo() << "PC" << m_id << "received" << packets.size()
                        << "packets from data generator with coming id:" << id;

                if(id == this->m_id)
                {
                    fillStorage(packets);
                }
            });
}
