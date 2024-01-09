import QtQuick.Controls 1.5
import QtQuick.Controls.Styles 1.4
import QtQuick 2.12

Rectangle {
   id: waitingIndicator
   width: parent.width
   height: parent.height
//    width: dialogContainer.width + 20
//    height: dialogContainer.height + 20
    anchors.centerIn: parent
//    color: "lightgrey"
//    color: "lightblue"
      color: "#B4525252"
   Column {
        id: waitingContainer
        spacing: 4
        anchors.centerIn: parent
        BusyIndicator {
            running: true
        }
        Text
        {
            id: textCaption
            font.pixelSize: 20
            anchors.horizontalCenter: parent.horizontalCenter
//            color: "#F69678"
            text: "Waiting..."
        }

    }
   MouseArea {
       anchors.fill: parent
       cursorShape: Qt.PointingHandCursor
       onClicked: {

       }
   }
}
