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


#include "nocfgsvcpage.h"

#include <QProcess>
#include <MLayout>
#include <MLinearLayoutPolicy>
#include <MLabel>

#include <DuiControlPanelIf>

NoCfgSvcPage::NoCfgSvcPage() :
        MApplicationPage()
{
    //% "Social Networking"
    setTitle(qtTrId("page_title_no_accounts"));

    MLayout *layout = new MLayout;
    MLinearLayoutPolicy *policy =
            new MLinearLayoutPolicy(layout, Qt::Vertical);
    this->setEscapeMode(MApplicationPageModel::EscapeCloseWindow);
    this->setPannable(false);

    btnNext = new MButton;
    btnNext->setObjectName("NoCfgSvcPageNextButton");
    //% "Set up your Social Networking accounts"
    btnNext->setText(qtTrId("button_setup_accounts"));
    btnNext->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);


    MLabel *lblHeader = new MLabel;
    lblHeader->setWrapMode(QTextOption::WordWrap);
    lblHeader->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    lblHeader->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    //% "<b></b>You have no Social Networking services configured. Would you like to configure them now?"
    lblHeader->setText(qtTrId("label_no_services_configured_configure_now?"));
    lblHeader->setObjectName("NoCfgSvcPageHeaderLabel");

    connect(btnNext,
            SIGNAL(clicked()),
            SLOT(btnNextClicked()));

    policy->addItem(lblHeader, Qt::AlignHCenter);
    policy->addItem(btnNext, Qt::AlignHCenter);
    layout->setPolicy(policy);
    centralWidget()->setLayout(layout);
}

NoCfgSvcPage::~NoCfgSvcPage()
{

}

void NoCfgSvcPage::btnNextClicked()
{
    connect(this,
            SIGNAL(displayExited()),
            this,
            SLOT(dismissPage()));
    DuiControlPanelIf *dcpIf = new DuiControlPanelIf();

    dcpIf->appletPage("SocialWebApplet");
    //We don't immediately dismiss, as it can take several tens of seconds
    //for the DCPIF page to come up, and we don't want to have the user see
    //a transition back to the main page for a few seconds, before the setup
    //page comes up. The escape mode settings and backButtonClicked signal
    //connection is a fail-safe - just in case we never get the displayExited() signal
    //from above (as is the case when testing the app on a regular MeeGo Netbook image)
    this->setEscapeMode(MApplicationPageModel::EscapeManualBack);
    connect(this,
            SIGNAL(backButtonClicked()),
            this,
            SLOT(dismissPage()));
}

void NoCfgSvcPage::dismissPage()
{
    this->dismiss();
}

