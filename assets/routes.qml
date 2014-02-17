import bb.cascades 1.2
import "constants.js" as Constants

Page {
    titleBar: TitleBar {
        title: qsTr(Constants.ROUTES)
    }
    
    Container {        
        ListView {
            id: routesListView
            horizontalAlignment: HorizontalAlignment.Center
            
            dataModel: app.dataModel
            
            listItemComponents: [
                ListItemComponent {
                    type: "item"
                    StandardListItem {
                        id: listItem
                        title: qsTr("%1 - %2").arg(ListItemData.routeNumber).arg(ListItemData.routeName)
                    }
                }
            ]
            
            onCreationCompleted: {
                app.readRecords();
            }  
        }
    }    
}
