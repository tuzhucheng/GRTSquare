import bb.cascades 1.2
import "constants.js" as Constants

NavigationPane {
    id: stopsNavigationPane
    
    Page {
        titleBar: TitleBar {
            title: qsTr(Constants.STOPS)
        }
        
        Container {
            TextField {
                id: stopNumberTextField
                hintText: "Enter stop number here (e.g. 1000)"
                inputMode: TextFieldInputMode.Pin
                maximumLength: 4
                clearButtonVisible: true
                onTextChanging: {
                    if (stopNumberTextField.text.length > 1) {
                        stopsSearchListView.updateListView(stopNumberTextField.text);    
                    } else if (stopNumberTextField.text.length == 0) {
                        stopsSearchListView.updateListView("Some text that matches nothing");
                    }              
                }
                validator: Validator {
                    errorMessage: "You can only enter numbers 0-9"
                    mode: ValidationMode.Immediate
                    onValidate: {
                        var text = stopNumberTextField.text;
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
                                    var stopDetails = Qt.stopDetailsDefinition.createObject();
                                    stopDetails.stopNumber = ListItemData.stopNumber;
                                    stopDetails.stopName = ListItemData.stopName;
                                    Qt.stopsNavigationPane.push(stopDetails);
                                }
                            }
                        }
                    }
                ]
                
                function updateListView(query) {
                    app.searchStops(query);
                }
                
                onCreationCompleted: {
                    Qt.stopDetailsDefinition = stopDetailsDefinition;
                    Qt.stopsNavigationPane = stopsNavigationPane;
                }
            }
        }
        attachedObjects: [
            ComponentDefinition {
                id: stopDetailsDefinition
                source: "StopDetails.qml"
            }
        ]
    }
    
    onPopTransitionEnded: {
        page.destroy();
    }
}
