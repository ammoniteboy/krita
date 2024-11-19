/*
 *  SPDX-FileCopyrightText: 2024 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KISDISPLAYCONFIG_H
#define KISDISPLAYCONFIG_H

#include <kritaui_export.h>
#include <boost/operators.hpp>
#include <KoColorConversionTransformation.h>

class KoColorProfile;
class KisConfig;

class KRITAUI_EXPORT KisDisplayConfig : public boost::equality_comparable<KisDisplayConfig>
{
public:
    KisDisplayConfig();
    KisDisplayConfig(int screen, const KisConfig &config);
    KisDisplayConfig(const KoColorProfile *_profile,
                     KoColorConversionTransformation::Intent _intent,
                     KoColorConversionTransformation::ConversionFlags _conversionFlags);
    bool operator==(const KisDisplayConfig &rhs) const;

    const KoColorProfile *profile;
    KoColorConversionTransformation::Intent intent;
    KoColorConversionTransformation::ConversionFlags conversionFlags;
};

#endif // KISDISPLAYCONFIG_H
