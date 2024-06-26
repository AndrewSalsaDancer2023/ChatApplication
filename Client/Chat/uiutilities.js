.pragma library
/*
 var busyIndicator: null

function createBusyIndicator(root)
{
    if(busyIndicator == null) {
        var component = Qt.createComponent("BusyIndicator.qml")
        busyIndicator = component.createObject(root, {"x" : 0, "y" : 0})
        if(busyIndicator !== null) {
            busyIndicator.anchors.centerIn = root
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

function helloFunc() {
    console.log("Hello from javascript!!!")
}
var minChatMsgLength = 2
var maxChatMsgLength = 200

var minChatTitleLength = 8
var minChatDescrLength = 20

var notEnoughParticipants = "minimum two participants should be added"
var tooShortChatTitle = "Short chat title. Should be min 8 symbols"
var tooShortChatDescr = "Short chat description. Should be min 20 symbols"

var connection = "Connection..."
var authenticate = "Authentication..."

//var examplShortChatDescr = String.format('{0} is dead, but {1} is alive! {0} {2}', 'ASP', 'ASP.NET');

function createBusyIndicator(root, message)
{
        console.log("Show busy form!")
        var component = Qt.createComponent("BusyIndicator.qml")
        var indicator = component.createObject(root, {"x" : 0, "y" : 0})
        if(indicator !== null) {
            indicator.anchors.centerIn = root
            indicator.waitingMessage = message
            return indicator;
        }
        return null;
}

function destroyBusyIndocator(indicator) {
    if(indicator !== null) {
        console.log("Destroy busy form!")
        indicator.destroy()
        indicator = null
    }
}

var messageWindow = null

function createMessageWindow(root, text)
{
        if(messageWindow === null) {
         var component = Qt.createComponent("MessageBox.qml")
         messageWindow = component.createObject(root, {"x" : 0, "y" : 0})
         if(messageWindow !== null) {
             messageWindow.dialogMessage = text
             messageWindow.anchors.centerIn = root
             messageWindow.hideWindowClicked.connect(function(){
                 messageWindow.destroy()
                 messageWindow = null
             })
         }
     }
 }
