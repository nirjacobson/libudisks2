#include "application.h"

Application::Application()
    : _mainLoop(Glib::MainLoop::create()) {
}

void Application::run() {
    _udisks2.signal_drive_added().connect(sigc::mem_fun(*this, &Application::on_drive_added));
    _udisks2.signal_drive_removed().connect(sigc::mem_fun(*this, &Application::on_drive_removed));

    _mainLoop->run();
}

void Application::on_drive_added(const std::string& path) {
    std::cout << "Added: " << path << std::endl;
}

void Application::on_drive_removed(const std::string& path) {
    std::cout << "Removed: " << path << std::endl;
}