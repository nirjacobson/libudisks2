#include "drive.h"

const std::string UDisks2::Drive::Interface = "org.freedesktop.UDisks2.Drive";

UDisks2::Drive::Drive(const std::string& path)
    : _path(path) {
    _drive_proxy = Gio::DBus::Proxy::create_for_bus_sync (
                       Gio::DBus::BusType::SYSTEM,
                       UDisks2::BusName,
                       path,
                       UDisks2::Drive::Interface
                   );

}

void UDisks2::Drive::eject() {
    Glib::Variant<std::map<Glib::ustring, Glib::VariantBase>> options = Glib::Variant<std::map<Glib::ustring, Glib::VariantBase>>::create({});
    Glib::VariantContainerBase args = Glib::VariantContainerBase::create_tuple(options);
    _drive_proxy->call_sync(UDisks2::Drive::Methods::Eject, args);
}