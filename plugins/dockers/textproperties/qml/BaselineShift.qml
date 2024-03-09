/*
 *  SPDX-FileCopyrightText: 2024 Wolthera van Hövell tot Westerflier <griffinvalley@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.12

TextPropertyBase {
    GridLayout {
        columns: 3;
        columnSpacing: columnSpacing;
        width: parent.width;

        Item {
            width: firstColumnWidth;
            height: firstColumnWidth;
            ToolButton {
                id: revert;
                icon.width: 22;
                icon.height: 22;
                display: AbstractButton.IconOnly
                icon.source: "qrc:///light_view-refresh.svg"
            }
        }

        Label {
            text: "Baseline-Shift:"
            Layout.columnSpan: 2;
        }


        Item {
            width: firstColumnWidth;
            height: 1;
        }

        ComboBox {
            Layout.columnSpan: 2;
            model: ["Number", "Super", "Sub"]
            id: baselineShiftCmb;
        }

        Item {
            width: firstColumnWidth;
            height: 1;
        }
        SpinBox {
            id: baselineShiftSpn
            Layout.fillWidth: true;
        }

        ComboBox {
            model: ["Pt", "Em", "Ex"]
        }
    }
}
