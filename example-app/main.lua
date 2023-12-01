local mainCnavasId = nil;

print(package.path)
local loadingModule = require("src.levels.loading")

function sge_load()
    sge.window.setTitle("example-app")
    sge.window.setWidth(960)
    sge.window.setHeight(720)
    
    mainCanvasId = sge.resources.createCanvas(480, 360)

    loadingModule.load()
end

function sge_exit()
    loadingModule.quit()
    sge.resources.deleteCanvas(mainCanvasId)
end

function sge_update()
    loadingModule.tick()
end

function sge_render()
    sge.commands.canvas(mainCanvasId)
    sge.commands.clear(0.0, 1.0, 1.0, 1.0)

    loadingModule.show()
end