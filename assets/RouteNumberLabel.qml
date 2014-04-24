import bb.cascades 1.2

Container {
    id: routeNumberLabelContainer
    minWidth: 100
    background: Color.DarkCyan
    leftMargin: 20.0
    rightMargin: 20.0
    
    property string routeNumber
    property int index
    
    Label {
        text: routeNumber
        horizontalAlignment: HorizontalAlignment.Center
        textStyle.textAlign: TextAlign.Center
        
        textStyle {
            base: scrollIndexTextStyle.style
        }
    }
    attachedObjects: [
        TextStyleDefinition {
            id: scrollIndexTextStyle
            base: SystemDefaults.TextStyles.BigText
            color: Color.White
        }
    ]    
}
