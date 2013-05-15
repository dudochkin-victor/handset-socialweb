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


#ifndef SWITEMPAGE_H
#define SWITEMPAGE_H

#include <QString>
#include <QPixmap>
#include <QGraphicsSceneMouseEvent>
#include <QUrl>
#include <MApplicationPage>
#include <MWidget>
#include <MLayout>
#include <MImageWidget>
#include <MLabel>
#include <MStylableWidget>
#include <MGridLayoutPolicy>
#include <MButton>

class SwClientItem;

class SwItemPage : public MApplicationPage
{
Q_OBJECT
public:
    SwItemPage(QGraphicsItem *parent = 0);
    ~SwItemPage() {}
    void setItem(SwClientItem *swItem);

signals:
    void clicked();
    void replyClicked(SwClientItem *);
    void retweetClicked(SwClientItem *);

public slots:

private slots:
    void onReplyClicked();
    void onRetweetClicked();
    void onViewOnClicked();
    void onHideClicked();
    void onBtnOpenURL();


private:
    SwClientItem *mItem;
    MButton *mBtnReply;
    MButton *mBtnRetweet;
    MButton *mBtnViewOn;
    MButton *mBtnHide;
    QMap<MButton *, QUrl> mURLButtons;

};

#endif // SWITEMPAGE_H
