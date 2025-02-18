TEMPLATE = app
TARGET = cnca3app
CONFIG += console c++20
QT += core

SOURCES += $$PWD/main.cpp

INCLUDEPATH += $$PWD/../src \
               $$PWD/../src/Globals

# LIBS += -L$$PWD/../lib -lcnca3lib

SRC = $$PWD/../src

RESOURCES += \
    resources.qrc

QT += core
QT += network

DESTDIR = $$SRC/../lib

INCLUDEPATH += $$SRC/Globals

QMAKE_CFLAGS += -fuse-ld=lld

RESOURCES += \
    $$SRC/../app/resources.qrc

SOURCES += \
    $$SRC/DHCPServer/DHCPServer.cpp \
    $$SRC/EventsCoordinator/EventsCoordinator.cpp \
    $$SRC/IP/IP.cpp \
    $$SRC/PortBindingManager/PortBindingManager.cpp \
    $$SRC/Port/Port.cpp \
    $$SRC/MACAddress/MACAddress.cpp \
    $$SRC/MACAddress/MACAddressGenerator.cpp \
    $$SRC/DataGenerator/DataGenerator.cpp \
    $$SRC/Packet/Packet.cpp \
    $$SRC/Header/DataLinkHeader.cpp \
    $$SRC/Header/TCPHeader.cpp \
    $$SRC/NetworkSimulator/Simulator.cpp \
    $$SRC/NetworkSimulator/Network.cpp \
    $$SRC/Network/AutonomousSystem.cpp \
    $$SRC/Network/Router.cpp \
    $$SRC/Network/PC.cpp \
    $$SRC/Network/Node.cpp \
    $$SRC/NetworkSimulator/ApplicationContext.cpp \
    $$SRC/IP/IPHeader.cpp \
    $$SRC/Topology/TopologyController.cpp \
    $$SRC/Topology/TopologyBuilder.cpp \
    $$SRC/BroadCast/UDP.cpp \
    $$SRC/Globals/RouterRegistry.cpp \
    $$SRC/MetricsCollector/MetricsCollector.cpp

HEADERS += \
    $$SRC/DHCPServer/DHCPServer.h \
    $$SRC/EventsCoordinator/EventsCoordinator.h \
    $$SRC/Globals/Globals.h \
    $$SRC/IP/IP.h \
    $$SRC/PortBindingManager/PortBindingManager.h \
    $$SRC/Port/Port.h \
    $$SRC/MACAddress/MACAddress.h \
    $$SRC/MACAddress/MACAddressGenerator.h \
    $$SRC/DataGenerator/DataGenerator.h \
    $$SRC/Packet/Packet.h \
    $$SRC/Header/DataLinkHeader.h \
    $$SRC/Header/TCPHeader.h \
    $$SRC/NetworkSimulator/Simulator.h \
    $$SRC/NetworkSimulator/Network.h \
    $$SRC/Network/AutonomousSystem.h \
    $$SRC/Network/Router.h \
    $$SRC/Network/PC.h \
    $$SRC/Network/Node.h \
    $$SRC/NetworkSimulator/ApplicationContext.h \
    $$SRC/IP/IPHeader.h \
    $$SRC/Topology/TopologyController.h \
    $$SRC/Topology/TopologyBuilder.h \
    $$SRC/Globals/IdAssignment.h \
    $$SRC/BroadCast/UDP.h \
    $$SRC/Globals/RouterRegistry.h \
    $$SRC/Logger/Logger.h \
    $$SRC/MetricsCollector/MetricsCollector.h
