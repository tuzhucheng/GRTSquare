import bb.cascades 1.2

Page {
    property int stopNumber
    property string stopName
    property string datetimeString
    property string dateReadable
    property string timeReadable
    
    titleBar: TitleBar {
        title: qsTr("Stop " + stopNumber + " - " + stopName)
    }
    
    onCreationCompleted: {
        app.stopTimesFinishedLoadingFuture.connect(onStopTimesFinishedLoading);
    }
    
    onDatetimeStringChanged: {
    	futureTimesLoadIndicator.start();
    	app.getNextBusTimes(stopNumber, datetimeString);
    }
    
    function onStopTimesFinishedLoading() {
        futureTimesLoadIndicator.stop();
        futureTimesListView.opacity = 1;
        var numberOfRoutes = app.stoptimesDataModel.childCount(futureTimesListView.rootIndexPath)
        console.log("Number of routes found: " + numberOfRoutes);
        for (var i = 0; i < numberOfRoutes; i++) {
            var createdScrollIndex = futureRouteNumberLabel.createObject();
            createdScrollIndex.routeNumber = app.stoptimesDataModel.data([i]).toString();
            createdScrollIndex.index = i;
            futureRouteScrollIndices.add(createdScrollIndex);
        }
    }
    
    Container {
        Label {
            id: futureDate
            text: dateReadable
            horizontalAlignment: HorizontalAlignment.Center
            textStyle {
                base: SystemDefaults.TextStyles.PrimaryText
                color: Color.DarkCyan
            }
        }
        
        Label {
            id: futureTime
            text: timeReadable
            horizontalAlignment: HorizontalAlignment.Center
            textStyle {
                base: SystemDefaults.TextStyles.SmallText
                color: Color.DarkCyan
            }
        }
        
        ActivityIndicator {
            id: futureTimesLoadIndicator
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
                id: futureRouteScrollIndices
                layout: StackLayout {
                    orientation: LayoutOrientation.LeftToRight
                }
                leftPadding: 25.0
                rightPadding: 25.0
            }
        }
        
        ListView {
            id: futureTimesListView
            
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
        ComponentDefinition {
            id: futureRouteNumberLabel
            RouteNumberLabel {
                onTouch: {
                    if (event.isUp()) {
                        futureTimesListView.scrollToItem([index], ScrollAnimation.Default);
                    }
                }
            }
        }
    ]
}
