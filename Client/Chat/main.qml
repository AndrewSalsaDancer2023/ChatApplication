import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0
import "qrc:/uiutilities.js" as Utils

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")
    id: rootWindow
    property var busyIndicator: null
    StackView {
           id: stackView

           Connections {
               target: Coordinator
               onAuthSuccess: {
                  stackView.pop()
                  stackView.push("qrc:/ChatView.qml")
               }
/*
               onAuthError: {
                     stackView.push("qrc:/LoginForm.qml")
               }
*/
               onShowLoginForm: {
                     stackView.push("qrc:/LoginForm.qml")
               }

               onShowBusyIndicator: {
                   Utils.createBusyIndicator(stackView, busyIndicator)
               }

               onHideBusyIndicator: {
                   Utils.destroyBusyIndocator()
               }
           }


           anchors.fill: parent
//           initialItem: "qrc:/ChatView.qml"
//           initialItem: "qrc:/LoginForm.qml"
//             initialItem: "qrc:/ProfileForm.qml"
//            initialItem: "qrc:/AddChannelForm.qml"
//           initialItem: "qrc:/AddParticipantsForm.qml"
       }

}
