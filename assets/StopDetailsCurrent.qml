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
    ActivityIndicator {
        id: stopDetailsCurrentLoadIndicator
        horizontalAlignment: HorizontalAlignment.Center
        minHeight: 150
        minWidth: 150
    }
    ListView {
        id: stopDetailsCurrentListView
        
        dataModel: app.stoptimesDataModel
        
        opacity: 0
        
        listItemComponents: [
            ListItemComponent {
                type: "item"
                
                StandardListItem {
                    title: qsTr("%1").arg(ListItemData.time)
                    description: qsTr("%1").arg(ListItemData.headSign)
                }
            }
        ]
    }
    
    onStopNumberChanged: {
        stopDetailsCurrentLoadIndicator.start();
        app.getNextBusTimes(stopDetailsCurrentContainer.stopNumber);
    }
    
    onCreationCompleted: {
        console.log("stopDetailsCurrent created.");
        app.stopTimesFinishedLoading.connect(onStopTimesFinishedLoading);
    }
    
    function onStopTimesFinishedLoading() {
        stopDetailsCurrentLoadIndicator.stop();
        stopDetailsCurrentListView.opacity = 1;
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
