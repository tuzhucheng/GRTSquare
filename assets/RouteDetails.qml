import bb.cascades 1.2

Page {
    property string routeName
    property int routeNumber
    
    titleBar: TitleBar {
        title: qsTr("Route " + routeNumber + " - " + routeName)
    }
    
    Container {
        Label {
            text: qsTr(routeName)
        }        
    }
}
