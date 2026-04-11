/**
 * @file manager.h
 * @author Nir Jacobson
 * @date 2026-04-07
 */

#ifndef UDISKS2_MANAGER_H
#define UDISKS2_MANAGER_H

#include <mutex>
#include <thread>
#include <vector>
#include <regex>
#include <iostream>
#include <map>

#include <glibmm/refptr.h>
#include <glibmm/variant.h>
#include <giomm/dbusproxy.h>
#include <glibmm/dispatcher.h>
#include <glibmm/variant.h>
#include <sigc++/sigc++.h>

#include "freedesktop.h"
#include "udisks2.h"
#include "drive.h"
#include "filesystem.h"

namespace UDisks2 {


    /// @brief UDisks2 objects (drives and filesystem) manager proxy
    class Manager {
        public:
            /// @brief Manager constructor.
            Manager();

            typedef sigc::signal<void(std::string)> sig_drive;  ///< "Filesystem added/removed" signal type.
            typedef sigc::signal<void(void)> sig_init;          ///< "Initialized" signal type.

            /// @brief Returns the D-Bus paths of registered Drives
            /// @return a list of D-Bus paths
            std::vector<std::string> drives() const;

            /// @brief Returns the D-Bus paths of registered Filesystems
            /// @return a list of D-Bus paths
            std::vector<std::string> filesystems() const;

            /// @brief Return a Drive proxy for a given drive D-Bus path
            /// @param [in] path the D-Bus path of the drive
            /// @return a Drive proxy
            UDisks2::Drive* drive(const std::string& path);

            /// @brief Return a Filesystem proxy for a given filesystem D-Bus path
            /// @param [in] path the D-Bus path of the filesystem
            /// @return a Filesystem proxy
            UDisks2::Filesystem* filesystem(const std::string& path);

            /// @brief Return a Drive proxy for a given Filesystem proxy
            /// @param [in] fs the Filesystem proxy
            /// @return a Drive proxy
            UDisks2::Drive* drive_for_filesystem(const UDisks2::Filesystem* fs) const;

            sig_init  signal_init();        ///< Getter for ::_signal_init.
            sig_drive signal_fs_added();    ///< Getter for ::_signal_fs_added.
            sig_drive signal_fs_removed();  ///< Getter for ::_signal_fs_removed.

        private:
            sig_init  _signal_init;       ///< Emitted when the manager has initialized its list of preregistered drives and filesystems.
            sig_drive _signal_fs_added;   ///< Emitted when a new filesystem appears.
            sig_drive _signal_fs_removed; ///< Emitted when a filesystem disappears.

            Glib::RefPtr<Gio::DBus::Proxy> _object_manager_proxy; ///< D-Bus proxy for the UDisks2 object using the manager interface.

            std::vector<std::string> _drives;      ///< D-Bus object paths of registered drives.
            std::vector<std::string> _filesystems; ///< D-Bus object paths of registered filesystems.

            /// @brief Filesystem D-Bus paths mapped to D-Bus paths of registered drives.
            std::map<std::string, std::string> _fs_to_drive_map;

            /// @brief A regular expression capturing the D-Bus path of a filesystem.
            static const std::regex FSPathPattern;

            /// @brief Initialize  internal lists of preregistered drives' and filesystems' information.
            void init();

            /// @brief Called when the poller thread discovers a disc has been inserted.
            void on_notification_from_poller();

            /// @brief Called when the disc drive has ejected.
            void on_cddrive_eject();

            /// @brief Called when the object manager interface emits a signal.
            /// @param [in] signal_name The signal emitted.
            /// @param [in] parameters  The signal arguments.
            /// @see Freedesktop::ObjectManager::Signals.
            void on_signal(const Glib::ustring& sender_name, const Glib::ustring& signal_name, const Glib::VariantContainerBase& parameters);
            
            /// @brief A delegate called when the object manager emits that new drives or filesystems have been discovered.
            /// @param [in] parameters The signal arguments.
            void on_interfaces_added(const Glib::VariantContainerBase& parameters);

            /// @brief A delegate called when the object manager emits that drives or filesystems have been removed.
            /// @param [in] parameters The signal arguments.
            void on_interfaces_removed(const Glib::VariantContainerBase& parameters);

            /// @brief A convenience method to retrieve a property from a Glib::VariantContainerBase with a given type.
            /// @tparam T The data type of the property.
            /// @param [in]  properties the set of properties.
            /// @param [in]  propName   the property to retrieve .
            /// @param [out] propValue  the property value.
            template <typename T>
            static void get_property_value(const Glib::VariantContainerBase& properties, const std::string& propName, T& propValue);
    };
}

#endif // UDISKS2_MANAGER_H