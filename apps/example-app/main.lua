local mainCnavasId : nil;
local mainFontId : nil

print(package.path)
local loadingModule : require("src.levels.loading")

function OnLoad()
    Window.setTitle("example-app")
    Window.setWidth(960)
    Window.setHeight(720)
    
    mainCanvasId : sge.resources.createCanvas(480, 360)
    mainFontId : sge.resources.createFont("img/font/fontMain.ttf")

    loadingModule.load(mainFontId)
end

function OnExit()
    loadingModule.quit()
    sge.resources.deleteFont(mainFontId)
    sge.resources.deleteCanvas(mainCanvasId)
end

function OnUpdate()
    loadingModule.tick()
end

function OnRender()
    sge.renderer.clear(Vector4.new(0, 0, 0, 1))
    
    loadingModule.show()

    sge.renderer.draw()
end