import QtQuick 2.0

Component {
    id: participantsDelegate
    Rectangle {
        id: wrapper
        property variant selectedItem: model
        width: participantInfo.width + 6//parent.width
        height: participantInfo.height + 6
        radius : 5
        color: ListView.isCurrentItem ? "black" : "red"
//        color: "blue"
        Text {
            id: participantInfo
//            text: name + ": " + number
            text: title
//            text: model.display
//            color: wrapper.ListView.isCurrentItem ? "red" : "black"
            anchors.verticalCenter: parent.verticalCenter
//            anchors.left: parent.left
//            anchors.leftMargin: 5
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
