/**
 * @file filesystem.h
 * @author Nir Jacobson
 * @date 2026-04-07
 */

#ifndef UDISKS2_FILESYSTEM_H
#define UDISKS2_FILESYSTEM_H

#include <string>

#include <glibmm/variant.h>
#include <sigc++/signal.h>
#include <giomm/dbusproxy.h>

#include "udisks2.h"

namespace UDisks2 {
    
    /// @brief Filesystem proxy
    class Filesystem {
        public:
            static const std::string Interface;

            typedef sigc::signal<void(std::string path)> sig_mounted;
            typedef sigc::signal<void(void)> sig_unmounted;

            /// @brief Filesystem constructor
            /// @param [in] path The D-Bus path of the Filesystem
            Filesystem(const std::string& path);

            sig_mounted signal_mounted();
            sig_unmounted signal_unmounted();

            const std::string& path() const;
            const std::string& mount_point() const;

            /// @brief Unmount the Filesystem.
            /// The filesystem must be unmounted before its drive is ejected.
            /// @see Drive::eject().
            void unmount();

        private:
            class Methods;
            class Properties;

            std::string _path;                        ///< D-Bus path to the Filesystem.
            Glib::RefPtr<Gio::DBus::Proxy> _fs_proxy; ///< D-Bus proxy for the Filesystem object using filesystem interface
            std::string _mount_point;                 ///< Most recently recorded mount point

            sig_mounted _sig_mounted;                 ///< Emitted when the filesystem is mounted
            sig_unmounted _sig_unmounted;             ///< Emitted when the filesystem is unmounted

            /// @brief Called when properties change on the Filesystem object (mount point, etc.)
            /// @param [in] changed_properties     properties that have changed
            /// @param [in] invalidated_properties properties that are no longer valid
            void on_fs_properties_changed(const Gio::DBus::Proxy::MapChangedProperties& changed_properties, const std::vector<Glib::ustring>& invalidated_properties);

    };

    /// @brief D-Bus method names supported by the D-Bus Filesystem interface
    class Filesystem::Methods {
        public:
            inline static const std::string Unmount = "Unmount";
    };

    /// @brief D-Bus property names supported by the D-Bus Filesystem interface
    class Filesystem::Properties {
        public:
            inline static const std::string MountPoints = "MountPoints";
    };
}

#endif // UDISKS2_FILESYSTEM_H