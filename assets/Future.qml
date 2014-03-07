import bb.cascades 1.2

NavigationPane {
    id: futureTabNavPane
    
    Page {
        titleBar: TitleBar {
            title: qsTr("Future Bus Times")
        }
        
        Container {
            leftPadding: 25.0
            rightPadding: 25.0
            
            TextField {
                id: futureStopNumberTextField
                verticalAlignment: VerticalAlignment.Center
                hintText: "Enter stop number here (e.g. 1000)"
                inputMode: TextFieldInputMode.Pin
                maximumLength: 4
                clearButtonVisible: true
                validator: Validator {
                    errorMessage: "You can only enter numbers 0-9"
                    mode: ValidationMode.Immediate
                    onValidate: {
                        var text = futureStopNumberTextField.text;
                        var intRegex = /^\d+$/;
                        if (text && !intRegex.test(text)) {
                            state = ValidationState.Invalid;
                        }
                    }
                }
            }
            
            DateTimePicker {
                id: datePicker
                verticalAlignment: VerticalAlignment.Center
                horizontalAlignment: HorizontalAlignment.Center
                title: qsTr("Date")
                mode: DateTimePickerMode.Date
                value: futureDateObject.currentDateTime
            }
            
            DateTimePicker {
                id: timePicker
                verticalAlignment: VerticalAlignment.Center
                horizontalAlignment: HorizontalAlignment.Center
                title: qsTr("Time")
                mode: DateTimePickerMode.Time
                value: futureDateObject.currentDateTime
                minuteInterval: 1
            }
            
            Button {
                id: futureStopTimeSearchButton
                verticalAlignment: VerticalAlignment.Center
                text: qsTr("Search")
                horizontalAlignment: HorizontalAlignment.Center
                onClicked: {
                    var datePart = datePicker.value;
                    var timePart = timePicker.value;
                    var month = (datePart.getMonth() < 10 ? "0" : "") + (datePart.getMonth()+1);
                    var day = (datePart.getDate() < 10 ? "0" : "") + datePart.getDate();
                    var hour = (timePart.getHours() < 10 ? "0" : "") + timePart.getHours();
                    var min = (timePart.getMinutes() < 10 ? "0" : "") + timePart.getMinutes();
                    var datetimeStr = datePart.getFullYear() + month + day + " " + hour + ":" + min + ":00";
                    var dateReadable = datePart.toDateString();
                    var timeReadable = timePart.toTimeString();
                    var newPage = futureTimesPageDefinition.createObject();
                    newPage.dateReadable = dateReadable;
                    newPage.timeReadable = timeReadable;
                    newPage.stopNumber = parseInt(futureStopNumberTextField.text)
                    newPage.datetimeString = datetimeStr;
                    futureTabNavPane.push(newPage);
                }
            }
        }
        
/*        actions: ActionItem {
            title: qsTr("Second page")
            ActionBar.placement: ActionBarPlacement.OnBar
            
            onTriggered: {
                navigationPane.push(secondPageDefinition.createObject());
            }
        }*/
    }
    
    attachedObjects: [
        QtObject {
            id: futureDateObject
            property variant currentDateTime
            
            onCreationCompleted: {
                currentDateTime = new Date();
            }
        },
        ComponentDefinition {
            id: futureTimesPageDefinition
            Page {
                property int stopNumber
                property string datetimeString
                property string dateReadable
                property string timeReadable
                
                titleBar: TitleBar {
                    title: qsTr("Stop " + stopNumber)
                }
                
                onDatetimeStringChanged: {
                    futureTimesLoadIndicator.start();
                    app.getNextBusTimes(stopNumber, datetimeString);
                }
                
                onCreationCompleted: {
                    app.stopTimesFinishedLoadingFuture.connect(onStopTimesFinishedLoading);
                }
                
                function onStopTimesFinishedLoading() {
                    futureTimesLoadIndicator.stop();
                    var numberOfRoutes = app.stoptimesDataModel.childCount(futureTimesListView.rootIndexPath)
                    console.log("Number of routes found: " + numberOfRoutes);
                    for (var i = 0; i < numberOfRoutes; i++) {
                        var createdScrollIndex = routeNumberLabel.createObject();
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
    ]
    
    onPopTransitionEnded: {
        page.deleteLater();
    }
}
