import QtQuick 2.12
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0

Rectangle {
    anchors.fill: parent
    color: "grey"

    Text {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 20
        font.pixelSize: 20
        text: "Settings"
    }

    Grid {
        id: profileInfo
        rows: 2
        columns: 2
        anchors.centerIn: parent
        spacing: 10

        Text {
            font.pixelSize: 20
            text: "server ip address"
        }
        Row {
            spacing: 5
            TextField {
                id: addrByte3
                width: 50
    //            text: "Name"
                placeholderText: "127"
                inputMethodHints: Qt.ImhDigitsOnly
            }
            TextInput {
                id: addrByte2
                width: 50
                validator: IntValidator {bottom: 1; top: 255}
//                placeholderText: "0"
            }
            TextInput {
                id: addrByte1
                width: 50
                validator: IntValidator {bottom: 1; top: 255}
//                placeholderText: "0"
            }
            TextInput {
                id: addrByte0
                width: 50
                validator: IntValidator {bottom: 1; top: 255}
//                placeholderText: "1"
            }
        }
        Text {
            font.pixelSize: 20
            text: "db title"
        }
        TextField {
            id: dbTitle
            placeholderText: "some title"
        }
    }
}
