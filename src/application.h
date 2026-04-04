#ifndef APPLICATION_H
#define APPLICATION_H

#include <iostream>
#include <vector>

#include <glibmm/refptr.h>
#include <glibmm/main.h>
#include <sigc++/sigc++.h>

#include "manager.h"

class Application {

    public:
        Application();

        void run();

    private:
        Glib::RefPtr<Glib::MainLoop> _main_loop;

        UDisks2::Manager _udisks2;

        void on_drive_added(const std::string& path);
        void on_drive_removed(const std::string& path);
};

#endif // APPLICATION_H