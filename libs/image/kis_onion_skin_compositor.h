/*
 *  SPDX-FileCopyrightText: 2015 Jouni Pentikäinen <joupent@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_ONION_SKIN_COMPOSITOR_H
#define KIS_ONION_SKIN_COMPOSITOR_H

#include "kis_types.h"
#include "kritaimage_export.h"

#include <QObject>

class KRITAIMAGE_EXPORT KisOnionSkinCompositor : public QObject
{
    Q_OBJECT

public:
    KisOnionSkinCompositor();
    ~KisOnionSkinCompositor() override;
    static KisOnionSkinCompositor *instance();

    void composite(const KisPaintDeviceSP sourceDevice, KisPaintDeviceSP targetDevice, const QRect &rect);

    QRect calculateFullExtent(const KisPaintDeviceSP device);
    QRect calculateExtent(const KisPaintDeviceSP device, int time);
    QRect calculateExtent(const KisPaintDeviceSP device);

    QRect updateExtentOnAddition(const KisPaintDeviceSP device, int addedTime);

    int configSeqNo() const;

    void setColorLabelFilter(QSet<int> colors);
    QSet<int> colorLabelFilter();

public Q_SLOTS:
    void configChanged();

Q_SIGNALS:
    void sigOnionSkinChanged();

private:
    struct Private;
    QScopedPointer<Private> m_d;

};

#endif
