/*
 *  SPDX-FileCopyrightText: 2020 Scott Petrovic <scottpetrovic@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef LIBKIS_PAINTINGRESOURCES_H
#define LIBKIS_PAINTINGRESOURCES_H

#include "kis_figure_painting_tool_helper.h"

/**
 * @brief The PaintingResources namespace
 * Sets up information related to making painting strokes.
 * Used primarily in the Document class
 *
 */
namespace PaintingResources
{
    KisFigurePaintingToolHelper createHelper(KisImageWSP image);
};

#endif // LIBKIS_PAINTINGRESOURCES_H
