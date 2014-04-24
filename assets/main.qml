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
        title: qsTr(Constants.SEARCH)
        imageSource: "icons/ic_search.png"
        SearchStops {}
    }
}
