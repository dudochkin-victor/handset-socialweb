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


#include "switemlistitem.h"

#include <libsocialweb-qt/swclientitem.h>
#include <libsocialweb-qt/swclientservice.h>

#include <marmazon/stringutils.h>

#include <MWidgetCreator>
#include <MWidget>
#include <MGridLayoutPolicy>
#include <MAction>
#include <MCancelEvent>

#include <QTextOption>
#include <QDesktopServices>


M_REGISTER_WIDGET(SwItemListItem);

SwItemListItem::SwItemListItem(MWidget *parent) :
    MStylableWidget(parent),
    mActReply(0),
    mActRetweet(0),
    mActViewOn(0),
    mActHide(0),
    mPressed(false)
{
    //Temp hack for the moment...
    //this->setPreferredHeight(200);
    this->setObjectName("SwItemListItem");
}


void SwItemListItem::setItem(SwClientItem *swItem)
{
    if (!swItem)
        return;

    MLayout *oldLayout = dynamic_cast<MLayout *>(this->layout());
    if (oldLayout && oldLayout->isLayout()) {
        while (!oldLayout->isEmpty()) {
            QGraphicsLayoutItem *item = oldLayout->takeAt(0);
            delete item;
        }
    }

    mItem = swItem;

    int offset = 0;
    setObjectName("SwItemListItem");


    MLayout *mLayout = new MLayout(this);
    MGridLayoutPolicy *mPolicy = new MGridLayoutPolicy(mLayout);

    mLayout->setPolicy(mPolicy);
    setLayout(mLayout);

    QPixmap *authPic = mItem->getAuthorIcon();
    MImageWidget *mAuthorPic = new MImageWidget();
    if (!authPic)
        mAuthorPic->setImage("icon-m-content-avatar-placeholder");
    else
        mAuthorPic->setPixmap(*authPic);
    mAuthorPic->setObjectName("SwItemListItemAuthorPic");
    mAuthorPic->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    mPolicy->addItem(mAuthorPic, 0, 0, Qt::AlignHCenter | Qt::AlignTop);

    MLabel *mAuthorName = new MLabel(mItem->getAuthorName());
    mAuthorName->setTextElide(false);
    mAuthorName->setObjectName("SwItemListItemAuthorName");
    mAuthorName->setSizePolicy(QSizePolicy::Expanding,
                                QSizePolicy::Expanding);
    mPolicy->addItem(mAuthorName, 0, 1, Qt::AlignLeft);

    if (mItem->getItemType() == SwClientItem::ItemTypePic) {
        MImageWidget *mPicContent = new MImageWidget(mItem->getThumbnail());
        mPicContent->setObjectName("SwItemListItemPicContent");
        mPicContent->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        mPolicy->addItem(mPicContent, 1, 0, 1, 2);

        offset++;
    }

    MLabel *mText = new MLabel(mItem->getContent());
    mText->setTextElide(false);
    mText->setWrapMode(QTextOption::WordWrap);
    mText->setWordWrap(true);
    mText->setObjectName("SwItemListItemText");
    mText->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mPolicy->addItem(mText, 1+offset, 0, 1, 2, Qt::AlignLeft);

    MLabel *mDateTime = new MLabel(mItem->getDateTime().toString());
    mDateTime->setTextElide(false);
    mDateTime->setObjectName("SwItemListItemDateTime");
    mDateTime->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mPolicy->addItem(mDateTime, 2+offset, 0, 1, 2, Qt::AlignLeft);

    const SwClientService *service = swItem->getService();
    if ((!mActReply) && service->canUpdateStatus()) {
        //% "Reply"
        mActReply = new MAction(qtTrId("action_reply"), this);
        this->addAction(mActReply);
        connect(mActReply,
                SIGNAL(triggered()),
                this,
                SLOT(onReplyClicked()));
    } else if ((mActReply) && !service->canUpdateStatus()) {
        this->removeAction(mActReply);
        delete mActReply;
        mActReply = 0;
    }

    //We have to special case twitter, so we can RT and reply with the @...
    if ((!mActRetweet) && (swItem->getServiceName() == "twitter")
        && service->canUpdateStatus()) {
        //% "Retweet"
        mActRetweet = new MAction(qtTrId("action_retweet"), this);
        this->addAction(mActRetweet);
        connect(mActRetweet,
                SIGNAL(triggered()),
                this,
                SLOT(onRetweetClicked()));
    } else if ((mActRetweet) && (swItem->getServiceName() != "twitter")) {
        this->removeAction(mActRetweet);
        delete mActRetweet;
        mActRetweet = 0;
    }

    if ((!mActViewOn) && !swItem->getURL().isEmpty()) {
        mActViewOn = new MAction("", this);
        this->addAction(mActViewOn);
        connect(mActViewOn,
                SIGNAL(triggered()),
                this,
                SLOT(onViewOnClicked()));
    } else if ((mActViewOn) && swItem->getURL().isEmpty()) {
        this->removeAction(mActViewOn);
        delete mActViewOn;
        mActViewOn = 0;
    }
    //Centralize the text setting, as it may need to change as the ListItem is recycled between
    //different SwClientItems
    if (mActViewOn) {
        //% "View on %1 - 1 is service Display Name"
        mActViewOn->setText(qtTrId("action_view_on").arg(service->getDisplayName()));
    }

    if ((!mActHide) && service->hasBanishItems()) {
        //% "Remove from timeline"
        mActHide = new MAction(qtTrId("action_remove_from_timeline"), this);
        this->addAction(mActHide);
        connect(mActHide,
                SIGNAL(triggered()),
                this,
                SLOT(onHideClicked()));
    } else if ((mActHide) && !service->hasBanishItems()) {
        this->removeAction(mActHide);
        delete mActHide;
        mActHide = 0;
    }

    while (mURLActions.count()) {
        MAction *act = mURLActions.uniqueKeys()[0];
        this->removeAction(act);
        mURLActions.remove(act);
        delete act;
    }

    foreach (QUrl url, Marmazon::Utils::GetURLsFromText(swItem->getContent())) {
        //% "Open %1 - 1 is link from item text"
        MAction *actOpenURL = new MAction(qtTrId("action_open_url").arg(url.toString()), this);
        this->addAction(actOpenURL);
        connect(actOpenURL,
                SIGNAL(triggered()),
                this,
                SLOT(onActOpenURL()));
        mURLActions.insert(actOpenURL, url);
    }

}

SwClientItem * SwItemListItem::getItem()
{
    return mItem;
}

//Protected functions
void SwItemListItem::mousePressEvent(QGraphicsSceneMouseEvent *ev)
{
    mPressed = true;
    style().setModePressed();
    update();
    ev->accept();
}

void SwItemListItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *ev)
{
    if (mPressed) {
        mPressed = false;
        style().setModeDefault();
        update();
        ev->accept();
        emit clicked(mItem);
    } else {
        ev->ignore();
    }
}

void SwItemListItem::cancelEvent(MCancelEvent *ev)
{
    mPressed = false;
    style().setModeDefault();
    update();
    ev->accept();
}

//Private slots

void SwItemListItem::onReplyClicked()
{
    emit this->replyClicked(mItem);
}

void SwItemListItem::onRetweetClicked()
{
    emit this->retweetClicked(mItem);
}

void SwItemListItem::onViewOnClicked()
{
    QDesktopServices::openUrl(QUrl::fromEncoded(mItem->getURL().toLatin1()));
}

void SwItemListItem::onHideClicked()
{
    mItem->hideItem();
}

void SwItemListItem::onActOpenURL()
{
    MAction *sender = qobject_cast<MAction *>(QObject::sender());
    QUrl url = mURLActions.value(sender, QUrl());
    QDesktopServices::openUrl(url);
}
