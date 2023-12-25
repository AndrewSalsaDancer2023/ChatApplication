import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")
    id: rootWindow
    StackView {
           id: stackView
           anchors.fill: parent
//           initialItem: "qrc:/ChatView.qml"
           initialItem: "qrc:/LoginForm.qml"
//             initialItem: "qrc:/ProfileForm.qml"
//            initialItem: "qrc:/AddChannelForm.qml"
//           initialItem: "qrc:/AddParticipantsForm.qml"
       }

}
