local constants = require ("constants")

local MenuScene = class("MenuScene",function()
    return cc.Scene:create()
end)

function MenuScene.create()
    local scene = MenuScene.new()
    return scene
end

function MenuScene:ctor()
    self.visibleSize = cc.Director:getInstance():getVisibleSize()
    self.middle = {x = self.visibleSize.width * 0.5, y = self.visibleSize.height * 0.5}
    self.origin = cc.Director:getInstance():getVisibleOrigin()
    self:init()
end

function MenuScene:init ()
    local bg = cc.Sprite:create("introbg.jpg")
    bg:setPosition(self.middle.x, self.middle.y)
    self:addChild(bg)
    
    --create pterodactyl animation
    local pterodactyl = cc.Sprite:create("ptero_frame1.png")
    pterodactyl:setPosition(cc.p(self.visibleSize.width + 100, self.visibleSize.height * 0.8))
    self:addChild(pterodactyl)
    
    local animation = cc.Animation:create()
    local number, name
    for i = 1, 3 do
        number = i
        name = "ptero_frame"..number..".png"
        animation:addSpriteFrameWithFile(name)
    end
    animation:setDelayPerUnit(0.5 / 3.0)
    animation:setRestoreOriginalFrame(true)
    animation:setLoops(-1)
    
    local animate = cc.Animate:create(animation)
    pterodactyl:runAction( animate )
    
    local moveOut = cc.MoveTo:create(0, cc.p(self.visibleSize.width + 100, self.visibleSize.height * 0.8))
    local moveIn = cc.MoveTo:create(4.0, cc.p(-100, self.visibleSize.height * 0.8))
    local delay = cc.DelayTime:create(2.5)
    pterodactyl:runAction(cc.RepeatForever:create(cc.Sequence:create(moveOut, moveIn, delay) ) )
    
    local character = cc.Sprite:create("introCharacter.png")
    character:setPosition(self.middle.x, self.middle.y + 110)
    self:addChild(character)
    
    local frame = cc.Sprite:create("frame.png")
    frame:setPosition(self.middle.x, self.middle.y)
    self:addChild(frame)
    
    --create play button
    local function playGame()
        
        local bgMusicPath = cc.FileUtils:getInstance():fullPathForFilename("background.mp3") 
        cc.SimpleAudioEngine:getInstance():playMusic(bgMusicPath, true)
            
        local scene = require("GameScene")
        local gameScene = scene.create()
        cc.Director:getInstance():replaceScene(gameScene)
    end

    -- add a popup menu
    local btnPlay = cc.MenuItemImage:create("playBtn.png", "playBtnOver.png")
    btnPlay:setPosition(0,0)
    btnPlay:registerScriptTapHandler(playGame)
    local menu  = cc.Menu:create(btnPlay)
    menu:setPosition(self.middle.x, 80)
    --menu:setVisible(false)
    self:addChild(menu)
    
end

return MenuScene
