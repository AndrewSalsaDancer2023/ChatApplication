import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0
import "qrc:/uiutilities.js" as Utils

SplitView {
     id: mainChatWindow
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
            changeSendButtonState(chatsList.notEmpty(), channelMessage.text.length)
        }

        onGetChatsUserBelongError:{
            createMessageWindow(message)
        }

        onClearMessageField : {
            console.log("onClearMessageField")
            clearMessageField()
        }

        onScrollListToTheEnd : {
            console.log("onScrollListToTheEnd")
            conversationList.positionViewAtEnd()
        }
    }

    function changeSendButtonState(chatsListNotEmpty, chatMsgSize)
    {
        if((chatsListNotEmpty === true) &&
                (chatMsgSize >= Utils.minChatMsgLength) && (chatMsgSize <= Utils.maxChatMsgLength))
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

     Rectangle {
         width: parent.width / 3
         Layout.maximumWidth: 400
         color: "lightblue"

         Column {
            id: mainMenu
            anchors.fill: parent
            spacing: 4
             Button {
                 id: channelsButton
                 width: parent.width
                 text: "Add channel"
                 onClicked: {
                     Coordinator.prepareMembersList()
                     stackView.push("qrc:/AddChannelForm.qml")
                 }
             }
             Button {
                 id: profileButton
                 width: parent.width
                 text: "Change members"
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
//         Layout.minimumWidth: 50
         Layout.fillWidth: true
         color: "lightgray"
            Rectangle {
                id: chatInfoBlock
                width: parent.width
                radius: 5
                border.color: "black"
                height: 50
                z: 5
//                color: "lightgrey"
                color: "lightblue"
                Column {
                    width: parent.width
                    id: infoColumn
                    spacing: 5
                    Text {
                        width: parent.width
                        id: participants
                        text: "participants:"
                    }
                    ListView {
                        width: parent.width
                        height: 20
//                        height: chatInfoBlock - participants.height - infoColumn.spacing
                        orientation: ListView.Horizontal
                        layoutDirection: Qt.LeftToRight
                        spacing: 5
                        model: members
                        ParticipantListItem {
                            id: participantsDelegate
                        }
                        delegate: participantsDelegate
                    }
                }
            }

            ListView {
                id: conversationList
                width: parent.width
                anchors.top : chatInfoBlock.bottom
                height: parent.height - chatInfoBlock.height - messageBlock.height
                spacing: 5
                 model: conversation
                 ConversationItem {
                     id: conversationDelegate
                 }
                 delegate: conversationDelegate
/*
                 onMovementEnded: {
                     console.log("movement ended")
                 }

                 onAtYEndChanged: {
                     console.log("scrolled")
                     var descr = 'current index: ' + conversationList.currentIndex.toString();
                     console.log(descr)
                     if(conversationList.atYBeginning) {
                           console.log("scrolled to begin")
                     }
                     if (conversationList.atYEnd) {
                       console.log("scrolled to end")
                     }
                   }
                 */
             }
            Rectangle {
                id: messageBlock
                width: parent.width
                border.color: "black"
                color: "lightblue"
                height: 80
                radius: 5
                z: 5
//                color: "lightgrey"
                anchors {
                   left: chatPart.left
                    right: chatPart.right
                    bottom: chatPart.bottom
                }
               Column {
                   Row {
                       id: infoRow
//                       anchors.fill.width: parent.width
                       spacing: 5
                    Text {
                         text: "Message limits:"
                       }
                    Text {
                         text: {
                             return "min:" + Utils.minChatMsgLength.toString()
                         }
                       }
                    Text {
                         text: {
                             return "max:" + Utils.maxChatMsgLength.toString()
                         }
                       }
                    Text {
                         text: {
                             return "current:" + channelMessage.text.length.toString()
                         }
                       }
                   }
                Row {
                    id: messageRow
                    spacing: 4
//                    anchors.fill.width: parent.width
                    anchors {
                       left: messageBlock.left
                        right: messageBlock.right
//                        top: messageBlock.top
                        bottom: messageBlock.bottom
                    }

                    TextArea {
                        id: channelMessage
                        height: messageBlock.height - infoRow.height - 3
//                        anchors.bottom: messageBlock.bottom
                        text: "Sample message"
                        width: messageBlock.width - sendButton.width - 2*messageRow.spacing
                        onTextChanged: {
                            console.log("Text has changed to:", text)
                            changeSendButtonState(chatsList.notEmpty(), channelMessage.text.length)
                        }
                    }
                    Button {
                        id:sendButton
                        enabled: false
/*                        anchors {
                           top: messageRow.top
                           right: messageRow.right
                           rightMargin: 3
                           topMargin: 5
                        }
*/
                        text: "Send";
                        onClicked: {
                            sendMessage()
                        }
                    }
                }
            }
           }
         }
 }
