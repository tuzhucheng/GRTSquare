import bb.cascades 1.2
import "constants.js" as Constants

TabbedPane {
    id: root
    
    Tab {
        title: Constants.HOME
        imageSource: "icons/ic_home.png"
        Home {}
    }
    Tab {
        title: Constants.ROUTES
        imageSource: "icons/ic_routes.png"
        Routes {}
    }
    Tab {
        title: Constants.SETTINGS
        imageSource: "icons/ic_settings.png"
        Settings {}
    }
}
