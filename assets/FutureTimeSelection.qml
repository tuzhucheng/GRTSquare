import bb.cascades 1.2

Page {
    property int stopNumber
    property string stopName
    
    titleBar: TitleBar {
        title: qsTr("Select future time")
    }
    
    Container {
        leftPadding: 25.0
        rightPadding: 25.0
        
        DateTimePicker {
            id: datePicker
            horizontalAlignment: HorizontalAlignment.Center
            title: qsTr("Date")
            mode: DateTimePickerMode.Date
            value: futureDateObject.currentDateTime
        }

        DateTimePicker {
            id: timePicker
            horizontalAlignment: HorizontalAlignment.Center
            title: qsTr("Time")
            mode: DateTimePickerMode.Time
            value: futureDateObject.currentDateTime
            minuteInterval: 1
        }

        Button {
            text: "Go"
            horizontalAlignment: HorizontalAlignment.Center
            onClicked: {
                var datePart = datePicker.value;
                var timePart = timePicker.value;
                var month = (datePart.getMonth() < 10 ? "0" : "") + (datePart.getMonth()+1);
                var day = (datePart.getDate() < 10 ? "0" : "") + datePart.getDate();
                var hour = (timePart.getHours() < 10 ? "0" : "") + timePart.getHours();
                var min = (timePart.getMinutes() < 10 ? "0" : "") + timePart.getMinutes();
                var datetimeStr = datePart.getFullYear() + month + day + " " + hour + ":" + min + ":00";
                var dateReadable = datePart.toDateString();
                var timeReadable = timePart.toTimeString();
                var newPage = futureStopTimesPageDefinition.createObject();
                newPage.stopNumber = stopNumber;
                newPage.stopName = stopName;
                newPage.dateReadable = dateReadable;
                newPage.timeReadable = timeReadable;
                newPage.datetimeString = datetimeStr;
                Qt.searchTabNavPane.push(newPage);
            }
        }
    }

    attachedObjects: [
        QtObject {
            id: futureDateObject
            property variant currentDateTime
            
            onCreationCompleted: {
                currentDateTime = new Date();
            }
        },
        ComponentDefinition {
            id: futureStopTimesPageDefinition
            source: "FutureStopTimesPage.qml"
        }
    ]
}
