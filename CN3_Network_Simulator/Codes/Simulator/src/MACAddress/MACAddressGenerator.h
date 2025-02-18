#ifndef MACADDRESSGENERATOR_H
#define MACADDRESSGENERATOR_H

#include "MACAddress.h"

#include <QMutex>
#include <QMutexLocker>
#include <QSet>
#include <QString>

class MACAddressGenerator
{
public:
    static MACAddress generate();

private:
    static QSet<QString> usedAddresses;
    static QString       generateRandomAddress();
    inline static QMutex m_mutex = QMutex();
};

#endif    // MACADDRESSGENERATOR_H
