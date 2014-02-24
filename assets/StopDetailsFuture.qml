import bb.cascades 1.2

Container {
    id: stopDetailsFutureContainer
    
    property string stopNumber
    
    layout: StackLayout {
        orientation: LayoutOrientation.TopToBottom
    }
    
    ScrollView {
        scrollViewProperties {
            scrollMode: ScrollMode.Vertical
        }
        
        Container {
            
            leftPadding: 25.0
            rightPadding: 25.0
            
            onCreationCompleted: {
                app.stopTimesFinishedLoadingFuture.connect(onStopTimesFinishedLoading);
            }
            
            function onStopTimesFinishedLoading() {
                stopDetailsFutureLoadIndicator.stop();
                stopDetailsFutureListView.opacity = 1;
                var numberOfRoutes = app.stoptimesDataModel.childCount(stopDetailsFutureListView.rootIndexPath)
                console.log("Number of routes found: " + numberOfRoutes);
                for (var i = 0; i < numberOfRoutes; i++) {
                    var createdScrollIndex = routeNumberLabel.createObject();
                    createdScrollIndex.routeNumber = app.stoptimesDataModel.data([i]).toString();
                    createdScrollIndex.index = i;
                    routeScrollIndicesFuture.add(createdScrollIndex);
                }
            }
            
            Label {
                text: qsTr("")
                multiline: true
                horizontalAlignment: HorizontalAlignment.Center
            }
            
            DateTimePicker {
                id: datePicker
                horizontalAlignment: HorizontalAlignment.Center
                title: qsTr("Date")
                mode: DateTimePickerMode.Date
                value: futureContainerDateObject.currentDateTime
            }
            
            DateTimePicker {
                id: timePicker
                horizontalAlignment: HorizontalAlignment.Center
                title: qsTr("Time")
                mode: DateTimePickerMode.Time
                value: futureContainerDateObject.currentDateTime
                minuteInterval: 1
            }
            
            Button {
                id: futureStopTimeSearchButton
                text: qsTr("Search")
                horizontalAlignment: HorizontalAlignment.Center
                onClicked: {
                    var datePart = datePicker.value;
                    var timePart = timePicker.value;
                    var month = (datePart.getMonth() < 10 ? "0" : "") + (datePart.getMonth()+1);
                    var day = (datePart.getDate() < 10 ? "0" : "") + datePart.getDate();
                    var hour = (timePart.getHours() < 10 ? "0" : "") + timePart.getHours();
                    var min = (timePart.getMinutes() < 10 ? "0" : "") + timePart.getMinutes();
                    var datetimeStr = datePart.getFullYear() + month + day + " " + hour + ":" + min + ":00"
                    console.log(stopNumber);
                    console.log(datetimeStr);
                    routeScrollIndicesFuture.removeAll();
                    stopDetailsFutureListView.opacity = 0;
                    app.getNextBusTimes(stopNumber, datetimeStr);
                    stopDetailsFutureLoadIndicator.start();
                }
            }
            
            ActivityIndicator {
                id: stopDetailsFutureLoadIndicator
                horizontalAlignment: HorizontalAlignment.Center
                minHeight: 150
                minWidth: 150
                running: false
            }
            
            ScrollView {
                bottomMargin: 25.0
                scrollViewProperties {
                    scrollMode: ScrollMode.Horizontal
                }
                Container {
                    id: routeScrollIndicesFuture
                    layout: StackLayout {
                        orientation: LayoutOrientation.LeftToRight
                    }
                    leftPadding: 25.0
                    rightPadding: 25.0
                }
            }
            
            ListView {
                id: stopDetailsFutureListView
                
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
            
            attachedObjects: [
                QtObject {
                    id: futureContainerDateObject
                    property variant currentDateTime
                    
                    onCreationCompleted: {
                        currentDateTime = new Date();
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
                                stopDetailsFutureListView.scrollToItem([index], ScrollAnimation.Default);
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
    }

}
