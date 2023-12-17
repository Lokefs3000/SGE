local golfBallTex = nil

function OnLoad()
    golfBallTex = Texture.new("img\\golfball.png", {Filter=Filtering.BiLinear, Repeat=Repeating.Clamp})
end

function OnExit()
    
end

function OnUpdate()
    
end

function OnRender()
    Renderer.DrawSprite(Vector2.new(0, 0), Vector2.new(100, 100), Vector4.new(1, 1, 1, 1), golfBallTex)
end
