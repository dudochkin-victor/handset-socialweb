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



#ifndef SWITEMLISTITEMSTYLE_H
#define SWITEMLISTITEMSTYLE_H

#include <QObject>
#include <MStyle>
#include <MWidgetStyle>

class M_EXPORT SwItemListItemStyle : public MWidgetStyle
{
    Q_OBJECT
    M_STYLE(SwItemListItemStyle)

};

class M_EXPORT SwItemListItemStyleContainer : public MWidgetStyleContainer
{
    M_STYLE_CONTAINER(SwItemListItemStyle);
    M_STYLE_MODE(Read);
    M_STYLE_MODE(Pressed);
};

#endif // SWITEMLISTITEMSTYLE_H
