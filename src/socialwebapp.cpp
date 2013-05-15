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


#include "socialwebapp.h"
#include "switemlistpage.h"

SocialWebApp::SocialWebApp(int &argc, char**argv)
        : MApplication(argc, argv)
{

    mWindow = new MApplicationWindow();
    mWindow->show();

    mSwItemListPage = new SwItemListPage();
    mSwItemListPage->appear();

}

SocialWebApp::~SocialWebApp()
{
    delete mSwItemListPage;
    delete mWindow;
}
