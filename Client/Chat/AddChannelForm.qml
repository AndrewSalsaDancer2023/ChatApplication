import QtQuick.Controls 1.5
import QtQuick.Controls.Styles 1.4
import QtQuick 2.12
import Authentication 1.0

Rectangle {
    anchors.fill: parent
    color: "grey"
    Column {
         anchors.centerIn: parent
         spacing: 10
    Text {
            font.pixelSize: 32
            text: "Channel creation"
            anchors.horizontalCenter: parent.horizontalCenter
    }
    Grid {
//    anchors.centerIn: parent
    rows: 2
    columns: 2
    spacing: 10
//    Row {
//        spacing: 15
        Text {
                font.pixelSize: 20
                text: "title"
        }
        TextField {
            id: channelTitle
            width: 200
            placeholderText: "new channel"
        }
//    }
//    Row {
//        spacing: 15
        Text {
                font.pixelSize: 20
                text: "description"
        }
        TextArea {
            id: channelDescription
//            placeholderText: "channel is about..."
            width: 200
            height: 50
        }
//     }
    }
    Row {
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 15
                 Button { text: "Create"; onClicked: console.log("submit") }
                 Button { text: "Cancel"; onClicked: console.log("cancellation") }
       }
    }
}
