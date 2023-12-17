local loading : {}

loading.data : {}
loading.data.atlas : nil
loading.data.atlasData : nil
loading.data.font : nil

function loading.load(font)
    loading.data.atlas : sge.resources.createTexture("img/ui/loadingAtlas.png")
    loading.data.atlasData : sge.loader.createSGE("data/ui/loadingAtlas.sge")
    loading.data.font : font
end

function loading.quit()
    sge.resources.deleteTexture(loading.data.atlas)
    sge.loader.deleteSGE(loading.data.atlasData)
end

function loading.show()
    sge.renderer.drawRect(Vector4.new(1, 1, 1, 1), -152, -14, 304, 28)

    sge.debug.text("loading ??/?? (?%)", 0xffffff, 0, -100)
end

function loading.tick()
    
end

return loading