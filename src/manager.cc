/**
 * @file manager.cc
 * @author Nir Jacobson
 * @date 2026-04-07
 */

#include "manager.h"

const std::regex UDisks2::Manager::FSPathPattern("^.+/(sd[a-z]\\d+)$");

UDisks2::Manager::Manager() {
    Gio::DBus::Proxy::create_for_bus(
        Gio::DBus::BusType::SYSTEM,
        UDisks2::BusName,
        UDisks2::Path,
        Freedesktop::ObjectManager::Interface,
    [&](Glib::RefPtr<Gio::AsyncResult>& result) {
        _object_manager_proxy = Gio::DBus::Proxy::create_for_bus_finish(result);

        init();

        _object_manager_proxy->signal_signal().connect(sigc::mem_fun(*this, &UDisks2::Manager::on_signal));
    });
}

UDisks2::Manager::sig_init UDisks2::Manager::signal_init() {
    return _sig_init;
}

UDisks2::Manager::sig_drive UDisks2::Manager::signal_fs_added() {
    return _sig_fs_added;
}

UDisks2::Manager::sig_drive UDisks2::Manager::signal_fs_removed() {
    return _sig_fs_removed;
}

void UDisks2::Manager::on_signal(const Glib::ustring& sender_name, const Glib::ustring& signal_name, const Glib::VariantContainerBase& parameters) {
    Glib::Variant<std::string> path;
    parameters.get_child(path, 0);

    if (signal_name == Freedesktop::ObjectManager::Signals::InterfacesAdded.data()) {
        on_interfaces_added(parameters);
    } else if (signal_name == Freedesktop::ObjectManager::Signals::InterfacesRemoved.data()) {
        on_interfaces_removed(parameters);
    }
}

void UDisks2::Manager::on_interfaces_added(const Glib::VariantContainerBase& parameters) {
    Glib::Variant<std::string> path;
    parameters.get_child(path, 0);

    Glib::VariantContainerBase ifaceDict;
    parameters.get_child(ifaceDict, 1);

    for (unsigned i = 0; i < ifaceDict.get_n_children(); i++) {
        Glib::VariantContainerBase ifaceDictEntry;
        ifaceDict.get_child(ifaceDictEntry, i);

        Glib::Variant<std::string> iface;
        Glib::VariantContainerBase properties;
        ifaceDictEntry.get_child(iface, 0);
        ifaceDictEntry.get_child(properties, 1);


        if (iface.get() == UDisks2::Drive::Interface) {
            _drives.push_back(path.get());
        } else if (iface.get() == UDisks2::Block::Interface) {
            Glib::RefPtr<Gio::DBus::Proxy> proxy = Gio::DBus::Proxy::create_for_bus_sync (
                    Gio::DBus::BusType::SYSTEM,
                    UDisks2::BusName,
                    path.get(),
                    UDisks2::Block::Interface
                                                   );
            Glib::Variant<std::string> device;
            Glib::Variant<std::string> drive;
            proxy->get_cached_property(device, "Device");
            proxy->get_cached_property(drive, "Drive");

            _fs_to_drive_map.insert(std::make_pair(device.get(), drive.get()));
        } else if (iface.get() != UDisks2::Filesystem::Interface) {
            continue;
        }

        _filesystems.push_back(path.get());

        _sig_fs_added.emit(path.get());
    }
}

void UDisks2::Manager::on_interfaces_removed(const Glib::VariantContainerBase& parameters) {
    Glib::Variant<std::string> path;
    parameters.get_child(path, 0);

    Glib::VariantContainerBase ifaceArr;
    parameters.get_child(ifaceArr, 1);

    for (unsigned int i = 0; i < ifaceArr.get_n_children(); i++) {
        Glib::Variant<std::string> iface;
        ifaceArr.get_child(iface, i);

        if (iface.get() != UDisks2::Filesystem::Interface)
            continue;

        _filesystems.erase(std::remove(_filesystems.begin(), _filesystems.end(), path.get()), _filesystems.end());

        _sig_fs_removed.emit(path.get());

        return;
    }
}
#include <iostream>
void UDisks2::Manager::init() {
    const Glib::VariantContainerBase result = _object_manager_proxy->call_sync(Freedesktop::ObjectManager::Methods::GetManagedObjects);
    Glib::VariantContainerBase objects;
    result.get_child(objects);

    for (unsigned int i = 0; i < objects.get_n_children(); i++) {
        Glib::VariantContainerBase object;
        objects.get_child(object, i);

        Glib::Variant<std::string> path;
        object.get_child(path, 0);

        Glib::VariantContainerBase ifaceDict;
        object.get_child(ifaceDict, 1);

        for (unsigned j = 0; j < ifaceDict.get_n_children(); j++) {
            Glib::VariantContainerBase ifaceDictEntry;
            ifaceDict.get_child(ifaceDictEntry, j);

            Glib::Variant<std::string> iface;
            Glib::VariantContainerBase properties;
            ifaceDictEntry.get_child(iface, 0);
            ifaceDictEntry.get_child(properties, 1);


            if (iface.get() == UDisks2::Drive::Interface) {
                _drives.push_back(path.get());
            } else if (iface.get() == UDisks2::Block::Interface) {
                Glib::RefPtr<Gio::DBus::Proxy> proxy = Gio::DBus::Proxy::create_for_bus_sync (
                        Gio::DBus::BusType::SYSTEM,
                        UDisks2::BusName,
                        path.get(),
                        UDisks2::Block::Interface
                                                       );
                Glib::Variant<std::string> device;
                Glib::Variant<std::string> drive;
                proxy->get_cached_property(device, "Device");
                proxy->get_cached_property(drive, "Drive");

                _fs_to_drive_map.insert(std::make_pair(device.get(), drive.get()));
            } else if (iface.get() != UDisks2::Filesystem::Interface) {
                continue;
            }

            _filesystems.push_back(path.get());
        }
    }

    _sig_init.emit();
}

std::vector<std::string> UDisks2::Manager::drives() const {
    return _drives;
}

std::vector<std::string> UDisks2::Manager::filesystems() const {
    return _filesystems;
}

UDisks2::Drive* UDisks2::Manager::drive(const std::string& path) {
    return new Drive(path);
}

UDisks2::Filesystem* UDisks2::Manager::filesystem(const std::string& path) {
    return new Filesystem(path);
}

UDisks2::Drive* UDisks2::Manager::drive_for_filesystem(const UDisks2::Filesystem* fs) const {
    std::smatch matches;

    if (std::regex_search(fs->path(), matches, FSPathPattern)) {
        const std::string device = matches[1];

        return new Drive(_fs_to_drive_map.at("/dev/"+device));
    }

    return nullptr;
}

template <typename T>
void UDisks2::Manager::get_property_value(const Glib::VariantContainerBase& properties, const std::string& propName,  T& propValue) {
    for (unsigned int i = 0; i < properties.get_n_children(); i++) {
        Glib::VariantContainerBase property;
        properties.get_child(property, i);

        Glib::Variant<std::string> propertyName;
        Glib::VariantContainerBase propertyValue;
        property.get_child(propertyName, 0);
        property.get_child(propertyValue, 1);

        std::string propertyNameValue = propertyName.get();

        if (propertyNameValue == propName) {
            Glib::Variant<T> variant;
            propertyValue.get_child(variant);

            propValue = variant.get();
        }
    }
}