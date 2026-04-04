#ifndef UDISKS2_FILESYSTEM_H
#define UDISKS2_FILESYSTEM_H

#include <string>

#include <glibmm/variant.h>
#include <sigc++/signal.h>
#include <giomm/dbusproxy.h>

#include "udisks2.h"

namespace UDisks2 {
    class Filesystem {
        public:
            static const std::string Interface;
            
            typedef sigc::signal<void(std::string path)> sig_mounted;
            typedef sigc::signal<void(void)> sig_unmounted;

            Filesystem(const std::string& path);

            sig_mounted signal_mounted();
            sig_unmounted signal_unmounted();

            const std::string& path() const;
            const std::string& mountPoint() const;

            void unmount();
            
        private:
            class Methods;
            class Properties;

            std::string _path;
            Glib::RefPtr<Gio::DBus::Proxy> _fsProxy;
            std::string _mountPoint;

            sig_mounted _sig_mounted;
            sig_unmounted _sig_unmounted;

            void on_drive_properties_changed(const Gio::DBus::Proxy::MapChangedProperties& changed_properties, const std::vector<Glib::ustring>& invalidated_properties);

    };

    class Filesystem::Methods {
        public:
            inline static const std::string Unmount = "Unmount";
    };


    class Filesystem::Properties {
        public:
            inline static const std::string MountPoints = "MountPoints";
    };
}

#endif // UDISKS2_FILESYSTEM_H