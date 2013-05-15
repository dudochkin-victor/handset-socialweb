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



#ifndef SWITEMLISTITEMCREATOR_H
#define SWITEMLISTITEMCREATOR_H

#include "switemlistitem.h"

#include <MAbstractCellCreator>
#include <MWidget>

class SwItemListItemCreator : public MAbstractCellCreator<SwItemListItem>
{
public:
    void updateCell(const QModelIndex &, MWidget *) const;
};

#endif // SWITEMLISTITEMCREATOR_H
