/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2012-2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import Hawaii.Shell 1.0

Item {
    id: shutdownView
    implicitWidth: mainLayout.width
    implicitHeight: mainLayout.height

    signal childClicked()

    ShutdownDialog {
        id: shutdownDialog
        visible: false
    }

    RowLayout {
        id: mainLayout

        ToolButton {
            iconName: "preferences-system-symbolic"
            onClicked: {
                shutdownView.childClicked();
                ProcessLauncher.launch("hawaii-system-preferences");
            }
        }

        ToolButton {
            iconName: "changes-prevent-symbolic"
            onClicked: {
                shutdownView.childClicked();
                SessionManager.lock();
            }
        }

        ToolButton {
            iconName: "system-shutdown-symbolic"
            onClicked: {
                shutdownView.childClicked();
                shutdownDialog.visible = true;
            }
        }

        ToolButton {
            iconName: "media-playback-pause-symbolic"
            visible: false
            onClicked: {
                shutdownView.childClicked();
            }
        }
    }
}
