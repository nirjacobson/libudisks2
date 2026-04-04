#ifndef UDISKS2_DRIVE_H
#define UDISKS2_DRIVE_H

#include <string>

#include <glibmm/variant.h>
#include <sigc++/signal.h>
#include <giomm/dbusproxy.h>

#include "udisks2.h"

namespace UDisks2 {
    class Drive {
        public:
            static const std::string Interface;

            Drive(const std::string& path);

            void eject();
            
        private:
    
            class Methods {
                public:
                    inline static const std::string Eject = "Eject";
            };

            std::string _path;
            Glib::RefPtr<Gio::DBus::Proxy> _driveProxy;
    };
}

#endif // UDISKS2_DRIVE_H