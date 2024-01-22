import QtQuick.Controls 1.5
import QtQuick.Controls.Styles 1.4
import QtQuick 2.12
import Authentication 1.0
import "qrc:/uiutilities.js" as Utils

Rectangle {
    id: addParticipantsForm
    anchors.fill: parent
    color: "grey"
//    property var buttonText

    Connections {
        target: Coordinator

        onShowError:{
             Utils.createMessageWindow(addParticipantsForm, message)
        }
    }

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
    ParticipantList {
        id: partList
    }
 /*   Grid {
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
    }*/
    Row {
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 10
        Button {
            text: "Ok"
            onClicked: {
                if(Coordinator.hasCorrectChatParticipants() === true)
                {
                    Coordinator.modifyChatParticipants(chatsList.getItem(chatList.currentItem))
                    stackView.pop()
                }
                else
                {
                    Utils.createMessageWindow(addParticipantsForm, Utils.notEnoughParticipants)
                }
            }
        }

        Button {
            text: "Cancel"
            onClicked: {
                console.log("cancel")
                stackView.pop()
            }
        }
    }
}
}

















