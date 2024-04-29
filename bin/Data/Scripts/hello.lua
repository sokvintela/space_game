-- This first example, maintaining tradition, prints a "Hello World" message.
-- Furthermore it shows:
--     - Using the Sample utility functions as a base for the application
--     - Adding a Text element to the graphical user interface
--     - Subscribing to and handling of update events

function Start()
    -- Create "Hello World" Text
    CreateText()
end

function CreateText()
    -- Construct new Text object
    local helloText = Text:new()

    -- Set String to display
    helloText.text = "Hello World from Urho3D!"

    -- Set font and text color
    helloText:SetFont(cache:GetResource("Font", "Fonts/Anonymous Pro.ttf"), 30)
    helloText.color = Color(0.0, 1.0, 0.0)

    -- Align Text center-screen
    helloText.horizontalAlignment = HA_CENTER
    helloText.verticalAlignment = VA_CENTER

    -- Add Text instance to the UI root element
    ui.root:AddChild(helloText)
end

