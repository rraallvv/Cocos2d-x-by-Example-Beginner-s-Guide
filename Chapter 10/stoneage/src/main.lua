
cc.FileUtils:getInstance():setPopupNotify(false)
cc.FileUtils:getInstance():addSearchPath("src/")
cc.FileUtils:getInstance():addSearchPath("res/")

require "config"
require "cocos.init"

local function main()
    -- initialize director
    local director = cc.Director:getInstance()

    --turn on display FPS
    director:setDisplayStats(false)

    --set FPS. the default value is 1.0/60 if you don't call this
    director:setAnimationInterval(1.0 / 60)

    cc.Director:getInstance():getOpenGLView():setDesignResolutionSize(640, 960, cc.ResolutionPolicy.SHOW_ALL)
    local screenSize = cc.Director:getInstance():getVisibleSize()
    local designSize = cc.size(640, 960)

    if (screenSize.width > 320) then
        cc.Director:getInstance():setContentScaleFactor(640/designSize.width)       
        cc.FileUtils:getInstance():addSearchPath("res/hd/") 
    else
        cc.Director:getInstance():setContentScaleFactor(320/designSize.width)
        cc.FileUtils:getInstance():addSearchPath("res/sd/")         
    end

    --create scene 
    local scene = require("MenuScene")
    --local scene = require("GameScene")
    local menuScene = scene.create()

    local bgMusicPath = cc.FileUtils:getInstance():fullPathForFilename("background.mp3") 
    cc.SimpleAudioEngine:getInstance():preloadMusic(bgMusicPath)

    local effectPath = cc.FileUtils:getInstance():fullPathForFilename("match.wav")
    cc.SimpleAudioEngine:getInstance():preloadEffect(effectPath)

    effectPath = cc.FileUtils:getInstance():fullPathForFilename("diamond.wav")
    cc.SimpleAudioEngine:getInstance():preloadEffect(effectPath)

    effectPath = cc.FileUtils:getInstance():fullPathForFilename("diamond2.wav")
    cc.SimpleAudioEngine:getInstance():preloadEffect(effectPath)

    effectPath = cc.FileUtils:getInstance():fullPathForFilename("wrong.wav")
    cc.SimpleAudioEngine:getInstance():preloadEffect(effectPath)

    if cc.Director:getInstance():getRunningScene() then
        cc.Director:getInstance():replaceScene(menuScene)
    else
        cc.Director:getInstance():runWithScene(menuScene)
    end
end

local status, msg = xpcall(main, __G__TRACKBACK__)
if not status then
    print(msg)
end
