import bb.cascades 1.2
import QtQuick 1.0

Container {
    id: stopDetailsCurrentContainer
    
    property string stopNumber
    
    layout: StackLayout {
        orientation: LayoutOrientation.TopToBottom
    }
    
    Label {
        id: currentTime
        text: Qt.formatDateTime(new Date(), "hh:mm:ss AP")
        horizontalAlignment: HorizontalAlignment.Center
        textStyle {
            base: SystemDefaults.TextStyles.BigText
            color: Color.DarkCyan
        }
    }
    
    ActivityIndicator {
        id: stopDetailsCurrentLoadIndicator
        horizontalAlignment: HorizontalAlignment.Center
        minHeight: 150
        minWidth: 150
    }
    
    ScrollView {
        bottomMargin: 25.0
        scrollViewProperties {
            scrollMode: ScrollMode.Horizontal
        }
        Container {
            id: routeScrollIndices
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            }
            leftPadding: 25.0
            rightPadding: 25.0
        }
    }
    
    ListView {
        id: stopDetailsCurrentListView
        
        dataModel: app.stoptimesDataModel
        
        opacity: 0
        
        listItemComponents: [
            ListItemComponent {
                type: "item"
                
                StandardListItem {
                    title: qsTr("%1").arg(ListItemData.timeFormatted)
                    description: qsTr("%1").arg(ListItemData.headSign)
                }
            }
        ]
    }
    
    onStopNumberChanged: {
        console.log("onStopNumberChanged");
        stopDetailsCurrentLoadIndicator.start();
        app.getNextBusTimes(stopDetailsCurrentContainer.stopNumber);
    }
    
    onCreationCompleted: {
        console.log("stopDetailsCurrent created.");
        app.stopTimesFinishedLoadingCurrent.connect(onStopTimesFinishedLoading);
    }
    
    function onStopTimesFinishedLoading() {
        stopDetailsCurrentLoadIndicator.stop();
        stopDetailsCurrentListView.opacity = 1;
        var numberOfRoutes = app.stoptimesDataModel.childCount(stopDetailsCurrentListView.rootIndexPath)
        console.log("Number of routes found: " + numberOfRoutes);
        for (var i = 0; i < numberOfRoutes; i++) {
            var createdScrollIndex = routeNumberLabel.createObject();
            createdScrollIndex.routeNumber = app.stoptimesDataModel.data([i]).toString();
            createdScrollIndex.index = i;
            routeScrollIndices.add(createdScrollIndex);
        }
    }
    
    attachedObjects: [
        Timer {
            interval: 1000
            running: true
            repeat: true
            onTriggered: {
                currentTime.text = Qt.formatDateTime(new Date(), "hh:mm:ss AP")
            }
        },
        ComponentDefinition {
            id: routeNumberLabel      
            Container {
                id: routeNumberLabelContainer
                minWidth: 100
                background: Color.DarkCyan
                leftMargin: 20.0
                rightMargin: 20.0
                
                property string routeNumber  
                property int index
                
                onTouch: {
                    if (event.isUp()) {
                        stopDetailsCurrentListView.scrollToItem([index], ScrollAnimation.Default);
                    }
                }
                
                Label {
                    text: routeNumberLabelContainer.routeNumber
                    horizontalAlignment: HorizontalAlignment.Center
                    textStyle.textAlign: TextAlign.Center
                    
                    textStyle {
                        base: scrollIndexTextStyle.style
                    }
                }
                attachedObjects: [
                    TextStyleDefinition {
                        id: scrollIndexTextStyle
                        base: SystemDefaults.TextStyles.BigText
                        color: Color.White
                    }
                ]
            }
        }
    ]
    
}
