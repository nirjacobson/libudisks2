#include "filesystem.h"

const std::string UDisks2::Filesystem::Interface = "org.freedesktop.UDisks2.Filesystem";

UDisks2::Filesystem::Filesystem(const std::string& path)
    : _path(path) {
    _fs_proxy = Gio::DBus::Proxy::create_for_bus_sync (
                    Gio::DBus::BusType::SYSTEM,
                    UDisks2::BusName,
                    path,
                    UDisks2::Filesystem::Interface
                );

    _fs_proxy->signal_properties_changed().connect(sigc::mem_fun(*this, &UDisks2::Filesystem::on_fs_properties_changed));

    Glib::Variant<std::vector<std::string>> mount_points;

    _fs_proxy->get_cached_property(mount_points, UDisks2::Filesystem::Properties::MountPoints);

    if (!mount_points.get().empty()) {
        _mount_point = mount_points.get()[0];
    }
}

/**
 * @details If the MountPoints property changes on the object, Filesystem::_mount_point is compared
 * against the MountPoints list and is updated if necessary. If updated, Filesystem::_sig_mounted
 * or Filesystem::_sig_unmounted will be emitted as necessary.
 */
void UDisks2::Filesystem::on_fs_properties_changed(const Gio::DBus::Proxy::MapChangedProperties& changed_properties, const std::vector<Glib::ustring>& invalidated_properties) {
    if (changed_properties.find(UDisks2::Filesystem::Properties::MountPoints) != changed_properties.end()) {
        Glib::Variant<std::vector<std::string>> mount_points_variant =
            Glib::Variant<std::vector<std::string>>
            ::cast_dynamic<Glib::Variant<std::vector<std::string>>>
            (changed_properties
             .at(UDisks2::Filesystem::Properties::MountPoints));

        std::vector<std::string> mount_points = mount_points_variant.get();

        if (!mount_points.empty()) {
            if (_mount_point.empty()) {
                _mount_point = mount_points[0];
                _sig_mounted.emit(_mount_point);
            }
            else {
                if (std::find(mount_points.begin(), mount_points.end(), _mount_point) == mount_points.end()) {
                    _mount_point = mount_points[0];
                    _sig_unmounted.emit();
                    _sig_mounted.emit(_mount_point);
                }
            }
        }
        else {
            if (!_mount_point.empty()) {
                _mount_point.clear();
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

const std::string& UDisks2::Filesystem::mount_point() const {
    return _mount_point;
}

void UDisks2::Filesystem::unmount() {
    Glib::Variant<std::map<Glib::ustring, Glib::VariantBase>> options = Glib::Variant<std::map<Glib::ustring, Glib::VariantBase>>::create({});
    Glib::VariantContainerBase args = Glib::VariantContainerBase::create_tuple(options);
    _fs_proxy->call_sync(UDisks2::Filesystem::Methods::Unmount, args);
}