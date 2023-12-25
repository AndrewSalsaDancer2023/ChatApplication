import QtQuick.Controls 1.5
import QtQuick.Controls.Styles 1.4
import QtQuick 2.12

Component {
    id: memberDelegate
    Rectangle {
        id: wrapper
        width: contactInfo.width + addContactBtn.width + 15
        color: "lightgreen"
        height: contactInfo.height + 12
        radius : 5
        Row {
            id: infoRow
            anchors.fill: parent
            spacing: 15
        Text {
            id: contactInfo
            text: name
            width: 100
            clip:true
            anchors.verticalCenter: parent.verticalCenter
            color: "black"
        }
        Button {
            id: addContactBtn
            anchors.verticalCenter: parent.verticalCenter
            text: "Remove";
            onClicked: console.log("remove member")
        }
      }
    }
}
