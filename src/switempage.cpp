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


#include "switempage.h"

#include <libsocialweb-qt/swclientitem.h>
#include <libsocialweb-qt/swclientservice.h>

#include <marmazon/stringutils.h>

#include <MWidgetCreator>
#include <MWidget>
#include <MGridLayoutPolicy>
#include <MButton>
#include <MCancelEvent>

#include <QTextOption>
#include <QDesktopServices>


M_REGISTER_WIDGET(SwItemPage);

SwItemPage::SwItemPage(QGraphicsItem *parent) :
    MApplicationPage(parent),
    mBtnReply(0),
    mBtnRetweet(0),
    mBtnViewOn(0),
    mBtnHide(0)
{
    this->setObjectName("SwItemPage");
    this->setPannable(true);
    //% "Status - update"
    this->setTitle(qtTrId("page_title_status_update_single"));
}


void SwItemPage::setItem(SwClientItem *swItem)
{
    if (!swItem)
        return;

    MLayout *oldLayout = dynamic_cast<MLayout *>(centralWidget()->layout());
    if (oldLayout && oldLayout->isLayout()) {
        while (!oldLayout->isEmpty()) {
            QGraphicsLayoutItem *item = oldLayout->takeAt(0);
            delete item;
        }
        mBtnReply = 0;
        mBtnRetweet = 0;
        mBtnViewOn = 0;
        mBtnHide = 0;
        mURLButtons.clear();
    }

    mItem = swItem;

    int offset = 0;
    setObjectName("SwItemPage");


    MWidgetController *mwcSwItem = new MWidgetController();
    mwcSwItem->setObjectName("mwcSwItem");
    MLayout *mLayout = new MLayout(mwcSwItem);
    MGridLayoutPolicy *mPolicy = new MGridLayoutPolicy(mLayout);

    mLayout->setPolicy(mPolicy);
    mwcSwItem->setLayout(mLayout);
    this->setCentralWidget(mwcSwItem);

    QPixmap *authPic = mItem->getAuthorIcon();
    MImageWidget *mAuthorPic = new MImageWidget();
    if (!authPic)
        mAuthorPic->setImage("icon-m-content-avatar-placeholder");
    else
        mAuthorPic->setPixmap(*authPic);
    mAuthorPic->setObjectName("SwItemPageAuthorPic");
    mAuthorPic->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mPolicy->addItem(mAuthorPic, 0, 0, Qt::AlignHCenter | Qt::AlignTop);

    MLabel *mAuthorName = new MLabel(mItem->getAuthorName());
    mAuthorName->setTextElide(false);
    mAuthorName->setObjectName("SwItemPageAuthorName");
    mAuthorName->setSizePolicy(QSizePolicy::Expanding,
                                QSizePolicy::Expanding);
    mPolicy->addItem(mAuthorName, 0, 1, Qt::AlignLeft);

    if (mItem->getItemType() == SwClientItem::ItemTypePic) {
        MImageWidget *mPicContent = new MImageWidget(mItem->getThumbnail());
        mPicContent->setObjectName("SwItemPagePicContent");
        mPicContent->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        mPolicy->addItem(mPicContent, 1, 0, 1, 2);

        offset++;
    }

    MLabel *mText = new MLabel(mItem->getContent());
    mText->setTextElide(false);
    mText->setWrapMode(QTextOption::WordWrap);
    mText->setWordWrap(true);
    mText->setObjectName("SwItemPageText");
    mText->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mPolicy->addItem(mText, 1+offset, 0, 1, 2, Qt::AlignLeft);

    MLabel *mDateTime = new MLabel(mItem->getDateTime().toString());
    mDateTime->setTextElide(false);
    mDateTime->setObjectName("SwItemPageDateTime");
    mDateTime->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mPolicy->addItem(mDateTime, 2+offset, 0, 1, 2, Qt::AlignLeft);

    const SwClientService *service = swItem->getService();
    if ((!mBtnReply) && service->canUpdateStatus()) {
        //% "Reply"
        mBtnReply = new MButton(qtTrId("btn_reply"), this);
        connect(mBtnReply,
                SIGNAL(clicked()),
                this,
                SLOT(onReplyClicked()));
        mPolicy->addItem(mBtnReply, 3+offset++, 0, 1, 2, Qt::AlignCenter);
    } else if ((mBtnReply) && !service->canUpdateStatus()) {
        mLayout->animatedDeleteItem(mBtnReply);
        mBtnReply = 0;
    }

    //We have to special case twitter, so we can RT and reply with the @...
    if ((!mBtnRetweet) && (swItem->getServiceName() == "twitter")
        && service->canUpdateStatus()) {
        //% "Retweet"
        mBtnRetweet = new MButton(qtTrId("btn_retweet"), this);
        connect(mBtnRetweet,
                SIGNAL(clicked()),
                this,
                SLOT(onRetweetClicked()));
        mPolicy->addItem(mBtnRetweet, 3+offset++, 0, 1, 2, Qt::AlignCenter);
    } else if ((mBtnRetweet) && (swItem->getServiceName() != "twitter")) {
        mLayout->animatedDeleteItem(mBtnRetweet);
        mBtnRetweet = 0;
    }

    if ((!mBtnViewOn) && !swItem->getURL().isEmpty()) {
        mBtnViewOn = new MButton("", this);
        connect(mBtnViewOn,
                SIGNAL(clicked()),
                this,
                SLOT(onViewOnClicked()));
        mPolicy->addItem(mBtnViewOn, 3+offset++, 0, 1, 2, Qt::AlignCenter);
    } else if ((mBtnViewOn) && swItem->getURL().isEmpty()) {
        mLayout->animatedDeleteItem(mBtnViewOn);
        mBtnViewOn = 0;
    }
    //Centralize the text setting, as it may need to change as the ListItem is recycled between
    //different SwClientItems
    if (mBtnViewOn) {
        //% "View on %1 - 1 is the service Display Name"
        mBtnViewOn->setText(qtTrId("btn_view_on").arg(service->getDisplayName()));
    }

    if ((!mBtnHide) && service->hasBanishItems()) {
        //% "Remove from timeline"
        mBtnHide = new MButton(qtTrId("btn_remove_from_timeline"), this);
        connect(mBtnHide,
                SIGNAL(clicked()),
                this,
                SLOT(onHideClicked()));
        mPolicy->addItem(mBtnHide, 3+offset++, 0, 1, 2, Qt::AlignCenter);
    } else if ((mBtnHide) && !service->hasBanishItems()) {
        mLayout->animatedDeleteItem(mBtnHide);
        mBtnHide = 0;
    }

/*    while (mURLButtons.count()) {
        MButton *btn = mURLButtons.uniqueKeys()[0];
        mURLButtons.remove(btn);
        mLayout->animatedDeleteItem(btn);
    }
    */

    foreach (QUrl url, Marmazon::Utils::GetURLsFromText(swItem->getContent())) {
        //% "Open %1 - 1 is URL from item text"
        MButton *btnOpenURL = new MButton(qtTrId("btn_open_url").arg(url.toString()), this);
        connect(btnOpenURL,
                SIGNAL(clicked()),
                this,
                SLOT(onBtnOpenURL()));
        mURLButtons.insert(btnOpenURL, url);
        mPolicy->addItem(btnOpenURL, 3+offset++, 0, 1, 2, Qt::AlignCenter);
    }

    //relayoutButtons();

}

//Private slots

void SwItemPage::onReplyClicked()
{
    emit this->replyClicked(mItem);
    dismiss();
}

void SwItemPage::onRetweetClicked()
{
    emit this->retweetClicked(mItem);
    dismiss();
}

void SwItemPage::onViewOnClicked()
{
    QDesktopServices::openUrl(QUrl::fromEncoded(mItem->getURL().toLatin1()));
}

void SwItemPage::onHideClicked()
{
    mItem->hideItem();
}

void SwItemPage::onBtnOpenURL()
{
    MButton *sender = qobject_cast<MButton *>(QObject::sender());
    QUrl url = mURLButtons.value(sender, QUrl());
    QDesktopServices::openUrl(url);
}
