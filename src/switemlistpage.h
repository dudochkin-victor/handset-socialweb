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



#ifndef SWITEMLISTPAGE_H
#define SWITEMLISTPAGE_H

#include <QObject>
#include <QMetaType>
#include <QPixmap>

#include <MApplicationPage>
#include <MLinearLayoutPolicy>
#include <MGridLayoutPolicy>
#include <MButton>
#include <MList>
#include <MPannableViewport>
#include <MTextEdit>

#include <libsocialweb-qt/swclient.h>
#include <libsocialweb-qt/swclientservice.h>
#include <libsocialweb-qt/swclientitemview.h>
#include <libsocialweb-qt/swclientitemaggregatemodel.h>
#include <libsocialweb-qt/swclientitemsortfilterproxymodel.h>

class SwItemListItem;
class SwItemPage;

class SwItemListPage : public MApplicationPage
{
Q_OBJECT
public:
    SwItemListPage();
    ~SwItemListPage();
    void createContent();

    //Quick hack to group related items together - will revisit in the future
    class SwServiceGroup {
    public:
        explicit SwServiceGroup();
        ~SwServiceGroup();
        SwClientService *service;
        SwClientItemView *view;
    };

protected:
//    void timerEvent(QTimerEvent *event);
    void resizeEvent(QGraphicsSceneResizeEvent *event);

private slots:
    void onSwItemListItemClicked(SwClientItem *item);
    void onServiceDynCapsChanged(SwClientService *service, QStringList newProps);
    void resizeViewports();
    void onClearButtonClicked();
    void onSubmitButtonClicked();
    void onStatusEditTextChanged();
    void onItemsAdded(QList<SwClientItem *> itemsAdded);
    void onItemsChanged(QList<SwClientItem *> itemsChanged);
    void onItemsRemoved(ArrayOfSwItemId itemsRemoved);
    void setItemViewWidget();

    void onReplyClicked(SwClientItem *item);
    void onRetweetClicked(SwClientItem *item);

    void showNoAccountPage();

    void onSettingsActionTriggered();
    void onRefreshActionTriggered();


private:
    void addServiceToGroups(SwClientService *service);
    void addView(SwClientItemView *view);
    void addItem(SwClientItem *);
    void changeItem(SwClientItem *);
    void removeItem(QString uuid);

    SwClient *mClient;
    QStringList mServiceNames;
    QList<SwServiceGroup *> mSvcGroups;
    QList<SwItemListItem *> mListItemsP;
    QList<SwItemListItem *> mListItemsL;
    //SwClientItemAggregateModel *mAggModel;
    //SwClientItemSortFilterProxyModel *mSortedModel;

    MLayout *mLayout;
    MLinearLayoutPolicy *mPolicy;

    MWidgetController *mStatusUpdateCnt;
    MLayout *mStatusUpdateLayout;
    MLinearLayoutPolicy *mStatusUpdatePolicy;
    MTextEdit *mStatusUpdateEdit;
    //MButton *mStatusUpdateClearButton;
    MButton *mStatusUpdateSubmitButton;


    //Since MList doesn't currently support
    //varied-height items, we have to do it by hand...
    //MList *mItemList;

    MPannableViewport *mItemViewport;
    //Portrait
    MWidgetController *mItemPCnt;
    MLayout *mItemPLayout;
    MLinearLayoutPolicy *mItemPPolicy;

    //Landscape
    //Horizontal container for mItemViewport
    MWidgetController *mItemLCnt;
    MLayout *mItemLLayout;
    MLinearLayoutPolicy *mItemLPolicy;

    //Two vertical containers in horizontal container
    MWidgetController *mItemLCntV1;
    MWidgetController *mItemLCntV2;
    MLayout *mItemLLayoutV1;
    MLayout *mItemLLayoutV2;
    MLinearLayoutPolicy *mItemLPolicyV1;
    MLinearLayoutPolicy *mItemLPolicyV2;

    SwItemPage *mItemPage;

};

Q_DECLARE_METATYPE(SwItemListPage::SwServiceGroup *);

#endif // SWITEMLISTPAGE_H
