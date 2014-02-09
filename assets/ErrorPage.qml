import bb.cascades 1.2

Container {
    layout: DockLayout {
    }
    
    background: Color.Red
    
    Label {
        horizontalAlignment: HorizontalAlignment.Center
        verticalAlignment: VerticalAlignment.Center
        
        text: qsTr("ERROR: Database Initialization Failed")
    }
}
