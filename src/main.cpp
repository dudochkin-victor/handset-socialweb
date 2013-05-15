/*
 * meego-handset-socialweb - Meego Handset Social Networking application
 *
 * Copyright (c) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#include "main.h"

int main(int argc, char** argv)
{
    // initialize backends

    SocialWebApp *app = new SocialWebApp(argc, argv);

//    QDBusConnection::sessionBus().registerObject("/", app);
//    QDBusConnection::sessionBus().registerService("com.meego.meego_handset_socialweb");

    return app->exec();
}

