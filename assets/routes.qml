import bb.cascades 1.2

Container {
    
    bottomPadding: 30
    
    ListView {
        id: routesListView
        horizontalAlignment: HorizontalAlignment.Center
        
        dataModel: app.dataModel
        
        listItemComponents: [
            ListItemComponent {
                type: "item"
                StandardListItem {
                    id: listItem
                    title: qsTr("%1 - %2").arg(ListItemData.routeNumber).arg(ListItemData.routeLongName)
                    onTouch: {
                        if (event.isUp()) {
                            mySheet.open()
                        }
                    }
                    attachedObjects: Sheet {
                        id: mySheet
                        content: Page {
                            Container {
                                Label {
                                    text: "This is a sheet"
                                }
                                Button {
                                    text: "Close"
                                    onClicked: {
                                        mySheet.close()
                                    }
                                }
                            }
                        }
                    }
                }
            }
        ]
        
        
    }
    
    Button {
        horizontalAlignment: HorizontalAlignment.Center
        
        text: qsTr("Retrieve")
        onClicked: {
            app.readRecords(); // Refresh the list view.
        }
    }
    
}
