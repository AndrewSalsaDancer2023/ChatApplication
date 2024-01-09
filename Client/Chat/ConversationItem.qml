import QtQuick 2.0

Component {
    id: conversationDelegate
    Rectangle {
            id: wrapper
            width: parent.width
            height: textInfo.height + 20
            color: "lightblue"
            Row {
                 spacing: 5
            Image {
                width: 30
                height: 30
                source: "qrc:/profileImage.png"
            }
            Column {
                id: textInfo
                Row {
                    spacing: 5
                    Text {
                        id: conversationInfoName
                        text: user
                        color: "black"
                    }
                    Text {
                        id: conversationDate
                        text: date
                        color: "black"
                    }
                }
                Text {
                    id: conversationInfoNumber
                    text: message
                    color: "black"
                }
            }
            }
        }

/*    Rectangle {
        id: wrapper
        width: parent.width
        height: conversationInfo.height + 20
        color: "lightblue"
        Text {
            id: conversationInfo
            text: name + ": " + number
            color: "black"
        }
    }*/
}
