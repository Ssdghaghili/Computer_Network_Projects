#ifndef EVENTSCOORDINATOR_H
#define EVENTSCOORDINATOR_H

#include "../Network/PC.h"
#include "../Network/Router.h"

#include <chrono>
#include <vector>

#include <QObject>
#include <QSharedPointer>
#include <QThread>
#include <QTimer>

class DataGenerator;
class Packet;

class EventsCoordinator : public QThread
{
    Q_OBJECT

    typedef std::chrono::milliseconds Millis;

protected:
    explicit EventsCoordinator(QThread *parent = nullptr);

public:
    explicit EventsCoordinator(QObject *parent = nullptr);
    ~EventsCoordinator() override;
    static EventsCoordinator *instance(QThread *parent = nullptr);
    static void                   release();

    void                          startClock(Millis interval);
    void                          stopClock();

    void                          setDataGenerator(QSharedPointer<DataGenerator> generator);
    QSharedPointer<DataGenerator> dataGenerator() const;

    void                          addRouter(const QSharedPointer<Router> &router);


    bool                          convergencePhaseDone() const;
    void                          setConvergencePhaseDone(bool newConvergencePhaseDone);

    void                          startPacketSending();
    void                          stopPacketSending();

protected:
    void run() override;

signals:
    void tick();
    void packetGenerated(QSharedPointer<Packet> packet);
    void convergenceDetected();
    void nextTickForPCs();
    void thisIsTheEnd();

private slots:
    void onTick();
    void onPacketsGenerated(const std::vector<QSharedPointer<Packet>> &packets);
    void onRoutingTableUpdated(int routerId);

private:
    inline static EventsCoordinator    *m_self          = nullptr;
    QTimer                             *m_timer         = nullptr;
    QSharedPointer<DataGenerator>       m_dataGenerator = nullptr;

    std::vector<QSharedPointer<Packet>> m_packetQueue;
    std::vector<QSharedPointer<Router>> m_routers;
    std::vector<QSharedPointer<PC>>     m_pcs;

    bool                                m_routingChangedThisTick;
    bool                                m_convergencePhaseDone = false;
    int                                 m_convergenceTickCounter;
    const int                           REQUIRED_STABLE_TICKS = 20;

    void                                synchronizeRoutersWithDHCP();
    int                                 m_currentTime = 0;
};

#endif    // EVENTSCOORDINATOR_H
