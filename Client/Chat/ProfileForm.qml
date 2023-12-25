import QtQuick.Controls 1.5
import QtQuick.Controls.Styles 1.4
import QtQuick 2.12
import Authentication 1.0

Rectangle {
    anchors.fill: parent
    color: "grey"
Item {
    width: 200
    height: 200
    anchors.centerIn: parent
    Grid {
        id: profileInfo
        rows: 4
        columns: 2
        anchors.centerIn: parent
        spacing: 10

        Text {
            font.pixelSize: 20
            text: "name"
        }
        TextField {
            id: name
//            placeholderText: "name"
            text: "name"
            focus: true
        }

        Text {
            font.pixelSize: 20
            text: "surname"
        }
        TextField {
            id: surname
            placeholderText: "surname"
        }

        Text {
            font.pixelSize: 20
            text: "email"
        }
        TextField {
            id: email
            placeholderText: "email"
        }

        Text {
            font.pixelSize: 20
            text: "nickname"
        }
        TextField {
            id: nickname
            text: "nickname"
            Rectangle {
                anchors.fill: parent
                color: "lightgrey"
            }
            Text {
//                anchors.fill: parent
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                anchors.leftMargin: 5
                text: "nickname"
            }
        }
    }
    Row {
        anchors.horizontalCenter: profileInfo.horizontalCenter
        anchors.top: profileInfo.bottom
        anchors.topMargin: 10
        spacing: 15
             Button { text: "Submit"; onClicked: console.log("submit") }
             Button { text: "Cancel"; onClicked: console.log("cancellation") }
   }
}
}
