/**
 * @file udsisk2.h
 * @author Nir Jacobson
 * @date 2026-04-07
 */

#ifndef UDISKS2_H
#define UDISKS2_H

#include <string>

/// @brief UDisks2 D-Bus information
namespace UDisks2 {
    inline const std::string BusName    = "org.freedesktop.UDisks2";
    inline const std::string Path       = "/org/freedesktop/UDisks2";

    /// @brief UDisks2 Block D-Bus information
    namespace Block {
        inline const std::string Interface = "org.freedesktop.UDisks2.Block";
    }
}

#endif // UDISKS2_H