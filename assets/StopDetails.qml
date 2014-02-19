import bb.cascades 1.2

Page {
    id: stopDetailsPage
    
    property int stopNumber
    property string stopName
    
    titleBar: TitleBar {
        title: qsTr("Stop " + stopNumber + " - " + stopName)
    }
    
    Container {
        id: stopDetailsContainer
        property variant selectedTab;
        
        SegmentedControl {
            id: segmentedControl
            
            Option {
                id: currentStopTimes
                text: qsTr("Current")
                selected: true
            }
            Option {
                id: futureStopTimes
                text: qsTr("Future")
            }
            onCreationCompleted: {
                var component = Qt.createComponent("StopDetailsCurrent.qml");
                var currentContainer = component.createObject(segmentedControl.parent, {"stopNumber": stopDetailsPage.stopNumber.toString()});
                stopDetailsContainer.add(currentContainer);
                stopDetailsContainer.selectedTab = currentContainer;
            }
            onSelectedOptionChanged: {
                if (selectedOption == currentStopTimes) {
                	console.log("current selected");
                    stopDetailsContainer.remove(stopDetailsContainer.selectedTab);
                    var component = Qt.createComponent("StopDetailsCurrent.qml");
                    var currentContainer = component.createObject(null, {"stopNumber": stopDetailsPage.stopNumber.toString()});
                    stopDetailsContainer.add(currentContainer);
                    stopDetailsContainer.selectedTab = currentContainer;
                } else if (selectedOption == futureStopTimes) {
                    console.log("future selected");
                    stopDetailsContainer.remove(stopDetailsContainer.selectedTab);
                    var component = Qt.createComponent("StopDetailsFuture.qml");
                    var futureContainer = component.createObject(null, {"stopNumber": stopDetailsPage.stopNumber.toString()});
                    stopDetailsContainer.add(futureContainer);
                    stopDetailsContainer.selectedTab = futureContainer;
                }
            }
        }
    }
}
