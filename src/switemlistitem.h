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



#ifndef SWITEMLISTITEM_H
#define SWITEMLISTITEM_H

#include <QString>
#include <QPixmap>
#include <QGraphicsSceneMouseEvent>
#include <QUrl>
#include <MStylableWidget>
#include <MWidget>
#include <MLayout>
#include <MImageWidget>
#include <MLabel>
#include <MStylableWidget>
#include <MGridLayoutPolicy>
#include <MAction>

#include "switemlistitemstyle.h"

class SwClientItem;

class SwItemListItem : public MStylableWidget
{
Q_OBJECT
public:
    SwItemListItem(MWidget *parent = 0);
    ~SwItemListItem() {}
    void setItem(SwClientItem *swItem);
    SwClientItem * getItem();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent*);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent*);
    void cancelEvent(MCancelEvent*);

signals:
    void clicked(SwClientItem *);
    void replyClicked(SwClientItem *);
    void retweetClicked(SwClientItem *);

public slots:

private slots:
    void onReplyClicked();
    void onRetweetClicked();
    void onViewOnClicked();
    void onHideClicked();
    void onActOpenURL();


private:
    SwClientItem *mItem;
    MAction *mActReply;
    MAction *mActRetweet;
    MAction *mActViewOn;
    MAction *mActHide;
    QMap<MAction *, QUrl> mURLActions;

    bool mPressed;

    M_STYLABLE_WIDGET(SwItemListItemStyle);

};

#endif // SWITEMLISTITEM_H
