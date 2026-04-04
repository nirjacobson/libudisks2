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
    class Manager {
        public:
            Manager();

            typedef sigc::signal<void(std::string)> sig_drive;
            typedef sigc::signal<void(void)> sig_init;

            std::vector<std::string> drives() const;
            std::vector<std::string> filesystems() const;

            UDisks2::Drive* drive(const std::string& path);
            UDisks2::Filesystem* filesystem(const std::string& path);

            UDisks2::Drive* drive_for_filesystem(const UDisks2::Filesystem* fs) const;

            sig_init  signal_init();
            sig_drive signal_drive_added();
            sig_drive signal_drive_removed();

        private:
            sig_init  _sig_init;
            sig_drive _sig_fs_added;
            sig_drive _sig_fs_removed;

            Glib::RefPtr<Gio::DBus::Proxy> _object_manager_proxy;

            Glib::Dispatcher _dispatcher;

            std::vector<std::string> _drives;
            std::vector<std::string> _filesystems;

            std::map<std::string, std::string> _device_to_drive_map;

            static const std::regex FSPathPattern;

            void init();

            void on_notification_from_poller();
            void on_cddrive_eject();

            void on_signal(const Glib::ustring& sender_name, const Glib::ustring& signal_name, const Glib::VariantContainerBase& parameters);
            void on_interfaces_added(const Glib::VariantContainerBase& parameters);
            void on_interfaces_removed(const Glib::VariantContainerBase& parameters);
            
            template <typename T>
            static void get_property_value(const Glib::VariantContainerBase& properties, const std::string& propName, T& propValue);
    };
}

#endif // UDISKS2_MANAGER_H