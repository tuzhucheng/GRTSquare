import bb.cascades 1.2

Page {
    property int stopNumber
    property string stopName
    property string selectedOptionTab
    
    titleBar: TitleBar {
        title: qsTr("Stop " + stopNumber + " - " + stopName)
    }
    
    Container {
        id: stopDetailsContainer
        property variant selectedTab;
        
        SegmentedControl {
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
                var currentContainer = stopDetailsCurrent.createObject();
                stopDetailsContainer.add(currentContainer);
                stopDetailsContainer.selectedTab = currentContainer;
            }
            onSelectedOptionChanged: {
                if (selectedOption == currentStopTimes) {
                	console.log("current selected");
                    stopDetailsContainer.remove(stopDetailsContainer.selectedTab);
                    var currentContainer = stopDetailsCurrent.createObject();
                    stopDetailsContainer.add(currentContainer);
                    stopDetailsContainer.selectedTab = currentContainer;
                } else if (selectedOption == futureStopTimes) {
                    console.log("future selected");
                    stopDetailsContainer.remove(stopDetailsContainer.selectedTab);
                    var futureContainer = stopDetailsFuture.createObject();
                    stopDetailsContainer.add(futureContainer);
                    stopDetailsContainer.selectedTab = futureContainer;
                }
            }
        }
        attachedObjects: [
            ComponentDefinition {
                id: stopDetailsCurrent
                source: "StopDetailsCurrent.qml"
            },
            ComponentDefinition {
                id: stopDetailsFuture
                source: "StopDetailsFuture.qml"
            }
        ]
    }
}
