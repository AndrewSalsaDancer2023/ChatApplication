import QtQuick 2.0

Component {
    id: participantsDelegate
    Rectangle {
        id: wrapper
        property variant selectedItem: model
        width: participantInfo.width + 6//parent.width
        height: participantInfo.height + 6
        radius : 5
        color: "lightblue"
        Row {
            id: participantInfo
            spacing: 5
            Text {
                text: surname
                anchors.verticalCenter: parent.verticalCenter
            }
            Text {
                text: name
                anchors.verticalCenter: parent.verticalCenter
            }
        }
/*
        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            onClicked: {
                chatList.currentIndex = index
                chatsList.slotSelect(chatList.currentIndex)
            }
        }*/
    }
}
