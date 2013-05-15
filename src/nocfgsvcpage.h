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



#ifndef NOCFGSVCPAGE_H
#define NOCFGSVCPAGE_H

#include <QObject>
#include <MApplicationPage>
#include <MButton>

class NoCfgSvcPage : public MApplicationPage
{
    Q_OBJECT
public:
    NoCfgSvcPage();
    ~NoCfgSvcPage();

private slots:
    void btnNextClicked();
    void dismissPage();

private:
    MButton *btnNext;
};

#endif // NOCFGSVCPAGE_H
