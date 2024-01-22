import QtQuick.Controls 1.5
import QtQuick.Controls.Styles 1.4
import QtQuick 2.12
import Authentication 1.0

Rectangle {
    anchors.fill: parent
    anchors.centerIn: parent
    color: "grey"
    Column {
         anchors.centerIn: parent
         anchors.fill: parent
         spacing: 10
    Text {
            font.pixelSize: 32
            text: "Channel creation"
            anchors.horizontalCenter: parent.horizontalCenter
    }
    Grid {
    anchors.horizontalCenter: parent.horizontalCenter
    rows: 2
    columns: 2
    spacing: 10
        Text {
                font.pixelSize: 20
                text: "title"
        }
        TextField {
            id: channelTitle
            width: 200
            placeholderText: "new channel"
        }
/*
        Text {
                font.pixelSize: 20
                text: "description"
        }
        TextArea {
            id: channelDescription
            width: 200
            height: 50
        }
*/
    }
    Rectangle {
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width * 0.8
        height: parent.height / 2
        color: "lightgrey"
        ParticipantList {
            id: partList10
        }
    }
    Row {
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 15
                 Button {
                     text: "Create";
                     onClicked: {
                         if(channelTitle.text.length < Utils.minChatTitleLength)
                         {
                            Utils.createMessageWindow(addParticipantsForm, Utils.tooShortChatTitle)
                             return
                         }
/*
                         if(channelDescription.text.length < Utils.minChatDescrLength)
                         {
                             Utils.createMessageWindow(addParticipantsForm, Utils.tooShortChatDescr)
                              return
                         }
*/
                         if(Coordinator.hasCorrectChatParticipants() === true)
                         {
                             Coordinator.createChat(channelTitle, channelDescription)
                             stackView.pop()
                         }
                         else
                         {
                             Utils.createMessageWindow(addParticipantsForm, Utils.notEnoughParticipants)
                         }
                     }
                 }
                 Button {
                     text: "Cancel";
                     onClicked: {
                         console.log("cancellation")
                         stackView.pop()
                     }
                 }
       }
    }
}
