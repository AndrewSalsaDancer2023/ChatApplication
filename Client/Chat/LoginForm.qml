import QtQuick.Controls 1.5
import QtQuick.Controls.Styles 1.4
import QtQuick 2.12
import Authentication 1.0
//import Coordinator 1.0

Rectangle {
 //   anchors.fill: parent
    id: authForm
    width: parent.width / 3
    height: parent.height / 3
    color: "grey"
//    border.color: "black"


    AuthenticationForm {
        id: authentication
    }

    Connections {
        target: Coordinator
        onAuthSuccess: {
           stackView.pop()
           stackView.push("qrc:/ChatView.qml")
        }

        onAuthError: {
            createMessageWindow(message)
        }
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

    function checkAuthInfo() {
        Coordinator.setAuthenticationData(login.text, password.text, dbName.text)
    }

    Column {
        anchors.centerIn: parent
        spacing: 16
        Column {
          spacing: 4
          Text {
              font.pixelSize: 32
              color: "#F69678"
              text: "Username"
            }
          TextField {
            id: login
            //Layout.fillWidth: true
            anchors.horizontalCenter: parent.horizontalCenter
            placeholderText: "Nickname"
          }
        }
        Column {
          spacing: 4
          Text
          {
              font.pixelSize: 32
              color: "#F69678"
              text: "Password"
          }
          TextField {
            id: password
            anchors.horizontalCenter: parent.horizontalCenter
            placeholderText: "Password"
            echoMode: TextInput.Password//TextInput.PasswordEchoOnEdit
            }
        }
        Column {
          spacing: 4
          Text
          {
              font.pixelSize: 32
              color: "#F69678"
              text: "Database"
          }
          TextField {
            id: dbName
                anchors.horizontalCenter: parent.horizontalCenter
                placeholderText: "Database"
            }
        }
        Row {
          spacing: 16
          //anchors.left: parent.left
          anchors.horizontalCenter: parent.horizontalCenter
          Button {
              text: "Login";
              onClicked: {
                  checkAuthInfo()
              }
          }
       //   Button { text: "Guest"; onClicked: console.log("guest") }
        }
    }
}
//https://www.youtube.com/watch?v=9BcAYDlpuT8
//https://www.youtube.com/watch?v=0Xl3tHynVu4
//https://www.youtube.com/@Marko-Qt
