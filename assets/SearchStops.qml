import bb.cascades 1.2
import QtQuick 1.0

NavigationPane {
    id: searchTabNavPane
    
    Page {
        titleBar: TitleBar {
            title: qsTr("Search for stops")
        }
        
        Container {
            TextField {
                id: searchTextField
                hintText: "Enter stop number here (e.g. 1000)"
                inputMode: TextFieldInputMode.Pin
                maximumLength: 4
                clearButtonVisible: true
                onTextChanging: {
                    if (searchTextField.text.length > 1) {
                        stopsSearchListView.updateListView(searchTextField.text);    
                    } else if (searchTextField.text.length == 0) {
                        stopsSearchListView.updateListView("Some text that matches nothing");
                    }              
                }
                validator: Validator {
                    errorMessage: "You can only enter numbers 0-9"
                    mode: ValidationMode.Immediate
                    onValidate: {
                        var text = searchTextField.text;
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
									var newPage = Qt.stopPageDefinition.createObject();
                                    newPage.stopNumber = ListItemData.stopNumber;
                                    newPage.stopName = ListItemData.stopName;
									Qt.searchTabNavPane.push(newPage);
                                }
                            }
                        }
                    }
                ]
                
                function updateListView(query) {
                    app.searchStops(query);
                }
                
                onCreationCompleted: {
                    Qt.searchTabNavPane = searchTabNavPane;
                    Qt.stopPageDefinition = stopPageDefinition;
                }
            }
        }
    }

    attachedObjects: [
        ComponentDefinition{
            id: stopPageDefinition
            source: "StopPage.qml"
        }
    ]

    onPopTransitionEnded: {
        page.deleteLater();
    }
}
