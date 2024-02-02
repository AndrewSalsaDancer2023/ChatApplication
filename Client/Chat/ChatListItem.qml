import QtQuick 2.0

Component {
    id: contactsDelegate
    Rectangle {
        id: wrapper
        border.width: 1
        width: parent.width
        height: contactInfo.height + 6
        radius : 5
        color: ListView.isCurrentItem ? "blue" : "lightblue"
        Text {
            id: contactInfo
//            text: name + ": " + number
            text: title
            color: wrapper.ListView.isCurrentItem ? "white" : "black"
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 5
        }

        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            onClicked: {
                chatList.currentIndex = index
                chatsList.slotSelect(contactInfo.text)//chatList.currentIndex)
            }
        }
    }
}
