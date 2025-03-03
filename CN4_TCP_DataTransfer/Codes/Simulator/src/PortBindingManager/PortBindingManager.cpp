#include <QDebug>
#include "PortBindingManager.h"

PortBindingManager::PortBindingManager(QObject *parent) : QObject(parent) {}

void PortBindingManager::bind(const QSharedPointer<Port> &port1, const QSharedPointer<Port> &port2, int router1Id, int router2Id)
{
    if (!port1 || !port2) {
        qWarning() << "Invalid ports provided for binding.";
        return;
    }

    QMutexLocker locker(&m_mutex);

    if (m_bindings.contains(port1) || m_bindings.contains(port2)) {
        qWarning() << "One of the ports is already bound.";
        return;
    }

    connect(port1.data(), &Port::packetSent, port2.data(), &Port::receivePacket, Qt::QueuedConnection);
    connect(port2.data(), &Port::packetSent, port1.data(), &Port::receivePacket, Qt::QueuedConnection);

    port1->setConnected(true);
    port2->setConnected(true);

    bool port1IsPC = port1->getConnectedPC() != nullptr;
    bool port2IsPC = port2->getConnectedPC() != nullptr;

    if (!port1IsPC && !port2IsPC) {
        port1->setConnectedRouterId(router2Id);
        port2->setConnectedRouterId(router1Id);
    }
    else if (port1IsPC && !port2IsPC) {
        port2->setConnectedRouterId(router1Id);
    }
    else if (!port1IsPC && port2IsPC) {
        port2->setConnectedRouterId(router1Id);
    }
    else {
        qWarning() << "Attempting to bind two Ports connected to PCs. Binding skipped.";
        return;
    }

    m_bindings.insert(port1, port2);
    m_bindings.insert(port2, port1);

    emit bindingChanged(router1Id, port1->getPortNumber(), router2Id, port2->getPortNumber(), true);
    qDebug() << "Ports bound between Router ID" << router1Id << "Port" << port1->getPortNumber()
             << "and Router ID" << router2Id << "Port" << port2->getPortNumber();
}

bool PortBindingManager::unbind(const QSharedPointer<Port> &port1, const QSharedPointer<Port> &port2)
{
    if (!port1 || !port2) {
        qWarning() << "Invalid ports provided for unbinding.";
        return false;
    }

    QMutexLocker locker(&m_mutex);

    if (m_bindings.value(port1) != port2) {
        qWarning() << "Ports are not bound.";
        return false;
    }

    disconnect(port1.data(), &Port::packetSent, port2.data(), &Port::receivePacket);
    disconnect(port2.data(), &Port::packetSent, port1.data(), &Port::receivePacket);

    port1->setConnected(false);
    port2->setConnected(false);

    m_bindings.remove(port1);
    m_bindings.remove(port2);

    emit bindingChanged(port1->getPortNumber(), port1->getPortNumber(), port2->getPortNumber(), port2->getPortNumber(), false);
    qDebug() << "Ports unbound.";
    return true;
}

bool PortBindingManager::isBound(const QSharedPointer<Port> &port) const
{
    QMutexLocker locker(&m_mutex);
    return m_bindings.contains(port);
}
