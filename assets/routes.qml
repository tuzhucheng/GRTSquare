import bb.cascades 1.2

Container {
    
    bottomPadding: 30
    
    ListView {
        horizontalAlignment: HorizontalAlignment.Center
        
        dataModel: app.dataModel
        
        listItemComponents: [
            ListItemComponent {
                type: "item"
                StandardListItem {
                    title: qsTr("%1").arg(ListItemData.routeLongName)
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
