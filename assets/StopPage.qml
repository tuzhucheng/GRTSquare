import bb.cascades 1.2

Page {
    property int stopNumber
    property string stopName
    
    titleBar: TitleBar {
        title: qsTr("Stop " + stopNumber + " - " + stopName)
    }
    
    Container {
        layout: StackLayout {}

        Button {
            text: "Current Stop Times"
            horizontalAlignment: HorizontalAlignment.Center
            onClicked: {
                var newPage = currentStopTimesPageDefinition.createObject();
                newPage.stopNumber = stopNumber;
                newPage.stopName = stopName;
                Qt.searchTabNavPane.push(newPage);
            }
        }
        
        Button {
            text: "Future Stop Times"
            horizontalAlignment: HorizontalAlignment.Center
            onClicked: {
                var newPage = futureTimeSelectionPageDefinition.createObject();
                newPage.stopNumber = stopNumber;
                newPage.stopName = stopName;
                Qt.searchTabNavPane.push(newPage);
            }
        }
    }
    
    attachedObjects: [
        ComponentDefinition {
            id: currentStopTimesPageDefinition
            source: "CurrentStopTimesPage.qml"
        },
        ComponentDefinition {
            id: futureTimeSelectionPageDefinition
            source: "FutureTimeSelection.qml"
        }
    ]
    
    actions: ActionItem {
        title: qsTr("Add to Favourites")
        ActionBar.placement: ActionBarPlacement.OnBar
        
        onTriggered: {
            //navigationPane.push(secondPageDefinition.createObject());
        }
    }

}
