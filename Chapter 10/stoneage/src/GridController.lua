local constants = require ("constants")
local GridController = class("GridController")

function GridController:ctor()
    self.enabled = true
    self.touchDown = false
    self.gameLayer = nil
    self.matchArray = {}
end

function GridController.create (  )
    local gc = GridController.new ()
    return gc
end

function GridController:setGameLayer (value) 
    self.gameLayer = value
end

function GridController:checkGridMatches ()
    
    self.matchArray = {}
    
    for c = 1, constants.GRID_SIZE_X do
        for r = 1, constants.GRID_SIZE_Y do
            self:checkTypeMatch(c,r)
        end
    end
    if (#self.matchArray >= 2) then
        self.gameLayer:addToScore()
        return true
    end
    print("no matches")
    return false
end

function GridController:checkTypeMatch (c, r)
    
    local type = self.gameLayer.grid[c][r]
    local stepC = c
    local stepR = r
    local temp_matches = {}

    --check top
    while stepR -1 >= 1 and self.gameLayer.grid[c][stepR-1] == type do
        stepR = stepR - 1
        table.insert (temp_matches, {x = c, y = stepR})
    end 
    
    if (#temp_matches >= 2) then self:addMatches (temp_matches) end
    temp_matches = {}

    --check bottom
    stepR = r
    while stepR + 1 <= constants.GRID_SIZE_Y and self.gameLayer.grid[c][stepR + 1] == type do
        stepR = stepR + 1
        table.insert (temp_matches, {x = c, y= stepR})
    end

    if (#temp_matches >= 2) then self:addMatches (temp_matches) end
    temp_matches = {}

    --check left
    while stepC - 1 >= 1 and self.gameLayer.grid[stepC - 1][r] == type do
        stepC = stepC - 1
        table.insert (temp_matches, {x = stepC, y= r})
    end

    if (#temp_matches >= 2) then self:addMatches (temp_matches) end
    temp_matches = {}

    --check right
    stepC = c;
    while stepC + 1 <= constants.GRID_SIZE_X and self.gameLayer.grid[stepC + 1][r] == type do
        stepC = stepC + 1
        table.insert (temp_matches, {x = stepC, y = r})
    end

    if (#temp_matches >= 2) then self:addMatches (temp_matches) end
    
end

function GridController:addMatches (matches)
    for key, value in pairs(matches) do
        if (self:find(value, self.matchArray) == false) then
            table.insert(self.matchArray, value)
        end
    end
end

function GridController:find (np, array)
    for key, value in pairs(array) do
        if (value.x == np.x and value.y == np.y) then return true end
    end
    return false
end

function GridController:findGemAtPosition (position)
    
    local mx = position.x
    local my = position.y
    
    local gridWidth = constants.GRID_SIZE_X * (constants.TILE_SIZE + constants.GRID_SPACE)
    local gridHeight = constants.GRID_SIZE_Y * (constants.TILE_SIZE + constants.GRID_SPACE)
    
    mx = mx - self.gameLayer.gemsContainer:getPositionX()
    my = my - self.gameLayer.gemsContainer:getPositionY()
    
    if (mx < 0) then mx = 0 end
    if (my < 0) then my = 0 end

    if (mx > gridWidth) then mx = gridWidth end
    if (my > gridHeight) then my = gridHeight end
    
    local x = math.ceil ((mx - constants.TILE_SIZE * 0.5) / (constants.TILE_SIZE + constants.GRID_SPACE))
    local y = math.ceil ((my - constants.TILE_SIZE * 0.5) / (constants.TILE_SIZE + constants.GRID_SPACE))
    
    if (x < 1) then x = 1 end
    if (y < 1) then y = 1 end
    if (x > constants.GRID_SIZE_X) then x = constants.GRID_SIZE_X end
    if (y > constants.GRID_SIZE_Y) then y = constants.GRID_SIZE_Y end
    
    return {x = x, y = y, gem = self.gameLayer.gridGemsColumnMap[x][y]}
    
end

function GridController:selectStartGem (touchedGem)
    
    if (self.gameLayer.selectedGem == nil) then
        self.gameLayer.selectedGem = touchedGem.gem
        self.gameLayer.targetGem = nil
        self.gameLayer.targetIndex = nil
        touchedGem.gem:setLocalZOrder(constants.Z_SWAP_2)
        self.gameLayer.selectedIndex = {x = touchedGem.x, y = touchedGem.y}
        self.gameLayer.selectedGemPosition = {x = touchedGem.gem:getPositionX(),
                                              y = touchedGem.gem:getPositionY()}
        self.gameLayer.gridAnimations:animateSelected (touchedGem.gem)                                              
    end
end

function GridController:selectTargetGem (touchedGem)

    if (self.gameLayer.targetGem ~= nil) then return end
    self.enabled = false
    self.gameLayer.targetIndex = {x = touchedGem.x, y = touchedGem.y}
    self.gameLayer.targetGem = touchedGem.gem
    self.gameLayer.targetGem:setLocalZOrder(constants.Z_SWAP_1)
    self.gameLayer:swapGemsToNewPosition()
end

function GridController:onTouchDown (touch)
    if (self.gameLayer.running == false) then
        local scene = require("GameScene")
        local gameScene = scene.create()
        cc.Director:getInstance():replaceScene(gameScene)
        local bgMusicPath = cc.FileUtils:getInstance():fullPathForFilename("background.mp3") 
        cc.SimpleAudioEngine:getInstance():playMusic(bgMusicPath, true)
        return 
    end
    self.touchDown = true
    if (self.enabled == false) then return end
    local touchedGem = self:findGemAtPosition (touch)
    if (touchedGem.gem ~= nil ) then 
        if (self.gameLayer.selectedGem == nil) then
            self:selectStartGem(touchedGem)
        else
            if (self:isValidTarget(touchedGem.x, touchedGem.y, touch) == true) then 
                self:selectTargetGem(touchedGem)
            else
                if (self.gameLayer.selectedGem ~= nil) then self.gameLayer.selectedGem:deselect() end
                self.gameLayer.selectedGem = nil
                self:selectStartGem (touchedGem)
            end
        end
    end
end

function GridController:onTouchUp (touch)
    if (self.gameLayer.running == false) then return end
    self.touchDown = false
    if (self.enabled == false) then return end
    if (self.gameLayer.selectedGem ~= nil) then self.gameLayer:dropSelectedGem() end
end

function GridController:onTouchMove (touch)
    if (self.gameLayer.running == false) then return end
    if (self.enabled == false) then return end
    --track to see if we have a valid target
    if (self.gameLayer.selectedGem ~= nil and self.touchDown == true) then
        self.gameLayer.selectedGem:setPosition(
        touch.x - self.gameLayer.gemsContainer:getPositionX(), 
        touch.y - self.gameLayer.gemsContainer:getPositionY())
        local touchedGem = self:findGemAtPosition (touch)
        if (touchedGem.gem ~= nil and self:isValidTarget(touchedGem.x, touchedGem.y, touch) == true ) then
            self:selectTargetGem(touchedGem)
        end
    end
end

function GridController:isValidTarget (px, py, touch)
    
    local offbounds = false
    
    if (px > self.gameLayer.selectedIndex.x + 1) then offbounds = true end
    if (px < self.gameLayer.selectedIndex.x - 1) then offbounds = true end
    if (py > self.gameLayer.selectedIndex.y + 1) then offbounds = true end
    if (py < self.gameLayer.selectedIndex.y - 1) then offbounds = true end

    local cell = math.sin (math.atan2 (math.pow( self.gameLayer.selectedIndex.x - px, 2), math.pow( self.gameLayer.selectedIndex.y- py, 2) ) )
    if (cell ~= 0 and cell ~= 1) then
        offbounds = true
    end

    if (offbounds == true) then
        return false
    end
    
    local touchedGem = self.gameLayer.gridGemsColumnMap[px][py]
    
    if (touchedGem.gem == self.gameLayer.selectedGem or (px == self.gameLayer.selectedIndex.x and py == self.gameLayer.selectedIndex.y)) then
        self.gameLayer.targetGem = nil
        return false
    end
    return true
end

return GridController