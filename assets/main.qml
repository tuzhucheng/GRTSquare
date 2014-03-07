import bb.cascades 1.2
import "constants.js" as Constants

TabbedPane {
    id: root
    showTabsOnActionBar: true
    
    Tab {
        title: Constants.HOME
        imageSource: "icons/ic_home.png"
        Home {}
    }
    Tab {
        title: qsTr("Current")
        imageSource: "icons/ic_browser.png"
        Current {}
    }
    Tab {
        title: qsTr("Future")
        imageSource: "icons/ic_next.png"
        Future {}
    }
}
