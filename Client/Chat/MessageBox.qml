import QtQuick.Controls 1.5
import QtQuick.Controls.Styles 1.4
import QtQuick 2.12

    Rectangle {
        id: messageBox
        signal hideWindowClicked()
        property var dialogMessage
        width: dialogContainer.width + 10
        height: dialogContainer.height + 20
        anchors.centerIn: parent
        color: "lightgrey"
        Column {
          id: dialogContainer
          spacing: 4
          anchors.centerIn: parent
          Text
          {
              id: textCaption
              font.pixelSize: 20
              color: "#F69678"
              text: dialogMessage
          }
          Button {
              anchors.horizontalCenter: parent.horizontalCenter
              id: buttonAction
              text: "Ok";
              onClicked: {
                  messageBox.hideWindowClicked()
              }
          }
        }
}
