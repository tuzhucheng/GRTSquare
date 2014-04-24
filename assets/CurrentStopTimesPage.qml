import bb.cascades 1.2
import QtQuick 1.0

Page {
    property int stopNumber
    property string stopName
    
    titleBar: TitleBar {
        title: qsTr("Stop " + stopNumber + " - " + stopName)
    }
    
    onStopNumberChanged: {
        console.log("onStopNumberChanged");
        currentTimesLoadingIndicator.start();
        app.getNextBusTimes(stopNumber);
    }
    
    onCreationCompleted: {
        console.log("onStopNumberChanged");   
        app.stopTimesFinishedLoadingCurrent.connect(onStopTimesFinishedLoading);                 
    }
    
    function onStopTimesFinishedLoading() {
        currentTimesLoadingIndicator.stop();
        currentTimesListView.opacity = 1;
        var numberOfRoutes = app.stoptimesDataModel.childCount(currentTimesListView.rootIndexPath)
        console.log("Number of routes found: " + numberOfRoutes);
        for (var i = 0; i < numberOfRoutes; i++) {
            var createdScrollIndex = currentRouteNumberLabel.createObject();
            createdScrollIndex.routeNumber = app.stoptimesDataModel.data([i]).toString();
            createdScrollIndex.index = i;
            routeScrollIndices.add(createdScrollIndex);
        }
    }
    
    Container {
        
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
            id: currentTimesLoadingIndicator
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
            id: currentTimesListView
            
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
            id: currentRouteNumberLabel
            RouteNumberLabel {
                onTouch: {
                    if (event.isUp()) {
                        currentTimesListView.scrollToItem([index], ScrollAnimation.Default);
                    }
                }
            }
        }
    ]
}
