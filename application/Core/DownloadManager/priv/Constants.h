#ifndef DOWNLOADMANAGER_CONSTANTS_H
#define DOWNLOADMANAGER_CONSTANTS_H

#include <QString>

namespace DM
{
   const int CHECK_DEAD_PEER_PERIOD = 10000; // [ms]. TODO : create a signal in PeerManager instead of checking continuously.
}

#endif