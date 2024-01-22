import QtQuick 2.0

//Item {
    Grid {
        id: userLists
        rows: 2
        columns: 2
        spacing: 5
        width: parent.width
        height: parent.height - 32//captionTitle.height

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
            height: parent.height - 32//captionTitle.height
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
            height: parent.height - 32//captionTitle.height
            spacing: 5
            model: participants
            MemberItem {
                id: memberDelegate
            }
            delegate: memberDelegate
         }
    }
//}
