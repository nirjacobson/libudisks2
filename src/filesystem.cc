#include "filesystem.h"

const std::string UDisks2::Filesystem::Interface = "org.freedesktop.UDisks2.Filesystem";

UDisks2::Filesystem::Filesystem(const std::string& path)
    : _path(path) {
    _fsProxy = Gio::DBus::Proxy::create_for_bus_sync (
                       Gio::DBus::BusType::SYSTEM,
                       UDisks2::BusName,
                       path,
                       UDisks2::Filesystem::Interface
                   );

    _fsProxy->signal_properties_changed().connect(sigc::mem_fun(*this, &UDisks2::Filesystem::on_drive_properties_changed));

    Glib::Variant<std::vector<std::string>> mountPoints;

    _fsProxy->get_cached_property(mountPoints, UDisks2::Filesystem::Properties::MountPoints);

    if (!mountPoints.get().empty()) {
        _mountPoint = mountPoints.get()[0];
    }
}

void UDisks2::Filesystem::on_drive_properties_changed(const Gio::DBus::Proxy::MapChangedProperties& changed_properties, const std::vector<Glib::ustring>& invalidated_properties) {
    if (changed_properties.find(UDisks2::Filesystem::Properties::MountPoints) != changed_properties.end()) {
        Glib::Variant<std::vector<std::string>> mountPointsVariant =
            Glib::Variant<std::vector<std::string>>
                ::cast_dynamic<Glib::Variant<std::vector<std::string>>>
                    (changed_properties
                        .at(UDisks2::Filesystem::Properties::MountPoints));

        std::vector<std::string> mountPoints = mountPointsVariant.get();

        if (!mountPoints.empty()) {
            if (_mountPoint.empty()) {
                _mountPoint = mountPoints[0];
                _sig_mounted.emit(_mountPoint);
            } else {
                if (std::find(mountPoints.begin(), mountPoints.end(), _mountPoint) == mountPoints.end()) {
                    _mountPoint = mountPoints[0];
                    _sig_unmounted.emit();
                    _sig_mounted.emit(_mountPoint);
                }
            }
        } else {
            if (!_mountPoint.empty()) {
                _mountPoint.clear();
                _sig_unmounted.emit();
            }
        }
    }
}


UDisks2::Filesystem::sig_mounted UDisks2::Filesystem::signal_mounted() {
    return _sig_mounted;
}

UDisks2::Filesystem::sig_unmounted UDisks2::Filesystem::signal_unmounted() {
    return _sig_unmounted;
}

const std::string& UDisks2::Filesystem::path() const {
    return _path;
}

const std::string& UDisks2::Filesystem::mountPoint() const {
    return _mountPoint;
}

void UDisks2::Filesystem::unmount() {
    Glib::Variant<std::map<Glib::ustring, Glib::VariantBase>> options = Glib::Variant<std::map<Glib::ustring, Glib::VariantBase>>::create({});
    Glib::VariantContainerBase args = Glib::VariantContainerBase::create_tuple(options);
    _fsProxy->call_sync(UDisks2::Filesystem::Methods::Unmount, args);
}