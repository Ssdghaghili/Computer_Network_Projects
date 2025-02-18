#include "DataGenerator.h"

#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTimer>

DataGenerator::DataGenerator(QObject *parent) :
    QObject(parent), m_lambda(1.0), m_distribution(m_lambda)
{
    std::random_device rd;
    m_generator.seed(rd());
}

void
DataGenerator::setLambda(double lambda)
{
    m_lambda       = lambda;
    m_distribution = std::poisson_distribution<int>(m_lambda);
}

void
DataGenerator::setSenders(const std::vector<QSharedPointer<PC>> &senders)
{
    m_senders = senders;
    qDebug() << "DataGenerator: Set" << m_senders.size() << "senders.";
}

std::vector<int>
DataGenerator::generatePoissonLoads(int numSamples, int timeScale)
{
    std::vector<int> loads(timeScale, 0);
    for(int i = 0; i < numSamples; ++i)
    {
        int value = m_distribution(m_generator);
        if(value < timeScale)
        {
            loads[value]++;
        }
    }
    return loads;
}

QList<QList<QSharedPointer<Packet>>>
DataGenerator::splitFileToPackets(const QString &filePath, size_t packetSize, size_t numParts)
{
    QFile file(filePath);
    if(!file.open(QIODevice::ReadOnly))
    {
        qCritical() << Q_FUNC_INFO << "DataGenerator: Unable to open file:" << filePath;
        qFatal("Unable to open file");
        return {};
    }

    QByteArray data = file.readAll();
    file.close();

    qInfo() << "DataGenerator: Read" << data.size() << "bytes from file:" << filePath;

    QList<QSharedPointer<Packet>> packets;
    for(qint64 i = 0; i < data.size(); i += packetSize)
    {
        QByteArray             payload = data.mid(i, packetSize);
        QSharedPointer<Packet> packet =
          QSharedPointer<Packet>::create(PacketType::Data, payload, 64);

        /**
         * ======================================================
         * ======================================================
         * ======================================================
         * ======================================================
         * @attention Set the path of the packet inside the sender pc.
         * ======================================================
         * ======================================================
         * ======================================================
         * ======================================================
         **/

        packets.append(packet);
    }

    int                                  partSize = packets.size() / numParts;
    QList<QList<QSharedPointer<Packet>>> parts;
    for(int i = 0; i < numParts; ++i)
    {
        QList<QSharedPointer<Packet>> part = packets.mid(i * partSize, partSize);
        parts.append(part);
    }

    int remaining = packets.size() % numParts;
    for(int i = 0; i < remaining; ++i)
    {
        parts[i].append(packets[numParts * partSize + i]);
    }

    return parts;
}

void
DataGenerator::generatePackets()
{
    if(m_senders.empty())
    {
        qWarning() << Q_FUNC_INFO << "DataGenerator: No senders set. Cannot generate packets.";
        return;
    }

    /**
     * ======================================================
     * ======================================================
     * ======================================================
     * ======================================================
     * @attention Read the music file KB by KB and Assign Each Packet to a Sender + set its Sequence number.
     * @attention Each PC will receive the packets and store them in a buffer.
     * @attention Then on every tick, the PC will send the packets to the destination.
     * ======================================================
     * ======================================================
     * ======================================================
     * ======================================================
     **/

    QString                              filePath   = ":/configs/mainConfig/assets/Taasiaan.mp3";
    size_t                               packetSize = 1'024;    // 1KB
    size_t                               pcCount    = getSenders().size();
    QList<QList<QSharedPointer<Packet>>> chunks = splitFileToPackets(filePath, packetSize, pcCount);

    for(size_t i = 0; i < pcCount; ++i)
    {
        auto pc = getSenders()[i];

        // we are listening to this singal on every pc
        qInfo() << "Packets generated for PC" << pc->getId() << ":" << chunks[i].size();
        pc->initDataGeneratorListener();
        emit packetsGeneratedForPC(pc->getId(), chunks[i]);
    }

    // qDebug() << packets.size() << "packets generated and emitted over a timescale of" << timeScale << "seconds.";

    // QTimer::singleShot(4000, this, [distributionMap]() {
    //     qDebug() << "---- Packet Distribution ----";
    //     for (auto itOrigin = distributionMap.constBegin(); itOrigin != distributionMap.constEnd(); ++itOrigin) {
    //         QString origin = itOrigin.key();
    //         for (auto itDest = itOrigin.value().constBegin(); itDest != itOrigin.value().constEnd(); ++itDest) {
    //             QString destination = itDest.key();
    //             int count = itDest.value();
    //             qDebug() << "Origin:" << origin << "-> Destination:" << destination << " | Packets:" << count;
    //         }
    //     }
    //     qDebug() << "------------------------------";
    // });
}

std::vector<QSharedPointer<PC>>
DataGenerator::getSenders() const
{
    return m_senders;
}

void
DataGenerator::loadConfig(const QString &configFilePath)
{
    QFile file(configFilePath);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qWarning() << "DataGenerator: Could not open config file:" << configFilePath;
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    if(doc.isNull() || !doc.isObject())
    {
        qWarning() << "DataGenerator: Invalid JSON in config file.";
        return;
    }

    QJsonObject rootObj = doc.object();

    if(rootObj.contains("packets_per_simulation") && rootObj["packets_per_simulation"].isDouble())
    {
        m_packetsPerSimulation = rootObj["packets_per_simulation"].toInt();
        qDebug() << "DataGenerator: Loaded packets_per_simulation =" << m_packetsPerSimulation;
    }
    else
    {
        qWarning() << "DataGenerator: 'packets_per_simulation' not found or invalid in config "
                      "file. Using default value."
                   << m_packetsPerSimulation;
    }
}
