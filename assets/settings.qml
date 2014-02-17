import bb.cascades 1.2
import "constants.js" as Constants

Page {
    titleBar: TitleBar {
        title: qsTr(Constants.SETTINGS);
    }
    
    Container {
        layout: StackLayout {}
        
        topPadding: 40.0
        bottomPadding: 40.0
        leftPadding: 20.0
        rightPadding: 20.0
        
        Label {
            text: "On this page you can adjust some settings and explore the underlying data"
            horizontalAlignment: HorizontalAlignment.Center
            multiline: true
        }
    }
}
