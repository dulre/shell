/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:GPL2+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

import QtQuick 2.0
import Hawaii.Themes 1.0 as Themes
import org.hawaii.launcher 0.1 as Launcher

GridView {
    readonly property int iconSize: Themes.Units.iconSizes.large
    readonly property int numRows: 5
    readonly property int numColumns: 3
    readonly property int numItemsPerPage: numRows * numColumns
    readonly property int numPages: Math.ceil(grid.count / numItemsPerPage)
    property int currentPage: 0

    signal appLaunched()

    id: grid
    cellWidth: (iconSize * 1.5) + (Themes.Units.largeSpacing * 2)
    cellHeight: iconSize + Themes.Units.gu(2) + (Themes.Units.largeSpacing * 2) + Themes.Units.smallSpacing
    width: cellWidth * numRows
    height: cellHeight * numColumns
    snapMode: GridView.SnapOneRow
    flow: GridView.LeftToRight
    interactive: false
    clip: true
    preferredHighlightBegin: 0
    preferredHighlightEnd: 0
    highlightRangeMode: GridView.StrictlyEnforceRange
    highlightFollowsCurrentItem: true
    model: Launcher.AppsModel {
        id: appsModel
        onAppLaunched: grid.appLaunched()
    }
    delegate: LauncherGridDelegate {
        id: delegate
        iconSize: grid.iconSize
        width: grid.cellWidth
        height: grid.cellHeight

        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton
            hoverEnabled: true
            onEntered: delegate.hovered = true
            onExited: delegate.hovered = false
            onClicked: appsModel.trigger(index)
        }
    }
}
