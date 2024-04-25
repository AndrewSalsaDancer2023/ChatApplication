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
                   if(busyIndicator !== null)
                        return;

                   busyIndicator = stackView.push("qrc:/BusyIndicator.qml")
                   busyIndicator.waitingMessage = Utils.connection
               }

               onHideBusyIndicator: {
                   if(busyIndicator !== null)
                       stackView.pop(busyIndicator);
//                  Utils.destroyBusyIndocator(busyIndicator)
               }
           }

           anchors.fill: parent
       }

}
