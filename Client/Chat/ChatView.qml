import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0

SplitView {
     anchors.fill: parent
     orientation: Qt.Horizontal

     function sendMessage() {
//              chatsList.changeSampleData(channelMessage.text)
            Coordinator.sendChatMessage(chatsList.getItem(chatList.currentItem), channelMessage.text)
            console.log(chatList.currentItem)
         }

    Component.onCompleted: {
        Coordinator.mainWindowLoaded()
    }

    function clearMessageField() {
        channelMessage.text = "";
    }

    Connections {
        target: Coordinator
        onShowError:{
            createMessageWindow(message)
        }

        onChangeSendButtonState:{
            changeSendButtonState()
        }

        onGetChatsUserBelongError:{
            createMessageWindow(message)
        }

        onClearMessageField : {
            console.log("onClearMessageField")
            clearMessageField()
        }
    }

    function changeSendButtonState()
    {
        if(chatsList.notEmpty() === true)
            sendButton.enabled = true
        else
             sendButton.enabled = false
    }

    property var messageWindow: null
    function createMessageWindow(text) {
        if(messageWindow == null) {
            var component = Qt.createComponent("MessageBox.qml")
            messageWindow = component.createObject(authForm, {"x" : 0, "y" : 0})
            if(messageWindow !== null) {
                messageWindow.dialogMessage = text
                messageWindow.anchors.centerIn = authForm
                messageWindow.hideWindowClicked.connect(destroyMessageWindow)
            }
        }
    }

    function destroyMessageWindow() {
        if(messageWindow !== null) {
            messageWindow.destroy()
            messageWindow = null
        }
    }
/*
    property var busyIndicator: null

    function createBusyIndicator()
    {
        if(busyIndicator == null) {
            var component = Qt.createComponent("BusyIndicator.qml")
            busyIndicator = component.createObject(chatPart, {"x" : 0, "y" : 0})
            if(busyIndicator !== null) {
                busyIndicator.anchors.centerIn = chatPart
            }
        }
    }

    function destroyBusyIndocator() {
        if(busyIndicator !== null) {
            busyIndicator.destroy()
            busyIndicator = null
        }
    }

*/


     Rectangle {
         width: parent.width / 3
         Layout.maximumWidth: 400
         color: "lightblue"

         Column {
            id: mainMenu
            anchors.fill: parent
            spacing: 4
/*            Text {
                font.pixelSize: 20
                color: "#F69678"
                text: "Settings"
              }
             Button {
                 id: profileButton
                 width: parent.width
                 text: "Login"
                 onClicked:  {
                     destroyBusyIndocator()
                 }
             }
//                     console.log("Create channel")
                     createBusyIndicator()
*/
             Button {
                 id: channelsButton
                 width: parent.width
                 text: "Create channel"
                 onClicked: {
                     stackView.push("qrc:/AddChannelForm.qml")
                 }
             }
             Button {
                 id: profileButton
                 width: parent.width
                 text: "Add members"
                 onClicked:  {
                   Coordinator.prepareUsersLists(chatList.currentIndex)
//                     chatsList.getItem(chatList.currentIndex).
                     stackView.push("qrc:/AddParticipantsForm.qml")
                 }
             }
             Text {
                 id: channelsTitle
                 font.pixelSize: 20
                 color: "#F69678"
                 text: "Channels"
               }
             ListView {
                 id: chatList
                 width: parent.width
                 height: parent.height - profileButton.height - channelsButton.height - channelsTitle.height
                 spacing: 5
                  model: chatsList
                  ChatListItem {
                      id: contactsDelegate
                  }
                  delegate: contactsDelegate
              }

        }


     }
     Rectangle {
         id: chatPart
         Layout.minimumWidth: 50
         Layout.fillWidth: true
         color: "lightgray"

         Column {
            anchors.fill: parent
            spacing: 4
            Rectangle {
                id: chatInfoBlock
                width: parent.width
                radius: 5
                border.color: "black"
                height: 50
                z: 5
                color: "lightgrey"

                Column {
                    anchors.fill: parent
                    spacing: 10
                    Row {
                        spacing: 14
                        width: parent.width
/*                        Text {
                            id: channelTitle
                            text: "channel"
                          }*/
                         Text {
                            id: participants
//                            font.pixelSize: 20
                            text: "participants:"
                          }
                    }
/*                    ListView {
                        id: partList
                        width: parent.width
                        height: 20
                        orientation: ListView.Horizontal
                        layoutDirection: Qt.LeftToRight
                        spacing: 5
                        model: participantsList
                        ParticipantListItem {
                            id: participantsDelegate
                        }
                        delegate: participantsDelegate
                    }*/
                    ListView {
                        width: parent.width
                        height: 20
                        orientation: ListView.Horizontal
                        layoutDirection: Qt.LeftToRight
                        spacing: 5
                        model: members
                        ParticipantListItem {
                            id: participantsDelegate
                        }
                        delegate: participantsDelegate
                    }
  /*                  Row {
                        width: parent.width
                        Rectangle {
                            width: parent.width
                            height: findText.height + 10
                            radius: 5
                            border.color: "black"
                            Text {
                                  id:findText
                                  text: "find in this channel..."
                              }
                        }
                        }*/
                    }
            }
            ListView {
                id: conversationList
                width: parent.width
                height: parent.height - chatInfoBlock.height - messageBlock.height
//                 anchors.fill: parent
                spacing: 5
                 model: conversation
                 ConversationItem {
                     id: conversationDelegate
                 }
                 delegate: conversationDelegate
             }
            Rectangle {
                id: messageBlock
                width: parent.width
//                anchors.fill.width: parent.width
                border.color: "black"
                radius: 5
                height: 70
                z: 5
                color: "lightgrey"
                anchors {
                   left: chatPart.left
                    right: chatPart.right
                    bottom: chatPart.bottom
                    bottomMargin: 5
                }
                Row {
                    id: messageRow
//                    spacing: 4
                    anchors.fill: parent
                    TextArea {
                        id: channelMessage
                        width: messageRow.width - sendButton.width - 3
                        anchors {
                            top: messageRow.top
                            bottom: messageRow.bottom
                            topMargin: 5
                            bottomMargin: 10
                        }
//                        placeholderText: qsTr("Enter description")
//                        text: "message to channel"
                    }
                    Button {
                        id:sendButton
                        enabled: false
                        anchors {
                           top: messageRow.top
                           right: messageRow.right
                           rightMargin: 3
                           topMargin: 5
                        }
                        text: "Send";
                        onClicked: {
                            sendMessage()
                        }
                    }
                }
            }
         }
     }
/*       Rectangle {
         width: 200
         color: "lightgreen"
         Text {
             text: "View 3"
             anchors.centerIn: parent
         }
     }*/
 }
