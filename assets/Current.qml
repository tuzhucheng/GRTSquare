import bb.cascades 1.2
import QtQuick 1.0

NavigationPane {
    id: currentTabNavPane
    
    Page {
        titleBar: TitleBar {
            title: qsTr("Current Bus Times")
        }
        
        Container {
            TextField {
                id: currentStopNumberTextField
                hintText: "Enter stop number here (e.g. 1000)"
                inputMode: TextFieldInputMode.Pin
                maximumLength: 4
                clearButtonVisible: true
                onTextChanging: {
                    if (currentStopNumberTextField.text.length > 1) {
                        stopsSearchListView.updateListView(currentStopNumberTextField.text);    
                    } else if (currentStopNumberTextField.text.length == 0) {
                        stopsSearchListView.updateListView("Some text that matches nothing");
                    }              
                }
                validator: Validator {
                    errorMessage: "You can only enter numbers 0-9"
                    mode: ValidationMode.Immediate
                    onValidate: {
                        var text = currentStopNumberTextField.text;
                        var intRegex = /^\d+$/;
                        if (text && !intRegex.test(text)) {
                            state = ValidationState.Invalid;
                        }
                    }
                }
            }
            
            ListView {
                id: stopsSearchListView
                
                dataModel: app.stopsDataModel
                
                listItemComponents: [
                    ListItemComponent {
                        type: "item"
                        StandardListItem {
                            id: listItem
                            title: qsTr("%1").arg(ListItemData.stopNumber)
                            onTouch: {
                                if (event.isUp()) {
									var newPage = Qt.currentTimesPageDefinition.createObject();
                                    newPage.stopNumber = ListItemData.stopNumber;
                                    newPage.stopName = ListItemData.stopName;
									Qt.currentTabNavPane.push(newPage);
                                }
                            }
                        }
                    }
                ]
                
                function updateListView(query) {
                    app.searchStops(query);
                }
                
                onCreationCompleted: {
                    Qt.currentTimesPageDefinition = currentTimesPageDefinition;
                    Qt.currentTabNavPane = currentTabNavPane;
                }
            }
        }
    }
    
    attachedObjects: [
        ComponentDefinition {
            id: currentTimesPageDefinition
            Page {
                property int stopNumber
                property string stopName
                
                titleBar: TitleBar {
                    title: qsTr("Stop " + stopNumber + " - " + stopName)
                }
                
                onStopNumberChanged: {
                    console.log("onStopNumberChanged");
                    currentTimesLoadIndicator.start();
                    app.getNextBusTimes(stopNumber);
                }
                
                onCreationCompleted: {
                    console.log("onStopNumberChanged");   
                    app.stopTimesFinishedLoadingCurrent.connect(onStopTimesFinishedLoading);                 
                }
                
                function onStopTimesFinishedLoading() {
                    currentTimesLoadIndicator.stop();
                    currentTimesListView.opacity = 1;
                    var numberOfRoutes = app.stoptimesDataModel.childCount(currentTimesListView.rootIndexPath)
                    console.log("Number of routes found: " + numberOfRoutes);
                    for (var i = 0; i < numberOfRoutes; i++) {
                        var createdScrollIndex = routeNumberLabelCurrent.createObject();
                        createdScrollIndex.routeNumber = app.stoptimesDataModel.data([i]).toString();
                        createdScrollIndex.index = i;
                        currentRouteScrollIndices.add(createdScrollIndex);
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
                        id: currentTimesLoadIndicator
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
                            id: currentRouteScrollIndices
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
                        id: routeNumberLabelCurrent      
                        Container {
                            id: routeNumberLabelCurrentContainer
                            minWidth: 100
                            background: Color.DarkCyan
                            leftMargin: 20.0
                            rightMargin: 20.0
                            
                            property string routeNumber  
                            property int index
                            
                            onTouch: {
                                if (event.isUp()) {
                                    currentTimesListView.scrollToItem([index], ScrollAnimation.Default);
                                }
                            }
                            
                            Label {
                                text: routeNumberLabelCurrentContainer.routeNumber
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
