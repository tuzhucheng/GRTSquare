import bb.cascades 1.2
import "constants.js" as Constants

NavigationPane {
    id: stopsNavigationPane
    
    Page {
        id: stopsPage
        
        titleBar: TitleBar {
            title: qsTr(Constants.STOPS)
        }
        
        Container {
            id: stopsMainContainer
            
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
                                    var component = Qt.createComponent("StopDetails.qml")
                                    var properties = {
                                        "stopNumber": ListItemData.stopNumber,
                                        "stopName": ListItemData.stopName
                                    }
                                    var stopDetails = component.createObject(Qt.stopsPage.parent, properties);
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
                    Qt.stopsNavigationPane = stopsNavigationPane;
                    Qt.stopsPage = stopsPage;
                }
            }
        }
    }
    
    onPopTransitionEnded: {
        page.destroy();
    }
}
