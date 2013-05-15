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



#ifndef SOCIALWEBAPP_H
#define SOCIALWEBAPP_H

#include <QObject>
#include <MApplication>
#include <MApplicationWindow>

class SwItemListPage;

class SocialWebApp : public MApplication
{
    Q_OBJECT

public:
        SocialWebApp(int &argc, char **argv);
        ~SocialWebApp();

private:
        MApplicationWindow *mWindow;
        SwItemListPage *mSwItemListPage;

};


#endif // SOCIALWEBAPP_H
