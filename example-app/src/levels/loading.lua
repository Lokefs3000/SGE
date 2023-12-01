local loading = {}

loading.data = {}
loading.data.atlas = nil
loading.data.atlasData = nil
loading.data.atlasReal = nil
loading.data.font = nil

function loading.load()
    loading.data.atlas = sge.resources.createTexture("img/ui/loadingAtlas.png")
    loading.data.atlasData = sge.resources.createSGE("data/ui/loadingAtals.sge")
    loading.data.font = sge.resources.getFont("img/font/fontMain.ttf")
end

function loading.quit()
    sge.resources.deleteTexture(loading.data.atlas)
    sge.resources.deleteSGE(loading.data.atlasData)
end

function loading.show()
    sge.commands.texture(loading.data.atlas)
    sge.commands.atlas(loading.data.atlasData, "foreground")
    sge.commands.rectAtlas(0, 0, 76, 7)
end

function loading.tick()
    
end

return loading