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


#include "switemlistpage.h"
#include "switemlistitemcreator.h"
#include "switemlistitem.h"
#include "nocfgsvcpage.h"
#include "switempage.h"

#include <QDebug>
#include <QGraphicsItem>
#include <MWidgetCreator>
#include <MSceneManager>
#include <MAction>
#include <DuiControlPanelIf>

#include <libsocialweb-qt/swclientitem.h>
#include <libsocialweb-qt/swclientserviceconfig.h>

M_REGISTER_WIDGET(SwItemListPage);

bool swItemListItemLessThan(SwItemListItem *one, SwItemListItem *two)
{
    return (one->getItem()->getDateTime() < two->getItem()->getDateTime());
}

SwItemListPage::SwServiceGroup::SwServiceGroup() :
        service(0),
        view(0)
{
}

SwItemListPage::SwServiceGroup::~SwServiceGroup()
{
    delete service;
    delete view;
}



SwItemListPage::SwItemListPage() :
    MApplicationPage(),
    mClient(new SwClient(QDBusConnection::sessionBus(), this)),
    //mAggModel(new SwClientItemAggregateModel(this)),
    //mSortedModel(new SwClientItemSortFilterProxyModel(this)),
    mStatusUpdateCnt(new MWidgetController(this)),
    mStatusUpdateLayout(new MLayout(mStatusUpdateCnt)),
    mStatusUpdatePolicy(new MLinearLayoutPolicy(mStatusUpdateLayout, Qt::Horizontal)),
    mItemPage(new SwItemPage())
{
/*    mSortedModel->setSourceModel(mAggModel);
    mSortedModel->setSort(SwClientItemSortFilterProxyModel::SORT_RCVDTIME_DESC);
    mSortedModel->sort(0);*/
    connect(mItemPage,
            SIGNAL(replyClicked(SwClientItem*)),
            this,
            SLOT(onReplyClicked(SwClientItem*)));
    connect(mItemPage,
            SIGNAL(retweetClicked(SwClientItem*)),
            this,
            SLOT(onRetweetClicked(SwClientItem*)));

}

SwItemListPage::~SwItemListPage()
{
    foreach (SwServiceGroup *group, mSvcGroups) {
        if (group->view) {
            group->view->stopView();
            group->view->closeView();
        }
        delete group;
    }
}

void SwItemListPage::createContent()
{
    //% "Status - updates"
    setTitle(qtTrId("page_title_status_updates"));
    MApplicationPage::createContent();
    setPannable(false);

    bool haveCfg = false;
    foreach (QString sName, mClient->getServices()) {
        SwClientService *service = mClient->getService(sName);
        //We only care about services that we can get an ItemView from
        if (service->hasOpenView()) {
            if (service->isConfigured()) {
                haveCfg = true;
                addServiceToGroups(service);
            }
            connect(service,
                    SIGNAL(DynCapsChanged(SwClientService*,QStringList)),
                    this,
                    SLOT(onServiceDynCapsChanged(SwClientService *, QStringList)));
        }
    }

    mStatusUpdateEdit = new MTextEdit(MTextEditModel::MultiLine);
    mStatusUpdateEdit->setObjectName("SwItemListPageStatusUpdateEdit");
    //% "New status..."
    mStatusUpdateEdit->setPrompt(qtTrId("edit_prompt_new_status"));
    mStatusUpdateEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mStatusUpdatePolicy->addItem(mStatusUpdateEdit, Qt::AlignTop);

/*    mStatusUpdateClearButton = new MButton("icon-m-framework-close-alt", "");
    mStatusUpdateClearButton->setObjectName("SwItemListPageStatusUpdateClearButton");
    mStatusUpdateClearButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    mStatusUpdatePolicy->addItem(mStatusUpdateClearButton, Qt::AlignTop);
*/
    mStatusUpdateSubmitButton = new MButton("icon-m-toolbar-send", "");
    mStatusUpdateSubmitButton->setObjectName("SwItemListPageStatusUpdateSubmitButton");
    mStatusUpdateSubmitButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    mStatusUpdatePolicy->addItem(mStatusUpdateSubmitButton, Qt::AlignTop);

    connect(mStatusUpdateEdit,
            SIGNAL(textChanged()),
            this,
            SLOT(onStatusEditTextChanged()));
    connect(mStatusUpdateEdit,
            SIGNAL(returnPressed()),
            this,
            SLOT(onSubmitButtonClicked()));
/*    connect(mStatusUpdateClearButton,
            SIGNAL(clicked()),
            this,
            SLOT(onClearButtonClicked()));*/
    connect(mStatusUpdateSubmitButton,
            SIGNAL(clicked()),
            this,
            SLOT(onSubmitButtonClicked()));


    mLayout = new MLayout();
    mPolicy = new MLinearLayoutPolicy(mLayout, Qt::Vertical);

    //Begin mItemList pannable area...
    mItemViewport = new MPannableViewport();
    mItemViewport->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mItemViewport->setObjectName("SwItemListPageViewport");

/*    mItemList = new MList;
    mItemList->setViewType("fast");
    mItemList->setObjectName("SwItemListPageItemList");
    mItemList->setCellCreator(new SwItemListItemCreator());
    mItemList->setSizePolicy(QSizePolicy::Expanding,
                               QSizePolicy::Fixed);
    mItemList->setItemModel(mSortedModel);

    connect(mItemList,
            SIGNAL(itemClicked(QModelIndex)),
            this,
            SLOT(onSwItemListItemClicked(const QModelIndex &)));
    mViewport->setWidget(mItemList);
*/
    mItemPCnt = new MWidgetController(mItemViewport);
    mItemPCnt->setObjectName("SwItemListPageItemPCnt");
    mItemPLayout = new MLayout(mItemPCnt);
    mItemPPolicy = new MLinearLayoutPolicy(mItemPLayout, Qt::Vertical);
    mItemPCnt->setLayout(mItemPLayout);

    mItemLCnt = new MWidgetController(mItemViewport);
    mItemLCnt->setObjectName("SwItemListPageItemLCnt");
    mItemLLayout = new MLayout(mItemLCnt);
    mItemLPolicy = new MLinearLayoutPolicy(mItemLLayout, Qt::Horizontal);
    mItemLCnt->setLayout(mItemLLayout);

    //First column in Landscape mode
    mItemLCntV1 = new MWidgetController(mItemLCnt);
    mItemLCntV1->setObjectName("SwItemListPageItemLCntV1");
    mItemLLayoutV1 = new MLayout(mItemLCntV1);
    mItemLPolicyV1 = new MLinearLayoutPolicy(mItemLLayoutV1, Qt::Vertical);
    mItemLCntV1->setLayout(mItemLLayoutV1);

    //Second column in Landscape mode
    mItemLCntV2 = new MWidgetController(mItemLCnt);
    mItemLCntV2->setObjectName("SwItemListPageItemLCntV2");
    mItemLLayoutV2 = new MLayout(mItemLCntV2);
    mItemLPolicyV2 = new MLinearLayoutPolicy(mItemLLayoutV2, Qt::Vertical);
    mItemLCntV2->setLayout(mItemLLayoutV2);

    mItemLPolicy->addItem(mItemLCntV1);
    mItemLPolicy->addItem(mItemLCntV2);


    //End mItemList pannable area...

    mPolicy->addItem(mStatusUpdateCnt);
    mPolicy->addItem(mItemViewport);

    mLayout->setPolicy(mPolicy);
    centralWidget()->setLayout(mLayout);
    setItemViewWidget();

    connect(this->sceneManager(),
            SIGNAL(orientationChanged(M::Orientation)),
            this,
            SLOT(setItemViewWidget()));
    //% "Settings"
    MAction *actSettings = new MAction(qtTrId("action_settings"), this);
    actSettings->setLocation(MAction::ApplicationMenuLocation);
    addAction(actSettings);
    connect(actSettings,
            SIGNAL(triggered()),
            this,
            SLOT(onSettingsActionTriggered()));

    //% "Refresh"
    MAction *actRefresh = new MAction(qtTrId("action_refresh"), this);
    actRefresh->setLocation(MAction::ApplicationMenuLocation);
    addAction(actRefresh);
    connect(actRefresh,
            SIGNAL(triggered()),
            this,
            SLOT(onRefreshActionTriggered()));

    //This is ugly - need to refactor whole app to real MVC structure.
    //Until then, we trigger display of the NoCfgSvcPage with a QTimer
    //in order to avoid weirdness (causes a black page w/ a
    //non-responsive close button if we directly show it here...)
    if (!haveCfg) {
        QTimer::singleShot(1, this, SLOT(showNoAccountPage()));
    }
}

//Protected functions
void SwItemListPage::resizeEvent(QGraphicsSceneResizeEvent *event)
{
    MApplicationPage::resizeEvent(event);
    resizeViewports();
}

//Private slots

void SwItemListPage::onSwItemListItemClicked(SwClientItem *item)
{
    mItemPage->setItem(item);
    mItemPage->appear();
}

void SwItemListPage::onServiceDynCapsChanged(SwClientService *service, QStringList newProps)
{
    Q_UNUSED(newProps);
    SwServiceGroup *theGroup = 0;

    if (service->isConfigured()) {
        foreach (SwServiceGroup *group, mSvcGroups) {
            if (group->service == service) {
                theGroup = group;
                break;
            }
        }
        //TODO - see if we can simplify this, or at least refactor it...
        //If we found it in mSvcGroups
        if (theGroup) {
            if (service->hasVerifyCreds()) {
                if (service->credsState() == SwClientService::CredsValid) {
                    //If our creds are valid, make sure we have a view and a model, and start it
                    if (!theGroup->view) {
                        theGroup->view = theGroup->service->openView("feed");
                        //mAggModel->addView(theGroup->view);
                        //mSortedModel->sort(0);
                        addView(theGroup->view);
                    }
                    theGroup->view->startView();
                } else {
                    //If we have invalid creds, and a valid view, make sure the view is stopped and closed,
                    //then delete the view
                    if (theGroup->view) {
                        theGroup->view->stopView();
                        theGroup->view->closeView();
                        delete theGroup->view;
                        theGroup->view = 0;
                    }
                }
            } else if (!theGroup->view) {
                //If we can't verify creds, just assume we're good, and make sure we have a valid/started view/model
                if (!theGroup->view) {
                    theGroup->view = theGroup->service->openView("feed");
                    //mAggModel->addView(theGroup->view);
                    addView(theGroup->view);
                }
                theGroup->view->startView();
            }
        } else {
            //If we don't have theGroup, add a new one:
            addServiceToGroups(service);
        }
    } else {
        //If ! isConfigured(), remove it from mSvcGroups if it exists
        foreach (SwServiceGroup *group, mSvcGroups) {
            if (group->service == service) {
                group->view->stopView();
                group->view->closeView();
                mSvcGroups.removeAll(group);
                delete group;
            }
        }
    }
}

void SwItemListPage::resizeViewports()
{
    if (mItemViewport && mStatusUpdateCnt) {
        //If this is 0, we haven't actually display yet, reschedule
        if (this->centralWidget()->geometry().top() == 0) {
            QTimer::singleShot(50, this, SLOT(resizeViewports()));
            return;
        }

        qreal nh = this->size().height();
        //qDebug() << QString("this->size: %1").arg(nh);
        nh -= this->centralWidget()->geometry().top();
        //qDebug() << QString("this->size - cW->size: %1").arg(nh);

        //Subtract the Account Status combobox height for all views
        nh -= mStatusUpdateCnt->size().height();
        //qDebug() << QString("after mStatusUpdateCnt height: %1").arg(nh);

        if (nh < 0)
            nh = 0;

//        qDebug() << QString("Setting mViewport height to: %1").arg(nh);
        mItemViewport->setMinimumHeight(nh);
        mItemViewport->setMaximumHeight(nh);
        mItemViewport->setPreferredHeight(nh);

    }
}

void SwItemListPage::onClearButtonClicked()
{
    mStatusUpdateEdit->clear();
    onStatusEditTextChanged();
}

void SwItemListPage::onSubmitButtonClicked()
{
    //TODO - see if there's a better UX than updating all statuses...
    foreach (SwServiceGroup *group, mSvcGroups) {
        if (group->service->canUpdateStatus()) {
            group->service->updateStatus(mStatusUpdateEdit->text());
        }
    }
    mStatusUpdateEdit->clear();
    onStatusEditTextChanged();
}

void SwItemListPage::onStatusEditTextChanged()
{
    bool set = !mStatusUpdateEdit->text().isEmpty();
    //mStatusUpdateClearButton->setEnabled(set);
    mStatusUpdateSubmitButton->setEnabled(set);
}

void SwItemListPage::onItemsAdded(QList<SwClientItem *> itemsAdded) {
    foreach (SwClientItem *item, itemsAdded) {
        addItem(item);
/*        qDebug("Received a new item!");
        qDebug() << QString("\tService: %1").arg(item->getServiceName());
        qDebug() << QString("\tUUID: %1").arg(item->getSwUUID());
        qDebug() << QString("\tDate: %1").arg(item->getDateTime().toString());
        qDebug() << QString("\tType: %1").arg(item->getItemType());
        qDebug() << QString("\tCached: %1").arg(item->getCached());
        qDebug("\tProps:");
        QHash<QString, QString> propsHash = item->getSwItemProps();
        QHash<QString, QString>::iterator i;
        for (i = propsHash.begin(); i != propsHash.end(); ++i) {
            qDebug() << QString("\t\t%1 - %2").arg(i.key(), i.value());
        }
        qDebug("---------------------------------\n");*/
    }
    //We're not guaranteed to get items in-order from lsw,
    //so we have to do our own sort, then clear/re-add items
    //in the correct order for both Portrait and Landscape views...
    qSort(mListItemsP.begin(), mListItemsP.end(), swItemListItemLessThan);
    QGraphicsItem *item;
    while (!mItemPLayout->isEmpty()) {
        if ((item = mItemPLayout->takeAt(0)->graphicsItem()))
            item->hide();
    }
    foreach(SwItemListItem *listItem, mListItemsP) {
        mItemPPolicy->insertItem(0, listItem);
        listItem->show();
    }

    /*    if (mItemLLayoutV1->count() > mItemLLayoutV2->count()) {
            mItemLPolicyV2->insertItem(0, listItemL);
        } else {
            mItemLPolicyV1->insertItem(0, listItemL);
        }
        */
    qSort(mListItemsL.begin(), mListItemsL.end(), swItemListItemLessThan);
    while (!mItemLLayoutV1->isEmpty()) {
        if ((item = mItemLLayoutV1->takeAt(0)->graphicsItem()))
            item->hide();
    }
    while (!mItemLLayoutV2->isEmpty()) {
        if ((item = mItemLLayoutV2->takeAt(0)->graphicsItem()))
            item->hide();
    }
    //Had to swap this to start w/ false, in order to get the
    //correct left/right order - not sure why - seems like
    //there's an error in the initial page layout, but I'm
    //not seeing it quickly, and quickly is all I've got...
    bool left = false;
    foreach(SwItemListItem *listItem, mListItemsL) {
        if (left)
            mItemLPolicyV1->insertItem(0, listItem);
        else
            mItemLPolicyV2->insertItem(0, listItem);
        left = !left;
        listItem->show();
    }


}

void SwItemListPage::onItemsChanged(QList<SwClientItem *> itemsChanged)
{
    foreach (SwClientItem *item, itemsChanged)
        changeItem(item);
}

void SwItemListPage::onItemsRemoved(ArrayOfSwItemId itemsRemoved)
{
    foreach (SwItemId itemID, itemsRemoved) {
        removeItem(itemID.uuid);
    }
}

void SwItemListPage::setItemViewWidget()
{
    if (this->sceneManager()->orientation() == M::Portrait) {
        mItemViewport->setWidget(mItemPCnt);
        mItemLCnt->hide();
        mItemPCnt->show();
    } else {
        mItemViewport->setWidget(mItemLCnt);
        mItemPCnt->hide();
        mItemLCnt->show();
    }
    resizeViewports();
}

void SwItemListPage::onReplyClicked(SwClientItem *item)
{
    mStatusUpdateEdit->setText(QString("@%1").arg(item->getAuthorID()));
    mStatusUpdateEdit->setFocus();
}

void SwItemListPage::onRetweetClicked(SwClientItem *item)
{
    mStatusUpdateEdit->setText(QString("RT %1").arg(item->getContent()));
    mStatusUpdateEdit->setFocus();
}

void SwItemListPage::showNoAccountPage()
{
    NoCfgSvcPage *noCfgSvcPage = new NoCfgSvcPage();
    noCfgSvcPage->appear(MSceneWindow::DestroyWhenDone);
}

void SwItemListPage::onSettingsActionTriggered()
{
    DuiControlPanelIf *dcpIf = new DuiControlPanelIf();
    dcpIf->appletPage("SocialWebApplet");
}

void SwItemListPage::onRefreshActionTriggered()
{
    foreach (SwServiceGroup *swGroup, mSvcGroups) {
        swGroup->view->refreshView();
    }
}

//Private functions

void SwItemListPage::addServiceToGroups(SwClientService *service)
{
    if (service->hasOpenView() && service->isConfigured()) {
        //For now, only show the service if the creds are valid.
        if (service->hasVerifyCreds()
            && (service->credsState() != SwClientService::CredsValid))
            return;

        //For now, only show the service if we can successfully get a "feed" view
        SwClientItemView *view;
//        if (service->getServiceName() == "flickr")
//            view = service->openView("own");
//        else
        view = service->openView("feed");
        if (!view)
            return;

        //Have to read the service .keys file to get the Display Name and Icon path
        //SwClientServiceConfig *config = service->getServiceConfig();
        SwServiceGroup *swServiceGroup = new SwServiceGroup();
/*        if (config->isValid()) {
            swServiceGroup->svcDisplayName = config->getDisplayName();
            swServiceGroup->svcIcon = new QPixmap(config->getIconPath());
        } else {
            //If we couldn't read the service .keys file for some reason,
            //just use the service name from libsocialweb, and
            //don't set an icon
//            swServiceGroup->svcDisplayName = service->getServiceName();
        }*/
        swServiceGroup->service = service;
        swServiceGroup->view = view;
        mSvcGroups.append(swServiceGroup);
        //mAggModel->addView(swServiceGroup->view);
        addView(view);
/*        relayoutServiceButtons();
        connect(swServiceGroup->button,
                SIGNAL(clicked()),
                this,
                SLOT(onServiceButtonClicked()));*/
/*
        connect(swServiceGroup->view,
                SIGNAL(ItemsAdded(QList<SwClientItem*>)),
                this,
                SLOT(onItemsAdded(QList<SwClientItem *>)));*/
        swServiceGroup->view->startView();
    }
}

void SwItemListPage::addView(SwClientItemView *view)
{
    connect(view,
            SIGNAL(ItemsAdded(QList<SwClientItem*>)),
            this,
            SLOT(onItemsAdded(QList<SwClientItem*>)));
    connect(view,
            SIGNAL(ItemsChanged(QList<SwClientItem*>)),
            this,
            SLOT(onItemsChanged(QList<SwClientItem*>)));
    connect(view,
            SIGNAL(ItemsRemoved(ArrayOfSwItemId)),
            this,
            SLOT(onItemsRemoved(ArrayOfSwItemId)));
}

void SwItemListPage::addItem(SwClientItem *item)
{
    //TODO: Currently we don't sort by date/time
    SwItemListItem *listItemP = new SwItemListItem(this);
    listItemP->setItem(item);
    mListItemsP.append(listItemP);

    connect(listItemP,
            SIGNAL(clicked(SwClientItem*)),
            this,
            SLOT(onSwItemListItemClicked(SwClientItem*)));
    connect(listItemP,
            SIGNAL(replyClicked(SwClientItem*)),
            this,
            SLOT(onReplyClicked(SwClientItem*)));
    connect(listItemP,
            SIGNAL(retweetClicked(SwClientItem*)),
            this,
            SLOT(onRetweetClicked(SwClientItem*)));

    //We can't have the same List Item in both of the layouts - doesn't work!
    //So we have to maintain 2 separate lists of items. *Very* ugly...
    SwItemListItem *listItemL = new SwItemListItem(this);
    listItemL->setItem(item);
    mListItemsL.append(listItemL);

    connect(listItemL,
            SIGNAL(clicked(SwClientItem*)),
            this,
            SLOT(onSwItemListItemClicked(SwClientItem*)));
    connect(listItemL,
            SIGNAL(replyClicked(SwClientItem*)),
            this,
            SLOT(onReplyClicked(SwClientItem*)));
    connect(listItemL,
            SIGNAL(retweetClicked(SwClientItem*)),
            this,
            SLOT(onRetweetClicked(SwClientItem*)));
}

void SwItemListPage::changeItem(SwClientItem *item)
{
    foreach (SwItemListItem *listItem, mListItemsP) {
        if (listItem->getItem()->getSwUUID() == item->getSwUUID()) {
            listItem->setItem(item);
            break;
        }
    }
    foreach (SwItemListItem *listItem, mListItemsL) {
        if (listItem->getItem()->getSwUUID() == item->getSwUUID()) {
            listItem->setItem(item);
            break;
        }
    }

}

void SwItemListPage::removeItem(QString uuid)
{
    foreach (SwItemListItem *listItem, mListItemsP) {
        if (listItem->getItem()->getSwUUID() == uuid) {
            mListItemsP.removeAll(listItem);
            delete listItem;
            break;
        }
    }
    foreach (SwItemListItem *listItem, mListItemsL) {
        if (listItem->getItem()->getSwUUID() == uuid) {
            mListItemsL.removeAll(listItem);
            delete listItem;
            break;
        }
    }
}
