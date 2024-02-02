import QtQuick.Controls 1.5
import QtQuick.Controls.Styles 1.4
import QtQuick 2.12
import Authentication 1.0
import "qrc:/uiutilities.js" as Utils

Rectangle {
    id: modifyParticipantsForm
    anchors.fill: parent
    color: "grey"
    property var chatTitle

    Component.onCompleted: {
        chatTitle = Coordinator.getCurrentChatTitle()
        Coordinator.copyChatParticipants()
    }


    Connections {
        target: Coordinator

        onShowError:{
             Utils.createMessageWindow(modifyParticipantsForm, message)
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
            text: "Participants modify"
            anchors.horizontalCenter: parent.horizontalCenter
    }
    Row {
        spacing: 10
        anchors.horizontalCenter: parent.horizontalCenter
        Text {
                font.pixelSize: 20
                text: "chat:"
        }
        Text {
                font.pixelSize: 20
                text: chatTitle
        }
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
                    console.log("modify chat"+chatTitle)
                    Coordinator.modifyChatParticipants(chatTitle)
                    stackView.pop()
                }
                else
                {
                    Utils.createMessageWindow(modifyParticipantsForm, Utils.notEnoughParticipants)
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

















