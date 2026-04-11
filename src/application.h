/**
 * @file application.h
 * @author Nir Jacobson
 * @date 2026-04-06
 */

#ifndef APPLICATION_H
#define APPLICATION_H

#include <iostream>
#include <vector>

#include <glibmm/refptr.h>
#include <glibmm/main.h>
#include <sigc++/sigc++.h>

#include "manager.h"

/// @brief An included test application.
/// Use it to test the proper configuration of your system.
class Application {

    public:
        /// @brief Application constructor.
        Application();

        /// @brief Runs the Application.
        void run();

    private:
        Glib::RefPtr<Glib::MainLoop> _main_loop;            ///< Main loop.

        UDisks2::Manager _udisks2;                          ///< UDisks2.

        /// @brief Called when a new drive is registered.
        void on_drive_added(const std::string& path);

        /// @brief Called when a drive is removed.
        void on_drive_removed(const std::string& path);
};

#endif // APPLICATION_H