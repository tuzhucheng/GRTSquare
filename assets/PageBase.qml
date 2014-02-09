import bb.cascades 1.2

Page {
    id: root
    
    property string page
    property string titleText: "GRTSquare"
    property bool databaseOpen: false;
    
    titleBar: TitleBar {
        title: qsTr(titleText)
    }
    
    ControlDelegate {
        source: databaseOpen ? root.page : "ErrorPage.qml"
        delegateActive: true
    }
}
