local constants = require ("constants")

local Gem = class("Gem",function()
    return cc.Sprite:create()
end)

function Gem.create()
    local node = Gem.new()
    return node
end


function Gem:ctor()
    self.visibleSize = cc.Director:getInstance():getVisibleSize()
    self.origin = cc.Director:getInstance():getVisibleOrigin()
    self.type = constants.TYPE_GEM_BLUE;
    self.gemContainer = cc.Node:create()
    self.skin = cc.Sprite:create("gem_blue.png")
    self.selected = false
    self:init()
end

function Gem:select()
    if (self.selected == true) then return end
    --show selected state
    self.selected = true
end

function Gem:deselect ()
    if (self.selected == false) then return end
    --clear selected state
    self.selected = false
end

function Gem:reset ()
    self.gemContainer:setScale (1, 1)
    self.gemContainer:setRotation(0)
end

function Gem:getType ()
    return self.type
end

function Gem:setType (value)
    
    self.type = value
    
    local textureCache = cc.Director:getInstance():getTextureCache()
    if (self.type == constants.TYPE_GEM_BLUE) then
        self.skin:setTexture(textureCache:addImage("gem_blue.png"))
    elseif (self.type == constants.TYPE_GEM_GREEN) then
        self.skin:setTexture(textureCache:addImage("gem_green.png"))
    elseif (self.type == constants.TYPE_GEM_YELLOW) then
        self.skin:setTexture(textureCache:addImage("gem_yellow.png"))
    elseif (self.type == constants.TYPE_GEM_RED) then
        self.skin:setTexture(textureCache:addImage("gem_red.png"))
    elseif (self.type == constants.TYPE_GEM_PINK) then
        self.skin:setTexture(textureCache:addImage("gem_pink.png"))
    elseif (self.type == constants.TYPE_GEM_WHITE) then
        self.skin:setTexture(textureCache:addImage("gem_white.png"))
   end
end

function Gem:init ()
	--self:setVisible(true)
    self:setVisible(false)
    self.gemContainer:addChild(self.skin)
    self:addChild(self.gemContainer)
end



return Gem
