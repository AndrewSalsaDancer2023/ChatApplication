import QtQuick.Controls 1.5
import QtQuick.Controls.Styles 1.4
import QtQuick 2.12

Component {
    id: memberDelegate
    Rectangle {
        id: wrapper
//        width: contactInfo.width + addContactBtn.width + 15
        color: "lightgreen"
        height: contactInfo.height + 12
        radius : 5
        Row {
            id: infoRow
            spacing: 5
            anchors.fill: parent
//            spacing: 15
        Row {
            width: 150
            clip:true
            spacing: 5
            Text {
                id: contactInfo
                text: surname
                anchors.verticalCenter: parent.verticalCenter
            }
            Text {
                text: name
                anchors.verticalCenter: parent.verticalCenter
            }
        }
        Button {
            id: addContactBtn
            anchors.verticalCenter: parent.verticalCenter
            text: "Remove";
            onClicked: {
                console.log(nickname)
                Coordinator.removeParticipant(nickname);
            }
        }
      }
    }
}
