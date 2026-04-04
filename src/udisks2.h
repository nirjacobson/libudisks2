#ifndef UDISKS2_H
#define UDISKS2_H

#include <string>

namespace UDisks2 {
    inline const std::string BusName    = "org.freedesktop.UDisks2";
    inline const std::string Path       = "/org/freedesktop/UDisks2";

    namespace Block {
        inline const std::string Interface = "org.freedesktop.UDisks2.Block";
    }
}

#endif // UDISKS2_H