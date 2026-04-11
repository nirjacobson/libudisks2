/**
 * @file drive.h
 * @author Nir Jacobson
 * @date 2026-04-07
 */

#ifndef UDISKS2_DRIVE_H
#define UDISKS2_DRIVE_H

#include <string>

#include <glibmm/variant.h>
#include <sigc++/signal.h>
#include <giomm/dbusproxy.h>

#include "udisks2.h"

namespace UDisks2 {

    /// @brief Drive proxy
    class Drive {
        public:
            /// @brief D-Bus interface name for UDisks2 Drive.
            static const std::string Interface;

            /// @brief Drive constructor
            /// @param [in] path The D-Bus path of the Drive
            Drive(const std::string& path);

            /// @brief Ejects the drive.
            /// All drive filesystems must be unmounted first.
            /// @see Filesystem::unmount().
            void eject();

        private:

            /// @brief D-Bus method names supported by the D-Bus Drive interface
            class Methods {
                public:
                    /// @brief Eject method name.
                    inline static const std::string Eject = "Eject";
            };

            /// @brief The D-Bus path of the Drive
            std::string _path;

            /// @brief D-Bus proxy for the Drive object using Drive interface
            Glib::RefPtr<Gio::DBus::Proxy> _drive_proxy;
    };
}

#endif // UDISKS2_DRIVE_H