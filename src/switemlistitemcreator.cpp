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


#include "switemlistitemcreator.h"

#include <libsocialweb-qt/swclientitem.h>

void SwItemListItemCreator::updateCell(const QModelIndex &index, MWidget *cell) const
{
    if (!index.isValid() || !cell)
        return;
    SwItemListItem *swListItem = qobject_cast<SwItemListItem *>(cell);

    QVariant data = index.data(Qt::UserRole);//SwClientItem *
    SwClientItem *item = data.value<SwClientItem *>();

    swListItem->setItem(item);
    SwParams props = item->getSwItemProps();
//    qDebug() << "Item props: ";
//    qDebug() << props;
}
