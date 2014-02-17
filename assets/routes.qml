import bb.cascades 1.2
import "constants.js" as Constants

NavigationPane {
    id: routesNavigationPane
    
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
                            onTouch: {
                                if (event.isUp()) {
                                    var routeDetails = Qt.routeDetailsDefinition.createObject();
                                    routeDetails.routeName = ListItemData.routeName;
                                    routeDetails.routeNumber = ListItemData.routeNumber;
                                    Qt.routesNavigationPane.push(routeDetails);
                                }
                            }
                        }
                    }
                ]
                
                onCreationCompleted: {
                    app.readRecords();
                    Qt.routeDetailsDefinition = routeDetailsDefinition;
                    Qt.routesNavigationPane = routesNavigationPane;
                }
            }
        }
        attachedObjects: [
            ComponentDefinition {
                id: routeDetailsDefinition
                source: "RouteDetails.qml"
            }
        ]    
    }
    
    onPopTransitionEnded: {
        page.destroy();
    }
}
