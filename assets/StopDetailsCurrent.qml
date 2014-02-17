import bb.cascades 1.2
import QtQuick 1.0

Container {
    Label {
        text: qsTr("Current time:")
    }
    Label {
        id: currentTime
        text: Qt.formatDateTime(new Date(), "hh:mm:ss")
    }
    
    attachedObjects: [
        Timer {
            interval: 1000
            running: true
            repeat: true
            onTriggered: {
                currentTime.text = Qt.formatDateTime(new Date(), "hh:mm:ss")
            }
        }
    ]
    
}
