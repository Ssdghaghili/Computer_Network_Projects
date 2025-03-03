#ifndef NETWORK_H
#define NETWORK_H

#include <vector>
#include <QObject>
#include <QJsonObject>
#include <QSharedPointer>

#include "../Network/AutonomousSystem.h"
#include "../Globals/IdAssignment.h"
#include "../Network/Router.h"

class Network : public QObject
{
    Q_OBJECT

public:
    explicit Network(const QJsonObject &config, QObject *parent = nullptr);
    ~Network();

    void initialize(const IdAssignment &idAssignment, const bool torus);
    void connectAutonomousSystems();
    void initiateDHCPPhase();
    void initiateDHCPPhaseForPC();
    void checkAssignedIP();
    void checkAssignedIPPC();

    void enableRIPOnAllRouters();
    void enableOSPFOnAllRouters();
    void startBGP(RoutingProtocol protocolAS1, RoutingProtocol protocolAS2);
    void printAllRoutingTables();
    void setupDirectRoutesForRouters(RoutingProtocol protocol);
    void startEBGP();
    void startIBGP();

    void finalizeRoutesAfterDHCP(RoutingProtocol protocol, bool bgp, RoutingProtocol protocolAS1, RoutingProtocol protocolAS2);
    std::vector<QSharedPointer<Router>> getAllRouters() const;
    std::vector<QSharedPointer<AutonomousSystem>> getAutonomousSystems() const;

private:
    QJsonObject m_config;
    std::vector<QSharedPointer<AutonomousSystem>> m_autonomousSystems;

    void createAutonomousSystems(const IdAssignment &idAssignment, bool torus);
};

#endif // NETWORK_H
