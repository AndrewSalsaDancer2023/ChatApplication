import QtQuick.Controls 1.5
import QtQuick.Controls.Styles 1.4
import QtQuick 2.12
import Authentication 1.0

Rectangle {
    id: addPartForm
    anchors.fill: parent
    color: "grey"
    property var buttonText
    Column {
         anchors.centerIn: parent
         width: parent.width * .8
         height: parent.height / 2
         spacing: 10
    Text {
            id: captionTitle
            font.pixelSize: 32
            text: "Participants adding"
            anchors.horizontalCenter: parent.horizontalCenter
    }
    Grid {
        id: userLists
        rows: 2
        columns: 2
        spacing: 5
        width: parent.width
        height: parent.height - captionTitle.height

        Text {
            z: 10
            font.pixelSize: 20
            text: "participants"
        }
        Text {
            z: 10
            font.pixelSize: 20
            text: "members"
        }

        ListView {
            id: participantsList
            width: parent.width / 2
            height: parent.height - captionTitle.height
            spacing: 5
            model: allUsers
            ParticipantItem {
                id: participantDelegate
            }
            delegate: participantDelegate
         }
        ListView {
            id: membersList
            width: parent.width / 2
            height: parent.height - captionTitle.height
            spacing: 5
            model: participants
            MemberItem {
                id: memberDelegate
            }
            delegate: memberDelegate
         }
    }
    Row {
        anchors.horizontalCenter: parent.horizontalCenter
//        anchors.top: userLists.bottom
//        anchors.topMargin: 10
        spacing: 10

        Button {
//            anchors.horizontalCenter: parent.horizontalCenter
            text: "Ok"
            onClicked: {
                if(Coordinator.hasCorrectChatParticipants() === true)
                {
                    console.log("submit")
                    stackView.pop()
                }
            }
        }

        Button {
//            anchors.horizontalCenter: parent.horizontalCenter
            text: "Cancel"
            onClicked: {
                console.log("cancel")
                stackView.pop()
            }
        }
    }


/*    Row {
        anchors.horizontalCenter: parent.horizontalCenter
//        anchors.top: userLists.bottom
//        anchors.topMargin: 10
        spacing: 15
             Button { text: "Ok"; onClicked: console.log("submit") }
             Button { text: "Cancel"; onClicked: console.log("cancellation") }
   }*/
}
}

















