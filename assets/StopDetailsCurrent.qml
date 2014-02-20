import bb.cascades 1.2
import QtQuick 1.0

Container {
    id: stopDetailsCurrentContainer
    
    property string stopNumber
    
    Label {
        text: qsTr("Current time:")
    }
    Label {
        id: currentTime
        text: Qt.formatDateTime(new Date(), "hh:mm:ss")
    }
    ListView {
        id: stopDetailsCurrentListView
        
        dataModel: app.stoptimesDataModel
        
        listItemComponents: [
            ListItemComponent {
                type: "item"
                StandardListItem {
                    title: qsTr("%1 %2").arg(ListItemData.route).arg(ListItemData.time)
                }
            }
        ]
    }
    
    onStopNumberChanged: {
        app.getNextBusTimes(stopDetailsCurrentContainer.stopNumber);
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
